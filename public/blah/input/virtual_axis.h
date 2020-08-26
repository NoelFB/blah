#pragma once
#include <blah/input/input.h>

namespace Blah
{
	class VirtualAxis
	{
	private:
		struct KeysNode
		{
			Key positive = Key::Unknown;
			Key negative = Key::Unknown;

			int value = 0;

			void init(Key negative, Key positive);
			void update();
		};

		struct ButtonsNode
		{
			int gamepad_id = 0;
			Button positive = Button::None;
			Button negative = Button::None;

			int value = 0;

			void init(int gamepad_id, Button negative, Button positive);
			void update();
		};

		struct AxisNode
		{
			int gamepad_id = 0;
			Axis axis = Axis::None;
			float deadzone = 0;

			float value = 0;

			void init(int gamepad_id, Axis axis, float deadzone);
			void update();
		};

		KeysNode m_keys[BLAH_MAX_VIRTUAL_NODES];
		ButtonsNode m_buttons[BLAH_MAX_VIRTUAL_NODES];
		AxisNode m_axes[BLAH_MAX_VIRTUAL_NODES];
		int m_keys_len = 0;
		int m_buttons_len = 0;
		int m_axes_len = 0;

		float m_press_buffer = 0;
		float m_release_buffer = 0;
		float m_repeat_delay = 0;
		float m_repeat_interval = 0;

		float m_value = 0;
		int m_value_i = 0;
		float m_last_value = 0;
		int m_last_value_i = 0;
		bool m_pressed = false;
		bool m_released = false;
		float m_last_press_time = 0;
		float m_last_release_time = 0;
		float m_repeat_press_time = 0;

	public:
		VirtualAxis& add_keys(Key negative, Key positive);
		VirtualAxis& add_buttons(int gamepad_id, Button negative, Button positive);
		VirtualAxis& add_axis(int gamepad_id, Axis axis, float deadzone);
		VirtualAxis& repeat(float m_repeat_delay, float m_repeat_interval);
		VirtualAxis& press_buffer(float duration);
		VirtualAxis& release_buffer(float duration);

		void update();
		float value() const { return m_value; }
		int value_i() const { return m_value_i; }
		float last_value() const { return m_last_value; }
		int last_value_i() const { return m_last_value_i; }
		bool pressed() const { return m_pressed; }
		bool released() const { return m_released; }
		void clear_press_buffer() { m_last_press_time = -1; m_pressed = false; }
		void clear_release_buffer() { m_last_release_time = -1; m_released = false; }
	};
}