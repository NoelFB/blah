#include <blah/app.h>
#include <blah/common.h>
#include <blah/time.h>
#include "internal/internal.h"
#include "internal/platform.h"
#include "internal/renderer.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

using namespace Blah;

#define BLAH_ASSERT_RUNNING() BLAH_ASSERT(app_is_running, "The App is not running (call App::run)")

// Interal Platform Pointer
Platform* App::Internal::platform = nullptr;

// Internal Renderer Pointer
Renderer* App::Internal::renderer = nullptr;

namespace
{
	// Global App State
	Config     app_config;
	bool       app_is_running = false;
	bool       app_is_exiting = false;
	u64        app_time_last;
	u64        app_time_accumulator = 0;
	TargetRef  app_backbuffer;

	void get_drawable_size(int* w, int* h)
	{
		// Some renderer implementations might return their own size
		if (App::Internal::renderer->get_draw_size(w, h))
			return;

		// otherwise fallback to the platform size
		App::Internal::platform->get_draw_size(w, h);
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
			if (App::Internal::renderer)
				App::Internal::renderer->clear_backbuffer(color, depth, stencil, mask);
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
		App::Internal::shutdown();
		return false;
	}

	// default values
	app_is_running = true;
	app_is_exiting = false;
	app_backbuffer = TargetRef(new BackBuffer());

	// initialize the system
	{
		Internal::platform = Platform::try_make_platform(app_config);
		if (!Internal::platform)
		{
			Log::error("Failed to create Platform module");
			App::Internal::shutdown();
			return false;
		}

		if (!Internal::platform->init(app_config))
		{
			Log::error("Failed to initialize Platform module");
			App::Internal::shutdown();
			return false;
		}
	}

	// initialize graphics
	{
		Internal::renderer = Renderer::try_make_renderer(app_config.renderer_type);
		if (Internal::renderer == nullptr)
		{
			Log::error("Renderer module was not found");
			App::Internal::shutdown();
			return false;
		}

		if (!Internal::renderer->init())
		{
			Log::error("Failed to initialize Renderer module");
			App::Internal::shutdown();
			return false;
		}
	}

	// input + poll the platform once
	Input::Internal::init();
	Input::Internal::step_state();
	Internal::platform->update(Input::state);

	// startup
	if (app_config.on_startup != nullptr)
		app_config.on_startup();
	app_time_last = Internal::platform->ticks();
	app_time_accumulator = 0;

	// display window
	Internal::platform->ready();

	// Begin main loop
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(App::Internal::iterate, 0, 1);
#else
	while (!app_is_exiting)
		App::Internal::iterate();
#endif

	// shutdown
	if (app_config.on_shutdown != nullptr)
		app_config.on_shutdown();
	App::Internal::shutdown();
	return true;
}

bool App::is_running()
{
	return app_is_running;
}

void App::Internal::iterate()
{
	// update at a fixed timerate
	// TODO: allow a non-fixed step update?
	{
		u64 time_target = (u64)((1.0 / app_config.target_framerate) * Time::ticks_per_second);
		u64 time_curr = App::Internal::platform->ticks();
		u64 time_diff = time_curr - app_time_last;
		app_time_last = time_curr;
		app_time_accumulator += time_diff;

		// do not let us run too fast
		while (app_time_accumulator < time_target)
		{
			int milliseconds = (int)(time_target - app_time_accumulator) / (Time::ticks_per_second / 1000);
			App::Internal::platform->sleep(milliseconds);

			time_curr = App::Internal::platform->ticks();
			time_diff = time_curr - app_time_last;
			app_time_last = time_curr;
			app_time_accumulator += time_diff;
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

			Input::Internal::step_state();
			platform->update(Input::state);
			Input::Internal::update_bindings();
			renderer->update();

			if (app_config.on_update != nullptr)
				app_config.on_update();
		}
	}

	// render
	{
		renderer->before_render();

		if (app_config.on_render != nullptr)
			app_config.on_render();

		renderer->after_render();
		platform->present();
	}
}

void App::Internal::shutdown()
{
	Input::Internal::shutdown();

	if (renderer)
		renderer->shutdown();

	if (platform)
		platform->shutdown();

	if (renderer)
		delete renderer;
	renderer = nullptr;

	if (platform)
		delete platform;
	platform = nullptr;

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
	return Internal::platform->app_path();
}

const char* App::user_path()
{
	BLAH_ASSERT_RUNNING();
	return Internal::platform->user_path();
}

const char* App::get_title()
{
	BLAH_ASSERT_RUNNING();
	return Internal::platform->get_title();
}

void App::set_title(const char* title)
{
	BLAH_ASSERT_RUNNING();
	Internal::platform->set_title(title);
}

Point App::get_position()
{
	BLAH_ASSERT_RUNNING();
	Point result;
	Internal::platform->get_position(&result.x, &result.y);
	return result;
}

void App::set_position(Point point)
{
	BLAH_ASSERT_RUNNING();
	Internal::platform->set_position(point.x, point.y);
}

Point App::get_size()
{
	BLAH_ASSERT_RUNNING();
	Point result;
	Internal::platform->get_size(&result.x, &result.y);
	return result;
}

void App::set_size(Point point)
{
	BLAH_ASSERT_RUNNING();
	Internal::platform->set_size(point.x, point.y);
}

Point App::get_backbuffer_size()
{
	BLAH_ASSERT_RUNNING();
	if (Internal::renderer)
		return Point(app_backbuffer->width(), app_backbuffer->height());
	return Point(0, 0);
}

float App::content_scale()
{
	BLAH_ASSERT_RUNNING();
	return Internal::platform->get_content_scale();
}

bool App::focused()
{
	BLAH_ASSERT_RUNNING();
	return Internal::platform->get_focused();
}

void App::fullscreen(bool enabled)
{
	BLAH_ASSERT_RUNNING();
	Internal::platform->set_fullscreen(enabled);
}

const RendererInfo& App::renderer()
{
	BLAH_ASSERT_RUNNING();
	BLAH_ASSERT_RENDERER();
	return Internal::renderer->info;
}

const TargetRef& App::backbuffer()
{
	BLAH_ASSERT_RUNNING();
	return app_backbuffer;
}

void System::open_url(const char* url)
{
	BLAH_ASSERT_RUNNING();
	App::Internal::platform->open_url(url);
}