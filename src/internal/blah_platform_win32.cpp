#ifdef BLAH_PLATFORM_WIN32

// Note:
// This is unfinished! It is missing Controller Support!

#include "blah_platform.h"
#include "blah_internal.h"
#include <blah_input.h>
#include <blah_app.h>
#include <blah_filesystem.h>
#include <blah_common.h>
#include <blah_time.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winuser.h>    // for SetProcessDPIAware
#include <filesystem>   // for File Reading/Writing
#include <shellapi.h>   // for file explore
#include <shlobj.h>     // for known folder
#include <chrono>       // for ticks method
#include <Xinput.h>     // for XInput

namespace Blah
{
	using Duration = std::chrono::system_clock::duration;

	typedef HRESULT(WINAPI* DirectInput8Create_fn)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
	typedef DWORD(WINAPI* XInputGetCapabilities_fn)(DWORD, DWORD, XINPUT_CAPABILITIES*);
	typedef DWORD(WINAPI* XInputGetState_fn)(DWORD, XINPUT_STATE*);
	typedef void*(WINAPI* wglGetProcAddress_fn)(const char*);
	typedef HGLRC(WINAPI* wglCreateContext_fn)(HDC);
	typedef BOOL(WINAPI* wglDeleteContext_fn)(HGLRC);
	typedef BOOL(WINAPI* wglMakeCurrent_fn)(HDC, HGLRC);

	class Win32File : public File
	{
	private:
		HANDLE m_handle;
		LARGE_INTEGER m_size;

	public:
		Win32File(HANDLE handle);
		~Win32File();
		size_t length() override;
		size_t position() override;
		size_t seek(size_t position) override;
		size_t read(void* buffer, size_t length) override;
		size_t write(const void* buffer, size_t length) override;
	};

	struct Win32_Platform : public Platform
	{
		// Main State
		HWND         hwnd;
		FilePath     working_directory;
		FilePath     user_directory;
		Duration     start_time;
		RECT         windowed_position;
		bool         fullscreen = false;
		InputState*  input_state = nullptr;
		String       clipboard;

		// XInput
		struct
		{
			HMODULE dll;
			XInputGetCapabilities_fn get_capabilities;
			XInputGetState_fn get_state;
		} xinput;

		struct Joystick
		{
			bool connected = false;
			bool accounted = false;
			GUID dinstance = GUID_NULL;
			DWORD xindex = 0;
		} joysticks[Input::max_controllers];

		// OpenGL Methods
		// These are only loaded if built using the OpenGL Backend
		struct
		{
			HMODULE dll;
			wglGetProcAddress_fn get_proc_address;
			wglCreateContext_fn create_context;
			wglDeleteContext_fn delete_context;
			wglMakeCurrent_fn make_current;
		} gl;

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
		void set_app_flags(u32 flags) override;
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

		void detect_joysticks();
	};

	// Main Windows Procedure callback
	LRESULT CALLBACK win32_window_procedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Converts Windows scancode to Blah key
	Key win32_scancode_to_key(WPARAM wParam, LPARAM lParam);
}

using namespace Blah;

Win32File::Win32File(HANDLE handle)
{
	m_handle = handle;
	m_size.QuadPart = 0;

	LARGE_INTEGER file_size;
	if (GetFileSizeEx(m_handle, &file_size))
		m_size = file_size;
}

Win32File::~Win32File()
{
	CloseHandle(m_handle);
}

size_t Win32File::length()
{
	return m_size.QuadPart;
}

size_t Win32File::position()
{
	LARGE_INTEGER move;
	LARGE_INTEGER result;

	move.QuadPart = 0;
	result.QuadPart = 0;

	SetFilePointerEx(m_handle, move, &result, FILE_CURRENT);

	return result.QuadPart;
}

size_t Win32File::seek(size_t position)
{
	LARGE_INTEGER move;
	LARGE_INTEGER result;

	move.QuadPart = position;
	result.QuadPart = 0;

	SetFilePointerEx(m_handle, move, &result, FILE_BEGIN);

	return result.QuadPart;
}

size_t Win32File::read(void* buffer, size_t length)
{
	static const DWORD read_step = 65536;

	size_t read = 0;

	while (read < length)
	{
		DWORD to_read = read_step;
		if (to_read > length - read)
			to_read = (DWORD)(length - read);

		DWORD moved = 0;
		if (ReadFile(m_handle, (char *)buffer + read, to_read, &moved, NULL))
			read += moved;

		if (moved < to_read)
			break;
	}

	return read;
}

size_t Win32File::write(const void* buffer, size_t length)
{
	static const DWORD write_step = 65536;

	size_t written = 0;

	while (written < length)
	{
		DWORD to_write = write_step;
		if (to_write > length - written)
			to_write = (DWORD)(length - written);

		DWORD moved = 0;
		if (WriteFile(m_handle, (char *)buffer + written, to_write, &moved, NULL))
			written += moved;

		if (moved < to_write)
			break;
	}

	return written;
}

bool Win32_Platform::init(const Config& config)
{
	// Required to call this for Windows
	SetProcessDPIAware();

	// Get the hInstance
	HINSTANCE hInstance = GetModuleHandle(NULL);

	// Create the Window Class
	WNDCLASS wc = {};
	wc.lpfnWndProc = DefWindowProc;
	wc.lpszClassName = "BLAH WINDOW";
	wc.hInstance = hInstance;
	wc.lpfnWndProc = win32_window_procedure;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = NULL;
	wc.lpszMenuName = NULL;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	// Register the Window class
	RegisterClass(&wc);

	// Create the Window Instance
	hwnd = CreateWindow("BLAH WINDOW", config.name, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 640, 480, NULL, NULL, hInstance, NULL);

	// Failed to create the Window
	if (hwnd == NULL)
	{
		Log::error("Window Creation Failed");
		return false;
	}

	// Setup Window Size based on content scale
	{
		auto scale = get_content_scale();
		int sw = (int)(App::config().width * scale);
		int sh = (int)(App::config().height * scale);
		set_size(sw, sh);
	}

	// Create the OpenGL device info
	if (config.renderer_type == RendererType::OpenGL)
	{
		// Load the DLL
		gl.dll = LoadLibraryA("opengl32.dll");
		if (gl.dll == NULL)
		{
			Log::error("OpenGL Instantiation Failed - unable to fine opengl32.dll");
			return false;
		}

		// Get the Windows GL functions we need
		gl.get_proc_address = (wglGetProcAddress_fn)GetProcAddress(gl.dll, "wglGetProcAddress");
		gl.create_context = (wglCreateContext_fn)GetProcAddress(gl.dll, "wglCreateContext");
		gl.delete_context = (wglDeleteContext_fn)GetProcAddress(gl.dll, "wglDeleteContext");
		gl.make_current = (wglMakeCurrent_fn)GetProcAddress(gl.dll, "wglMakeCurrent");

		// TODO:
		// Allow the user to apply (some of) these values before instantiation.
		// Also applies to the SDL2 Backend

		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd  
			1,                     // version number  
			PFD_DRAW_TO_WINDOW |   // support window  
			PFD_SUPPORT_OPENGL |   // support OpenGL  
			PFD_DOUBLEBUFFER,      // double buffered  
			PFD_TYPE_RGBA,         // RGBA type  
			32,                    // 32-bit color depth  
			0, 0, 0, 0, 0, 0,      // color bits ignored  
			0,                     // no alpha buffer  
			0,                     // shift bit ignored  
			0,                     // no accumulation buffer  
			0, 0, 0, 0,            // accum bits ignored  
			24,                    // 24-bit z-buffer  
			8,                     // 8-bit stencil buffer  
			0,                     // no auxiliary buffer  
			PFD_MAIN_PLANE,        // main layer  
			0,                     // reserved  
			0, 0, 0                // layer masks ignored  
		};

		HDC hdc = GetDC(hwnd);

		// get the best available match of pixel format for the device context   
		int pixel_format = ChoosePixelFormat(hdc, &pfd);

		// make that the pixel format of the device context  
		SetPixelFormat(hdc, pixel_format, &pfd);
	}

	// xinput api
	{
		const char* dlls[] = { "xinput1_4.dll", "xinput1_3.dll", "xinput9_1_0.dll", "xinput1_2.dll", "xinput1_1.dll", NULL };

		for (int i = 0; dlls[i]; i++)
		{
			xinput.dll = LoadLibraryA(dlls[i]);

			if (xinput.dll)
			{
				xinput.get_capabilities = (XInputGetCapabilities_fn)GetProcAddress(xinput.dll, "XInputGetCapabilities");
				xinput.get_state = (XInputGetState_fn)GetProcAddress(xinput.dll, "XInputGetState");
				break;
			}
		}

		if (!xinput.dll)
			Log::warn("Failed to find XInput dll; No Controller Support");
	}

	// Get Working Directory
	{
		TCHAR buffer[MAX_PATH];
		GetModuleFileName(NULL, buffer, MAX_PATH);

		auto normalized = Path::normalize(buffer);
		auto end = normalized.last_index_of('/');;
		if (end >= 0)
			working_directory = FilePath(normalized.begin(), normalized.begin() + end);
		else
			working_directory = normalized;
		working_directory.append("/");
	}

	// Get Application User Directory
	{
		PWSTR path = NULL;
		if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_CREATE, NULL, &path)))
		{
			auto end = path;
			while (*end != 0) end++;

			FilePath result;
			result.append_utf16((u16*)path, (u16*)end);

			user_directory = Path::join(Path::normalize(result), config.name) + "/";
		}
		CoTaskMemFree(path);
	}

	// Reset our game timer
	start_time = std::chrono::system_clock::now().time_since_epoch();

	// Not currently fullscreen
	fullscreen = false;

	// Finished Platform Setup
	return true;
}

void Win32_Platform::ready()
{
	// Display the game window
	ShowWindow(hwnd, SW_SHOW);
}

void Win32_Platform::shutdown()
{
	if (xinput.dll)
		FreeLibrary(xinput.dll);

	if (gl.dll)
		FreeLibrary(gl.dll);

	DestroyWindow(hwnd);
}

u64 Win32_Platform::ticks()
{
	// Todo:
	// This should account for whatever Time::ticks_per_second is set to

	auto now = std::chrono::system_clock::now().time_since_epoch();
	return std::chrono::duration_cast<std::chrono::microseconds>(now - start_time).count();
}

void Win32_Platform::update(InputState& state)
{
	// store reference to input state
	bool first_update = input_state == nullptr;
	input_state = &state;

	// if this is the first update, poll joysticks that are already connected
	if (first_update)
		detect_joysticks();

	// Catch & Dispatch Window Messages
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Win32_Platform::sleep(int milliseconds)
{
	if (milliseconds > 0)
		Sleep(milliseconds);
}

void Win32_Platform::present()
{
	if (App::renderer().type == RendererType::OpenGL)
	{
		HDC hdc = GetDC(hwnd);
		SwapBuffers(hdc);
	}
}

const char* Win32_Platform::get_title()
{
	return nullptr;
}

void Win32_Platform::set_title(const char* title)
{
	SetWindowText(hwnd, title);
}

void Win32_Platform::get_position(int* x, int* y)
{
	RECT rect;
	if (GetWindowRect(hwnd, &rect))
	{
		*x = rect.left;
		*y = rect.top;
	}
}

void Win32_Platform::set_position(int x, int y)
{
	int w, h;
	get_size(&w, &h);
	SetWindowPos(hwnd, NULL, x, y, w, h, 0);
}

bool Win32_Platform::get_focused()
{
	Log::warn("App::focused not implemented for Win32 yet");
	return true;
}

void Win32_Platform::set_app_flags(u32 flags)
{
	// toggle fullscreen
	{
		bool enabled = (flags & Flags::Fullscreen) != 0;
		if (fullscreen == enabled)
			return;
		fullscreen = enabled;

		if (fullscreen)
		{
			GetWindowRect(hwnd, &windowed_position);

			int w = GetSystemMetrics(SM_CXSCREEN);
			int h = GetSystemMetrics(SM_CYSCREEN);
			SetWindowLongPtr(hwnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
			SetWindowPos(hwnd, HWND_TOP, 0, 0, w, h, 0);
			ShowWindow(hwnd, SW_SHOW);
		}
		else
		{
			SetWindowLongPtr(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
			SetWindowPos(hwnd, HWND_TOP,
				windowed_position.left,
				windowed_position.top,
				windowed_position.right - windowed_position.left,
				windowed_position.bottom - windowed_position.top, 0);
			ShowWindow(hwnd, SW_SHOW);
		}
	}

	// toggle resizable
	// TODO: ...
}

void Win32_Platform::get_size(int* width, int* height)
{
	RECT rect;
	if (GetClientRect(hwnd, &rect))
	{
		*width = rect.right - rect.left;
		*height = rect.bottom - rect.top;
	}
}

void Win32_Platform::set_size(int width, int height)
{
	RECT client_rect;
	RECT border_rect;

	GetClientRect(hwnd, &client_rect);
	GetWindowRect(hwnd, &border_rect);

	int border_width = (border_rect.right - border_rect.left) - (client_rect.right - client_rect.left);
	int border_height = (border_rect.bottom - border_rect.top) - (client_rect.bottom - client_rect.top);

	SetWindowPos(hwnd, NULL, border_rect.left, border_rect.top, width + border_width, height + border_height, 0);
}

void Win32_Platform::get_draw_size(int* width, int* height)
{
	RECT rect;
	if (GetClientRect(hwnd, &rect))
	{
		*width = rect.right - rect.left;
		*height = rect.bottom - rect.top;
	}
}

float Win32_Platform::get_content_scale()
{
	// base value of Windows DPI
	// as seen here: https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getdpiforwindow
	constexpr float base_raw_value = 96.0f;

	UINT raw_value = GetDpiForWindow(hwnd);

	return (raw_value / base_raw_value);
}

const char* Win32_Platform::app_path()
{
	return working_directory.cstr();
}

const char* Win32_Platform::user_path()
{
	return user_directory.cstr();
}

bool Win32_Platform::file_exists(const char* path)
{
	return std::filesystem::is_regular_file(path);
}

bool Win32_Platform::file_delete(const char* path)
{
	return std::filesystem::remove(path);
}

bool Win32_Platform::dir_create(const char* path)
{
	std::error_code error;
	return std::filesystem::create_directories(path, error);
}

bool Win32_Platform::dir_exists(const char* path)
{
	return std::filesystem::is_directory(path);
}

bool Win32_Platform::dir_delete(const char* path)
{
	return std::filesystem::remove_all(path) > 0;
}

void Win32_Platform::dir_enumerate(Vector<FilePath>& list, const char* path, bool recursive)
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

void Win32_Platform::dir_explore(const char* path)
{
	ShellExecute(NULL, "open", path, NULL, NULL, SW_SHOWDEFAULT);
}

FileRef Win32_Platform::file_open(const char* path, FileMode mode)
{
	int access = 0;
	int creation = 0;

	switch (mode)
	{
	case FileMode::OpenRead:
		access = GENERIC_READ;
		creation = OPEN_EXISTING;
		break;
	case FileMode::Open:
		access = GENERIC_READ | GENERIC_WRITE;
		creation = OPEN_EXISTING;
		break;
	case FileMode::CreateWrite:
		access = GENERIC_WRITE;
		creation = CREATE_ALWAYS;
		break;
	case FileMode::Create:
		access = GENERIC_READ | GENERIC_WRITE;
		creation = CREATE_ALWAYS;
		break;
	}

	auto result = CreateFile(path, access, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, creation, FILE_ATTRIBUTE_NORMAL, NULL);

	if (result == INVALID_HANDLE_VALUE)
		return FileRef();

	return FileRef(new Win32File(result));
}

void* Win32_Platform::gl_get_func(const char* name)
{
	// this check is taken from https://www.khronos.org/opengl/wiki/Load_OpenGL_Functions
	// wglGetProcAddress doesn't always return valid pointers for some specific methods?

	void* p = (void*)gl.get_proc_address(name);
	if ((p == 0) ||
		(p == (void*)0x1) ||
		(p == (void*)0x2) ||
		(p == (void*)0x3) ||
		(p == (void*)-1))
	{
		p = (void*)GetProcAddress(gl.dll, name);
	}

	return p;
}

void* Win32_Platform::gl_context_create()
{
	HDC hdc = GetDC(hwnd);
	return gl.create_context(hdc);
}

void Win32_Platform::gl_context_make_current(void* context)
{
	if (context != nullptr)
	{
		HDC hdc = GetDC(hwnd);
		gl.make_current(hdc, (HGLRC)context);
	}
	else
		gl.make_current(NULL, NULL);
}

void Win32_Platform::gl_context_destroy(void* context)
{
	gl.delete_context((HGLRC)context);
}

void* Win32_Platform::d3d11_get_hwnd()
{
	return hwnd;
}

void Win32_Platform::set_clipboard(const char* text)
{
	auto len = strlen(text);
	if (auto glob = GlobalAlloc(GMEM_MOVEABLE, len))
	{
		if (auto data = GlobalLock(glob))
		{
			memcpy(data, text, len);
			GlobalUnlock(glob);

			if (OpenClipboard(nullptr))
			{
				SetClipboardData(CF_TEXT, data);
				CloseClipboard();
			}
		}

		GlobalFree(glob);
	}
}

const char* Win32_Platform::get_clipboard()
{
	if (OpenClipboard(nullptr))
	{
		HANDLE data = GetClipboardData(CF_TEXT);
		if (data)
		{
			auto text = static_cast<const char*>(GlobalLock(data));
			if (text)
				clipboard = text;
			GlobalUnlock(data);
		}
		CloseClipboard();
	}

	return clipboard.cstr();
}

void Win32_Platform::open_url(const char* url)
{
	auto cmd = String("start ") + url;
	system(cmd.cstr());
}

void Win32_Platform::detect_joysticks()
{
	auto platform = ((Win32_Platform*)App::Internal::platform);

	// mark all joysticks as unnacounted for
	for (int i = 0; i < Input::max_controllers; i++)
		platform->joysticks[i].accounted = false;

	// check for xinput controllers
	if (platform->xinput.dll)
	{
		for (DWORD index = 0; index < XUSER_MAX_COUNT; index++)
		{
			// can't get capabilities; not connected
			XINPUT_CAPABILITIES xic;
			if (platform->xinput.get_capabilities(index, 0, &xic) != ERROR_SUCCESS)
				continue;

			// already connected
			bool already_connected = false;
			for (int i = 0; i < Input::max_controllers; i++)
			{
				auto& it = platform->joysticks[i];
				if (it.connected && it.dinstance == GUID_NULL && it.xindex == index)
				{
					it.accounted = true;
					already_connected = true;
					break;
				}
			}

			if (already_connected)
				continue;

			// find an empty slot and mark connected
			for (int i = 0; i < Input::max_controllers; i++)
			{
				auto& it = platform->joysticks[i];
				if (!it.connected)
				{
					it.connected = it.accounted = true;
					it.dinstance = GUID_NULL;
					it.xindex = index;

					Log::info("Connected XInput [%i]", i);

					// TODO:
					// Get Product Info & Proper Name
					platform->input_state->controllers[i].on_connect("Xbox Controller", true, 15, 6, 0, 0, 0);

					break;
				}
			}
		}
	}

	// call disconnect on joysticks that aren't accounted for
	for (int i = 0; i < Input::max_controllers; i++)
	{
		auto& it = platform->joysticks[i];
		if (it.connected && !it.accounted)
		{
			Log::info("Disconnected [%i]", i);
			platform->input_state->controllers[i].on_disconnect();
			it = Win32_Platform::Joystick();
		}
	}
}

LRESULT CALLBACK Blah::win32_window_procedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto platform = ((Win32_Platform*)App::Internal::platform);
	auto input_state = platform->input_state;

	switch (msg)
	{
	case WM_CLOSE:
	{
		auto& config = App::config();
		if (config.on_exit_request != nullptr)
			config.on_exit_request();
		return 0;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// Controller connected event
	case WM_DEVICECHANGE:
	{
		// DBT_DEVNODES_CHANGED = 0x0007
		// https://docs.microsoft.com/en-us/windows/win32/devio/wm-devicechange
		if (wParam == 0x0007)
			platform->detect_joysticks();
		return 0;
	}

	// Mouse Input
	case WM_LBUTTONDOWN:
		input_state->mouse.on_press(MouseButton::Left);
		return 0;

	case WM_LBUTTONUP:
		input_state->mouse.on_release(MouseButton::Left);
		return 0;

	case WM_RBUTTONDOWN:
		input_state->mouse.on_press(MouseButton::Right);
		return 0;

	case WM_RBUTTONUP:
		input_state->mouse.on_release(MouseButton::Right);
		return 0;

	case WM_MBUTTONDOWN:
		input_state->mouse.on_press(MouseButton::Middle);
		return 0;

	case WM_MBUTTONUP:
		input_state->mouse.on_release(MouseButton::Middle);
		return 0;

	case WM_MOUSEMOVE:
		input_state->mouse.on_move(Vec2f((float)((u16)lParam), (float)(lParam >> 16)), Vec2f::zero);
		return 0;

	case WM_MOUSEWHEEL:
		input_state->mouse.wheel = Point(0, GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
		return 0;

		// Text Input
	case WM_UNICHAR:
		if (wParam == UNICODE_NOCHAR)
			return 1;
	case WM_CHAR:
	{
		String result;
		result.append((u32)wParam);
		if (result.length() > 0)
			input_state->keyboard.text += result.cstr();
		return 0;
	}

	// Keyboard Input
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		auto is_repeat = ((lParam & (1 << 30)) >> 30) == 1;
		if (!is_repeat)
		{
			auto key = Blah::win32_scancode_to_key(wParam, lParam);
			if (key != Key::Unknown)
				input_state->keyboard.on_press(key);
		}
		return 0;
	}

	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		auto key = Blah::win32_scancode_to_key(wParam, lParam);
		if (key != Key::Unknown)
			input_state->keyboard.on_release(key);
		return 0;
	}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

Blah::Key Blah::win32_scancode_to_key(WPARAM wParam, LPARAM lParam)
{
	// scancodes
	switch ((lParam >> 16) & 0xFF)
	{
	case 1:   return Key::Escape;
	case 2:   return Key::D1;
	case 3:   return Key::D2;
	case 4:   return Key::D3;
	case 5:   return Key::D4;
	case 6:   return Key::D5;
	case 7:   return Key::D6;
	case 8:   return Key::D7;
	case 9:   return Key::D8;
	case 10:  return Key::D9;
	case 11:  return Key::D0;
	case 12:  return Key::Minus;
	case 13:  return Key::Equals;
	case 14:  return Key::Backspace;
	case 15:  return Key::Tab;
	case 16:  return Key::Q;
	case 17:  return Key::W;
	case 18:  return Key::E;
	case 19:  return Key::R;
	case 20:  return Key::T;
	case 21:  return Key::Y;
	case 22:  return Key::U;
	case 23:  return Key::I;
	case 24:  return Key::O;
	case 25:  return Key::P;
	case 26:  return Key::LeftBracket;
	case 27:  return Key::RightBracket;
	case 28:  return Key::Enter;
	case 29:  return Key::LeftControl;
	case 30:  return Key::A;
	case 31:  return Key::S;
	case 32:  return Key::D;
	case 33:  return Key::F;
	case 34:  return Key::G;
	case 35:  return Key::H;
	case 36:  return Key::J;
	case 37:  return Key::K;
	case 38:  return Key::L;
	case 39:  return Key::Semicolon;
	case 40:  return Key::Apostrophe;
	case 41:  return Key::Tilde;
	case 42:  return Key::LeftShift;
	case 43:  return Key::Backslash;
	case 44:  return Key::Z;
	case 45:  return Key::X;
	case 46:  return Key::C;
	case 47:  return Key::V;
	case 48:  return Key::B;
	case 49:  return Key::N;
	case 50:  return Key::M;
	case 51:  return Key::Comma;
	case 52:  return Key::Period;
	case 53:  return Key::Slash;
	case 54:  return Key::RightShift;
	case 55:  return Key::PrintScreen;
	case 56:  return Key::LeftAlt;
	case 57:  return Key::Space;
	case 58:  return Key::Capslock;
	case 59:  return Key::F1;
	case 60:  return Key::F2;
	case 61:  return Key::F3;
	case 62:  return Key::F4;
	case 63:  return Key::F5;
	case 64:  return Key::F6;
	case 65:  return Key::F7;
	case 66:  return Key::F8;
	case 67:  return Key::F9;
	case 68:  return Key::F10;
	case 71:  return Key::Home;
	case 72:  return Key::Up;
	case 73:  return Key::PageUp;
	case 74:  return Key::KeypadMinus;
	case 75:  return Key::Left;
	case 76:  return Key::Keypad5;
	case 77:  return Key::Right;
	case 78:  return Key::KeypadPlus;
	case 79:  return Key::End;
	case 80:  return Key::Down;
	case 81:  return Key::PageDown;
	case 82:  return Key::Insert;
	case 83:  return Key::Delete;
	case 87:  return Key::F11;
	case 88:  return Key::F12;
	case 89:  return Key::Pause;
	case 91:  return Key::LeftOS;
	case 92:  return Key::RightOS;
	case 93:  return Key::Application;
	case 100: return Key::F13;
	case 101: return Key::F14;
	case 102: return Key::F15;
	case 103: return Key::F16;
	case 104: return Key::F17;
	case 105: return Key::F18;
	case 106: return Key::F19;
	}

	// virtual keys
	switch (wParam)
	{
	case VK_CANCEL:      return Key::Cancel;
	case VK_BACK:        return Key::Backspace;
	case VK_TAB:         return Key::Tab;
	case VK_CLEAR:       return Key::Clear;
	case VK_RETURN:      return Key::Enter;
	case VK_SHIFT:       return Key::LeftShift;
	case VK_CONTROL:     return Key::LeftControl;
	case VK_PAUSE:       return Key::Pause;
	case VK_CAPITAL:     return Key::Capslock;
	case VK_ESCAPE:      return Key::Escape;
	case VK_SPACE:       return Key::Space;
	case VK_PRIOR:       return Key::Prior;
	case VK_END:         return Key::End;
	case VK_HOME:        return Key::Home;
	case VK_LEFT:        return Key::Left;
	case VK_UP:          return Key::Up;
	case VK_RIGHT:       return Key::Right;
	case VK_DOWN:        return Key::Down;
	case VK_SELECT:      return Key::Select;
	case VK_PRINT:       return Key::PrintScreen;
	case VK_EXECUTE:     return Key::Execute;
	case VK_SNAPSHOT:    return Key::PrintScreen;
	case VK_INSERT:      return Key::Insert;
	case VK_DELETE:      return Key::Delete;
	case VK_HELP:        return Key::Help;
	case VK_LWIN:        return Key::LeftOS;
	case VK_RWIN:        return Key::RightOS;
	case VK_APPS:        return Key::Application;
	case VK_SLEEP:       return Key::Unknown;
	case VK_NUMPAD0:     return Key::Keypad0;
	case VK_NUMPAD1:     return Key::Keypad1;
	case VK_NUMPAD2:     return Key::Keypad2;
	case VK_NUMPAD3:     return Key::Keypad3;
	case VK_NUMPAD4:     return Key::Keypad4;
	case VK_NUMPAD5:     return Key::Keypad5;
	case VK_NUMPAD6:     return Key::Keypad6;
	case VK_NUMPAD7:     return Key::Keypad7;
	case VK_NUMPAD8:     return Key::Keypad8;
	case VK_NUMPAD9:     return Key::Keypad9;
	case VK_F1:          return Key::F1;
	case VK_F2:          return Key::F2;
	case VK_F3:          return Key::F3;
	case VK_F4:          return Key::F4;
	case VK_F5:          return Key::F5;
	case VK_F6:          return Key::F6;
	case VK_F7:          return Key::F7;
	case VK_F8:          return Key::F8;
	case VK_F9:          return Key::F9;
	case VK_F10:         return Key::F10;
	case VK_F11:         return Key::F11;
	case VK_F12:         return Key::F12;
	case VK_F13:         return Key::F13;
	case VK_F14:         return Key::F14;
	case VK_F15:         return Key::F15;
	case VK_F16:         return Key::F16;
	case VK_F17:         return Key::F17;
	case VK_F18:         return Key::F18;
	case VK_F19:         return Key::F19;
	case VK_F20:         return Key::F20;
	case VK_F21:         return Key::F21;
	case VK_F22:         return Key::F22;
	case VK_F23:         return Key::F23;
	case VK_F24:         return Key::F24;
	case VK_NUMLOCK:     return Key::Numlock;
	case VK_LSHIFT:      return Key::LeftShift;
	case VK_RSHIFT:      return Key::RightShift;
	case VK_LCONTROL:    return Key::LeftControl;
	case VK_RCONTROL:    return Key::RightControl;
	case VK_VOLUME_MUTE: return Key::Mute;
	case VK_VOLUME_DOWN: return Key::VolumeDown;
	case VK_VOLUME_UP:   return Key::VolumeUp;
	}

	return Key::Unknown;
}

Platform* Platform::try_make_platform(const Config& config)
{
	return new Win32_Platform();
}

#endif // BLAH_PLATFORM_WIN32
