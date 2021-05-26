#pragma once
#include <blah/input.h>

namespace Blah
{
	namespace Input
	{
		// Initializes the Input State
		void init();

		// Steps the input state
		void update_state();

		// Updates bindings
		void update_bindings();
	}
}