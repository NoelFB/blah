#include <blah/input/virtual_stick.h>
#include <blah/core/time.h>
#include <blah/core/common.h>

using namespace Blah;

VirtualStick::VirtualStick()
{
	this->m_i_deadzone = 0;
}

VirtualStick::VirtualStick(float iDeadzone)
{
	this->m_i_deadzone = iDeadzone;
}

VirtualStick& VirtualStick::add_keys(Key left, Key right, Key up, Key down)
{
	if (m_keys_len >= Input::max_virtual_nodes)
		BLAH_ERROR("VirtualStick Keys out of bounds!");
	else
	{
		m_keys[m_keys_len].init(left, right, up, down);
		m_keys_len++;
	}

	return *this;
}

VirtualStick& VirtualStick::add_buttons(int gamepad_id, Button left, Button right, Button up, Button down)
{
	if (m_buttons_len >= Input::max_virtual_nodes)
		BLAH_ERROR("VirtualStick Buttons out of bounds!");
	else
	{
		m_buttons[m_buttons_len].init(gamepad_id, left, right, up, down);
		m_buttons_len++;
	}

	return *this;
}

VirtualStick& VirtualStick::add_axes(int gamepad_id, Axis horizontal, Axis vertical, float deadzone)
{
	if (m_axes_len >= Input::max_virtual_nodes)
		BLAH_ERROR("VirtualStick Axes out of bounds!");
	else
	{
		m_axes[m_axes_len].init(gamepad_id, horizontal, vertical, deadzone);
		m_axes_len++;
	}

	return *this;
}

VirtualStick& VirtualStick::repeat(float repeat_delay, float repeat_interval)
{
	this->m_repeat_delay = repeat_delay;
	this->m_repeat_interval = repeat_interval;
	return *this;
}

VirtualStick& VirtualStick::press_buffer(float duration)
{
	m_press_buffer = duration;
	return *this;
}

VirtualStick& VirtualStick::release_buffer(float duration)
{
	m_release_buffer = duration;
	return *this;
}

void VirtualStick::update()
{
	m_last_value = m_value;
	m_value = Vec2::zero;

	for (int i = 0; i < m_keys_len; i++)
	{
		m_keys[i].update();
		if (m_value == Vec2::zero)
			m_value = m_keys[i].value;
	}

	for (int i = 0; i < m_buttons_len; i++)
	{
		m_buttons[i].update();
		if (m_value == Vec2::zero)
			m_value = m_buttons[i].value;
	}

	for (int i = 0; i < m_axes_len; i++)
	{
		m_axes[i].update();
		if (m_value == Vec2::zero)
			m_value = m_axes[i].value;
	}

	//Valuei
	m_last_value_i = m_value_i;
	if (m_value.x > m_i_deadzone)
		m_value_i.x = 1;
	else if (m_value.x < -m_i_deadzone)
		m_value_i.x = -1;
	else
		m_value_i.x = 0;
	if (m_value.y > m_i_deadzone)
		m_value_i.y = 1;
	else if (m_value.y < -m_i_deadzone)
		m_value_i.y = -1;
	else
		m_value_i.y = 0;

	//pressed?
	m_pressed = false;
	if (m_value_i != Point::zero && m_last_value_i != m_value_i)
	{
		m_pressed = true;
		m_last_press_time = m_repeat_press_time = Time::seconds;
	}
	else if (m_value_i == m_last_value_i && m_value_i != Point::zero)
	{
		if (Time::seconds - m_last_press_time <= m_press_buffer)
			m_pressed = true;
		else if (m_repeat_interval > 0 && Time::seconds >= m_repeat_press_time + m_repeat_delay)
		{
			int prev = (int)((Time::previous_seconds - m_repeat_press_time - m_repeat_delay) / m_repeat_interval);
			int cur = (int)((Time::seconds - m_repeat_press_time - m_repeat_delay) / m_repeat_interval);
			m_pressed = prev < cur;
		}
	}

	//released?
	if (m_last_value_i != Point::zero && m_value_i != m_last_value_i)
	{
		m_released = true;
		m_last_release_time = Time::seconds;
	}
	else if (Time::seconds - m_last_release_time <= m_release_buffer)
		m_released = true;
	else
		m_released = false;
}

void VirtualStick::KeysNode::init(Key left, Key right, Key up, Key down)
{
	this->left = left;
	this->right = right;
	this->up = up;
	this->down = down;
}

void VirtualStick::KeysNode::update()
{
	value.x = Input::axis_check(value.x, left, right);
	value.y = Input::axis_check(value.y, up, down);
}

void VirtualStick::ButtonsNode::init(int gamepad_id, Button left, Button right, Button up, Button down)
{
	this->gamepad_id = gamepad_id;
	this->left = left;
	this->right = right;
	this->up = up;
	this->down = down;
}

void VirtualStick::ButtonsNode::update()
{
	value.x = Input::axis_check(value.x, gamepad_id, left, right);
	value.y = Input::axis_check(value.y, gamepad_id, up, down);
}

void VirtualStick::AxesNode::init(int gamepad_id, Axis horizontal, Axis vertical, float deadzone)
{
	this->gamepad_id = gamepad_id;
	this->horizontal = horizontal;
	this->vertical = vertical;
	this->deadzone = deadzone;
}

void VirtualStick::AxesNode::update()
{
	value.x = Input::axis_check(gamepad_id, horizontal);
	value.y = Input::axis_check(gamepad_id, vertical);

	if (value.length() < deadzone)
		value = Vec2::zero;
}