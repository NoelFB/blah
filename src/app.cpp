#include <blah/app.h>
#include <blah/common.h>
#include <blah/time.h>
#include <blah/graphics/target.h>
#include "internal/platform.h"
#include "internal/renderer.h"
#include "internal/input.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

using namespace Blah;

#define BLAH_ASSERT_RUNNING() BLAH_ASSERT(app_is_running, "The App is not running (call App::run)")

namespace
{
	Config app_config;
	bool app_is_running = false;
	bool app_is_exiting = false;
	u64 app_time_last;
	u64 app_time_accumulator = 0;

	void app_iterate()
	{
		// update at a fixed timerate
		// TODO: allow a non-fixed step update?
		{
			u64 time_target = (u64)((1.0 / app_config.target_framerate) * Time::ticks_per_second);
			u64 time_curr = Platform::ticks();
			u64 time_diff = time_curr - app_time_last;
			app_time_last = time_curr;
			app_time_accumulator += time_diff;

			// do not let us run too fast
			while (app_time_accumulator < time_target)
			{
				int milliseconds = (int)(time_target - app_time_accumulator) / (Time::ticks_per_second / 1000);
				Platform::sleep(milliseconds);

				time_curr = Platform::ticks();
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

				Input::update_state();
				Platform::update(Input::state);
				Input::update_bindings();
				Renderer::instance->update();

				if (app_config.on_update != nullptr)
					app_config.on_update();
			}
		}

		// render
		{
			Renderer::instance->before_render();

			if (app_config.on_render != nullptr)
				app_config.on_render();

			Renderer::instance->after_render();
			Platform::present();
		}
	}

	// A dummy Frame Buffer that represents the Back Buffer
	// it doesn't actually contain any textures or details.
	class BackBuffer final : public Target
	{
	public:
		Attachments empty_textures;
		Attachments& textures() override { BLAH_ASSERT(false, "Backbuffer doesn't have any textures"); return empty_textures; }
		const Attachments& textures() const override { BLAH_ASSERT(false, "Backbuffer doesn't have any textures"); return empty_textures; }
		int width() const override { int w, h; Platform::get_draw_size(&w, &h); return w; }
		int height() const override { int w, h; Platform::get_draw_size(&w, &h); return h; }
		void clear(Color color, float depth, u8 stencil, ClearMask mask) override
		{
			BLAH_ASSERT_RENDERER();
			if (Renderer::instance) Renderer::instance->clear_backbuffer(color, depth, stencil, mask);
		}
	};

	BackBuffer app_backbuffer;
	TargetRef app_backbuffer_ref = TargetRef(&app_backbuffer);
}

bool App::run(const Config* c)
{
	BLAH_ASSERT(!app_is_running, "The Application is already running");
	BLAH_ASSERT(c != nullptr, "The Application requires a valid Config");
	BLAH_ASSERT(c->name != nullptr, "The Application Name cannot be null");
	BLAH_ASSERT(c->width > 0 && c->height > 0, "The Width and Height must be larget than 0");
	BLAH_ASSERT(c->max_updates > 0, "Max Updates must be >= 1");
	BLAH_ASSERT(c->target_framerate > 0, "Target Framerate must be >= 1");

	// copy config over
	app_config = *c;

	// exit the application by default
	if (!app_config.on_exit_request)
		app_config.on_exit_request = App::exit;

	// default renderer type
	if (app_config.renderer_type == RendererType::None)
		app_config.renderer_type = Renderer::default_type();

	// default values
	app_is_running = true;
	app_is_exiting = false;

	// initialize the system
	if (!Platform::init(app_config))
	{
		Log::error("Failed to initialize Platform module");
		return false;
	}

	// initialize graphics
	{
		// instantiate
		Renderer::instance = Renderer::try_make_renderer(app_config.renderer_type);

		// wasn't able to make any
		if (Renderer::instance == nullptr)
		{
			Log::error("Renderer implementation was not found");
			return false;
		}

		if (!Renderer::instance->init())
		{
			Log::error("Renderer failed to initialize");
			delete Renderer::instance;
			return false;
		}
	}

	// input
	Input::init();

	// prepare by updating input & platform once
	Input::update_state();
	Platform::update(Input::state);

	// startup
	if (app_config.on_startup != nullptr)
		app_config.on_startup();

	app_time_last = Platform::ticks();
	app_time_accumulator = 0;

	// display window
	Platform::ready();

	// Begin main loop
	// Emscripten requires the main loop be separated into its own call

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(app_iterate, 0, 1);
#else
	while (!app_is_exiting)
		app_iterate();
#endif

	// shutdown
	if (app_config.on_shutdown != nullptr)
		app_config.on_shutdown();

	Renderer::instance->shutdown();
	Platform::shutdown();

	delete Renderer::instance;
	Renderer::instance = nullptr;

	// clear static state
	app_is_running = false;
	app_is_exiting = false;

	Time::ticks = 0;
	Time::seconds = 0;
	Time::previous_ticks = 0;
	Time::previous_seconds = 0;
	Time::delta = 0;

	return true;
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
	if (Renderer::instance)
		return Point(app_backbuffer.width(), app_backbuffer.height());
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

void App::fullscreen(bool enabled)
{
	BLAH_ASSERT_RUNNING();
	Platform::set_fullscreen(enabled);
}

const RendererFeatures& App::renderer()
{
	BLAH_ASSERT_RUNNING();
	BLAH_ASSERT_RENDERER();
	return Renderer::instance->features;
}

const TargetRef& App::backbuffer()
{
	BLAH_ASSERT_RUNNING();
	return app_backbuffer_ref;
}

void System::open_url(const char* url)
{
	BLAH_ASSERT_RUNNING();
	Platform::open_url(url);
}