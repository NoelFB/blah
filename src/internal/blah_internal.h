#pragma once
#include "blah_renderer.h"
#include "blah_platform.h"

#define BLAH_ASSERT_RENDERER() BLAH_ASSERT(Blah::Internal::renderer, "Renderer has not been created")
#define BLAH_ASSERT_PLATFORM() BLAH_ASSERT(Blah::Internal::platform, "Platform has not been created")

namespace Blah
{
	namespace Internal
	{
		extern Platform* platform;
		extern Renderer* renderer;
		extern bool audio_is_init;

		void app_step();
		void app_shutdown();

		void input_init();
		void input_step_state();
		void input_step_bindings();
		void input_shutdown();

		// Pass in NULL for `os_handle`, except for the DirectSound backend this should be hwnd.
		// play_frequency_in_Hz depends on your audio file, 44100 seems to be fine.
		// buffered_samples is clamped to be at least 1024.
		bool audio_init(void* os_handle, unsigned play_frequency_in_Hz, int buffered_samples);
		void audio_shutdown();
		void audio_update();
	}
}