#include <blah/input/input.h>
#include <blah/app.h>
#include <blah/time.h>
#include <blah/log.h>
#include <blah/math/point.h>
#include <blah/internal/input_backend.h>
#include <string.h>

using namespace Blah;

namespace
{
	InputState g_last_state;
	InputState g_curr_state;
	InputState g_next_state;
	InputState g_empty_state;
	ControllerState g_empty_controller;
}

void InputBackend::init()
{
	g_empty_controller.name = "Disconnected";
	for (int i = 0; i < Blah::Input::max_controllers; i++)
		g_empty_state.controllers[i].name = g_empty_controller.name;

	g_last_state = g_empty_state;
	g_curr_state = g_empty_state;
	g_next_state = g_empty_state;
}

void InputBackend::frame()
{
	// cycle states
	g_last_state = g_curr_state;
	g_curr_state = g_next_state;

	// copy state, clear pressed / released values
	{
		for (int i = 0; i < Blah::Input::max_keyboard_keys; i++)
		{
			g_next_state.keyboard.pressed[i] = false;
			g_next_state.keyboard.released[i] = false;
		}

		for (int i = 0; i < Blah::Input::max_mouse_buttons; i++)
		{
			g_next_state.mouse.pressed[i] = false;
			g_next_state.mouse.released[i] = false;
		}
		g_next_state.mouse.wheel = Point::zero;

		for (int i = 0; i < Blah::Input::max_text_input; i++)
			g_next_state.keyboard.text[i] = 0;

		for (int i = 0; i < Blah::Input::max_controllers; i++)
		{
			ControllerState* controller = &(g_next_state.controllers[i]);

			if (!controller->is_connected)
				controller->name = nullptr;

			for (int j = 0; j < Blah::Input::max_controller_buttons; j++)
			{
				controller->pressed[j] = false;
				controller->released[j] = false;
			}
		}
	}
}

void InputBackend::on_mouse_move(float x, float y)
{
	g_next_state.mouse.position.x = x;
	g_next_state.mouse.position.y = y;

	Point size = Point(App::width(), App::height());
	Point draw = Point(App::draw_width(), App::draw_height());

	g_next_state.mouse.draw_position.x = (x / (float)size.x) * draw.x;
	g_next_state.mouse.draw_position.y = (y / (float)size.y) * draw.y;
}

void InputBackend::on_mouse_screen_move(float x, float y)
{
	g_next_state.mouse.screen_position.x = x;
	g_next_state.mouse.screen_position.y = y;
}

void InputBackend::on_mouse_down(MouseButton button)
{
	int i = (int)button;
	if (i >= 0 && i < Blah::Input::max_mouse_buttons)
	{
		g_next_state.mouse.down[i] = true;
		g_next_state.mouse.pressed[i] = true;
		g_next_state.mouse.timestamp[i] = Time::milliseconds;
	}
}

void InputBackend::on_mouse_up(MouseButton button)
{
	int i = (int)button;
	if (i >= 0 && i < Blah::Input::max_mouse_buttons)
	{
		g_next_state.mouse.down[i] = false;
		g_next_state.mouse.released[i] = true;
	}
}

void InputBackend::on_key_down(Key key)
{
	int i = (int)key;
	if (i >= 0 && i < Blah::Input::max_keyboard_keys)
	{
		g_next_state.keyboard.down[i] = true;
		g_next_state.keyboard.pressed[i] = true;
		g_next_state.keyboard.timestamp[i] = Time::milliseconds;
	}
}

void InputBackend::on_mouse_wheel(Point wheel)
{
	g_next_state.mouse.wheel = wheel;
}

void InputBackend::on_key_up(Key key)
{
	int i = (int)key;
	if (i >= 0 && i < Blah::Input::max_keyboard_keys)
	{
		g_next_state.keyboard.down[i] = false;
		g_next_state.keyboard.released[i] = true;
	}
}

void InputBackend::on_text_utf8(const char* text)
{
	strncat(g_next_state.keyboard.text, text, Blah::Input::max_text_input);
}

void InputBackend::on_controller_connect(int index, const char* name, int is_gamepad, int button_count, int axis_count, uint16_t vendor, uint16_t product, uint16_t version)
{
	if (index < Blah::Input::max_controllers)
	{
		ControllerState* controller = &(g_next_state.controllers[index]);
		*controller = g_empty_controller;
		controller->name = name;
		controller->is_connected = 1;
		controller->is_gamepad = is_gamepad;
		controller->button_count = button_count;
		controller->axis_count = axis_count;
		controller->vendor = vendor;
		controller->product = product;
		controller->version = version;
	}
}

void InputBackend::on_controller_disconnect(int index)
{
	if (index < Blah::Input::max_controllers)
		g_next_state.controllers[index] = g_empty_controller;
}

void InputBackend::on_button_down(int index, int button)
{
	if (index < Blah::Input::max_controllers &&
		button < Blah::Input::max_controller_buttons &&
		g_next_state.controllers[index].is_connected &&
		button < g_next_state.controllers[index].button_count)
	{
		g_next_state.controllers[index].down[button] = 1;
		g_next_state.controllers[index].pressed[button] = 1;
		g_next_state.controllers[index].button_timestamp[button] = Time::milliseconds;
	}
}

void InputBackend::on_button_up(int index, int button)
{
	if (index < Blah::Input::max_controllers &&
		button < Blah::Input::max_controller_buttons &&
		g_next_state.controllers[index].is_connected &&
		button < g_next_state.controllers[index].button_count)
	{
		g_next_state.controllers[index].down[button] = 0;
		g_next_state.controllers[index].released[button] = 1;
	}
}

void InputBackend::on_axis_move(int index, int axis, float value)
{
	if (index < Blah::Input::max_controllers &&
		axis < Blah::Input::max_controller_axis &&
		g_next_state.controllers[index].is_connected &&
		axis < g_next_state.controllers[index].axis_count)
	{
		g_next_state.controllers[index].axis[axis] = value;
		g_next_state.controllers[index].axis_timestamp[axis] = Time::milliseconds;
	}
}

const InputState* Input::state()
{
	return &g_curr_state;
}

const InputState* Input::last_state()
{
	return &g_last_state;
}

Vec2 Input::mouse()
{
	return g_curr_state.mouse.position;
}


Vec2 Input::mouse_draw()
{
	return Vec2(g_curr_state.mouse.draw_position);
}

Vec2 Input::mouse_screen()
{
	return Vec2(g_curr_state.mouse.screen_position);
}

bool Input::pressed(MouseButton button)
{
	int i = (int)button;
	return i >= 0 && i < Blah::Input::max_mouse_buttons&& g_curr_state.mouse.pressed[i];
}

bool Input::down(MouseButton button)
{
	int i = (int)button;
	return i >= 0 && i < Blah::Input::max_mouse_buttons&& g_curr_state.mouse.down[i];
}

bool Input::released(MouseButton button)
{
	int i = (int)button;
	return i >= 0 && i < Blah::Input::max_mouse_buttons&& g_curr_state.mouse.released[i];
}

Point Input::mouse_wheel()
{
	return g_curr_state.mouse.wheel;
}

bool Input::pressed(Key key)
{
	int i = (int)key;
	return i > 0 && i < Blah::Input::max_keyboard_keys&& g_curr_state.keyboard.pressed[i];
}

bool Input::down(Key key)
{
	int i = (int)key;
	return i > 0 && i < Blah::Input::max_keyboard_keys&& g_curr_state.keyboard.down[i];
}

bool Input::released(Key key)
{
	int i = (int)key;
	return i > 0 && i < Blah::Input::max_keyboard_keys&& g_curr_state.keyboard.released[i];
}

bool Input::ctrl()
{
	return down(Key::LeftControl) || down(Key::RightControl);
}

bool Input::shift()
{
	return down(Key::LeftShift) || down(Key::RightShift);
}

bool Input::alt()
{
	return down(Key::LeftAlt) || down(Key::RightAlt);
}

const char* Input::text()
{
	return g_curr_state.keyboard.text;
}

const ControllerState* Input::controller(int controllerIndex)
{
	if (controllerIndex >= Blah::Input::max_controllers)
	{
		Log::warn("Trying to access a out-of-range controller at %i", controllerIndex);
		return &g_empty_controller;
	}
	else if (!g_curr_state.controllers[controllerIndex].is_connected)
	{
		return &g_empty_controller;
	}
	else
	{
		return &g_curr_state.controllers[controllerIndex];
	}
}

bool Input::pressed(int controllerIndex, Button button)
{
	int i = (int)button;
	if (controllerIndex < Blah::Input::max_controllers && i >= 0 && i < Blah::Input::max_controller_buttons)
		return g_curr_state.controllers[controllerIndex].pressed[i];
	return false;
}

bool Input::down(int controllerIndex, Button button)
{
	int i = (int)button;
	if (controllerIndex < Blah::Input::max_controllers && i >= 0 && i < Blah::Input::max_controller_buttons)
		return g_curr_state.controllers[controllerIndex].down[i];
	return false;
}

bool Input::released(int controllerIndex, Button button)
{
	int i = (int)button;
	if (controllerIndex < Blah::Input::max_controllers && i >= 0 && i < Blah::Input::max_controller_buttons)
		return g_curr_state.controllers[controllerIndex].released[i];
	return false;
}

float Input::axis_check(int controllerIndex, Axis axis)
{
	int i = (int)axis;
	if (controllerIndex < Blah::Input::max_controllers && i >= 0 && i < Blah::Input::max_controller_axis)
		return g_curr_state.controllers[controllerIndex].axis[i];
	return 0;
}

int Input::axis_check(int prev, Key negative, Key positive)
{
	if (Input::pressed(positive))
		return 1;
	else if (Input::pressed(negative))
		return -1;
	else
	{
		bool pos = Input::down(positive);
		bool neg = Input::down(negative);

		if (pos && neg)
			return prev;
		else if (pos)
			return 1;
		else if (neg)
			return -1;
		else
			return 0;
	}
}

int Input::axis_check(int prev, int controllerIndex, Button negative, Button positive)
{
	if (Input::pressed(controllerIndex, positive))
		return 1;
	else if (Input::pressed(controllerIndex, negative))
		return -1;
	else
	{
		bool pos = Input::down(controllerIndex, positive);
		bool neg = Input::down(controllerIndex, negative);

		if (pos && neg)
			return prev;
		else if (pos)
			return 1;
		else if (neg)
			return -1;
		else
			return 0;
	}
}

const char* Input::name_of(Key key)
{
	switch (key)
	{
		#define DEFINE_KEY(name, value) case Key::name: return #name;
		BLAH_KEY_DEFINITIONS
		#undef DEFINE_KEY
	}

	return "Unknown";
}

const char* Input::name_of(Button button)
{
	switch (button)
	{
		#define DEFINE_BTN(name, value) case Button::name: return #name;
		BLAH_BUTTON_DEFINITIONS
		#undef DEFINE_BTN
	}

	return "Unknown";
}