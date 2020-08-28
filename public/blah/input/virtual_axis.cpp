#pragma once
#include <blah/input/virtual_axis.h>
#include <blah/time.h>
#include <blah/log.h>

using namespace Blah;

VirtualAxis& VirtualAxis::add_keys(Key negative, Key positive)
{
	if (m_axes_len >= Input::max_virtual_nodes)
		BLAH_ERROR("VirtualAxis Keys out of bounds!");
	else
	{
		m_keys[m_keys_len].init(negative, positive);
		m_keys_len++;
	}

	return *this;
}

VirtualAxis& VirtualAxis::add_buttons(int gamepad_id, Button negative, Button positive)
{
	if (m_axes_len >= Input::max_virtual_nodes)
		BLAH_ERROR("VirtualAxis Buttons out of bounds!");
	else
	{
		m_buttons[m_buttons_len].init(gamepad_id, negative, positive);
		m_buttons_len++;
	}

	return *this;
}

VirtualAxis& VirtualAxis::add_axis(int gamepad_id, Axis axis, float deadzone)
{
	if (m_axes_len >= Input::max_virtual_nodes)
		BLAH_ERROR("VirtualAxis Axes out of bounds!");
	else
	{
		m_axes[m_axes_len].init(gamepad_id, axis, deadzone);
		m_axes_len++;
	}

	return *this;
}

VirtualAxis& VirtualAxis::repeat(float m_repeat_delay, float m_repeat_interval)
{
	this->m_repeat_delay = m_repeat_delay;
	this->m_repeat_interval = m_repeat_interval;
	return *this;
}

VirtualAxis& VirtualAxis::press_buffer(float duration)
{
	this->m_press_buffer = duration;
	return *this;
}

VirtualAxis& VirtualAxis::release_buffer(float duration)
{
	this->m_release_buffer = duration;
	return *this;
}

void VirtualAxis::update()
{
	m_last_value = m_value;
	m_value = 0;

	for (int i = 0; i < m_keys_len; i++)
	{
		m_keys[i].update();
		if (m_value == 0)
			m_value = (float)m_keys[i].value;
	}

	for (int i = 0; i < m_buttons_len; i++)
	{
		m_buttons[i].update();
		if (m_value == 0)
			m_value = (float)m_buttons[i].value;
	}

	for (int i = 0; i < m_axes_len; i++)
	{
		m_axes[i].update();
		if (m_value == 0)
			m_value = m_axes[i].value;
	}

	//Valuei
	m_last_value_i = m_value_i;
	if (m_value > 0)
		m_value_i = 1;
	else if (m_value < 0)
		m_value_i = -1;
	else
		m_value_i = 0;

	//pressed?
	m_pressed = false;
	if (m_value_i != 0 && m_last_value_i != m_value_i)
	{
		m_pressed = true;
		m_last_press_time = m_repeat_press_time = Time::elapsed;
	}
	else if (m_value_i == m_last_value_i && m_value_i != 0)
	{
		if (Time::elapsed - m_last_press_time <= m_press_buffer)
			m_pressed = true;
		else if (m_repeat_interval > 0 && Time::elapsed >= m_repeat_press_time + m_repeat_delay)
		{
			int prev = (int)((Time::previous_elapsed - m_repeat_press_time - m_repeat_delay) / m_repeat_interval);
			int cur = (int)((Time::elapsed - m_repeat_press_time - m_repeat_delay) / m_repeat_interval);
			m_pressed = prev < cur;
		}
	}

	//released?
	if (m_last_value_i != 0 && m_value_i != m_last_value_i)
	{
		m_released = true;
		m_last_release_time = Time::elapsed;
	}
	else if (Time::elapsed - m_last_release_time <= m_release_buffer)
		m_released = true;
	else
		m_released = false;
}

void VirtualAxis::KeysNode::init(Key negative, Key positive)
{
	this->negative = negative;
	this->positive = positive;
}

void VirtualAxis::KeysNode::update()
{
	value = Input::axis_check(value, negative, positive);
}

void VirtualAxis::ButtonsNode::init(int gamepad_id, Button negative, Button positive)
{
	this->gamepad_id = gamepad_id;
	this->negative = negative;
	this->positive = positive;
}

void VirtualAxis::ButtonsNode::update()
{
	value = Input::axis_check(value, gamepad_id, negative, positive);
}

void VirtualAxis::AxisNode::init(int gamepad_id, Axis axis, float deadzone)
{
	this->gamepad_id = gamepad_id;
	this->axis = axis;
	this->deadzone = deadzone;
}

void VirtualAxis::AxisNode::update()
{
	value = Input::axis_check(gamepad_id, axis);
	if (value < deadzone && value > -deadzone)
		value = 0;
}