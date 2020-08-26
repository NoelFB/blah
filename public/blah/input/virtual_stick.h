#pragma once
#include <blah/input/input.h>
#include <blah/math/vec2.h>
#include <blah/math/point.h>

namespace Blah
{
	class VirtualStick
	{
	private:
		struct KeysNode
		{
			Key left;
			Key right;
			Key up;
			Key down;

			Point value;

			void init(Key left, Key right, Key up, Key down);
			void update();
		};

		struct ButtonsNode
		{
			int gamepad_id;
			Button left;
			Button right;
			Button up;
			Button down;

			Point value;

			void init(int gamepad_id, Button left, Button right, Button up, Button down);
			void update();
		};

		struct AxesNode
		{
			int gamepad_id;
			Axis horizontal;
			Axis vertical;
			float deadzone;

			Vec2 value;

			void init(int gamepad_id, Axis horizontal, Axis vertical, float deadzone);
			void update();
		};

		KeysNode m_keys[BLAH_MAX_VIRTUAL_NODES];
		ButtonsNode m_buttons[BLAH_MAX_VIRTUAL_NODES];
		AxesNode m_axes[BLAH_MAX_VIRTUAL_NODES];
		int m_keys_len = 0;
		int m_buttons_len = 0;
		int m_axes_len = 0;

		float m_press_buffer = 0;
		float m_release_buffer = 0;
		float m_repeat_delay = 0;
		float m_repeat_interval = 0;

		Vec2 m_value = Vec2();
		Point m_value_i = Point();
		Vec2 m_last_value = Vec2();
		Point m_last_value_i = Point();
		bool m_pressed = false;
		bool m_released = false;

		float m_i_deadzone;
		float m_last_press_time = 0;
		float m_last_release_time = 0;
		float m_repeat_press_time = 0;

	public:
		VirtualStick();
		VirtualStick(float iDeadzone);
		VirtualStick& add_keys(Key left, Key right, Key up, Key down);
		VirtualStick& add_buttons(int gamepad_id, Button left, Button right, Button up, Button down);
		VirtualStick& add_axes(int gamepad_id, Axis horizontal, Axis vertical, float deadzone);
		VirtualStick& repeat(float m_repeat_delay, float m_repeat_interval);
		VirtualStick& press_buffer(float duration);
		VirtualStick& release_buffer(float duration);

		void update();
		const Vec2& value() const { return m_value; }
		const Point& value_i() const { return m_value_i; }
		const Vec2& last_value() const { return m_last_value; }
		const Point& last_value_i() const { return m_last_value_i; }
		bool pressed() const { return m_pressed; }
		bool released() const { return m_released; }
		void clear_press_buffer() { m_last_press_time = 0; }
		void clear_release_buffer() { m_last_release_time = 0; }
	};
}