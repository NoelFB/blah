#pragma once

#include <blah/core/common.h>
#include <blah/math/vec2.h>
#include <blah/containers/str.h>

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

	struct ControllerState
	{
		// The name of the gamepad, or NULL if not connected
		const char* name;

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
	};

	struct KeyboardState
	{
		bool pressed[Input::max_keyboard_keys];
		bool down[Input::max_keyboard_keys];
		bool released[Input::max_keyboard_keys];
		u64 timestamp[Input::max_keyboard_keys];
		String text;
	};

	struct MouseState
	{
		bool pressed[Input::max_mouse_buttons];
		bool down[Input::max_mouse_buttons];
		bool released[Input::max_mouse_buttons];
		u64 timestamp[Input::max_mouse_buttons];
		Vec2 screen_position;
		Vec2 draw_position;
		Vec2 position;
		Point wheel;
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

	// Game Controller Axes
	enum class Axis
	{
		None = -1,
		LeftX = 0,
		LeftY = 1,
		RightX = 2,
		RightY = 3,
		LeftTrigger = 4,
		RightTrigger = 5
	};

	// Mouse Buttons
	enum class MouseButton
	{
		None = -1,
		Left = 0,
		Middle = 1,
		Right = 2,
	};

	namespace Input
	{
		// Returns the Input State of the current frame.
		// This pointer is only valid for the current frame and should not be stored.
		const InputState* state();

		// Returns the Input State of the previous frame.
		// This pointer is only valid for the current frame and should not be stored.
		const InputState* last_state();

		// Gets the Mouse Position
		Vec2 mouse();

		// Gets the Draw Mouse Position (Mouse Position / Window Size * Draw Size)
		Vec2 mouse_draw();

		// Gets the Mouse Position in Screen Coordinates
		Vec2 mouse_screen();

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

		// Checks if the Left or Right Ctrl Key is down
		bool ctrl();

		// Checks if the Left or Right Shift Key is down
		bool shift();

		// Checks if the Left or Right Alt Key is down
		bool alt();

		// Get the current Text Input
		const char* text();

		// Gets the controller info for the current controller index.
		// If the controller is not connected or the index it out of range, this will set an unconnected controller.
		const ControllerState* controller(int controller_index);

		// Checks if the button on the controller was pressed this frame.
		// If the controller is not connected, or the index is out of range, this will return false.
		bool pressed(int controller_index, Button button);

		// Checks if the button on the controller was held this frame.
		// If the controller is not connected, or the index is out of range, this will return false.
		bool down(int controller_index, Button button);

		// Checks if the button on the controller was released this frame.
		// If the controller is not connected, or the index is out of range, this will return false.
		bool released(int controller_index, Button button);
		
		// returns the value of the given axis
		float axis_check(int controller_index, Axis axis);

		// checks the given virtual axis, described by 2 keys. `fallback` is returned if both keys are held
		int axis_check(int fallback, Key negative, Key positive);

		// checks the given virtual axis, described by 2 buttons. `fallback` is returned if both buttons are held
		int axis_check(int fallback, int controller_index, Button negative, Button positive);

		// returns a string name of the given key
		const char* name_of(Key key);

		// returns a string name of the given button
		const char* name_of(Button button);
	}
}