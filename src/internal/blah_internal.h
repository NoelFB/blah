#pragma once
#include "blah_renderer.h"
#include "blah_platform.h"

#define BLAH_ASSERT_RENDERER() BLAH_ASSERT(Blah::Internal::app_renderer(), "Renderer has not been created")
#define BLAH_ASSERT_RUNNING() BLAH_ASSERT(Blah::App::is_running(), "The App is not running (call App::run)")

namespace Blah
{
	namespace Internal
	{
		extern bool audio_is_init;

		void app_step();
		void app_shutdown();
		Renderer* app_renderer();

		void input_init();
		void input_step_state();
		void input_step_bindings();
		void input_shutdown();

		// play_frequency_in_Hz depends on your audio file, 44100 seems to be fine.
		// buffered_samples is clamped to be at least 1024.
		bool audio_init(unsigned play_frequency_in_Hz, int buffered_samples);
		void audio_shutdown();
		void audio_update();
	}
}