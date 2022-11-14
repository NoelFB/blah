#pragma once
#include "blah_renderer.h"
#include "blah_platform.h"

#define BLAH_ASSERT_RENDERER() BLAH_ASSERT(App::Internal::renderer, "Renderer has not been created")
#define BLAH_ASSERT_PLATFORM() BLAH_ASSERT(App::Internal::platform, "Platform has not been created")

namespace Blah
{
	namespace App
	{
		namespace Internal
		{
			extern Platform* platform;
			extern Renderer* renderer;

			void iterate();
			void shutdown();
		}
	}

	namespace Input
	{
		namespace Internal
		{
			// Initializes the Input State
			void init();

			// Steps the input state
			void step_state();

			// Updates bindings
			void update_bindings();

			// Clears Input State
			void shutdown();
		}
	}

	namespace Audio
	{
		namespace Internal
		{
			extern bool is_init;

			// Pass in NULL for `os_handle`, except for the DirectSound backend this should be hwnd.
			// play_frequency_in_Hz depends on your audio file, 44100 seems to be fine.
			// buffered_samples is clamped to be at least 1024.
			bool init(void* os_handle, unsigned play_frequency_in_Hz, int buffered_samples);
			void shutdown();
		}
	}
}