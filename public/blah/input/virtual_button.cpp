#include <blah/input/virtual_button.h>
#include <blah/time.h>
#include <blah/log.h>

using namespace Blah;

VirtualButton& VirtualButton::add_key(Key key)
{
	if (m_keys_len >= Input::max_virtual_nodes)
		BLAH_ERROR("VirtualButton Keys out of bounds!");
	else
	{
		m_keys[m_keys_len].init(key);
		m_keys_len++;
	}

	return *this;
}

VirtualButton& VirtualButton::add_button(int gamepad_id, Button button)
{
	if (m_buttons_len >= Input::max_virtual_nodes)
		BLAH_ERROR("VirtualButton Buttons out of bounds!");
	else
	{
		m_buttons[m_buttons_len].init(gamepad_id, button);
		m_buttons_len++;
	}

	return *this;
}

VirtualButton& VirtualButton::add_axis(int gamepad_id, Axis axis, float threshold, bool greater_than)
{
	if (m_axes_len >= Input::max_virtual_nodes)
		BLAH_ERROR("VirtualButton Axes out of bounds!");
	else
	{
		m_axes[m_axes_len].init(gamepad_id, axis, threshold, greater_than);
		m_axes_len++;
	}

	return *this;
}

VirtualButton& VirtualButton::repeat(float m_repeat_delay, float m_repeat_interval)
{
	this->m_repeat_delay = m_repeat_delay;
	this->m_repeat_interval = m_repeat_interval;
	return *this;
}

VirtualButton& VirtualButton::press_buffer(float duration)
{
	this->m_press_buffer = duration;
	return *this;
}

VirtualButton& VirtualButton::release_buffer(float duration)
{
	this->m_release_buffer = duration;
	return *this;
}

void VirtualButton::update()
{
	m_down = false;
	m_pressed = false;
	m_released = false;

	//Keys
	for (int i = 0; i < m_keys_len; i++)
	{
		m_keys[i].update();

		m_down = m_down || m_keys[i].down;
		m_pressed = m_pressed || m_keys[i].pressed;
		m_released = m_released || m_keys[i].released;
	}

	//Buttons
	for (int i = 0; i < m_buttons_len; i++)
	{
		m_buttons[i].update();

		m_down = m_down || m_buttons[i].down;
		m_pressed = m_pressed || m_buttons[i].pressed;
		m_released = m_released || m_buttons[i].released;
	}

	//Axes
	for (int i = 0; i < m_axes_len; i++)
	{
		m_axes[i].update();

		m_down = m_down || m_axes[i].down;
		m_pressed = m_pressed || m_axes[i].pressed;
		m_released = m_released || m_axes[i].released;
	}

	//pressed?
	if (m_pressed)
	{
		m_repeat_press_time = m_last_press_time = Time::elapsed;
	}
	else if (Time::elapsed - m_last_press_time <= m_press_buffer)
	{
		m_pressed = true;
	}
	else if (m_down && m_repeat_interval > 0 && Time::elapsed >= m_repeat_press_time + m_repeat_delay)
	{
		int prev = (int)((Time::previous_elapsed - m_repeat_press_time - m_repeat_delay) / m_repeat_interval);
		int cur = (int)((Time::elapsed - m_repeat_press_time - m_repeat_delay) / m_repeat_interval);
		m_pressed = prev < cur;
	}

	//released?
	if (m_released)
		m_last_release_time = Time::elapsed;
	else
		m_released = Time::elapsed - m_last_release_time <= m_release_buffer;
}

void VirtualButton::KeyNode::init(Key key)
{
	this->key = key;
}

void VirtualButton::KeyNode::update()
{
	down = Input::down(key);
	pressed = Input::pressed(key);
	released = Input::released(key);
}

void VirtualButton::ButtonNode::init(int gamepad_id, Button button)
{
	this->gamepad_id = gamepad_id;
	this->button = button;
}

void VirtualButton::ButtonNode::update()
{
	down = Input::down(gamepad_id, button);
	pressed = Input::pressed(gamepad_id, button);
	released = Input::released(gamepad_id, button);
}

void VirtualButton::AxisNode::init(int gamepad_id, Axis axis, float threshold, bool greater_than)
{
	this->gamepad_id = gamepad_id;
	this->axis = axis;
	this->threshold = threshold;
	this->greater_than = greater_than;
}

void VirtualButton::AxisNode::update()
{
	float curr = Input::state()->controllers[gamepad_id].axis[(int)axis];
	float prev = Input::last_state()->controllers[gamepad_id].axis[(int)axis];

	if (greater_than)
	{
		down = curr >= threshold;
		pressed = down && prev < threshold;
		released = !down && prev >= threshold;
	}
	else
	{
		down = curr <= threshold;
		pressed = down && prev > threshold;
		released = !down && prev <= threshold;
	}
}