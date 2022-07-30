#ifdef BLAH_PLATFORM_SDL2

#include "platform.h"
#include "renderer.h"
#include <blah/input.h>
#include <blah/app.h>
#include <blah/filesystem.h>
#include <blah/common.h>
#include <blah/time.h>

#include <SDL.h>

// for File Reading / Writing
#include <filesystem>

// Windows requires a few extra includes
#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>    // for the following includes
#include <winuser.h>	// for SetProcessDPIAware
#include <shellapi.h>	// for ShellExecute for dir_explore
#include <SDL_syswm.h>  // for SDL_SysWMinfo for D3D11
#endif

// Macro defined by X11 conflicts with MouseButton enum
#undef None

namespace Blah
{
	void blah_sdl_log(void* userdata, int category, SDL_LogPriority priority, const char* message)
	{
		if (priority <= SDL_LOG_PRIORITY_INFO)
			Log::info(message);
		else if (priority <= SDL_LOG_PRIORITY_WARN)
			Log::warn(message);
		else
			Log::error(message);
	}

	int blah_sdl_find_joystick_index(SDL_Joystick** joysticks, SDL_JoystickID instance_id)
	{
		for (int i = 0; i < Input::max_controllers; i++)
			if (joysticks[i] != nullptr && SDL_JoystickInstanceID(joysticks[i]) == instance_id)
				return i;
		return -1;
	}

	int blah_sdl_find_gamepad_index(SDL_GameController** gamepads, SDL_JoystickID instance_id)
	{
		for (int i = 0; i < Input::max_controllers; i++)
		{
			if (gamepads[i] != nullptr)
			{
				auto joystick = SDL_GameControllerGetJoystick(gamepads[i]);
				if (SDL_JoystickInstanceID(joystick) == instance_id)
					return i;
			}
		}
		return -1;
	}

	struct SDL2_File : public File
	{
		SDL_RWops* handle;
		SDL2_File(SDL_RWops* handle) : handle(handle) { }
		~SDL2_File() { if (handle) SDL_RWclose(handle); }
		size_t length() override { return SDL_RWsize(handle); }
		size_t position() override { return SDL_RWtell(handle); }
		size_t seek(size_t position) override { return SDL_RWseek(handle, position, RW_SEEK_SET); }
		size_t read(void* buffer, size_t length) override { return SDL_RWread(handle, buffer, sizeof(char), length); }
		size_t write(const void* buffer, size_t length) override { return SDL_RWwrite(handle, buffer, sizeof(char), length); }
	};

	struct SDL2_Platform : public Platform
	{
		SDL_Window* window = nullptr;
		SDL_Joystick* joysticks[Input::max_controllers];
		SDL_GameController* gamepads[Input::max_controllers];
		char* base_path_value = nullptr;
		char* user_path_value = nullptr;
		char* clipboard_text = nullptr;
		bool displayed = false;

		SDL2_Platform();
		bool init(const Config& config) override;
		void ready() override;
		void shutdown() override;
		u64 ticks() override;
		void update(InputState& state) override;
		void sleep(int milliseconds) override;
		void present() override;
		const char* get_title() override;
		void set_title(const char* title) override;
		void get_position(int* x, int* y) override;
		void set_position(int x, int y) override;
		bool get_focused() override;
		void set_fullscreen(bool enabled) override;
		void get_size(int* width, int* height) override;
		void set_size(int width, int height) override;
		void get_draw_size(int* width, int* height) override;
		float get_content_scale() override;
		const char* app_path() override;
		const char* user_path() override;
		FileRef file_open(const char* path, FileMode mode) override;
		bool file_exists(const char* path) override;
		bool file_delete(const char* path) override;
		bool dir_create(const char* path) override;
		bool dir_exists(const char* path) override;
		bool dir_delete(const char* path) override;
		void dir_enumerate(Vector<FilePath>& list, const char* path, bool recursive) override;
		void dir_explore(const char* path) override;
		void set_clipboard(const char* text) override;
		const char* get_clipboard() override;
		void open_url(const char* url) override;
		void* gl_get_func(const char* name) override;
		void* gl_context_create() override;
		void gl_context_make_current(void* context) override;
		void gl_context_destroy(void* context) override;
		void* d3d11_get_hwnd() override;
	};
}

using namespace Blah;

SDL2_Platform::SDL2_Platform()
{
	for (int i = 0; i < Input::max_controllers; i++)
	{
		joysticks[i] = nullptr;
		gamepads[i] = nullptr;
	}
}

bool SDL2_Platform::init(const Config& config)
{
	// Required to call this for Windows
	// I'm not sure why SDL2 doesn't do this on Windows automatically?
#if _WIN32
	SetProcessDPIAware();
#endif

	// TODO:
	// control this via some kind of config flag
#ifndef __EMSCRIPTEN__
	// Note: Emscripten gets a Stack Overflow if this is assigned to Verbose, even when
	// increasing the node stack size to +8mb. Some kind of SDL2 problem?
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
#endif
	SDL_LogSetOutputFunction(blah_sdl_log, nullptr);

	// Get SDL version
	SDL_version version;
	SDL_GetVersion(&version);
	Log::info("SDL v%i.%i.%i", version.major, version.minor, version.patch);

	// initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0)
	{
		Log::error("Failed to initialize SDL2");
		return false;
	}

	int flags = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE;

	// enable OpenGL
	if (config.renderer_type == RendererType::OpenGL)
	{
		flags |= SDL_WINDOW_OPENGL;

#ifdef __EMSCRIPTEN__
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		// TODO:
		// This should be controlled via the gfx api somehow?
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
#endif
	}

	// create the window
	window = SDL_CreateWindow(config.name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, config.width, config.height, flags);
	if (window == nullptr)
	{
		Log::error("Failed to create a Window");
		return false;
	}

	// Scale Window to monitor for High DPI displays
	// Other platforms do this automatically ... Windows we need to explitely do so
#if _WIN32
	{
		// find the display index
		int display = SDL_GetWindowDisplayIndex(window);
		float ddpi, hdpi, vdpi;
		if (SDL_GetDisplayDPI(display, &ddpi, &hdpi, &vdpi) == 0)
		{
			// scale the window up basesd on the display DPI
			float hidpiRes = 96;
			float dpi = (ddpi / hidpiRes);
			if (dpi != 1)
			{
				SDL_DisplayMode mode;
				SDL_GetDesktopDisplayMode(display, &mode);
				SDL_SetWindowPosition(window, (int)(mode.w - config.width * dpi) / 2, (int)(mode.h - config.height * dpi) / 2);
				SDL_SetWindowSize(window, (int)(config.width * dpi), (int)(config.height * dpi));
			}
		}
	}
#endif

	// set window properties
	SDL_SetWindowResizable(window, SDL_TRUE);
	SDL_SetWindowMinimumSize(window, 256, 256);

	return true;
}

void SDL2_Platform::ready()
{
#ifndef __EMSCRIPTEN__
	// enable V-Sync
	// TODO:
	// This should be a toggle or controllable in some way
	if (App::renderer().type == RendererType::OpenGL)
		SDL_GL_SetSwapInterval(1);
#endif
}

void SDL2_Platform::shutdown()
{
	if (window != nullptr)
		SDL_DestroyWindow(window);
	window = nullptr;
	displayed = false;

	if (base_path_value != nullptr)
		SDL_free(base_path_value);

	if (user_path_value != nullptr)
		SDL_free(user_path_value);

	if (clipboard_text != nullptr)
		SDL_free(clipboard_text);

	SDL_Quit();
}

u64 SDL2_Platform::ticks()
{
	auto counter = SDL_GetPerformanceCounter();
	auto per_second = (double)SDL_GetPerformanceFrequency();
	return (u64)(counter * (Time::ticks_per_second / per_second));
}

void SDL2_Platform::update(InputState& state)
{
	// update the mouse every frame
	{
		int win_x, win_y, x, y;

		SDL_GetWindowPosition(window, &win_x, &win_y);
		SDL_GetGlobalMouseState(&x, &y);

		state.mouse.on_move(
			Vec2f((float)(x - win_x), (float)(y - win_y)),
			Vec2f((float)x, (float)y));
	}

	// poll normal events
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			auto& config = App::config();
			if (config.on_exit_request != nullptr)
				config.on_exit_request();
		}
		// Mouse
		else if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			MouseButton btn = MouseButton::None;
			if (event.button.button == SDL_BUTTON_LEFT)
				btn = MouseButton::Left;
			else if (event.button.button == SDL_BUTTON_RIGHT)
				btn = MouseButton::Right;
			else if (event.button.button == SDL_BUTTON_MIDDLE)
				btn = MouseButton::Middle;

			state.mouse.on_press(btn);
		}
		else if (event.type == SDL_MOUSEBUTTONUP)
		{
			MouseButton btn = MouseButton::None;
			if (event.button.button == SDL_BUTTON_LEFT)
				btn = MouseButton::Left;
			else if (event.button.button == SDL_BUTTON_RIGHT)
				btn = MouseButton::Right;
			else if (event.button.button == SDL_BUTTON_MIDDLE)
				btn = MouseButton::Middle;

			state.mouse.on_release(btn);
		}
		else if (event.type == SDL_MOUSEWHEEL)
		{
			state.mouse.wheel = Point(event.wheel.x, event.wheel.y);
		}
		// Keyboard
		else if (event.type == SDL_KEYDOWN)
		{
			if (event.key.repeat == 0)
				state.keyboard.on_press((Key)event.key.keysym.scancode);
		}
		else if (event.type == SDL_KEYUP)
		{
			if (event.key.repeat == 0)
				state.keyboard.on_release((Key)event.key.keysym.scancode);
		}
		else if (event.type == SDL_TEXTINPUT)
		{
			state.keyboard.text += event.text.text;
		}
		// Joystick Controller
		else if (event.type == SDL_JOYDEVICEADDED)
		{
			auto index = event.jdevice.which;

			if (SDL_IsGameController(index) == SDL_FALSE && index >= 0 && index < Input::max_controllers)
			{
				auto ptr = joysticks[index] = SDL_JoystickOpen(index);
				auto name = SDL_JoystickName(ptr);
				auto button_count = SDL_JoystickNumButtons(ptr);
				auto axis_count = SDL_JoystickNumAxes(ptr);
				auto vendor = SDL_JoystickGetVendor(ptr);
				auto product = SDL_JoystickGetProduct(ptr);
				auto version = SDL_JoystickGetProductVersion(ptr);

				state.controllers[index].on_connect(name, 0, button_count, axis_count, vendor, product, version);
			}
		}
		else if (event.type == SDL_JOYDEVICEREMOVED)
		{
			auto index = blah_sdl_find_joystick_index(joysticks, event.jdevice.which);
			if (index >= 0)
			{
				if (SDL_IsGameController(index) == SDL_FALSE)
				{
					state.controllers[index].on_disconnect();
					SDL_JoystickClose(joysticks[index]);
				}
			}
		}
		else if (event.type == SDL_JOYBUTTONDOWN)
		{
			auto index = blah_sdl_find_joystick_index(joysticks, event.jdevice.which);
			if (index >= 0)
			{
				if (SDL_IsGameController(index) == SDL_FALSE)
					state.controllers[index].on_press((Button)event.jbutton.button);
			}
		}
		else if (event.type == SDL_JOYBUTTONUP)
		{
			auto index = blah_sdl_find_joystick_index(joysticks, event.jdevice.which);
			if (index >= 0)
			{
				if (SDL_IsGameController(index) == SDL_FALSE)
					state.controllers[index].on_release((Button)event.jbutton.button);
			}
		}
		else if (event.type == SDL_JOYAXISMOTION)
		{
			auto index = blah_sdl_find_joystick_index(joysticks, event.jdevice.which);
			if (index >= 0)
			{
				if (SDL_IsGameController(index) == SDL_FALSE)
				{
					float value;
					if (event.jaxis.value >= 0)
						value = event.jaxis.value / 32767.0f;
					else
						value = event.jaxis.value / 32768.0f;
					state.controllers[index].on_axis((Axis)event.jaxis.axis, value);
				}
			}
		}
		// Gamepad Controller
		else if (event.type == SDL_CONTROLLERDEVICEADDED)
		{
			auto index = event.cdevice.which;
			if (index >= 0 && index < Input::max_controllers)
			{
				auto ptr = gamepads[index] = SDL_GameControllerOpen(index);
				auto name = SDL_GameControllerName(ptr);
				auto vendor = SDL_GameControllerGetVendor(ptr);
				auto product = SDL_GameControllerGetProduct(ptr);
				auto version = SDL_GameControllerGetProductVersion(ptr);

				state.controllers[index].on_connect(name, 1, 15, 6, vendor, product, version);
			}
		}
		else if (event.type == SDL_CONTROLLERDEVICEREMOVED)
		{
			auto index = blah_sdl_find_gamepad_index(gamepads, event.cdevice.which);
			if (index >= 0)
			{
				state.controllers[index].on_disconnect();
				SDL_GameControllerClose(gamepads[index]);
			}
		}
		else if (event.type == SDL_CONTROLLERBUTTONDOWN)
		{
			auto index = blah_sdl_find_gamepad_index(gamepads, event.cdevice.which);
			if (index >= 0)
			{
				Button button = Button::None;
				if (event.cbutton.button >= 0 && event.cbutton.button < 15)
					button = (Button)event.cbutton.button; // NOTE: These map directly to Engine Buttons enum!

				state.controllers[index].on_press(button);
			}
		}
		else if (event.type == SDL_CONTROLLERBUTTONUP)
		{
			auto index = blah_sdl_find_gamepad_index(gamepads, event.cdevice.which);
			if (index >= 0)
			{
				Button button = Button::None;
				if (event.cbutton.button >= 0 && event.cbutton.button < 15)
					button = (Button)event.cbutton.button; // NOTE: These map directly to Engine Buttons enum!

				state.controllers[index].on_release(button);
			}
		}
		else if (event.type == SDL_CONTROLLERAXISMOTION)
		{
			auto index = blah_sdl_find_gamepad_index(gamepads, event.cdevice.which);
			if (index >= 0)
			{
				Axis axis = Axis::None;
				if (event.caxis.axis >= 0 && event.caxis.axis < 6)
					axis = (Axis)event.caxis.axis; // NOTE: These map directly to Engine Axis enum!

				float value;
				if (event.caxis.value >= 0)
					value = event.caxis.value / 32767.0f;
				else
					value = event.caxis.value / 32768.0f;

				state.controllers[index].on_axis(axis, value);
			}
		}
	}
}

void SDL2_Platform::sleep(int milliseconds)
{
	if (milliseconds >= 0)
		SDL_Delay((u32)milliseconds);
}

void SDL2_Platform::present()
{
	if (App::renderer().type == RendererType::OpenGL)
	{
		SDL_GL_SwapWindow(window);
	}

	// display the window
	// this avoids a short black screen on macoS
	if (!displayed)
	{
		SDL_ShowWindow(window);
		displayed = true;
	}
}

const char* SDL2_Platform::get_title()
{
	return SDL_GetWindowTitle(window);
}

void SDL2_Platform::set_title(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

void SDL2_Platform::get_position(int* x, int* y)
{
	SDL_GetWindowPosition(window, x, y);
}

void SDL2_Platform::set_position(int x, int y)
{
	SDL_SetWindowPosition(window, x, y);
}

bool SDL2_Platform::get_focused()
{
	auto flags = SDL_GetWindowFlags(window);
	return (flags & SDL_WINDOW_INPUT_FOCUS) != 0 && (flags & SDL_WINDOW_MINIMIZED) == 0;
}

void SDL2_Platform::set_fullscreen(bool enabled)
{
	if (enabled)
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(window, 0);
}

void SDL2_Platform::get_size(int* width, int* height)
{
	SDL_GetWindowSize(window, width, height);
}

void SDL2_Platform::set_size(int width, int height)
{
	SDL_SetWindowSize(window, width, height);
}

void SDL2_Platform::get_draw_size(int* width, int* height)
{
	if (App::renderer().type == RendererType::OpenGL)
	{
		SDL_GL_GetDrawableSize(window, width, height);
	}
	else
	{
		SDL_GetWindowSize(window, width, height);
	}
}

float SDL2_Platform::get_content_scale()
{
	// TODO:
	// This is incorrect! but for some reason the scale
	// is HUGE if I use the Display DPI on macOS :/
#if __APPLE__
	return 2.0f;
#endif

	// TODO:
	// is there a way to get this value properly? My Windows & Linux PC's both seem to thing 96 is correct
	const float hidpi_res = 96;

	int index = SDL_GetWindowDisplayIndex(window);
	if (index < 0)
	{
		Log::error(SDL_GetError());
		return 1.0f;
	}

	float ddpi, x, y;
	if (SDL_GetDisplayDPI(index, &ddpi, &x, &y) != 0)
	{
		Log::error(SDL_GetError());
		return 1.0f;
	}

	return (ddpi / hidpi_res);
}

const char* SDL2_Platform::app_path()
{
	if (base_path_value == nullptr)
		base_path_value = SDL_GetBasePath();
	return base_path_value;
}

const char* SDL2_Platform::user_path()
{
	if (user_path_value == nullptr)
	{
		auto& config = App::config();
		user_path_value = SDL_GetPrefPath(nullptr, config.name);
	}

	return user_path_value;
}

FileRef SDL2_Platform::file_open(const char* path, FileMode mode)
{
	const char* sdl_mode = "";

	switch (mode)
	{
	case FileMode::OpenRead:
		sdl_mode = "rb";
		break;
	case FileMode::Open:
		sdl_mode = "r+b";
		break;
	case FileMode::CreateWrite:
		sdl_mode = "wb";
		break;
	case FileMode::Create:
		sdl_mode = "w+b";
		break;
	}

	auto ptr = SDL_RWFromFile(path, sdl_mode);
	if (!ptr)
		return FileRef();

	return FileRef(new SDL2_File(ptr));
}

bool SDL2_Platform::file_exists(const char* path)
{
	return std::filesystem::is_regular_file(path);
}

bool SDL2_Platform::file_delete(const char* path)
{
	return std::filesystem::remove(path);
}

bool SDL2_Platform::dir_create(const char* path)
{
	return std::filesystem::create_directories(path);
}

bool SDL2_Platform::dir_exists(const char* path)
{
	return std::filesystem::is_directory(path);
}

bool SDL2_Platform::dir_delete(const char* path)
{
	return std::filesystem::remove_all(path) > 0;
}

void SDL2_Platform::dir_enumerate(Vector<FilePath>& list, const char* path, bool recursive)
{
	if (std::filesystem::is_directory(path))
	{
		if (recursive)
		{
			for (auto& p : std::filesystem::recursive_directory_iterator(path))
				list.emplace_back(p.path().string().c_str());
		}
		else
		{
			for (auto& p : std::filesystem::directory_iterator(path))
				list.emplace_back(p.path().string().c_str());
		}
	}
}

void SDL2_Platform::dir_explore(const char* path)
{
#if _WIN32

	ShellExecute(NULL, "open", path, NULL, NULL, SW_SHOWDEFAULT);

#elif __linux__

	system(String::fmt("xdg-open \"%s\"", path).cstr());

#else

	BLAH_ASSERT(false, "'dir_explore' not implemented");

#endif
}

void SDL2_Platform::set_clipboard(const char* text)
{
	SDL_SetClipboardText(text);
}

const char* SDL2_Platform::get_clipboard()
{
	// free previous clipboard text
	if (clipboard_text != nullptr)
		SDL_free(clipboard_text);

	clipboard_text = SDL_GetClipboardText();
	return clipboard_text;
}

void* SDL2_Platform::gl_get_func(const char* name)
{
	return SDL_GL_GetProcAddress(name);
}

void* SDL2_Platform::gl_context_create()
{
	void* pointer = SDL_GL_CreateContext(window);
	if (pointer == nullptr)
		Log::error("SDL_GL_CreateContext failed: %s", SDL_GetError());
	return pointer;
}

void SDL2_Platform::gl_context_make_current(void* context)
{
	SDL_GL_MakeCurrent(window, context);
}

void SDL2_Platform::gl_context_destroy(void* context)
{
	SDL_GL_DeleteContext(context);
}

void* SDL2_Platform::d3d11_get_hwnd()
{
#if _WIN32
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(window, &info);
	return info.info.win.window;
#else
	return nullptr;
#endif
}

void SDL2_Platform::open_url(const char* url)
{
	SDL_OpenURL(url);
}

Platform* Platform::try_make_platform(const Config& config)
{
	return new SDL2_Platform();
}

#endif // BLAH_PLATFORM_SDL2

