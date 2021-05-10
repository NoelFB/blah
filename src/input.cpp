#include <blah/input.h>
#include <blah/app.h>
#include <blah/time.h>
#include <blah/common.h>
#include <blah/numerics/point.h>
#include <blah/numerics/calc.h>
#include "internal/input_backend.h"
#include <cstring>

using namespace Blah;

namespace
{
	InputState g_last_state;
	InputState g_curr_state;
	InputState g_next_state;
	InputState g_empty_state;
	ControllerState g_empty_controller;
	Vector<WeakRef<InputBinding>> g_bindings;
	Vector<WeakRef<AxisBinding>> g_axes;
	Vector<WeakRef<StickBinding>> g_sticks;
}

void InputBackend::init()
{
	g_empty_controller.name = "Disconnected";
	for (int i = 0; i < Blah::Input::max_controllers; i++)
		g_empty_state.controllers[i].name = g_empty_controller.name;

	g_last_state = g_empty_state;
	g_curr_state = g_empty_state;
	g_next_state = g_empty_state;
	g_bindings.dispose();
	g_axes.dispose();
	g_sticks.dispose();
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
		g_next_state.keyboard.text.clear();

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

	// update bindings

	for (int i = 0; i < g_bindings.size(); i++)
	{
		if (g_bindings[i].use_count() <= 0)
		{
			g_bindings.erase(i);
			i--;
		}
		else if (auto binding = g_bindings[i].lock())
		{
			binding->update();
		}
	}

	for (int i = 0; i < g_axes.size(); i++)
	{
		if (g_axes[i].use_count() <= 0)
		{
			g_axes.erase(i);
			i--;
		}
		else if (auto binding = g_axes[i].lock())
		{
			binding->update();
		}
	}

	for (int i = 0; i < g_sticks.size(); i++)
	{
		if (g_sticks[i].use_count() <= 0)
		{
			g_sticks.erase(i);
			i--;
		}
		else if (auto binding = g_sticks[i].lock())
		{
			binding->update();
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
		g_next_state.mouse.timestamp[i] = Time::ticks;
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
		g_next_state.keyboard.timestamp[i] = Time::ticks;
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
	g_next_state.keyboard.text += text;
}

void InputBackend::on_controller_connect(int index, const char* name, int is_gamepad, int button_count, int axis_count, u16 vendor, u16 product, u16 version)
{
	if (index < Blah::Input::max_controllers)
	{
		ControllerState* controller = &(g_next_state.controllers[index]);
		*controller = g_empty_controller;
		controller->name = name;
		controller->is_connected = true;
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
		g_next_state.controllers[index].down[button] = true;
		g_next_state.controllers[index].pressed[button] = true;
		g_next_state.controllers[index].button_timestamp[button] = Time::ticks;
	}
}

void InputBackend::on_button_up(int index, int button)
{
	if (index < Blah::Input::max_controllers &&
		button < Blah::Input::max_controller_buttons &&
		g_next_state.controllers[index].is_connected &&
		button < g_next_state.controllers[index].button_count)
	{
		g_next_state.controllers[index].down[button] = false;
		g_next_state.controllers[index].released[button] = true;
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
		g_next_state.controllers[index].axis_timestamp[axis] = Time::ticks;
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

int Input::axis_check(int fallback, Key negative, Key positive)
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
			return fallback;
		else if (pos)
			return 1;
		else if (neg)
			return -1;
		else
			return 0;
	}
}

int Input::axis_check(int fallback, int controllerIndex, Button negative, Button positive)
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
			return fallback;
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

InputBindingRef Input::register_binding(const InputBinding& binding)
{
	auto result = std::make_shared<InputBinding>(binding);
	g_bindings.push_back(WeakRef<InputBinding>(result));
	return result;
}

AxisBindingRef Input::register_binding(const AxisBinding& binding)
{
	auto result = std::make_shared<AxisBinding>(binding);
	g_axes.push_back(WeakRef<AxisBinding>(result));
	return result;
}

StickBindingRef Input::register_binding(const StickBinding& binding)
{
	auto result = std::make_shared<StickBinding>(binding);
	g_sticks.push_back(WeakRef<StickBinding>(result));
	return result;
}


InputBinding::TriggerBind::TriggerBind(Axis axis)
	: axis(axis)
{

}

InputBinding::TriggerBind::TriggerBind(int controller, Axis axis, float threshold, bool positive)
	: controller(controller), axis(axis), threshold(threshold), positive(positive)
{

}

bool InputBinding::TriggerBind::is_down(float axis_value) const
{
	if ((axis_value > 0 && positive) || (axis_value < 0 && !positive))
	{
		if (Calc::abs(axis_value) >= threshold)
			return true;
	}

	return false;
}

InputBinding::ButtonBind::ButtonBind(Button button)
	: button(button) {}

InputBinding::ButtonBind::ButtonBind(int controller, Button button)
	: controller(controller), button(button) {}

bool InputBinding::pressed() const
{
	if (m_press_consumed)
		return false;

	if (m_last_press_time >= 0 && (Time::seconds - m_last_press_time) <= press_buffer)
		return true;

	return m_pressed;
}

bool InputBinding::released() const
{
	if (m_release_consumed)
		return false;

	if (m_last_release_time >= 0 && (Time::seconds - m_last_release_time) <= release_buffer)
		return true;

	return m_released;
}

bool InputBinding::down() const
{
	return m_down;
}

float InputBinding::value() const
{
	return m_value;
}

int InputBinding::sign() const
{
	return (int)Calc::sign(m_value);
}

double InputBinding::timestamp() const
{
	return m_last_timestamp;
}

void InputBinding::update()
{
	m_press_consumed = false;
	m_release_consumed = false;

	if (get_pressed())
	{
		m_last_timestamp = Time::seconds;
		m_last_press_time = Time::seconds;
		m_pressed = true;
	}
	else
	{
		m_pressed = false;
	}

	if (get_released())
	{
		m_last_release_time = Time::seconds;
		m_released = true;
	}
	else
	{
		m_released = false;
	}

	m_down = get_down();
	m_value = get_value();
}

void InputBinding::consume_press()
{
	m_press_consumed = true;
	m_last_press_time = -1;
}

void InputBinding::consume_release()
{
	m_release_consumed = true;
	m_last_release_time = -1;
}

InputBinding& InputBinding::add(Key key)
{
	keys.push_back(key);
	return *this;
}

InputBinding& InputBinding::add(ButtonBind button)
{
	buttons.push_back(button);
	return *this;
}

InputBinding& InputBinding::add(TriggerBind trigger)
{
	triggers.push_back(trigger);
	return *this;
}

InputBinding& InputBinding::add(MouseButton button)
{
	mouse.push_back(button);
	return *this;
}

InputBinding& InputBinding::add_left_trigger(int controller, float threshold)
{
	triggers.push_back(TriggerBind(controller, Axis::LeftTrigger, threshold, true));
	return *this;
}

InputBinding& InputBinding::add_right_trigger(int controller, float threshold)
{
	triggers.push_back(TriggerBind(controller, Axis::RightTrigger, threshold, true));
	return *this;
}

InputBinding& InputBinding::set_controller(int index)
{
	for (auto& it : buttons)
		it.controller = index;
	for (auto& it : triggers)
		it.controller = index;

	return *this;
}

void InputBinding::clear()
{
	keys.clear();
	buttons.clear();
	triggers.clear();
	mouse.clear();
}

bool InputBinding::get_pressed() const
{
	for (auto& it : keys)
		if (Input::pressed(it))
			return true;

	for (auto& it : mouse)
		if (Input::pressed(it))
			return true;

	for (auto& it : buttons)
		if (Input::pressed(it.controller, it.button))
			return true;

	for (auto& it : triggers)
	{
		if (it.controller < 0 || it.controller >= Input::max_controllers)
			continue;

		if ((int)it.axis < 0 || (int)it.axis >= Input::max_controller_axis)
			continue;

		if (it.is_down(Input::state()->controllers[it.controller].axis[(int)it.axis]) &&
			!it.is_down(Input::last_state()->controllers[it.controller].axis[(int)it.axis]))
			return true;
	}

	return false;
}

bool InputBinding::get_released() const
{
	for (auto& it : keys)
		if (Input::released(it))
			return true;

	for (auto& it : mouse)
		if (Input::released(it))
			return true;

	for (auto& it : buttons)
		if (Input::released(it.controller, it.button))
			return true;

	for (auto& it : triggers)
	{
		if (it.controller < 0 || it.controller >= Input::max_controllers)
			continue;

		if ((int)it.axis < 0 || (int)it.axis >= Input::max_controller_axis)
			continue;

		if (!it.is_down(Input::state()->controllers[it.controller].axis[(int)it.axis]) &&
			it.is_down(Input::last_state()->controllers[it.controller].axis[(int)it.axis]))
			return true;
	}

	return false;
}

bool InputBinding::get_down() const
{
	for (auto& it : keys)
		if (Input::down(it))
			return true;

	for (auto& it : mouse)
		if (Input::down(it))
			return true;

	for (auto& it : buttons)
		if (Input::down(it.controller, it.button))
			return true;

	for (auto& it : triggers)
	{
		if (it.controller < 0 || it.controller >= Input::max_controllers)
			continue;

		if ((int)it.axis < 0 || (int)it.axis >= Input::max_controller_axis)
			continue;

		if (it.is_down(Input::state()->controllers[it.controller].axis[(int)it.axis]))
			return true;
	}

	return false;
}

float InputBinding::get_value() const
{
	for (auto& it : keys)
		if (Input::down(it))
			return 1.0f;

	for (auto& it : mouse)
		if (Input::down(it))
			return 1.0f;

	for (auto& it : buttons)
		if (Input::down(it.controller, it.button))
			return 1.0f;

	float highest = 0;

	for (auto& it : triggers)
	{
		if (it.controller < 0 || it.controller >= Input::max_controllers)
			continue;

		if ((int)it.axis < 0 || (int)it.axis >= Input::max_controller_axis)
			continue;

		float raw_value = Input::state()->controllers[it.controller].axis[(int)it.axis];

		if (it.is_down(raw_value))
		{
			float mapped_value = Calc::clamped_map(Calc::abs(raw_value), it.threshold, 1.0f, 0.0f, 1.0f);
			if (mapped_value > highest)
				highest = mapped_value;
		}
	}

	return highest;
}

float AxisBinding::value() const
{
	float neg = negative.value();
	float pos = positive.value();

	// neither are down
	if (neg <= 0 && pos <= 0)
		return 0;

	// negative-only is down
	if (neg > 0 && pos <= 0)
		return -neg;

	// positive-only is down
	if (pos > 0 && neg <= 0)
		return pos;

	// both are down:

	// overlap cancel out
	if (overlap == Overlap::Cancel)
		return 0;

	// overlap takes older
	if (overlap == Overlap::Older)
	{
		if (negative.timestamp() < positive.timestamp())
			return -neg;
		else
			return pos;
	}

	// overlap takes newer
	if (negative.timestamp() > positive.timestamp())
		return -neg;
	else
		return pos;
}

int AxisBinding::sign() const
{
	return (int)Calc::sign(value());
}

void AxisBinding::update()
{
	negative.update();
	positive.update();
}

void AxisBinding::consume_press()
{
	negative.consume_press();
	positive.consume_press();
}

void AxisBinding::consume_release()
{
	negative.consume_release();
	positive.consume_release();
}

AxisBinding& AxisBinding::add_left_stick_x(int controller, float threshold)
{
	negative.add(InputBinding::TriggerBind(controller, Axis::LeftX, threshold, false));
	positive.add(InputBinding::TriggerBind(controller, Axis::LeftX, threshold, true));
	return *this;
}

AxisBinding& AxisBinding::add_left_stick_y(int controller, float threshold)
{
	negative.add(InputBinding::TriggerBind(controller, Axis::LeftY, threshold, false));
	positive.add(InputBinding::TriggerBind(controller, Axis::LeftY, threshold, true));
	return *this;
}

AxisBinding& AxisBinding::add_right_stick_x(int controller, float threshold)
{
	negative.add(InputBinding::TriggerBind(controller, Axis::RightX, threshold, false));
	positive.add(InputBinding::TriggerBind(controller, Axis::RightX, threshold, true));
	return *this;
}

AxisBinding& AxisBinding::add_right_stick_y(int controller, float threshold)
{
	negative.add(InputBinding::TriggerBind(controller, Axis::RightY, threshold, false));
	positive.add(InputBinding::TriggerBind(controller, Axis::RightY, threshold, true));
	return *this;
}

AxisBinding& AxisBinding::set_controller(int index)
{
	negative.set_controller(index);
	positive.set_controller(index);
	return *this;
}

void AxisBinding::clear()
{
	negative.clear();
	positive.clear();
}

Vec2 StickBinding::value() const
{
	Vec2 result = Vec2(x.value(), y.value());
	if (round_threshold > 0 && result.length() < round_threshold)
		return Vec2::zero;
	return result;
}

Point StickBinding::sign() const
{
	Vec2 result = value();
	return Point((int)Calc::sign(result.x), (int)Calc::sign(result.y));
}

void StickBinding::update()
{
	x.update();
	y.update();
}

void StickBinding::consume_press()
{
	x.consume_press();
	y.consume_press();
}

void StickBinding::consume_release()
{
	x.consume_release();
	y.consume_release();
}

StickBinding& StickBinding::add_dpad(int controller)
{
	x.negative.add(InputBinding::ButtonBind(controller, Button::Left));
	x.positive.add(InputBinding::ButtonBind(controller, Button::Right));
	y.negative.add(InputBinding::ButtonBind(controller, Button::Up));
	y.positive.add(InputBinding::ButtonBind(controller, Button::Down));
	return *this;
}

StickBinding& StickBinding::add_left_stick(int controller, float threshold)
{
	x.add_left_stick_x(controller, threshold);
	y.add_left_stick_y(controller, threshold);
	return *this;
}

StickBinding& StickBinding::add_right_stick(int controller, float threshold)
{
	x.add_right_stick_x(controller, threshold);
	y.add_right_stick_y(controller, threshold);
	return *this;
}

StickBinding& StickBinding::set_controller(int index)
{
	x.set_controller(index);
	y.set_controller(index);
	return *this;
}

void StickBinding::clear()
{
	x.clear();
	y.clear();
}

