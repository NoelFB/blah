#ifdef BLAH_PLATFORM_SDL2

#include "../internal/platform_backend.h"
#include "../internal/input_backend.h"
#include "../internal/graphics_backend.h"
#include <blah/input/input.h>
#include <blah/core/app.h>
#include <blah/core/filesystem.h>
#include <blah/core/common.h>
#include <blah/core/time.h>

#include <SDL.h>
#include <SDL_syswm.h>

#if _WIN32
// on Windows we're using the C++ <filesystem> API for now
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winuser.h>	// for SetProcessDPIAware
#include <filesystem>	// for File Reading/Writing
#include <shellapi.h>	// for file explore
namespace fs = std::filesystem;
#else
// on non-Windows we use POSIX standard file system stuff
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

using namespace Blah;

namespace
{
	SDL_Window* window = nullptr;
	SDL_Joystick* joysticks[Blah::Input::max_controllers];
	SDL_GameController* gamepads[Blah::Input::max_controllers];
	char* basePath = nullptr;
	char* userPath = nullptr;
	bool displayed = false;

	void sdl_log(void* userdata, int category, SDL_LogPriority priority, const char* message)
	{
		if (priority <= SDL_LOG_PRIORITY_INFO)
			Log::info(message);
		else if (priority <= SDL_LOG_PRIORITY_WARN)
			Log::warn(message);
		else
			Log::error(message);
	}

	int find_joystick_index(SDL_JoystickID instance_id)
	{
		for (int i = 0; i < Blah::Input::max_controllers; i++)
			if (joysticks[i] != nullptr && SDL_JoystickInstanceID(joysticks[i]) == instance_id)
				return i;
		return -1;
	}

	int find_gamepad_index(SDL_JoystickID instance_id)
	{
		for (int i = 0; i < Blah::Input::max_controllers; i++)
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

bool PlatformBackend::init(const Config* config)
{
	// Required to call this for Windows
	// I'm not sure why SDL2 doesn't do this on Windows automatically?
#if _WIN32
	SetProcessDPIAware();
#endif

	// TODO:
	// control this via some kind of config flag
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetOutputFunction(sdl_log, nullptr);

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
	if (App::renderer() == Renderer::OpenGL)
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
	// enable DirectX
	else if (App::renderer() == Renderer::D3D11)
	{

	}

	// create the window
	window = SDL_CreateWindow(config->name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, config->width, config->height, flags);
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
				SDL_SetWindowPosition(window, (int)(mode.w - config->width * dpi) / 2, (int)(mode.h - config->height * dpi) / 2);
				SDL_SetWindowSize(window, (int)(config->width * dpi), (int)(config->height * dpi));
			}
		}
	}
#endif

	// set window properties
	SDL_SetWindowResizable(window, SDL_TRUE);
	SDL_SetWindowMinimumSize(window, 256, 256);

	return true;
}

void PlatformBackend::ready()
{
#ifndef __EMSCRIPTEN__
	// enable V-Sync
	// TODO:
	// This should be a toggle or controllable in some way
	if (App::renderer() == Renderer::OpenGL)
		SDL_GL_SetSwapInterval(1);
#endif
}

void PlatformBackend::shutdown()
{
	if (window != nullptr)
		SDL_DestroyWindow(window);
	window = nullptr;
	displayed = false;

	if (basePath != nullptr)
		SDL_free(basePath);

	if (userPath != nullptr)
		SDL_free(userPath);

	SDL_Quit();
}

u64 PlatformBackend::ticks()
{
	auto counter = SDL_GetPerformanceCounter();
	auto per_second = (double)SDL_GetPerformanceFrequency();
	return (u64)(counter * (Time::ticks_per_second / per_second));
}

// Macro defined by X11 conflicts with MouseButton enum
#undef None

void PlatformBackend::frame()
{
	// update the mouse every frame
	{
		int winX, winY, x, y;
		SDL_GetWindowPosition(window, &winX, &winY);
		SDL_GetGlobalMouseState(&x, &y);

		InputBackend::on_mouse_move((float)(x - winX), (float)(y - winY));
		InputBackend::on_mouse_screen_move((float)x, (float)y);
	}

	// poll normal events
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			auto config = App::config();
			if (config->on_exit_request != nullptr)
				config->on_exit_request();
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
			InputBackend::on_mouse_down(btn);
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
			InputBackend::on_mouse_up(btn);
		}
		else if (event.type == SDL_MOUSEWHEEL)
		{
			InputBackend::on_mouse_wheel(Point(event.wheel.x, event.wheel.y));
		}
		// Keyboard
		else if (event.type == SDL_KEYDOWN)
		{
			if (event.key.repeat == 0)
				InputBackend::on_key_down((Key)event.key.keysym.scancode);
		}
		else if (event.type == SDL_KEYUP)
		{
			if (event.key.repeat == 0)
				InputBackend::on_key_up((Key)event.key.keysym.scancode);
		}
		else if (event.type == SDL_TEXTINPUT)
		{
			InputBackend::on_text_utf8(event.text.text);
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

				InputBackend::on_controller_connect(index, name, 0, button_count, axis_count, vendor, product, version);
			}
		}
		else if (event.type == SDL_JOYDEVICEREMOVED)
		{
			auto index = find_joystick_index(event.jdevice.which);
			if (index >= 0)
			{
				if (SDL_IsGameController(index) == SDL_FALSE)
				{
					InputBackend::on_controller_disconnect(index);
					SDL_JoystickClose(joysticks[index]);
				}
			}
		}
		else if (event.type == SDL_JOYBUTTONDOWN)
		{
			auto index = find_joystick_index(event.jdevice.which);
			if (index >= 0)
			{
				if (SDL_IsGameController(index) == SDL_FALSE)
					InputBackend::on_button_down(index, event.jbutton.button);
			}
		}
		else if (event.type == SDL_JOYBUTTONUP)
		{
			auto index = find_joystick_index(event.jdevice.which);
			if (index >= 0)
			{
				if (SDL_IsGameController(index) == SDL_FALSE)
					InputBackend::on_button_up(index, event.jbutton.button);
			}
		}
		else if (event.type == SDL_JOYAXISMOTION)
		{
			auto index = find_joystick_index(event.jdevice.which);
			if (index >= 0)
			{
				if (SDL_IsGameController(index) == SDL_FALSE)
				{
					float value;
					if (event.jaxis.value >= 0)
						value = event.jaxis.value / 32767.0f;
					else
						value = event.jaxis.value / 32768.0f;
					InputBackend::on_axis_move(index, event.jaxis.axis, value);
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

				InputBackend::on_controller_connect(index, name, 1, 15, 6, vendor, product, version);
			}
		}
		else if (event.type == SDL_CONTROLLERDEVICEREMOVED)
		{
			auto index = find_gamepad_index(event.cdevice.which);
			if (index >= 0)
			{
				InputBackend::on_controller_disconnect(index);
				SDL_GameControllerClose(gamepads[index]);
			}
		}
		else if (event.type == SDL_CONTROLLERBUTTONDOWN)
		{
			auto index = find_gamepad_index(event.cdevice.which);
			if (index >= 0)
			{
				int button = (int)Button::None;
				if (event.cbutton.button >= 0 && event.cbutton.button < 15)
					button = event.cbutton.button; // NOTE: These map directly to Engine Buttons enum!

				InputBackend::on_button_down(index, button);
			}
		}
		else if (event.type == SDL_CONTROLLERBUTTONUP)
		{
			auto index = find_gamepad_index(event.cdevice.which);
			if (index >= 0)
			{
				int button = (int)Button::None;
				if (event.cbutton.button >= 0 && event.cbutton.button < 15)
					button = event.cbutton.button; // NOTE: These map directly to Engine Buttons enum!

				InputBackend::on_button_up(index, button);
			}
		}
		else if (event.type == SDL_CONTROLLERAXISMOTION)
		{
			auto index = find_gamepad_index(event.cdevice.which);
			if (index >= 0)
			{
				int axis = (int)Axis::None;
				if (event.caxis.axis >= 0 && event.caxis.axis < 6)
					axis = event.caxis.axis; // NOTE: These map directly to Engine Axis enum!

				float value;
				if (event.caxis.value >= 0)
					value = event.caxis.value / 32767.0f;
				else
					value = event.caxis.value / 32768.0f;

				InputBackend::on_axis_move(index, axis, value);
			}
		}
	}
}

void PlatformBackend::sleep(int milliseconds)
{
	if (milliseconds >= 0)
		SDL_Delay((u32)milliseconds);
}

void PlatformBackend::present()
{
	if (App::renderer() == Renderer::OpenGL)
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

const char* PlatformBackend::get_title()
{
	return nullptr;
}

void PlatformBackend::set_title(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

void PlatformBackend::get_position(int* x, int* y)
{
	SDL_GetWindowPosition(window, x, y);
}

void PlatformBackend::set_position(int x, int y)
{
	SDL_SetWindowPosition(window, x, y);
}

void PlatformBackend::set_fullscreen(bool enabled)
{
	if (enabled)
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(window, 0);
}

void PlatformBackend::get_size(int* width, int* height)
{
	SDL_GetWindowSize(window, width, height);
}

void PlatformBackend::set_size(int width, int height)
{
	SDL_SetWindowSize(window, width, height);
}

void PlatformBackend::get_draw_size(int* width, int* height)
{
	if (App::renderer() == Renderer::OpenGL)
	{
		SDL_GL_GetDrawableSize(window, width, height);
	}
	else
	{
		SDL_GetWindowSize(window, width, height);
	}
}

float PlatformBackend::get_content_scale()
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

	int index = SDL_GetWindowDisplayIndex(window);
	if (index < 0)
		Log::error(SDL_GetError());

	float ddpi, x, y;
	if (SDL_GetDisplayDPI(index, &ddpi, &x, &y) != 0)
		Log::error(SDL_GetError());

	return (ddpi / hidpiRes);
}

// FILE IO

const char* PlatformBackend::app_path()
{
	if (basePath == nullptr)
		basePath = SDL_GetBasePath();
	return basePath;
}

const char* PlatformBackend::user_path()
{
	if (userPath == nullptr)
	{
		const Config* config = App::config();
		userPath = SDL_GetPrefPath(nullptr, config->name);
	}

	return userPath;
}

// Windows File System methods
#if _WIN32

bool PlatformBackend::file_exists(const char* path)
{
	return fs::is_regular_file(path);
}

bool PlatformBackend::file_delete(const char* path)
{
	return fs::remove(path);
}

bool PlatformBackend::dir_create(const char* path)
{
	std::error_code error;
	return fs::create_directories(path, error);
}

bool PlatformBackend::dir_exists(const char* path)
{
	return fs::is_directory(path);
}

bool PlatformBackend::dir_delete(const char* path)
{
	BLAH_ERROR("not implemented");
	return false;
}

void PlatformBackend::dir_enumerate(Vector<FilePath>& list, const char* path, bool recursive)
{
	if (fs::is_directory(path))
	{
		if (recursive)
		{
			for (auto& p : fs::recursive_directory_iterator(path))
				list.emplace_back(p.path().string().c_str());
		}
		else
		{
			for (auto& p : fs::directory_iterator(path))
				list.emplace_back(p.path().string().c_str());
		}
	}
}

void PlatformBackend::dir_explore(const char* path)
{
	ShellExecute(NULL, "open", path, NULL, NULL, SW_SHOWDEFAULT);
}

// Non-Windows File System Methods
#else

bool PlatformBackend::file_exists(const char* path)
{
	struct stat buffer;
	return (stat(path, &buffer) == 0) && S_ISREG(buffer.st_mode);
}

bool PlatformBackend::file_delete(const char* path)
{
	BLAH_ERROR("not implemented");
	return false;
}

bool PlatformBackend::dir_create(const char* path)
{
	char tmp[265];
	char* p = NULL;
	size_t len;

	snprintf(tmp, sizeof(tmp), "%s", path);
	len = strlen(tmp);
	if (tmp[len - 1] == '/')
		tmp[len - 1] = 0;
	for (p = tmp + 1; *p; p++)
		if (*p == '/') {
			*p = 0;
			mkdir(tmp, S_IRWXU);
			*p = '/';
		}
	return mkdir(tmp, S_IRWXU) == 0;
}

bool PlatformBackend::dir_exists(const char* path)
{
	struct stat buffer;
	return (stat(path, &buffer) == 0) && S_ISDIR(buffer.st_mode);
}

bool PlatformBackend::dir_delete(const char* path)
{
	BLAH_ERROR("not implemented");
	return false;
}

void PlatformBackend::dir_enumerate(Vector<FilePath>& list, const char* path, bool recursive)
{
	DIR* dirp = opendir(path);
	if (dirp != NULL)
	{
		struct dirent* dp;
		while ((dp = readdir(dirp)) != NULL)
		{
			if (dp->d_name[0] == '.')
				continue;

			FilePath subpath = FilePath(path);
			if (subpath.end()[-1] != '/') subpath = subpath.append("/");
			subpath = subpath.append(dp->d_name);
			list.push_back(subpath);

			if (recursive && dp->d_type == DT_DIR)
				dir_enumerate(list, subpath + "/", true);
		}
		closedir(dirp);
	}
}

void PlatformBackend::dir_explore(const char* path)
{
	BLAH_ERROR("'dir_explore' Not Implemented");
}

#endif

bool PlatformBackend::file_open(const char* path, PlatformBackend::FileHandle* handle, FileMode mode)
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
	*handle = (PlatformBackend::FileHandle)ptr;
	return ptr != nullptr;
}

i64 PlatformBackend::file_length(PlatformBackend::FileHandle stream)
{
	return SDL_RWsize((SDL_RWops*)stream);
}

i64 PlatformBackend::file_position(PlatformBackend::FileHandle stream)
{
	return SDL_RWtell((SDL_RWops*)stream);
}

i64 PlatformBackend::file_seek(PlatformBackend::FileHandle stream, i64 seekTo)
{
	return SDL_RWseek((SDL_RWops*)stream, seekTo, RW_SEEK_SET);
}

i64 PlatformBackend::file_read(PlatformBackend::FileHandle stream, void* ptr, i64 length)
{
	return SDL_RWread((SDL_RWops*)stream, ptr, sizeof(char), length);
}

i64 PlatformBackend::file_write(PlatformBackend::FileHandle stream, const void* ptr, i64 length)
{
	return SDL_RWwrite((SDL_RWops*)stream, ptr, sizeof(char), length);
}

void PlatformBackend::file_close(PlatformBackend::FileHandle stream)
{
	if (stream != nullptr)
		SDL_RWclose((SDL_RWops*)stream);
}

void* PlatformBackend::gl_get_func(const char* name)
{
	return SDL_GL_GetProcAddress(name);
}

void* PlatformBackend::gl_context_create()
{
	void* pointer = SDL_GL_CreateContext(window);
	if (pointer == nullptr)
		Log::error("SDL_GL_CreateContext failed: %s", SDL_GetError());
	return pointer;
}

void PlatformBackend::gl_context_make_current(void* context)
{
	SDL_GL_MakeCurrent(window, context);
}

void PlatformBackend::gl_context_destroy(void* context)
{
	SDL_GL_DeleteContext(context);
}

void* PlatformBackend::d3d11_get_hwnd()
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

#endif // BLAH_PLATFORM_SDL2
