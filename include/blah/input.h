#pragma once
#include <blah/common.h>
#include <blah/math/spatial.h>
#include <blah/containers/str.h>
#include <blah/containers/stackvector.h>

// These are generally copied from the SDL2 Scancode Keys,
// which are in turn based on the USB standards:
// https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
#define BLAH_KEY_DEFINITIONS \
	DEFINE_KEY(Unknown, 0) \
	\
	DEFINE_KEY(A, 4) \
	DEFINE_KEY(B, 5) \
	DEFINE_KEY(C, 6) \
	DEFINE_KEY(D, 7) \
	DEFINE_KEY(E, 8) \
	DEFINE_KEY(F, 9) \
	DEFINE_KEY(G, 10) \
	DEFINE_KEY(H, 11) \
	DEFINE_KEY(I, 12) \
	DEFINE_KEY(J, 13) \
	DEFINE_KEY(K, 14) \
	DEFINE_KEY(L, 15) \
	DEFINE_KEY(M, 16) \
	DEFINE_KEY(N, 17) \
	DEFINE_KEY(O, 18) \
	DEFINE_KEY(P, 19) \
	DEFINE_KEY(Q, 20) \
	DEFINE_KEY(R, 21) \
	DEFINE_KEY(S, 22) \
	DEFINE_KEY(T, 23) \
	DEFINE_KEY(U, 24) \
	DEFINE_KEY(V, 25) \
	DEFINE_KEY(W, 26) \
	DEFINE_KEY(X, 27) \
	DEFINE_KEY(Y, 28) \
	DEFINE_KEY(Z, 29) \
	\
	DEFINE_KEY(D1, 30) \
	DEFINE_KEY(D2, 31) \
	DEFINE_KEY(D3, 32) \
	DEFINE_KEY(D4, 33) \
	DEFINE_KEY(D5, 34) \
	DEFINE_KEY(D6, 35) \
	DEFINE_KEY(D7, 36) \
	DEFINE_KEY(D8, 37) \
	DEFINE_KEY(D9, 38) \
	DEFINE_KEY(D0, 39) \
	\
	DEFINE_KEY(Enter, 40) \
	DEFINE_KEY(Escape, 41) \
	DEFINE_KEY(Backspace, 42) \
	DEFINE_KEY(Tab, 43) \
	DEFINE_KEY(Space, 44) \
	\
	DEFINE_KEY(Minus, 45) \
	DEFINE_KEY(Equals, 46) \
	DEFINE_KEY(LeftBracket, 47) \
	DEFINE_KEY(RightBracket, 48) \
	DEFINE_KEY(Backslash, 49) \
	DEFINE_KEY(Semicolon, 51) \
	DEFINE_KEY(Apostrophe, 52) \
	DEFINE_KEY(Tilde, 53) \
	DEFINE_KEY(Comma, 54) \
	DEFINE_KEY(Period, 55) \
	DEFINE_KEY(Slash, 56) \
	\
	DEFINE_KEY(Capslock, 57) \
	\
	DEFINE_KEY(F1, 58) \
	DEFINE_KEY(F2, 59) \
	DEFINE_KEY(F3, 60) \
	DEFINE_KEY(F4, 61) \
	DEFINE_KEY(F5, 62) \
	DEFINE_KEY(F6, 63) \
	DEFINE_KEY(F7, 64) \
	DEFINE_KEY(F8, 65) \
	DEFINE_KEY(F9, 66) \
	DEFINE_KEY(F10, 67) \
	DEFINE_KEY(F11, 68) \
	DEFINE_KEY(F12, 69) \
	DEFINE_KEY(F13, 104) \
	DEFINE_KEY(F14, 105) \
	DEFINE_KEY(F15, 106) \
	DEFINE_KEY(F16, 107) \
	DEFINE_KEY(F17, 108) \
	DEFINE_KEY(F18, 109) \
	DEFINE_KEY(F19, 110) \
	DEFINE_KEY(F20, 111) \
	DEFINE_KEY(F21, 112) \
	DEFINE_KEY(F22, 113) \
	DEFINE_KEY(F23, 114) \
	DEFINE_KEY(F24, 115) \
	\
	DEFINE_KEY(PrintScreen, 70) \
	DEFINE_KEY(ScrollLock, 71) \
	DEFINE_KEY(Pause, 72) \
	DEFINE_KEY(Insert, 73) \
	DEFINE_KEY(Home, 74) \
	DEFINE_KEY(PageUp, 75) \
	DEFINE_KEY(Delete, 76) \
	DEFINE_KEY(End, 77) \
	DEFINE_KEY(PageDown, 78) \
	DEFINE_KEY(Right, 79) \
	DEFINE_KEY(Left, 80) \
	DEFINE_KEY(Down, 81) \
	DEFINE_KEY(Up, 82) \
	\
	DEFINE_KEY(Numlock, 83) \
	\
	DEFINE_KEY(Application, 101) \
	\
	DEFINE_KEY(Execute, 116) \
	DEFINE_KEY(Help, 117) \
	DEFINE_KEY(Menu, 118) \
	DEFINE_KEY(Select, 119) \
	DEFINE_KEY(Stop, 120) \
	DEFINE_KEY(Redo, 121) \
	DEFINE_KEY(Undo, 122) \
	DEFINE_KEY(Cut, 123) \
	DEFINE_KEY(Copy, 124) \
	DEFINE_KEY(Paste, 125) \
	DEFINE_KEY(Find, 126) \
	DEFINE_KEY(Mute, 127) \
	DEFINE_KEY(VolumeUp, 128) \
	DEFINE_KEY(VolumeDown, 129) \
	\
	DEFINE_KEY(AltErase, 153) \
	DEFINE_KEY(SysReq, 154) \
	DEFINE_KEY(Cancel, 155) \
	DEFINE_KEY(Clear, 156) \
	DEFINE_KEY(Prior, 157) \
	DEFINE_KEY(Enter2, 158) \
	DEFINE_KEY(Separator, 159) \
	DEFINE_KEY(Out, 160) \
	DEFINE_KEY(Oper, 161) \
	DEFINE_KEY(ClearAgain, 162) \
	\
	DEFINE_KEY(KeypadA, 188) \
	DEFINE_KEY(KeypadB, 189) \
	DEFINE_KEY(KeypadC, 190) \
	DEFINE_KEY(KeypadD, 191) \
	DEFINE_KEY(KeypadE, 192) \
	DEFINE_KEY(KeypadF, 193) \
	DEFINE_KEY(Keypad0, 98) \
	DEFINE_KEY(Keypad00, 176) \
	DEFINE_KEY(Keypad000, 177) \
	DEFINE_KEY(Keypad1, 89) \
	DEFINE_KEY(Keypad2, 90) \
	DEFINE_KEY(Keypad3, 91) \
	DEFINE_KEY(Keypad4, 92) \
	DEFINE_KEY(Keypad5, 93) \
	DEFINE_KEY(Keypad6, 94) \
	DEFINE_KEY(Keypad7, 95) \
	DEFINE_KEY(Keypad8, 96) \
	DEFINE_KEY(Keypad9, 97) \
	DEFINE_KEY(KeypadDivide, 84) \
	DEFINE_KEY(KeypadMultiply, 85) \
	DEFINE_KEY(KeypadMinus, 86) \
	DEFINE_KEY(KeypadPlus, 87) \
	DEFINE_KEY(KeypadEnter, 88) \
	DEFINE_KEY(KeypadPeroid, 99) \
	DEFINE_KEY(KeypadEquals, 103) \
	DEFINE_KEY(KeypadComma, 133) \
	DEFINE_KEY(KeypadLeftParen, 182) \
	DEFINE_KEY(KeypadRightParen, 183) \
	DEFINE_KEY(KeypadLeftBrace, 184) \
	DEFINE_KEY(KeypadRightBrace, 185) \
	DEFINE_KEY(KeypadTab, 186) \
	DEFINE_KEY(KeypadBackspace, 187) \
	DEFINE_KEY(KeypadXor, 194) \
	DEFINE_KEY(KeypadPower, 195) \
	DEFINE_KEY(KeypadPercent, 196) \
	DEFINE_KEY(KeypadLess, 197) \
	DEFINE_KEY(KeypadGreater, 198) \
	DEFINE_KEY(KeypadAmpersand, 199) \
	DEFINE_KEY(KeypadColon, 203) \
	DEFINE_KEY(KeypadHash, 204) \
	DEFINE_KEY(KeypadSpace, 205) \
	DEFINE_KEY(KeypadClear, 216) \
	\
	DEFINE_KEY(LeftControl, 224) \
	DEFINE_KEY(LeftShift, 225) \
	DEFINE_KEY(LeftAlt, 226) \
	DEFINE_KEY(LeftOS, 227) \
	DEFINE_KEY(RightControl, 228) \
	DEFINE_KEY(RightShift, 229) \
	DEFINE_KEY(RightAlt, 230) \
	DEFINE_KEY(RightOS, 231)

#define BLAH_BUTTON_DEFINITIONS \
	DEFINE_BTN(None, -1) \
	DEFINE_BTN(A, 0) \
	DEFINE_BTN(B, 1) \
	DEFINE_BTN(X, 2) \
	DEFINE_BTN(Y, 3) \
	DEFINE_BTN(Back, 4) \
	DEFINE_BTN(Select, 5) \
	DEFINE_BTN(Start, 6) \
	DEFINE_BTN(LeftStick, 7) \
	DEFINE_BTN(RightStick, 8) \
	DEFINE_BTN(LeftShoulder, 9) \
	DEFINE_BTN(RightShoulder, 10) \
	DEFINE_BTN(Up, 11) \
	DEFINE_BTN(Down, 12) \
	DEFINE_BTN(Left, 13) \
	DEFINE_BTN(Right, 14)

namespace Blah
{
	namespace Input
	{
		// maximum number of controllers the input can handle
		constexpr int max_controllers = 8;

		// maximum number of buttons the input will track
		constexpr int max_controller_buttons = 64;

		// maximum number of controller axis the input will track
		constexpr int max_controller_axis = 16;

		// maximum number of mouse buttons the input will track
		constexpr int max_mouse_buttons = 16;

		// maximum number of keys the input will track
		constexpr int max_keyboard_keys = 512;
	}

	// Keyboard Keys
	enum class Key
	{
		#define DEFINE_KEY(name, value) name = value,
		BLAH_KEY_DEFINITIONS
		#undef DEFINE_KEY
	};

	// Game Controller Buttons
	enum class Button
	{
		#define DEFINE_BTN(name, value) name = value,
		BLAH_BUTTON_DEFINITIONS
		#undef DEFINE_BTN
	};

	// Game Controller Axis
	enum class Axis
	{
		None = -1,
		LeftX = 0,
		LeftY = 1,
		RightX = 2,
		RightY = 3,
		LeftTrigger = 4,
		RightTrigger = 5,
	};

	// Mouse Buttons
	enum class MouseButton
	{
		None = -1,
		Left = 0,
		Middle = 1,
		Right = 2,
	};

	// Controller State
	struct ControllerState
	{
		// The name of the gamepad, or NULL if not connected
		String name;

		// Whether this gamepad is connected
		bool is_connected;

		// Whether this gamepad is a standard Game Controller
		bool is_gamepad;

		// The total button count for this controller
		int button_count;

		// The total axis count for this controller
		int axis_count;

		// An array holding the pressed state of each button
		bool pressed[Input::max_controller_buttons];

		// An array holding the down state of each button
		bool down[Input::max_controller_buttons];

		// An array holding the released state of each button
		bool released[Input::max_controller_buttons];

		// An array holding the value state of each axis
		float axis[Input::max_controller_axis];

		// Timestamp, in milliseconds, since each button was last pressed
		u64 button_timestamp[Input::max_controller_buttons];

		// Timestamp, in milliseconds, since each axis last had a value set
		u64 axis_timestamp[Input::max_controller_axis];

		// The USB Vendor ID
		u16 vendor;

		// The USB Product ID
		u16 product;

		// the Product Version
		u16 version;

		// marks the controller as connected
		void on_connect(const String& name, bool is_gamepad, int button_count, int axis_count, u16 vendor, u16 product, u16 version);

		// marks the controller as disconnected
		void on_disconnect();

		// invokes a button press
		void on_press(Button button);

		// invokes a button release
		void on_release(Button button);

		// invokes an axis movement
		void on_axis(Axis axis, float value);
	};

	// Keyboard State
	struct KeyboardState
	{
		// whether a key was pressed this frame
		bool pressed[Input::max_keyboard_keys];

		// whether a key is currently held
		bool down[Input::max_keyboard_keys];

		// whether a key was released this frame
		bool released[Input::max_keyboard_keys];
		
		// the timestamp for the key being pressed
		u64 timestamp[Input::max_keyboard_keys];

		// current text input this frame
		String text;

		// Checks if the Left or Right Ctrl Key is down
		bool ctrl();

		// Checks if the Left or Right Shift Key is down
		bool shift();

		// Checks if the Left or Right Alt Key is down
		bool alt();

		// invokes a key press
		void on_press(Key key);

		// invokes a key release
		void on_release(Key key);
	};

	// Mouse State
	struct MouseState
	{
		// whether a button was pressed this frame
		bool pressed[Input::max_mouse_buttons];

		// whether a button was held this frame
		bool down[Input::max_mouse_buttons];

		// whether a button was released this frame
		bool released[Input::max_mouse_buttons];

		// the timestamp for the button being pressed
		u64 timestamp[Input::max_mouse_buttons];

		// mouse position in screen coordinates
		Vec2f screen_position;
		
		// mouse position in pixel coordinates
		Vec2f draw_position;
		
		// mouse position on the window
		Vec2f position;
		
		// mouse wheel value this frame
		Point wheel;

		// invokes a key press
		void on_press(MouseButton button);

		// invokes a mouse movement
		void on_move(const Vec2f& position, const Vec2f& screen_position);

		// invokes a key release
		void on_release(MouseButton button);
	};

	// An Input State, which stores the state for gamepads, keyboard, and mouse
	struct InputState
	{
		// All the Gamepads. Note that not all gamepads are necessarily connected,
		// and each one must be checked before use.
		ControllerState controllers[Input::max_controllers];

		// The current Keyboard state
		KeyboardState keyboard;

		// The current Mouse state
		MouseState mouse;
	};

	class ButtonBinding;
	using ButtonBindingRef = Ref<ButtonBinding>;

	class AxisBinding;
	using AxisBindingRef = Ref<AxisBinding>;

	class StickBinding;
	using StickBindingRef = Ref<StickBinding>;

	// Single input Binding
	// You must call Binding::update() every frame to poll the input state.
	// Alternatively, bindings can be registered to Input which will
	// automatically update them.
	class ButtonBinding
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

		ButtonBinding() = default;

		ButtonBinding(float press_buffer)
			: press_buffer(press_buffer)
		{

		}

		template<typename ... Args>
		ButtonBinding(float press_buffer, const Args&... args)
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
		ButtonBinding& add(Key key);

		// adds a button to the binding
		ButtonBinding& add(ButtonBind button);

		// adds an trigger to the binding
		ButtonBinding& add(TriggerBind trigger);

		// adds a mouse button to the binding
		ButtonBinding& add(MouseButton mouse);

		// adds an input to the binding
		template<typename T, typename T2, typename ... Args>
		ButtonBinding& add(T first, T2 second, const Args&... args)
		{
			add(first);
			add(second, args...);
			return *this;
		}

		// adds the left trigger to the binding
		ButtonBinding& add_left_trigger(int controller, float threshold);

		// adds the right trigger to the binding
		ButtonBinding& add_right_trigger(int controller, float threshold);

		// assigns all the bindings to the specific controller
		ButtonBinding& set_controller(int index);

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
	// Alternatively, bindings can be registered to Input which will
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
		ButtonBinding negative;

		// Positive Value Binding
		ButtonBinding positive;

		// How to handle overlaps (ex. Left and Right are both held)
		Overlap overlap = Overlap::Newer;

		AxisBinding() = default;

		AxisBinding(const ButtonBinding& negative, const ButtonBinding& positive, Overlap overlap = Overlap::Newer)
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

		// checks if the axis was pressed this frame (ie. went from 0 to -1 or 1)
		bool pressed() const { return negative.pressed() || positive.pressed(); }

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
	// Alternatively, bindings can be registered to Input which will
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
		Vec2f value() const;

		// Current value, either -1, 0, or 1
		Point sign() const;

		// Updates the Binding
		void update();

		// checks if the stick was pressed this frame
		bool pressed() const { return x.pressed() || y.pressed(); }

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

	namespace Input
	{
		// Input State for the current frame
		extern InputState state;

		// Input State for the previous frame
		extern InputState last_state;

		// Key-Repeating intervals
		extern float repeat_delay;
		extern float repeat_interval;

		// Gets the Mouse Position
		Vec2f mouse();

		// Gets the Draw Mouse Position (Mouse Position / Window Size * Draw Size)
		Vec2f mouse_draw();

		// Gets the Mouse Position in Screen Coordinates
		Vec2f mouse_screen();

		// Checks if the given Mouse Button is pressed
		bool pressed(MouseButton button);

		// Checks if the given Mouse Button is down
		bool down(MouseButton button);

		// Checks if the given Mouse Button is released
		bool released(MouseButton button);

		// Gets the Mouse Wheel
		Point mouse_wheel();

		// Checks if the keyboard key was pressed this frame
		bool pressed(Key key);

		// Checks if the keyboard key was held this frame
		bool down(Key key);

		// Checks if the keyboard key was released this frame
		bool released(Key key);

		// Pressed or repeating on-interval
		bool repeating(Key key);

		// Checks if the Left or Right Ctrl Key is down
		bool ctrl();

		// Checks if the Left or Right Shift Key is down
		bool shift();

		// Checks if the Left or Right Alt Key is down
		bool alt();

		// Checks if the given Controller Button is pressed
		bool pressed(int controller_index, Button button);

		// Checks if the given Controller Button is down
		bool down(int controller_index, Button button);

		// Checks if the given Controller Button is released
		bool released(int controller_index, Button button);

		// returns a string name of the key
		const char* name_of(Key key);

		// returns a string name of the button
		const char* name_of(Button button);

		// gets the string contents of the clipboard
		const String& get_clipboard();

		// sets the string contents of the clipboard
		void set_clipboard(const String& text);

		// registers a new binding and returns a handle to it
		ButtonBindingRef register_binding(const ButtonBinding& binding_data);

		// registers a new binding and returns a handle to it
		AxisBindingRef register_binding(const AxisBinding& binding_data);

		// registers a new binding and returns a handle to it
		StickBindingRef register_binding(const StickBinding& binding_data);
	}
}