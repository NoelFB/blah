#ifdef BLAH_PLATFORM_SDL2

#include "platform.h"
#include "input.h"
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
	// Blah SDL2 Platform State
	struct SDL2Platform
	{
		SDL_Window* window = nullptr;
		SDL_Joystick* joysticks[Input::max_controllers];
		SDL_GameController* gamepads[Input::max_controllers];
		char* base_path = nullptr;
		char* user_path = nullptr;
		bool displayed = false;
	} g_platform;

	// Blah SDL2 File
	struct SDL2File : public File
	{
		SDL_RWops* handle;
		SDL2File(SDL_RWops* handle) : handle(handle) { }
		~SDL2File() { if (handle) SDL_RWclose(handle); }
		size_t length() override { return SDL_RWsize(handle); }
		size_t position() override { return SDL_RWtell(handle); }
		size_t seek(size_t position) override { return SDL_RWseek(handle, position, RW_SEEK_SET); }
		size_t read(unsigned char* buffer, size_t length) override { return SDL_RWread(handle, buffer, sizeof(char), length); }
		size_t write(const unsigned char* buffer, size_t length) override { return SDL_RWwrite(handle, buffer, sizeof(char), length); }
	};

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
}

using namespace Blah;

bool Platform::init(const Config& config)
{
	g_platform = SDL2Platform();

	// Required to call this for Windows
	// I'm not sure why SDL2 doesn't do this on Windows automatically?
#if _WIN32
	SetProcessDPIAware();
#endif

	// TODO:
	// control this via some kind of config flag
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
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
	g_platform.window = SDL_CreateWindow(config.name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, config.width, config.height, flags);
	if (g_platform.window == nullptr)
	{
		Log::error("Failed to create a Window");
		return false;
	}

	// Scale Window to monitor for High DPI displays
	// Other platforms do this automatically ... Windows we need to explitely do so
#if _WIN32
	{
		// find the display index
		int display = SDL_GetWindowDisplayIndex(g_platform.window);
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
				SDL_SetWindowPosition(g_platform.window, (int)(mode.w - config.width * dpi) / 2, (int)(mode.h - config.height * dpi) / 2);
				SDL_SetWindowSize(g_platform.window, (int)(config.width * dpi), (int)(config.height * dpi));
			}
		}
	}
#endif

	// set window properties
	SDL_SetWindowResizable(g_platform.window, SDL_TRUE);
	SDL_SetWindowMinimumSize(g_platform.window, 256, 256);

	return true;
}

void Platform::ready()
{
#ifndef __EMSCRIPTEN__
	// enable V-Sync
	// TODO:
	// This should be a toggle or controllable in some way
	if (App::renderer().type == RendererType::OpenGL)
		SDL_GL_SetSwapInterval(1);
#endif
}

void Platform::shutdown()
{
	if (g_platform.window != nullptr)
		SDL_DestroyWindow(g_platform.window);
	g_platform.window = nullptr;
	g_platform.displayed = false;

	if (g_platform.base_path != nullptr)
		SDL_free(g_platform.base_path);

	if (g_platform.user_path != nullptr)
		SDL_free(g_platform.user_path);

	SDL_Quit();
}

u64 Platform::ticks()
{
	auto counter = SDL_GetPerformanceCounter();
	auto per_second = (double)SDL_GetPerformanceFrequency();
	return (u64)(counter * (Time::ticks_per_second / per_second));
}

void Platform::update(InputState& state)
{
	// update the mouse every frame
	{
		int win_x, win_y, x, y;

		SDL_GetWindowPosition(g_platform.window, &win_x, &win_y);
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
				auto ptr = g_platform.joysticks[index] = SDL_JoystickOpen(index);
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
			auto index = blah_sdl_find_joystick_index(g_platform.joysticks, event.jdevice.which);
			if (index >= 0)
			{
				if (SDL_IsGameController(index) == SDL_FALSE)
				{
					state.controllers[index].on_disconnect();
					SDL_JoystickClose(g_platform.joysticks[index]);
				}
			}
		}
		else if (event.type == SDL_JOYBUTTONDOWN)
		{
			auto index = blah_sdl_find_joystick_index(g_platform.joysticks, event.jdevice.which);
			if (index >= 0)
			{
				if (SDL_IsGameController(index) == SDL_FALSE)
					state.controllers[index].on_press((Button)event.jbutton.button);
			}
		}
		else if (event.type == SDL_JOYBUTTONUP)
		{
			auto index = blah_sdl_find_joystick_index(g_platform.joysticks, event.jdevice.which);
			if (index >= 0)
			{
				if (SDL_IsGameController(index) == SDL_FALSE)
					state.controllers[index].on_release((Button)event.jbutton.button);
			}
		}
		else if (event.type == SDL_JOYAXISMOTION)
		{
			auto index = blah_sdl_find_joystick_index(g_platform.joysticks, event.jdevice.which);
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
				auto ptr = g_platform.gamepads[index] = SDL_GameControllerOpen(index);
				auto name = SDL_GameControllerName(ptr);
				auto vendor = SDL_GameControllerGetVendor(ptr);
				auto product = SDL_GameControllerGetProduct(ptr);
				auto version = SDL_GameControllerGetProductVersion(ptr);

				state.controllers[index].on_connect(name, 1, 15, 6, vendor, product, version);
			}
		}
		else if (event.type == SDL_CONTROLLERDEVICEREMOVED)
		{
			auto index = blah_sdl_find_gamepad_index(g_platform.gamepads, event.cdevice.which);
			if (index >= 0)
			{
				state.controllers[index].on_disconnect();
				SDL_GameControllerClose(g_platform.gamepads[index]);
			}
		}
		else if (event.type == SDL_CONTROLLERBUTTONDOWN)
		{
			auto index = blah_sdl_find_gamepad_index(g_platform.gamepads, event.cdevice.which);
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
			auto index = blah_sdl_find_gamepad_index(g_platform.gamepads, event.cdevice.which);
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
			auto index = blah_sdl_find_gamepad_index(g_platform.gamepads, event.cdevice.which);
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

void Platform::sleep(int milliseconds)
{
	if (milliseconds >= 0)
		SDL_Delay((u32)milliseconds);
}

void Platform::present()
{
	if (App::renderer().type == RendererType::OpenGL)
	{
		SDL_GL_SwapWindow(g_platform.window);
	}

	// display the window
	// this avoids a short black screen on macoS
	if (!g_platform.displayed)
	{
		SDL_ShowWindow(g_platform.window);
		g_platform.displayed = true;
	}
}

const char* Platform::get_title()
{
	return SDL_GetWindowTitle(g_platform.window);
}

void Platform::set_title(const char* title)
{
	SDL_SetWindowTitle(g_platform.window, title);
}

void Platform::get_position(int* x, int* y)
{
	SDL_GetWindowPosition(g_platform.window, x, y);
}

void Platform::set_position(int x, int y)
{
	SDL_SetWindowPosition(g_platform.window, x, y);
}

bool Platform::get_focused()
{
	auto flags = SDL_GetWindowFlags(g_platform.window);
	return (flags & SDL_WINDOW_INPUT_FOCUS) != 0 && (flags & SDL_WINDOW_MINIMIZED) == 0;
}

void Platform::set_fullscreen(bool enabled)
{
	if (enabled)
		SDL_SetWindowFullscreen(g_platform.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(g_platform.window, 0);
}

void Platform::get_size(int* width, int* height)
{
	SDL_GetWindowSize(g_platform.window, width, height);
}

void Platform::set_size(int width, int height)
{
	SDL_SetWindowSize(g_platform.window, width, height);
}

void Platform::get_draw_size(int* width, int* height)
{
	if (App::renderer().type == RendererType::OpenGL)
	{
		SDL_GL_GetDrawableSize(g_platform.window, width, height);
	}
	else
	{
		SDL_GetWindowSize(g_platform.window, width, height);
	}
}

float Platform::get_content_scale()
{
	// TODO:
	// This is incorrect! but for some reason the scale
	// is HUGE if I use the Display DPI on macOS :/
#if __APPLE__
	return 2.0f;
#endif

#if _WIN32
	float hidpiRes = 96;
#else
	float hidpiRes = 72;
#endif

	int index = SDL_GetWindowDisplayIndex(g_platform.window);
	if (index < 0)
		Log::error(SDL_GetError());

	float ddpi, x, y;
	if (SDL_GetDisplayDPI(index, &ddpi, &x, &y) != 0)
		Log::error(SDL_GetError());

	return (ddpi / hidpiRes);
}

// FILE IO

const char* Platform::app_path()
{
	if (g_platform.base_path == nullptr)
		g_platform.base_path = SDL_GetBasePath();
	return g_platform.base_path;
}

const char* Platform::user_path()
{
	if (g_platform.user_path == nullptr)
	{
		auto& config = App::config();
		g_platform.user_path = SDL_GetPrefPath(nullptr, config.name);
	}

	return g_platform.user_path;
}

FileRef Platform::file_open(const char* path, FileMode mode)
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

	return FileRef(new SDL2File(ptr));
}

bool Platform::file_exists(const char* path)
{
	return std::filesystem::is_regular_file(path);
}

bool Platform::file_delete(const char* path)
{
	return std::filesystem::remove(path);
}

bool Platform::dir_create(const char* path)
{
	return std::filesystem::create_directories(path);
}

bool Platform::dir_exists(const char* path)
{
	return std::filesystem::is_directory(path);
}

bool Platform::dir_delete(const char* path)
{
	return std::filesystem::remove_all(path) > 0;
}

void Platform::dir_enumerate(Vector<FilePath>& list, const char* path, bool recursive)
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

void Platform::dir_explore(const char* path)
{
#if _WIN32

	ShellExecute(NULL, "open", path, NULL, NULL, SW_SHOWDEFAULT);

#elif __linux__

	system(String::fmt("xdg-open \"%s\"", path).cstr());

#else

	BLAH_ASSERT(false, "'dir_explore' not implemented");

#endif
}

void Platform::set_clipboard(const char* text)
{
	SDL_SetClipboardText(text);
}

const char* Platform::get_clipboard()
{
	return SDL_GetClipboardText();
}

void* Platform::gl_get_func(const char* name)
{
	return SDL_GL_GetProcAddress(name);
}

void* Platform::gl_context_create()
{
	void* pointer = SDL_GL_CreateContext(g_platform.window);
	if (pointer == nullptr)
		Log::error("SDL_GL_CreateContext failed: %s", SDL_GetError());
	return pointer;
}

void Platform::gl_context_make_current(void* context)
{
	SDL_GL_MakeCurrent(g_platform.window, context);
}

void Platform::gl_context_destroy(void* context)
{
	SDL_GL_DeleteContext(context);
}

void* Platform::d3d11_get_hwnd()
{
#if _WIN32
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(g_platform.window, &info);
	return info.info.win.window;
#else
	return nullptr;
#endif
}

void Platform::open_url(const char* url)
{
	SDL_OpenURL(url);
}

#endif // BLAH_PLATFORM_SDL2
