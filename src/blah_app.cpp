#include <blah_app.h>
#include <blah_common.h>
#include <blah_time.h>
#include "internal/blah_internal.h"
#include "internal/blah_platform.h"
#include "internal/blah_renderer.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

using namespace Blah;

// Internal Audio bool
bool Internal::audio_is_init = false;

namespace
{
	// Global App State
	Config     app_config;
	bool       app_is_running = false;
	bool       app_is_exiting = false;
	u64        app_time_last;
	u64        app_time_accumulator = 0;
	u32        app_flags = 0;
	TargetRef  app_backbuffer;
	Renderer*  app_renderer_api;

	void get_drawable_size(int* w, int* h)
	{
		// Some renderer implementations might return their own size
		if (app_renderer_api->get_draw_size(w, h))
			return;

		// otherwise fallback to the platform size
		Platform::get_draw_size(w, h);
	}

	// A dummy Target that represents the Back Buffer.
	// It doesn't contain any data, rather it forwards calls along to the actual BackBuffer.
	class BackBuffer final : public Target
	{
	public:
		Attachments empty_textures;
		Attachments& textures() override { BLAH_ASSERT(false, "Backbuffer doesn't have any textures"); return empty_textures; }
		const Attachments& textures() const override { BLAH_ASSERT(false, "Backbuffer doesn't have any textures"); return empty_textures; }
		int width() const override { int w, h; get_drawable_size(&w, &h); return w; }
		int height() const override { int w, h; get_drawable_size(&w, &h); return h; }
		void clear(Color color, float depth, u8 stencil, ClearMask mask) override
		{
			BLAH_ASSERT_RENDERER();
			if (app_renderer_api)
				app_renderer_api->clear_backbuffer(color, depth, stencil, mask);
		}
	};
}

bool App::run(const Config* c)
{
	BLAH_ASSERT(!app_is_running, "The Application is already running");

	// copy config over
	app_config = *c;

	// exit the application by default
	if (!app_config.on_exit_request)
		app_config.on_exit_request = App::exit;

	// default renderer type
	if (app_config.renderer_type == RendererType::None)
		app_config.renderer_type = Renderer::default_type();

	// exit out if setup is wrong
	BLAH_ASSERT(c != nullptr, "The Application requires a valid Config");
	BLAH_ASSERT(c->name != nullptr, "The Application Name cannot be null");
	BLAH_ASSERT(c->width > 0 && c->height > 0, "The Width and Height must be larget than 0");
	BLAH_ASSERT(c->max_updates > 0, "Max Updates must be >= 1");
	BLAH_ASSERT(c->target_framerate > 0, "Target Framerate must be >= 1");

	if (app_is_running || c == nullptr || c->width <= 0 || c->height <= 0 || c->max_updates <= 0 || c->target_framerate <= 0)
	{
		Internal::app_shutdown();
		return false;
	}

	// default values
	app_is_running = true;
	app_is_exiting = false;
	app_flags = app_config.flags;
	app_backbuffer = TargetRef(new BackBuffer());

	// initialize the system
	{
		if (!Platform::init(app_config))
		{
			Log::error("Failed to initialize Platform module");
			Internal::app_shutdown();
			return false;
		}
	}

	// initialize audio
	{
		if (!Internal::audio_is_init) {
			int more_on_emscripten = 1;
#ifdef __EMSCRIPTEN__
			more_on_emscripten = 4;
#endif
			Internal::audio_is_init = Internal::audio_init(c->audio_frequency_in_Hz, 1024 * more_on_emscripten);
		}
	}

	// initialize graphics
	{
		app_renderer_api = Renderer::try_make_renderer(app_config.renderer_type);
		if (app_renderer_api == nullptr)
		{
			Log::error("Renderer module was not found");
			Internal::app_shutdown();
			return false;
		}

		if (!app_renderer_api->init())
		{
			Log::error("Failed to initialize Renderer module");
			Internal::app_shutdown();
			return false;
		}
	}

	// apply default flags
	Platform::set_app_flags(app_flags);
	app_renderer_api->set_app_flags(app_flags);

	// input + poll the platform once
	Internal::input_init();
	Internal::input_step_state();
	Platform::update(Input::state);

	// startup
	if (app_config.on_startup != nullptr)
		app_config.on_startup();
	app_time_last = Platform::ticks();
	app_time_accumulator = 0;

	// display window
	Platform::ready();

	// Begin main loop
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(Internal::iterate, 0, 1);
#else
	while (!app_is_exiting)
		Internal::app_step();
#endif

	// shutdown
	if (app_config.on_shutdown != nullptr)
		app_config.on_shutdown();
	Internal::app_shutdown();
	return true;
}

bool App::is_running()
{
	return app_is_running;
}

void Internal::app_step()
{
	static const auto step = []()
	{
		Internal::input_step_state();
		Platform::update(Input::state);
		Internal::input_step_bindings();
		app_renderer_api->update();
		if (app_config.on_update != nullptr)
			app_config.on_update();
	};

	bool is_fixed_timestep = App::get_flag(Flags::FixedTimestep);

	// Update in Fixed Timestep
	if (is_fixed_timestep)
	{
		u64 time_target = (u64)((1.0 / app_config.target_framerate) * Time::ticks_per_second);
		u64 ticks_curr = Platform::ticks();
		u64 ticks_diff = ticks_curr - app_time_last;
		app_time_last = ticks_curr;
		app_time_accumulator += ticks_diff;

		// do not let us run too fast
		while (app_time_accumulator < time_target)
		{
			int milliseconds = (int)(time_target - app_time_accumulator) / (Time::ticks_per_second / 1000);
			Platform::sleep(milliseconds);

			ticks_curr = Platform::ticks();
			ticks_diff = ticks_curr - app_time_last;
			app_time_last = ticks_curr;
			app_time_accumulator += ticks_diff;
		}

		// Do not allow us to fall behind too many updates
		// (otherwise we'll get spiral of death)
		u64 time_maximum = app_config.max_updates * time_target;
		if (app_time_accumulator > time_maximum)
			app_time_accumulator = time_maximum;

		// do as many updates as we can
		while (app_time_accumulator >= time_target)
		{
			app_time_accumulator -= time_target;

			Time::delta = (1.0f / app_config.target_framerate);

			if (Time::pause_timer > 0)
			{
				Time::pause_timer -= Time::delta;
				if (Time::pause_timer <= -0.0001)
					Time::delta = -Time::pause_timer;
				else
					continue;
			}

			Time::previous_ticks = Time::ticks;
			Time::ticks += time_target;
			Time::previous_seconds = Time::seconds;
			Time::seconds += Time::delta;

			step();
		}
	}
	// Update with Variable Timestep
	else
	{
		u64 ticks_curr = Platform::ticks();
		u64 ticks_diff = ticks_curr - app_time_last;
		app_time_last = ticks_curr;
		app_time_accumulator += ticks_diff;

		Time::delta = ticks_diff / (float)Time::ticks_per_second;

		if (Time::pause_timer > 0)
		{
			Time::pause_timer -= Time::delta;
		}
		else
		{
			Time::previous_ticks = Time::ticks;
			Time::ticks += ticks_diff;
			Time::previous_seconds = Time::seconds;
			Time::seconds += Time::delta;

			step();
		}
	}

	// Draw Frame
	{
		app_renderer_api->before_render();
		if (app_config.on_render != nullptr)
			app_config.on_render();
		app_renderer_api->after_render();
		Platform::present();
	}

	// Update audio
	if (Internal::audio_is_init)
		Blah::Internal::audio_update();
}

void Internal::app_shutdown()
{
	Internal::input_shutdown();

	if (app_renderer_api)
	{
		app_renderer_api->shutdown();
		delete app_renderer_api;
	}
	app_renderer_api = nullptr;

	if (Internal::audio_is_init)
	{
		Internal::audio_shutdown();
		Internal::audio_is_init = false;
	}

	Platform::shutdown();

	// clear static App state
	app_config = Config();
	app_is_running = false;
	app_is_exiting = false;
	app_time_last = 0;
	app_time_accumulator = 0;
	app_backbuffer = TargetRef();

	// clear static Time state
	Time::ticks = 0;
	Time::seconds = 0;
	Time::previous_ticks = 0;
	Time::previous_seconds = 0;
	Time::delta = 0;
}

Renderer* Internal::app_renderer()
{
	BLAH_ASSERT_RUNNING();
	return app_renderer_api;
}

void App::exit()
{
	BLAH_ASSERT_RUNNING();
	if (!app_is_exiting && app_is_running)
		app_is_exiting = true;
}

const Config& App::config()
{
	BLAH_ASSERT_RUNNING();
	return app_config;
}

const char* App::path()
{
	BLAH_ASSERT_RUNNING();
	return Platform::app_path();
}

const char* App::user_path()
{
	BLAH_ASSERT_RUNNING();
	return Platform::user_path();
}

const char* App::get_title()
{
	BLAH_ASSERT_RUNNING();
	return Platform::get_title();
}

void App::set_title(const char* title)
{
	BLAH_ASSERT_RUNNING();
	Platform::set_title(title);
}

Point App::get_position()
{
	BLAH_ASSERT_RUNNING();
	Point result;
	Platform::get_position(&result.x, &result.y);
	return result;
}

void App::set_position(Point point)
{
	BLAH_ASSERT_RUNNING();
	Platform::set_position(point.x, point.y);
}

Point App::get_size()
{
	BLAH_ASSERT_RUNNING();
	Point result;
	Platform::get_size(&result.x, &result.y);
	return result;
}

void App::set_size(Point point)
{
	BLAH_ASSERT_RUNNING();
	Platform::set_size(point.x, point.y);
}

Point App::get_backbuffer_size()
{
	BLAH_ASSERT_RUNNING();
	if (app_renderer_api)
		return Point(app_backbuffer->width(), app_backbuffer->height());
	return Point(0, 0);
}

float App::content_scale()
{
	BLAH_ASSERT_RUNNING();
	return Platform::get_content_scale();
}

bool App::focused()
{
	BLAH_ASSERT_RUNNING();
	return Platform::get_focused();
}

void App::set_flag(u32 flag, bool enabled)
{
	BLAH_ASSERT_RUNNING();

	u32 was = app_flags;

	if (enabled)
		app_flags |= flag;
	else
		app_flags &= ~flag;

	if (was != app_flags)
	{
		Platform::set_app_flags(app_flags);
		if (app_renderer_api)
			app_renderer_api->set_app_flags(app_flags);
	}
}

bool App::get_flag(u32 flag)
{
	BLAH_ASSERT_RUNNING();
	return ((app_flags & flag) == flag);
}

const RendererInfo& App::renderer()
{
	BLAH_ASSERT_RUNNING();
	BLAH_ASSERT_RENDERER();
	return app_renderer_api->info;
}

const TargetRef& App::backbuffer()
{
	BLAH_ASSERT_RUNNING();
	return app_backbuffer;
}

void System::open_url(const char* url)
{
	BLAH_ASSERT_RUNNING();
	Platform::open_url(url);
}