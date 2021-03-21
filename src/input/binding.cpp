#include <blah/input/binding.h>
#include <blah/math/calc.h>
#include <blah/core/time.h>

using namespace Blah;

BoundTrigger::BoundTrigger(Axis axis)
	: axis(axis)
{

}

BoundTrigger::BoundTrigger(int controller, Axis axis, float threshold, bool positive)
	: controller(controller), axis(axis), threshold(threshold), positive(positive)
{

}

bool BoundTrigger::is_down(float axis_value) const
{
	if ((axis_value > 0 && positive) || (axis_value < 0 && !positive))
	{
		if (Calc::abs(axis_value) >= threshold)
			return true;
	}

	return false;
}

BoundTrigger BoundTrigger::left_stick_left(int controller, float threshold)
{
	return BoundTrigger(controller, Axis::LeftX, threshold, false);
}

BoundTrigger BoundTrigger::left_stick_right(int controller, float threshold)
{
	return BoundTrigger(controller, Axis::LeftX, threshold, true);
}

BoundTrigger BoundTrigger::left_stick_up(int controller, float threshold)
{
	return BoundTrigger(controller, Axis::LeftY, threshold, false);
}

BoundTrigger BoundTrigger::left_stick_down(int controller, float threshold)
{
	return BoundTrigger(controller, Axis::LeftY, threshold, true);
}

BoundTrigger BoundTrigger::right_stick_left(int controller, float threshold)
{
	return BoundTrigger(controller, Axis::RightX, threshold, false);
}

BoundTrigger BoundTrigger::right_stick_right(int controller, float threshold)
{
	return BoundTrigger(controller, Axis::RightX, threshold, true);
}

BoundTrigger BoundTrigger::right_stick_up(int controller, float threshold)
{
	return BoundTrigger(controller, Axis::RightY, threshold, false);
}

BoundTrigger BoundTrigger::right_stick_down(int controller, float threshold)
{
	return BoundTrigger(controller, Axis::RightY, threshold, true);
}

BoundTrigger BoundTrigger::left_trigger(int controller, float threshold)
{
	return BoundTrigger(controller, Axis::LeftTrigger, threshold, true);
}

BoundTrigger BoundTrigger::right_trigger(int controller, float threshold)
{
	return BoundTrigger(controller, Axis::RightTrigger, threshold, true);
}

BoundButton::BoundButton(Button button)
	: button(button) {}

BoundButton::BoundButton(int controller, Button button)
	: controller(controller), button(button) {}

bool Binding::pressed() const
{
	if (m_press_consumed)
		return false;

	if (m_last_press_time >= 0 && (Time::seconds - m_last_press_time) <= press_buffer)
		return true;

	return m_pressed;
}

bool Binding::released() const
{
	if (m_release_consumed)
		return false;

	if (m_last_release_time >= 0 && (Time::seconds - m_last_release_time) <= release_buffer)
		return true;

	return m_released;
}

bool Binding::down() const
{
	return m_down;
}

float Binding::value() const
{
	return m_value;
}

int Binding::sign() const
{
	return (int)Calc::sign(m_value);
}

double Binding::timestamp() const
{
	return m_last_timestamp;
}

void Binding::update()
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

void Binding::consume_press()
{
	m_press_consumed = true;
	m_last_press_time = -1;
}

void Binding::consume_release()
{
	m_release_consumed = true;
	m_last_release_time = -1;
}

void Binding::add(Key key)
{
	keys.push_back(key);
}

void Binding::add(BoundButton button)
{
	buttons.push_back(button);
}

void Binding::add(BoundTrigger trigger)
{
	triggers.push_back(trigger);
}

void Binding::add(MouseButton button)
{
	mouse.push_back(button);
}

void Binding::set_controller(int index)
{
	for (auto& it : buttons)
		it.controller = index;
	for (auto& it : triggers)
		it.controller = index;
}

void Binding::clear()
{
	keys.clear();
	buttons.clear();
	triggers.clear();
	mouse.clear();
}

bool Binding::get_pressed() const
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

bool Binding::get_released() const
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

bool Binding::get_down() const
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

float Binding::get_value() const
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

void AxisBinding::add_left_stick_x(int controller, float threshold)
{
	negative.add(BoundTrigger::left_stick_left(controller, threshold));
	positive.add(BoundTrigger::left_stick_right(controller, threshold));
}

void AxisBinding::add_left_stick_y(int controller, float threshold)
{
	negative.add(BoundTrigger::left_stick_up(controller, threshold));
	positive.add(BoundTrigger::left_stick_down(controller, threshold));
}

void AxisBinding::add_right_stick_x(int controller, float threshold)
{
	negative.add(BoundTrigger::right_stick_left(controller, threshold));
	positive.add(BoundTrigger::right_stick_right(controller, threshold));
}

void AxisBinding::add_right_stick_y(int controller, float threshold)
{
	negative.add(BoundTrigger::right_stick_up(controller, threshold));
	positive.add(BoundTrigger::right_stick_down(controller, threshold));
}

void AxisBinding::set_controller(int index)
{
	negative.set_controller(index);
	positive.set_controller(index);
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

void StickBinding::add_dpad(int controller)
{
	x.negative.add(BoundButton(controller, Button::Left));
	x.positive.add(BoundButton(controller, Button::Right));
	y.negative.add(BoundButton(controller, Button::Up));
	y.positive.add(BoundButton(controller, Button::Down));
}

void StickBinding::add_left_stick(int controller, float threshold)
{
	x.add_left_stick_x(controller, threshold);
	y.add_left_stick_y(controller, threshold);
}

void StickBinding::add_right_stick(int controller, float threshold)
{
	x.add_right_stick_x(controller, threshold);
	y.add_right_stick_y(controller, threshold);
}

void StickBinding::set_controller(int index)
{
	x.set_controller(index);
	y.set_controller(index);
}

void StickBinding::clear()
{
	x.clear();
	y.clear();
}