cmake_minimum_required(VERSION 3.14)
project(blah)

# C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(blah
	src/app.cpp
	src/filesystem.cpp
	src/common.cpp
	src/time.cpp
	src/input.cpp
	src/stream.cpp
	src/graphics.cpp
	src/containers/str.cpp
	src/drawing/batch.cpp
	src/drawing/spritefont.cpp
	src/drawing/subtexture.cpp
	src/images/aseprite.cpp
	src/images/font.cpp
	src/images/image.cpp
	src/images/packer.cpp
	src/internal/renderer_opengl.cpp
	src/internal/renderer_d3d11.cpp
	src/internal/platform_sdl2.cpp
	src/internal/platform_win32.cpp
)

target_include_directories(blah
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

# Platform Variables
option(BLAH_PLATFORM_SDL2 "Use SDL2 Platform Backend" ON)
option(BLAH_PLATFORM_WIN32 "Use Win32 Platform Backend" OFF)
option(BLAH_RENDERER_OPENGL "Make OpenGL Renderer available" ON)
if (WIN32)
	option(BLAH_RENDERER_D3D11 "Make D3D11 Renderer available" ON)
endif()
option(BLAH_NO_FUNCTIONAL "Don't use std::function" OFF)
option(BLAH_NO_SHARED_PTR "Don't use std::shared_ptr for Resources" OFF)

# tracks which libraries we need to link, depends on Options above
set(LIBS "")

# use the OpenGL Renderer Backend
if (BLAH_RENDERER_OPENGL)
	add_compile_definitions(BLAH_RENDERER_OPENGL)
endif()

# use the D3D11 Renderer Backend
if (BLAH_RENDERER_D3D11)
	add_compile_definitions(BLAH_RENDERER_D3D11)
	set(LIBS ${LIBS} d3d11.lib dxguid.lib D3Dcompiler.lib)
endif()

# use the SDL2 Platform Backend
# Link and create SDL2 Definition
if (BLAH_PLATFORM_SDL2)
	add_compile_definitions(BLAH_PLATFORM_SDL2)

	# Emscripten can import SDL2 directly
	if (EMSCRIPTEN)
		
		set_target_properties(blah PROPERTIES COMPILE_FLAGS "-s USE_SDL=2")
		target_link_libraries(blah "-s USE_SDL=2 -s USE_WEBGL2=1")

	# Pull SDL2 from its Github repo
	else()
		if (NOT DEFINED BLAH_SDL2_LIBS)
			include(FetchContent)
			set(FETCHCONTENT_QUIET FALSE)

			FetchContent_Declare(
				SDL2
				GIT_REPOSITORY https://github.com/libsdl-org/SDL
				GIT_TAG release-2.24.0 # grab latest stable release
				GIT_PROGRESS TRUE
			)
			FetchContent_MakeAvailable(SDL2)
			set(BLAH_SDL2_LIBS SDL2main SDL2-static)
			set(BLAH_SDL2_INCLUDE ${sdl2_SOURCE_DIRS}/include)
		endif()

		# statically link SDL2 since we're building it ourselves
		set(LIBS ${LIBS} ${BLAH_SDL2_LIBS})
		target_include_directories(blah PRIVATE ${BLAH_SDL2_INCLUDE})
	endif()

# use the Win32 Platform Backend
elseif (BLAH_PLATFORM_WIN32)

	add_compile_definitions(BLAH_PLATFORM_WIN32)

endif()

target_link_libraries(blah PRIVATE ${LIBS})

# toggle options
if (BLAH_NO_FUNCTIONAL)
	add_compile_definitions(BLAH_NO_FUNCTIONAL)
endif()
if (BLAH_NO_SHARED_PTR)
	add_compile_definitions(BLAH_NO_SHARED_PTR)
endif()

