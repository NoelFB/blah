#pragma once
#include <blah/input/input.h>

namespace Blah
{
	namespace Internal
	{
		namespace Input
		{
			// This is called internally by the app, and initializes the input state
			void init();

			// This is called internally by the app, and updates the input state
			void frame();

			// Call this when the Mouse moves relative to the window
			void on_mouse_move(float x, float y);

			// Call this when the Mouse moves relative to the screen
			void on_mouse_screen_move(float x, float y);

			// Call this when a Mouse Button is pressed
			void on_mouse_down(MouseButton button);

			// Call this when a Mouse Button is released
			void on_mouse_up(MouseButton button);

			// Call this when the Mouse Wheel moves
			void on_mouse_wheel(Point wheel);

			// Call this when a keyboard key is pressed
			void on_key_down(Key key);

			// Call this when a keyboard key is released
			void on_key_up(Key key);

			// Call this on Text Input
			void on_text_utf8(const char* text);

			// Call this when a Controller is connected. Note that the Name parameter must be kept valid
			// until on_controller_disconnect is called with the same index.
			void on_controller_connect(int index, const char* name, int isGamepad, int buttonCount, int axisCount);

			// Call this when a controller is disconnected
			void on_controller_disconnect(int index);

			// Call this when a controller button is pressed
			void on_button_down(int index, int button);

			// Call this when a controller button is released
			void on_button_up(int index, int button);

			/// Call this when a controller axis is moved
			void on_axis_move(int index, int axis, float value);
		}
	}
}