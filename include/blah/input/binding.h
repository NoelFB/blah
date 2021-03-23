#pragma once
#include <blah/input/input.h>
#include <blah/containers/stackvector.h>
#include <blah/math/point.h>
#include <blah/math/vec2.h>

namespace Blah
{
	// Single input Binding
	// You must call Binding::update() every frame to poll the input state.
	// Alternatively, bindings can be registered to BindingRegistry which will
	// automatically update them.
	class Binding
	{
	public:

		// Represents a Controller Trigger or a single direction of a Controller Axis.
		struct TriggerBind
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

			TriggerBind() = default;
			TriggerBind(Axis axis);
			TriggerBind(int controller, Axis axis, float threshold, bool positive);

			bool is_down(float axis_value) const;
		};

		// Represents a Controller Button.
		struct ButtonBind
		{
			// Controller Index we're bound to
			int controller = 0;

			// Button we're bound to
			Button button = Button::None;

			ButtonBind() = default;
			ButtonBind(Button button);
			ButtonBind(int controller, Button button);
		};

		// Input Buffer for press events
		float press_buffer = 0;

		// Input Buffer for release events
		float release_buffer = 0;

		// List of bound Keys
		StackVector<Key, 16> keys;

		// List of bound Buttons
		StackVector<ButtonBind, 16> buttons;

		// List of bound Triggers / Axis
		StackVector<TriggerBind, 16> triggers;

		// List of bound Mouse buttons
		StackVector<MouseButton, 16> mouse;

		Binding() = default;

		Binding(float press_buffer)
			: press_buffer(press_buffer)
		{

		}

		template<typename ... Args>
		Binding(float press_buffer, const Args&... args)
			: press_buffer(press_buffer)
		{
			add(args...);
		}

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
		Binding& add(Key key);

		// adds a button to the binding
		Binding& add(ButtonBind button);

		// adds an trigger to the binding
		Binding& add(TriggerBind trigger);

		// adds a mouse button to the binding
		Binding& add(MouseButton mouse);

		// adds an input to the binding
		template<typename T, typename T2, typename ... Args>
		Binding& add(T first, T2 second, const Args&... args)
		{
			add(first);
			add(second, args...);
			return *this;
		}

		// adds the left trigger to the binding
		Binding& add_left_trigger(int controller, float threshold);

		// adds the right trigger to the binding
		Binding& add_right_trigger(int controller, float threshold);

		// assigns all the bindings to the specific controller
		Binding& set_controller(int index);

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

	// Axis Binding (ex. Left/Right movement, or a Trigger)
	// You must call AxisBinding::update() every frame to poll the input state.
	// Alternatively, bindings can be registered to BindingRegistry which will
	// automatically update them.
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

		AxisBinding() = default;

		AxisBinding(const Binding& negative, const Binding& positive, Overlap overlap = Overlap::Newer)
			: negative(negative)
			, positive(positive)
			, overlap(overlap) 
		{}

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
		AxisBinding& add(NegativeT negative, PositiveT positive)
		{
			this->negative.add(negative);
			this->positive.add(positive);
			return *this;
		}

		// Adds a Stick binding
		AxisBinding& add_left_stick_x(int controller, float threshold);
		AxisBinding& add_left_stick_y(int controller, float threshold);
		AxisBinding& add_right_stick_x(int controller, float threshold);
		AxisBinding& add_right_stick_y(int controller, float threshold);

		// assigns all the bindings to the specific controller
		AxisBinding& set_controller(int index);

		// Clears all Bindings
		void clear();
	};

	// Stick Binding (ex. Joystick, Dpad, Arrow Keys, WASD, etc)
	// You must call StickBinding::update() every frame to poll the input state.
	// Alternatively, bindings can be registered to BindingRegistry which will
	// automatically update them.
	class StickBinding
	{
	public:

		// X Axis Binding
		AxisBinding x;

		// Y Axis Binding
		AxisBinding y;

		// An optional threshold for circular thresholds
		float round_threshold = 0.0f;

		StickBinding() = default;

		StickBinding(const AxisBinding& x, const AxisBinding& y, float round_threshold = 0)
			: x(x)
			, y(y)
			, round_threshold(round_threshold)
		{}

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
		StickBinding& add(LeftT left, RightT right, UpT up, DownT down)
		{
			x.negative.add(left);
			x.positive.add(right);
			y.negative.add(up);
			y.positive.add(down);
			return *this;
		}

		// Adds the dpad binding
		StickBinding& add_dpad(int controller);

		// Adds the left stick binding
		StickBinding& add_left_stick(int controller, float threshold);

		// Adds the right stick binding
		StickBinding& add_right_stick(int controller, float threshold);

		// assigns all the bindings to the specific controller
		StickBinding& set_controller(int index);

		// Clears all the bindings
		void clear();
	};
}