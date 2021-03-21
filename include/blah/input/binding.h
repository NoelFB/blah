#pragma once
#include <blah/input/input.h>
#include <blah/containers/stackvector.h>
#include <blah/math/point.h>
#include <blah/math/vec2.h>

namespace Blah
{
	// Represents a Controller Trigger or a single direction of a Controller Axis.
	// Used in the Binding implementation.
	struct BoundTrigger
	{
		// Controller Index we're bound to
		int controller = 0;

		// The Axis we're bound to
		Axis axis = Axis::None;

		// Minimum value of the axis
		float threshold = 0.01f;

		// requires a positive value
		// otherwise requires a negative value
		bool positive = true;

		BoundTrigger() = default;
		BoundTrigger(Axis axis);
		BoundTrigger(int controller, Axis axis, float threshold, bool positive);

		bool is_down(float axis_value) const;

		// helper functions for frequent use cases
		static BoundTrigger left_stick_left(int controller, float threshold);
		static BoundTrigger left_stick_right(int controller, float threshold);
		static BoundTrigger left_stick_up(int controller, float threshold);
		static BoundTrigger left_stick_down(int controller, float threshold);
		static BoundTrigger right_stick_left(int controller, float threshold);
		static BoundTrigger right_stick_right(int controller, float threshold);
		static BoundTrigger right_stick_up(int controller, float threshold);
		static BoundTrigger right_stick_down(int controller, float threshold);
		static BoundTrigger left_trigger(int controller, float threshold);
		static BoundTrigger right_trigger(int controller, float threshold);
	};

	struct BoundButton
	{
		// Controller Index we're bound to
		int controller = 0;

		// Button we're bound to
		Button button = Button::None;

		BoundButton() = default;
		BoundButton(Button button);
		BoundButton(int controller, Button button);
	};

	// Single input Binding
	class Binding
	{
	public:

		// Input Buffer for press events
		float press_buffer = 0;

		// Input Buffer for release events
		float release_buffer = 0;

		// List of bound Keys
		StackVector<Key, 16> keys;
		
		// List of bound Buttons
		StackVector<BoundButton, 16> buttons;

		// List of bound Triggers / Axis
		StackVector<BoundTrigger, 16> triggers;

		// List of bound Mouse buttons
		StackVector<MouseButton, 16> mouse;

		// if the binding has been pressed
		bool pressed() const;

		// if the binding has been released
		bool released() const;

		// if the binding is currently held
		bool down() const;

		// returns the binding's value from 0-1
		float value() const;

		// returns the bindings signed value (0 or 1)
		int sign() const;

		// returns the timestamp of the last time the binding was pressed
		double timestamp() const;

		// updates the binding state
		void update();

		// consumes the current press, and pressed() will return false until the next press
		void consume_press();

		// consumes the current release, and released() will return false until the next release
		void consume_release();

		// adds a key to the binding
		void add(Key key);

		// adds a button to the binding
		void add(BoundButton button);

		// adds an trigger to the binding
		void add(BoundTrigger trigger);

		// adds a mouse button to the binding
		void add(MouseButton mouse);

		// adds an input to the binding
		template<typename T, typename T2, typename ... Args>
		void add(T first, T2 second, const Args&... args)
		{
			add(first);
			add(second, args...);
		}

		// assigns all the bindings to the specific controller
		void set_controller(int index);

		// removes all bindings
		void clear();

	private:
		double m_last_timestamp = 0;
		double m_last_press_time = -1;
		double m_last_release_time = -1;
		float m_value = 0.0f;
		bool m_pressed = false;
		bool m_released = false;
		bool m_down = false;
		bool m_press_consumed = false;
		bool m_release_consumed = false;

		bool get_pressed() const;
		bool get_released() const;
		bool get_down() const;
		float get_value() const;
	};

	// Represents a Bound Axis (ex. Left/Right movement, or a Trigger)
	class AxisBinding
	{
	public:

		enum class Overlap
		{
			Newer,
			Older,
			Cancel
		};

		// Negative Value Binding
		Binding negative;

		// Positive Value Binding
		Binding positive;

		// How to handle overlaps (ex. Left and Right are both held)
		Overlap overlap = Overlap::Newer;

		// Current Value from -1 to 1
		float value() const;

		// Current value, either -1, 0, or 1
		int sign() const;

		// updates the Binding
		void update();

		// consumes the press buffer
		void consume_press();

		// consumes the release buffer
		void consume_release();

		// Adds a negative & positive binding pair
		template<typename NegativeT, typename PositiveT>
		void add(NegativeT negative, PositiveT positive)
		{
			this->negative.add(negative);
			this->positive.add(positive);
		}

		// Adds a Stick binding
		void add_left_stick_x(int controller, float threshold);
		void add_left_stick_y(int controller, float threshold);
		void add_right_stick_x(int controller, float threshold);
		void add_right_stick_y(int controller, float threshold);

		// assigns all the bindings to the specific controller
		void set_controller(int index);

		// Clears all Bindings
		void clear();
	};

	class StickBinding
	{
	public:

		// An optional threshold for circular thresholds
		float round_threshold = 0.0f;

		// X Axis Binding
		AxisBinding x;

		// Y Axis Binding
		AxisBinding y;

		// Current Value, -1 to 1
		Vec2 value() const;

		// Current value, either -1, 0, or 1
		Point sign() const;

		// Updates the Binding
		void update();

		// Consumes the Press Buffer
		void consume_press();

		// Consumes the Release Buffer
		void consume_release();

		// Adds directional bindings
		template<typename LeftT, typename RightT, typename UpT, typename DownT>
		void add(LeftT left, RightT right, UpT up, DownT down)
		{
			x.negative.add(left);
			x.positive.add(right);
			y.negative.add(up);
			y.positive.add(down);
		}

		// Adds the dpad binding
		void add_dpad(int controller);

		// Adds the left stick binding
		void add_left_stick(int controller, float threshold);

		// Adds the right stick binding
		void add_right_stick(int controller, float threshold);

		// assigns all the bindings to the specific controller
		void set_controller(int index);

		// Clears all the bindings
		void clear();
	};
}