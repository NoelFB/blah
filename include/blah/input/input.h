#pragma once

#include <inttypes.h>
#include <blah/math/vec2.h>

// These are generally copied from the SDL2 Scancode Keys
#define BLAH_KEY_DEFINITIONS \
	DEFINE_KEY(Unknown, 0) \
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
	DEFINE_KEY(Enter, 40) \
	DEFINE_KEY(Escape, 41) \
	DEFINE_KEY(Backspace, 42) \
	DEFINE_KEY(Tab, 43) \
	DEFINE_KEY(Space, 44) \
	DEFINE_KEY(Minus, 45) \
	DEFINE_KEY(Equals, 46) \
	DEFINE_KEY(LeftBracket, 47) \
	DEFINE_KEY(RightBracket, 48) \
	DEFINE_KEY(BackSlash, 49) \
	DEFINE_KEY(NonUSHash, 50) \
	DEFINE_KEY(Semicolon, 51) \
	DEFINE_KEY(Apostrophe, 52) \
	DEFINE_KEY(Grave, 53) \
	DEFINE_KEY(Comma, 54) \
	DEFINE_KEY(Period, 55) \
	DEFINE_KEY(Slash, 56) \
	DEFINE_KEY(Capslock, 57) \
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
	DEFINE_KEY(NumlockClear, 83) \
	DEFINE_KEY(KP_Divide, 84) \
	DEFINE_KEY(KP_Multiply, 85) \
	DEFINE_KEY(KP_Minus, 86) \
	DEFINE_KEY(KP_Plus, 87) \
	DEFINE_KEY(KP_Enter, 88) \
	DEFINE_KEY(KP_1, 89) \
	DEFINE_KEY(KP_2, 90) \
	DEFINE_KEY(KP_3, 91) \
	DEFINE_KEY(KP_4, 92) \
	DEFINE_KEY(KP_5, 93) \
	DEFINE_KEY(KP_6, 94) \
	DEFINE_KEY(KP_7, 95) \
	DEFINE_KEY(KP_8, 96) \
	DEFINE_KEY(KP_9, 97) \
	DEFINE_KEY(KP_0, 98) \
	DEFINE_KEY(KP_Period, 99) \
	DEFINE_KEY(NonUSBackSlash, 100) \
	DEFINE_KEY(Application, 101) \
	DEFINE_KEY(Power, 102) \
	DEFINE_KEY(KP_Equals, 103) \
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
	DEFINE_KEY(Execute, 116) \
	DEFINE_KEY(Help, 117) \
	DEFINE_KEY(Menu, 118) \
	DEFINE_KEY(Select, 119) \
	DEFINE_KEY(Stop, 120) \
	DEFINE_KEY(Again, 121) \
	DEFINE_KEY(Undo, 122) \
	DEFINE_KEY(Cut, 123) \
	DEFINE_KEY(Copy, 124) \
	DEFINE_KEY(Paste, 125) \
	DEFINE_KEY(Find, 126) \
	DEFINE_KEY(Mute, 127) \
	DEFINE_KEY(VolumeUp, 128) \
	DEFINE_KEY(VolumeDown, 129) \
	DEFINE_KEY(KP_Comma, 133) \
	DEFINE_KEY(KP_EqualsAs400, 134) \
	DEFINE_KEY(International1, 135) \
	DEFINE_KEY(International2, 136) \
	DEFINE_KEY(International3, 137) \
	DEFINE_KEY(International4, 138) \
	DEFINE_KEY(International5, 139) \
	DEFINE_KEY(International6, 140) \
	DEFINE_KEY(International7, 141) \
	DEFINE_KEY(International8, 142) \
	DEFINE_KEY(International9, 143) \
	DEFINE_KEY(Language1, 144) \
	DEFINE_KEY(Language2, 145) \
	DEFINE_KEY(Language3, 146) \
	DEFINE_KEY(Language4, 147) \
	DEFINE_KEY(Language5, 148) \
	DEFINE_KEY(Language6, 149) \
	DEFINE_KEY(Language7, 150) \
	DEFINE_KEY(Language8, 151) \
	DEFINE_KEY(Language9, 152) \
	DEFINE_KEY(AltErase, 153) \
	DEFINE_KEY(SysReq, 154) \
	DEFINE_KEY(Cancel, 155) \
	DEFINE_KEY(clear, 156) \
	DEFINE_KEY(Prior, 157) \
	DEFINE_KEY(Return2, 158) \
	DEFINE_KEY(Separator, 159) \
	DEFINE_KEY(Out, 160) \
	DEFINE_KEY(Oper, 161) \
	DEFINE_KEY(ClearAgain, 162) \
	DEFINE_KEY(CRSEL, 163) \
	DEFINE_KEY(EXSEL, 164) \
	DEFINE_KEY(KP_00, 176) \
	DEFINE_KEY(KP_000, 177) \
	DEFINE_KEY(ThousandsSeparator, 178) \
	DEFINE_KEY(DecimalSeparator, 179) \
	DEFINE_KEY(CurrencyUnit, 180) \
	DEFINE_KEY(CurrencySubUnit, 181) \
	DEFINE_KEY(KP_LeftParen, 182) \
	DEFINE_KEY(KP_RightParent, 183) \
	DEFINE_KEY(KP_LeftBrace, 184) \
	DEFINE_KEY(KP_RightBrace, 185) \
	DEFINE_KEY(KP_Tab, 186) \
	DEFINE_KEY(KP_BackSpace, 187) \
	DEFINE_KEY(KP_A, 188) \
	DEFINE_KEY(KP_B, 189) \
	DEFINE_KEY(KP_C, 190) \
	DEFINE_KEY(KP_D, 191) \
	DEFINE_KEY(KP_E, 192) \
	DEFINE_KEY(KP_F, 193) \
	DEFINE_KEY(KP_XOR, 194) \
	DEFINE_KEY(KP_Power, 195) \
	DEFINE_KEY(KP_Percent, 196) \
	DEFINE_KEY(KP_Less, 197) \
	DEFINE_KEY(KP_Greater, 198) \
	DEFINE_KEY(KP_Ampersand, 199) \
	DEFINE_KEY(KP_DoubleAmpersand, 200) \
	DEFINE_KEY(KP_VerticalBar, 201) \
	DEFINE_KEY(KP_DoubleVerticalBar, 202) \
	DEFINE_KEY(KP_Colon, 203) \
	DEFINE_KEY(KP_Hash, 204) \
	DEFINE_KEY(KP_Space, 205) \
	DEFINE_KEY(KP_At, 206) \
	DEFINE_KEY(KP_EXCLAM, 207) \
	DEFINE_KEY(KP_MemStore, 208) \
	DEFINE_KEY(KP_MemRecall, 209) \
	DEFINE_KEY(KP_MemClear, 210) \
	DEFINE_KEY(KP_MemAdd, 211) \
	DEFINE_KEY(KP_MemSubstract, 212) \
	DEFINE_KEY(KP_MemMultiply, 213) \
	DEFINE_KEY(KP_MemDivide, 214) \
	DEFINE_KEY(KP_PlusMinus, 215) \
	DEFINE_KEY(KP_Clear, 216) \
	DEFINE_KEY(KP_ClearEntry, 217) \
	DEFINE_KEY(KP_Binary, 218) \
	DEFINE_KEY(KP_Octal, 219) \
	DEFINE_KEY(KP_Decimal, 220) \
	DEFINE_KEY(KP_Hexadecimal, 221) \
	DEFINE_KEY(LeftControl, 224) \
	DEFINE_KEY(LeftShift, 225) \
	DEFINE_KEY(LeftAlt, 226) \
	DEFINE_KEY(LeftGui, 227) \
	DEFINE_KEY(RightControl, 228) \
	DEFINE_KEY(RightShift, 229) \
	DEFINE_KEY(RightAlt, 230) \
	DEFINE_KEY(RightGui, 231)

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

		// maximum length of text input that can be received per-frame
		constexpr int max_text_input = 256;

		// maximum number of nodes within a virtual input device
		constexpr int max_virtual_nodes = 32;
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
		uint64_t button_timestamp[Input::max_controller_buttons];

		// Timestamp, in milliseconds, since each axis last had a value set
		uint64_t axis_timestamp[Input::max_controller_axis];

		// The USB Vendor ID
		uint16_t vendor;

		// The USB Product ID
		uint16_t product;

		// the Product Version
		uint16_t version;
	};

	struct KeyboardState
	{
		bool pressed[Input::max_keyboard_keys];
		bool down[Input::max_keyboard_keys];
		bool released[Input::max_keyboard_keys];
		uint64_t timestamp[Input::max_keyboard_keys];
		char text[Input::max_text_input];
	};

	struct MouseState
	{
		bool pressed[Input::max_mouse_buttons];
		bool down[Input::max_mouse_buttons];
		bool released[Input::max_mouse_buttons];
		uint64_t timestamp[Input::max_mouse_buttons];
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