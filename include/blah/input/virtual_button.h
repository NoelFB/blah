#pragma once
#include <blah/input/input.h>

namespace Blah
{
	class VirtualButton
	{
	private:
		struct KeyNode
		{
			Key key = Key::Unknown;

			bool down = false;
			bool pressed = false;
			bool released = false;

			void init(Key key);
			void update();
		};

		struct ButtonNode
		{
			int gamepad_id = 0;
			Button button = Button::None;

			bool down = false;
			bool pressed = false;
			bool released = false;

			void init(int gamepad_id, Button button);
			void update();
		};

		struct AxisNode
		{
			int gamepad_id = 0;
			Axis axis = Axis::None;
			float threshold = 0;
			bool greater_than = false;

			bool down = false;
			bool pressed = false;
			bool released = false;

			void init(int gamepad_id, Axis axis, float threshold, bool greater_than);
			void update();
		};

		KeyNode m_keys[Input::max_virtual_nodes];
		ButtonNode m_buttons[Input::max_virtual_nodes];
		AxisNode m_axes[Input::max_virtual_nodes];
		int m_keys_len = 0;
		int m_buttons_len = 0;
		int m_axes_len = 0;

		float m_press_buffer = 0;
		float m_release_buffer = 0;
		float m_repeat_delay = 0;
		float m_repeat_interval = 0;

		bool m_down = false;
		bool m_pressed = false;
		bool m_released = false;
		double m_last_press_time = -1;
		double m_last_release_time = -1;
		double m_repeat_press_time = -1;

	public:
		VirtualButton& add_key(Key key);
		VirtualButton& add_button(int gamepad_id, Button button);
		VirtualButton& add_axis(int gamepad_id, Axis axis, float threshold, bool greater_than);
		VirtualButton& repeat(float m_repeat_delay, float m_repeat_interval);
		VirtualButton& press_buffer(float duration);
		VirtualButton& release_buffer(float duration);

		void update();
		bool down() const { return m_down; }
		bool pressed() const { return m_pressed; }
		bool released() const { return m_released; }
		void clear_press_buffer() { m_last_press_time = -1; m_pressed = false; }
		void clear_release_buffer() { m_last_release_time = -1; m_released = false; }
	};
}