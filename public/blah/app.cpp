#include <blah/app.h>
#include <blah/log.h>
#include <blah/time.h>
#include <blah/math/point.h>

#include <blah/internal/platform_backend.h>
#include <blah/internal/graphics_backend.h>
#include <blah/internal/input_backend.h>

using namespace Blah;

namespace
{
	static Config app_config;
	static bool app_is_running = false;
	static bool app_is_exiting = false;
}

Config::Config()
{
	name = nullptr;
	width = 0;
	height = 0;
	target_framerate = 60;
	max_updates = 5;

	on_startup = nullptr;
	on_shutdown = nullptr;
	on_update = nullptr;
	on_render = nullptr;
	on_exit_request = App::exit;
	on_info = nullptr;
	on_warn = nullptr;
	on_error = nullptr;
}

bool App::run(const Config* c)
{
	BLAH_ASSERT(!app_is_running, "The Application is already running");
	BLAH_ASSERT(c != nullptr, "The Application requires a valid Config");
	BLAH_ASSERT(c->name != nullptr, "The Application Name cannot be null");
	BLAH_ASSERT(c->width > 0 && c->height > 0, "The Width and Height must be larget than 0");
	BLAH_ASSERT(c->max_updates > 0, "Max Updates must be >= 1");
	BLAH_ASSERT(c->target_framerate > 0, "Target Framerate must be >= 1");

	app_config = *c;
	app_is_running = true;
	app_is_exiting = false;

	Log::print("Starting Up ...");

	// initialize the system
	if (!PlatformBackend::init(&app_config))
	{
		Log::error("Failed to initialize Platform module");
		return false;
	}

	// initialize graphics
	if (!GraphicsBackend::init())
	{
		Log::error("Failed to initialize Graphics module");
		return false;
	}

	// input
	InputBackend::init();

	// startup
	if (app_config.on_startup != nullptr)
		app_config.on_startup();

	uint64_t time_last = PlatformBackend::time();
	uint64_t time_accumulator = 0;

	// display window
	PlatformBackend::ready();

	while (!app_is_exiting)
	{
		// poll system events
		PlatformBackend::frame();

		// update at a fixed timerate
		// TODO: allow a non-fixed step update?
		{
			uint64_t time_target = (uint64_t)((1.0f / app_config.target_framerate) * 1000);
			uint64_t time_curr = PlatformBackend::time();
			uint64_t time_diff = time_curr - time_last;
			time_last = time_curr;
			time_accumulator += time_diff;

			// do not let us run too fast
			while (time_accumulator < time_target)
			{
				PlatformBackend::sleep((int)(time_target - time_accumulator));

				time_curr = PlatformBackend::time();
				time_diff = time_curr - time_last;
				time_last = time_curr;
				time_accumulator += time_diff;
			}

			// Do not allow us to fall behind too many updates
			// (otherwise we'll get spiral of death)
			uint64_t time_maximum = app_config.max_updates * time_target;
			if (time_accumulator > time_maximum)
				time_accumulator = time_maximum;

			// do as many updates as we can
			while (time_accumulator >= time_target)
			{
				time_accumulator -= time_target;

				Time::delta = (1.0f / app_config.target_framerate);

				if (Time::pause_timer > 0)
				{
					Time::pause_timer -= Time::delta;
					if (Time::pause_timer <= -0.0001f)
						Time::delta = -Time::pause_timer;
					else
						continue;
				}

				Time::milliseconds += time_target;
				Time::previous_elapsed = Time::elapsed;
				Time::elapsed += Time::delta;

				InputBackend::frame();
				GraphicsBackend::frame();

				if (app_config.on_update != nullptr)
					app_config.on_update();
			}
		}

		// render
		{
			GraphicsBackend::before_render();

			if (app_config.on_render != nullptr)
				app_config.on_render();

			GraphicsBackend::after_render();
			PlatformBackend::present();
		}

	}

	Log::print("Shutting down ...");

	// shutdown
	if (app_config.on_shutdown != nullptr)
		app_config.on_shutdown();

	GraphicsBackend::shutdown();
	PlatformBackend::shutdown();

	// clear static state
	Log::print("Exited");
	app_is_running = false;
	app_is_exiting = false;
	Time::milliseconds = 0;
	Time::elapsed = 0;
	Time::delta = 0;

	return true;
}

bool App::is_running()
{
	return app_is_running;
}

void App::exit()
{
	if (!app_is_exiting && app_is_running)
		app_is_exiting = true;
}

const Config* App::config()
{
	return &app_config;
}

const char* App::path()
{
	return PlatformBackend::app_path();
}

const char* App::user_path()
{
	return PlatformBackend::user_path();
}

int App::width()
{
	int w, h;
	PlatformBackend::get_size(&w, &h);
	return w;
}

int App::height()
{
	int w, h;
	PlatformBackend::get_size(&w, &h);
	return h;
}

int App::draw_width()
{
	int w, h;
	PlatformBackend::get_draw_size(&w, &h);
	return w;
}

int App::draw_height()
{
	int w, h;
	PlatformBackend::get_draw_size(&w, &h);
	return h;
}

float App::content_scale()
{
	return PlatformBackend::get_content_scale();
}

void App::fullscreen(bool enabled)
{
	PlatformBackend::set_fullscreen(enabled);
}