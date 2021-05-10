#ifdef BLAH_PLATFORM_WIN32

// Note:
// This backend implementation is unfinished! 
// It's missing a few things, namely:
// - Controller Support
// (And error testing)

#include "../internal/platform_backend.h"
#include "../internal/input_backend.h"
#include "../internal/graphics_backend.h"
#include <blah/input.h>
#include <blah/app.h>
#include <blah/filesystem.h>
#include <blah/common.h>
#include <blah/time.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winuser.h>    // for SetProcessDPIAware
#include <filesystem>   // for File Reading/Writing
#include <shellapi.h>   // for file explore
#include <shlobj.h>     // for known folder
#include <chrono>       // for ticks method

using namespace Blah;
namespace fs = std::filesystem;

namespace Blah
{
	// Primary Window
	HWND g_hwnd;

	// Working Directories
	FilePath g_working_directory;
	FilePath g_user_directory;

	// Timestamp for calculating ticks
	std::chrono::system_clock::duration g_start_time;

	// OpenGL Methods
	// These are only loaded if built using the OpenGL Backend
	HMODULE g_opengl_dll;
	void* (WINAPI* g_wglGetProcAddress) (const char* proc);
	HGLRC(WINAPI* g_wglCreateContext) (HDC hdc);
	BOOL(WINAPI* g_wglDeleteContext) (HGLRC hglrc);
	BOOL(WINAPI* g_wglMakeCurrent) (HDC hdc, HGLRC hglrc);

	// fullscreen state
	RECT g_windowed_position;
	bool g_fullscreen = false;

	// Converts Windows scancode to Blah key
	Key scancode_to_key(WPARAM wParam, LPARAM lParam);

	// Main Windows Procedure callback
	LRESULT CALLBACK window_procedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Custom File class
	class Blah_Win32_File : public File
	{
	private:
		HANDLE m_handle;

	public:
		Blah_Win32_File(HANDLE handle)
		{
			m_handle = handle;
		}

		~Blah_Win32_File()
		{
			CloseHandle(m_handle);
		}

		size_t length() override
		{
			// Todo: cache this value? not sure how performant it is

			LARGE_INTEGER file_size;
			if (GetFileSizeEx(m_handle, &file_size))
				return file_size.QuadPart;

			return 0;
		}

		size_t position() override
		{
			LARGE_INTEGER move;
			LARGE_INTEGER result;

			move.QuadPart = 0;
			result.QuadPart = 0;

			SetFilePointerEx(m_handle, move, &result, FILE_CURRENT);

			return result.QuadPart;
		}

		size_t seek(size_t position) override
		{
			LARGE_INTEGER move;
			LARGE_INTEGER result;

			move.QuadPart = position;
			result.QuadPart = 0;

			SetFilePointerEx(m_handle, move, &result, FILE_BEGIN);

			return result.QuadPart;
		}

		size_t read(unsigned char* buffer, size_t length) override
		{
			static const DWORD read_step = 65536;

			size_t read = 0;

			while (read < length)
			{
				DWORD to_read = read_step;
				if (to_read > length - read)
					to_read = (DWORD)(length - read);

				DWORD moved = 0;
				if (ReadFile(m_handle, buffer + read, to_read, &moved, NULL))
					read += moved;

				if (moved < to_read)
					break;
			}

			return read;
		}

		size_t write(const unsigned char* buffer, size_t length) override
		{
			static const DWORD write_step = 65536;

			size_t written = 0;

			while (written < length)
			{
				DWORD to_write = write_step;
				if (to_write > length - written)
					to_write = (DWORD)(length - written);

				DWORD moved = 0;
				if (WriteFile(m_handle, buffer + written, to_write, &moved, NULL))
					written += moved;

				if (moved < to_write)
					break;
			}

			return written;
		}
	};

	bool PlatformBackend::init(const Config* config)
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
		wc.lpfnWndProc = window_procedure;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hIcon = NULL;
		wc.lpszMenuName = NULL;
		wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;

		// Register the Window class
		RegisterClass(&wc);

		// Create the Window Instance
		g_hwnd = CreateWindow("BLAH WINDOW", config->name, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 640, 480, NULL, NULL, hInstance, NULL);

		// Failed to create the Window
		if (g_hwnd == NULL)
		{
			Log::error("Window Creation Failed");
			return false;
		}

		// Create the OpenGL device info
		if (App::renderer() == Renderer::OpenGL)
		{
			// Load the DLL
			g_opengl_dll = LoadLibraryA("opengl32.dll");
			if (g_opengl_dll == NULL)
			{
				Log::error("OpenGL Instantiation Failed - unable to fine opengl32.dll");
				return false;
			}

			// Get the Windows GL functions we need
			g_wglGetProcAddress = (void* (WINAPI*)(const char*))GetProcAddress(g_opengl_dll, "wglGetProcAddress");
			g_wglCreateContext = (HGLRC(WINAPI*) (HDC))GetProcAddress(g_opengl_dll, "wglCreateContext");
			g_wglDeleteContext = (BOOL(WINAPI*) (HGLRC))GetProcAddress(g_opengl_dll, "wglDeleteContext");
			g_wglMakeCurrent = (BOOL(WINAPI*) (HDC, HGLRC))GetProcAddress(g_opengl_dll, "wglMakeCurrent");

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

			HDC hdc = GetDC(g_hwnd);

			// get the best available match of pixel format for the device context   
			int pixel_format = ChoosePixelFormat(hdc, &pfd);

			// make that the pixel format of the device context  
			SetPixelFormat(hdc, pixel_format, &pfd);
		}

		// Reset our game timer
		g_start_time = std::chrono::system_clock::now().time_since_epoch();

		// Get Working Directory
		{
			TCHAR buffer[MAX_PATH];
			GetModuleFileName(NULL, buffer, MAX_PATH);

			auto normalized = Path::normalize(buffer);
			auto end = normalized.last_index_of('/');;
			if (end >= 0)
				g_working_directory = FilePath(normalized.begin(), normalized.begin() + end);
			else
				g_working_directory = normalized;
			g_working_directory.append("/");
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

				g_user_directory = Path::join(Path::normalize(result), config->name) + "/";
			}
			CoTaskMemFree(path);
		}

		// Not currently fullscreen
		g_fullscreen = false;

		// Finished Platform Setup
		return true;
	}

	void PlatformBackend::ready()
	{
		// Setup Window Size
		{
			auto scale = get_content_scale();
			int sw = (int)(App::config()->width * scale);
			int sh = (int)(App::config()->height * scale);
			set_size(sw, sh);
		}

		// Display the game window
		ShowWindow(g_hwnd, SW_SHOW);
	}

	void PlatformBackend::shutdown()
	{
		DestroyWindow(g_hwnd);
	}

	u64 PlatformBackend::ticks()
	{
		// Todo:
		// This should account for whatever Time::ticks_per_second is set to

		auto now = std::chrono::system_clock::now().time_since_epoch();
		return std::chrono::duration_cast<std::chrono::microseconds>(now - g_start_time).count();
	}

	LRESULT CALLBACK window_procedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CLOSE:
		{
			auto config = App::config();
			if (config->on_exit_request != nullptr)
				config->on_exit_request();
			return 0;
		}

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

			// Mouse Input
		case WM_LBUTTONDOWN:
			InputBackend::on_mouse_down(MouseButton::Left);
			return 0;

		case WM_LBUTTONUP:
			InputBackend::on_mouse_up(MouseButton::Left);
			return 0;

		case WM_RBUTTONDOWN:
			InputBackend::on_mouse_down(MouseButton::Right);
			return 0;

		case WM_RBUTTONUP:
			InputBackend::on_mouse_up(MouseButton::Right);
			return 0;

		case WM_MBUTTONDOWN:
			InputBackend::on_mouse_down(MouseButton::Middle);
			return 0;

		case WM_MBUTTONUP:
			InputBackend::on_mouse_up(MouseButton::Middle);
			return 0;

		case WM_MOUSEMOVE:
			InputBackend::on_mouse_move((float)((u16)lParam), (float)(lParam >> 16));
			return 0;

		case WM_MOUSEWHEEL:
			InputBackend::on_mouse_wheel(Point(0, GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA));
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
				InputBackend::on_text_utf8(result.cstr());
			return 0;
		}

		// Keyboard Input
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			auto is_repeat = ((lParam & (1 << 30)) >> 30) == 1;
			if (!is_repeat)
			{
				auto key = scancode_to_key(wParam, lParam);
				if (key != Key::Unknown)
					InputBackend::on_key_down(key);
			}
			return 0;
		}

		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			auto key = scancode_to_key(wParam, lParam);
			if (key != Key::Unknown)
				InputBackend::on_key_up(key);
			return 0;
		}
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	void PlatformBackend::frame()
	{
		// Catch & Dispatch Window Messages
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void PlatformBackend::sleep(int milliseconds)
	{
		if (milliseconds > 0)
			Sleep(milliseconds);
	}

	void PlatformBackend::present()
	{
		if (App::renderer() == Renderer::OpenGL)
		{
			HDC hdc = GetDC(g_hwnd);
			SwapBuffers(hdc);
		}
	}

	const char* PlatformBackend::get_title()
	{
		return nullptr;
	}

	void PlatformBackend::set_title(const char* title)
	{
		SetWindowText(g_hwnd, title);
	}

	void PlatformBackend::get_position(int* x, int* y)
	{
		RECT rect;
		if (GetWindowRect(g_hwnd, &rect))
		{
			*x = rect.left;
			*y = rect.top;
		}
	}

	void PlatformBackend::set_position(int x, int y)
	{
		int w, h;
		get_size(&w, &h);
		SetWindowPos(g_hwnd, NULL, x, y, w, h, 0);
	}

	void PlatformBackend::set_fullscreen(bool enabled)
	{
		if (g_fullscreen == enabled)
			return;

		g_fullscreen = enabled;

		if (g_fullscreen)
		{
			GetWindowRect(g_hwnd, &g_windowed_position);

			int w = GetSystemMetrics(SM_CXSCREEN);
			int h = GetSystemMetrics(SM_CYSCREEN);
			SetWindowLongPtr(g_hwnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
			SetWindowPos(g_hwnd, HWND_TOP, 0, 0, w, h, 0);
			ShowWindow(g_hwnd, SW_SHOW);
		}
		else
		{
			SetWindowLongPtr(g_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
			SetWindowPos(g_hwnd, HWND_TOP,
				g_windowed_position.left,
				g_windowed_position.top,
				g_windowed_position.right - g_windowed_position.left,
				g_windowed_position.bottom - g_windowed_position.top, 0);
			ShowWindow(g_hwnd, SW_SHOW);
		}
	}

	void PlatformBackend::get_size(int* width, int* height)
	{
		RECT rect;
		if (GetClientRect(g_hwnd, &rect))
		{
			*width = rect.right - rect.left;
			*height = rect.bottom - rect.top;
		}
	}

	void PlatformBackend::set_size(int width, int height)
	{
		RECT client_rect;
		RECT border_rect;

		GetClientRect(g_hwnd, &client_rect);
		GetWindowRect(g_hwnd, &border_rect);

		int border_width = (border_rect.right - border_rect.left) - (client_rect.right - client_rect.left);
		int border_height = (border_rect.bottom - border_rect.top) - (client_rect.bottom - client_rect.top);

		SetWindowPos(g_hwnd, NULL, border_rect.left, border_rect.top, width + border_width, height + border_height, 0);
	}

	void PlatformBackend::get_draw_size(int* width, int* height)
	{
		RECT rect;
		if (GetClientRect(g_hwnd, &rect))
		{
			*width = rect.right - rect.left;
			*height = rect.bottom - rect.top;
		}
	}

	float PlatformBackend::get_content_scale()
	{
		// base value of Windows DPI
		// as seen here: https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getdpiforwindow
		constexpr float base_raw_value = 96.0f;

		UINT raw_value = GetDpiForWindow(g_hwnd);

		return (raw_value / base_raw_value);
	}

	const char* PlatformBackend::app_path()
	{
		return g_working_directory.cstr();
	}

	const char* PlatformBackend::user_path()
	{
		return g_user_directory.cstr();
	}

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
		BLAH_ASSERT(false, "not implemented");
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

	FileRef PlatformBackend::file_open(const char* path, FileMode mode)
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

		return FileRef(new Blah_Win32_File(result));
	}

	void* PlatformBackend::gl_get_func(const char* name)
	{
		// this check is taken from https://www.khronos.org/opengl/wiki/Load_OpenGL_Functions
		// wglGetProcAddress doesn't always return valid pointers for some specific methods?

		void* p = (void*)g_wglGetProcAddress(name);
		if ((p == 0) ||
			(p == (void*)0x1) ||
			(p == (void*)0x2) ||
			(p == (void*)0x3) ||
			(p == (void*)-1))
		{
			p = (void*)GetProcAddress(g_opengl_dll, name);
		}

		return p;
	}

	void* PlatformBackend::gl_context_create()
	{
		HDC hdc = GetDC(g_hwnd);
		return g_wglCreateContext(hdc);
	}

	void PlatformBackend::gl_context_make_current(void* context)
	{
		if (context != nullptr)
		{
			HDC hdc = GetDC(g_hwnd);
			g_wglMakeCurrent(hdc, (HGLRC)context);
		}
		else
			g_wglMakeCurrent(NULL, NULL);
	}

	void PlatformBackend::gl_context_destroy(void* context)
	{
		g_wglDeleteContext((HGLRC)context);
	}

	void* PlatformBackend::d3d11_get_hwnd()
	{
		return g_hwnd;
	}

	Key scancode_to_key(WPARAM wParam, LPARAM lParam)
	{
		Key key = Key::Unknown;

		switch (wParam)
		{
		case VK_CANCEL: key = Key::Cancel; break;
		case VK_BACK: key = Key::Backspace; break;
		case VK_TAB: key = Key::Tab; break;
		case VK_CLEAR: key = Key::Clear; break;
		case VK_RETURN: key = Key::Enter; break;
		case VK_SHIFT: key = Key::LeftShift; break;
		case VK_CONTROL: key = Key::LeftControl; break;
		case VK_PAUSE: key = Key::Pause; break;
		case VK_CAPITAL: key = Key::Capslock; break;
		case VK_ESCAPE: key = Key::Escape; break;
		case VK_SPACE: key = Key::Space; break;
		case VK_PRIOR: key = Key::Prior; break;
		case VK_END: key = Key::End; break;
		case VK_HOME: key = Key::Home; break;
		case VK_LEFT: key = Key::Left; break;
		case VK_UP: key = Key::Up; break;
		case VK_RIGHT: key = Key::Right; break;
		case VK_DOWN: key = Key::Down; break;
		case VK_SELECT: key = Key::Select; break;
		case VK_PRINT: key = Key::PrintScreen; break;
		case VK_EXECUTE: key = Key::Execute; break;
		case VK_SNAPSHOT: key = Key::PrintScreen; break;
		case VK_INSERT: key = Key::Insert; break;
		case VK_DELETE: key = Key::Delete; break;
		case VK_HELP: key = Key::Help; break;
		case VK_LWIN: key = Key::LeftOS; break;
		case VK_RWIN: key = Key::RightOS; break;
		case VK_APPS: key = Key::Application; break;
		case VK_SLEEP: key = Key::Unknown; break;
		case VK_NUMPAD0: key = Key::Keypad0; break;
		case VK_NUMPAD1: key = Key::Keypad1; break;
		case VK_NUMPAD2: key = Key::Keypad2; break;
		case VK_NUMPAD3: key = Key::Keypad3; break;
		case VK_NUMPAD4: key = Key::Keypad4; break;
		case VK_NUMPAD5: key = Key::Keypad5; break;
		case VK_NUMPAD6: key = Key::Keypad6; break;
		case VK_NUMPAD7: key = Key::Keypad7; break;
		case VK_NUMPAD8: key = Key::Keypad8; break;
		case VK_NUMPAD9: key = Key::Keypad9; break;
		case VK_F1: key = Key::F1; break;
		case VK_F2: key = Key::F2; break;
		case VK_F3: key = Key::F3; break;
		case VK_F4: key = Key::F4; break;
		case VK_F5: key = Key::F5; break;
		case VK_F6: key = Key::F6; break;
		case VK_F7: key = Key::F7; break;
		case VK_F8: key = Key::F8; break;
		case VK_F9: key = Key::F9; break;
		case VK_F10: key = Key::F10; break;
		case VK_F11: key = Key::F11; break;
		case VK_F12: key = Key::F12; break;
		case VK_F13: key = Key::F13; break;
		case VK_F14: key = Key::F14; break;
		case VK_F15: key = Key::F15; break;
		case VK_F16: key = Key::F16; break;
		case VK_F17: key = Key::F17; break;
		case VK_F18: key = Key::F18; break;
		case VK_F19: key = Key::F19; break;
		case VK_F20: key = Key::F20; break;
		case VK_F21: key = Key::F21; break;
		case VK_F22: key = Key::F22; break;
		case VK_F23: key = Key::F23; break;
		case VK_F24: key = Key::F24; break;
		case VK_NUMLOCK: key = Key::Numlock; break;
		case VK_LSHIFT: key = Key::LeftShift; break;
		case VK_RSHIFT: key = Key::RightShift; break;
		case VK_LCONTROL: key = Key::LeftControl; break;
		case VK_RCONTROL: key = Key::RightControl; break;
		case VK_VOLUME_MUTE: key = Key::Mute; break;
		case VK_VOLUME_DOWN: key = Key::VolumeDown; break;
		case VK_VOLUME_UP: key = Key::VolumeUp; break;
		}

		if (key == Key::Unknown)
		{
			int scancode = (lParam >> 16) & 0xFF;

			switch (scancode)
			{
			case 1: key = Key::Escape; break;
			case 2: key = Key::D1; break;
			case 3: key = Key::D2; break;
			case 4: key = Key::D3; break;
			case 5: key = Key::D4; break;
			case 6: key = Key::D5; break;
			case 7: key = Key::D6; break;
			case 8: key = Key::D7; break;
			case 9: key = Key::D8; break;
			case 10: key = Key::D9; break;
			case 11: key = Key::D0; break;
			case 12: key = Key::Minus; break;
			case 13: key = Key::Equals; break;
			case 14: key = Key::Backspace; break;
			case 15: key = Key::Tab; break;
			case 16: key = Key::Q; break;
			case 17: key = Key::W; break;
			case 18: key = Key::E; break;
			case 19: key = Key::R; break;
			case 20: key = Key::T; break;
			case 21: key = Key::Y; break;
			case 22: key = Key::U; break;
			case 23: key = Key::I; break;
			case 24: key = Key::O; break;
			case 25: key = Key::P; break;
			case 26: key = Key::LeftBracket; break;
			case 27: key = Key::RightBracket; break;
			case 28: key = Key::Enter; break;
			case 29: key = Key::LeftControl; break;
			case 30: key = Key::A; break;
			case 31: key = Key::S; break;
			case 32: key = Key::D; break;
			case 33: key = Key::F; break;
			case 34: key = Key::G; break;
			case 35: key = Key::H; break;
			case 36: key = Key::J; break;
			case 37: key = Key::K; break;
			case 38: key = Key::L; break;
			case 39: key = Key::Semicolon; break;
			case 40: key = Key::Apostrophe; break;
			case 41: key = Key::Tilde; break;
			case 42: key = Key::LeftShift; break;
			case 43: key = Key::Backslash; break;
			case 44: key = Key::Z; break;
			case 45: key = Key::X; break;
			case 46: key = Key::C; break;
			case 47: key = Key::V; break;
			case 48: key = Key::B; break;
			case 49: key = Key::N; break;
			case 50: key = Key::M; break;
			case 51: key = Key::Comma; break;
			case 52: key = Key::Period; break;
			case 53: key = Key::Slash; break;
			case 54: key = Key::RightShift; break;
			case 55: key = Key::PrintScreen; break;
			case 56: key = Key::LeftAlt; break;
			case 57: key = Key::Space; break;
			case 58: key = Key::Capslock; break;
			case 59: key = Key::F1; break;
			case 60: key = Key::F2; break;
			case 61: key = Key::F3; break;
			case 62: key = Key::F4; break;
			case 63: key = Key::F5; break;
			case 64: key = Key::F6; break;
			case 65: key = Key::F7; break;
			case 66: key = Key::F8; break;
			case 67: key = Key::F9; break;
			case 68: key = Key::F10; break;
			case 71: key = Key::Home; break;
			case 72: key = Key::Up; break;
			case 73: key = Key::PageUp; break;
			case 74: key = Key::KeypadMinus; break;
			case 75: key = Key::Left; break;
			case 76: key = Key::Keypad5; break;
			case 77: key = Key::Right; break;
			case 78: key = Key::KeypadPlus; break;
			case 79: key = Key::End; break;
			case 80: key = Key::Down; break;
			case 81: key = Key::PageDown; break;
			case 82: key = Key::Insert; break;
			case 83: key = Key::Delete; break;
			case 87: key = Key::F11; break;
			case 88: key = Key::F12; break;
			case 89: key = Key::Pause; break;
			case 91: key = Key::LeftOS; break;
			case 92: key = Key::RightOS; break;
			case 93: key = Key::Application; break;
			case 100: key = Key::F13; break;
			case 101: key = Key::F14; break;
			case 102: key = Key::F15; break;
			case 103: key = Key::F16; break;
			case 104: key = Key::F17; break;
			case 105: key = Key::F18; break;
			case 106: key = Key::F19; break;
			}
		}

		return key;
	}
}

#endif // BLAH_PLATFORM_WINDOWS
