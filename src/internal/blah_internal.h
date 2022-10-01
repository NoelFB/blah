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
}