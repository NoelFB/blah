cmake_minimum_required(VERSION 3.12)
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

	src/graphics/batch.cpp
	src/graphics/blend.cpp
	src/graphics/material.cpp
	src/graphics/mesh.cpp
	src/graphics/renderpass.cpp
	src/graphics/shader.cpp
	src/graphics/spritefont.cpp
	src/graphics/subtexture.cpp
	src/graphics/target.cpp
	src/graphics/texture.cpp

	src/containers/str.cpp

	src/images/aseprite.cpp
	src/images/font.cpp
	src/images/image.cpp
	src/images/packer.cpp

	src/numerics/calc.cpp
	src/numerics/color.cpp

	src/streams/bufferstream.cpp
	src/streams/filestream.cpp
	src/streams/memorystream.cpp
	src/streams/stream.cpp

	src/internal/graphics_gl.cpp
	src/internal/graphics_d3d11.cpp
	src/internal/graphics_dummy.cpp
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
set(PLATFORM_SDL2 true CACHE BOOL "Use SDL2 Platform Backend")
set(PLATFORM_WIN32 false CACHE BOOL "Use Win32 Platform Backend")
set(GRAPHICS_OPENGL true CACHE BOOL "Use OpenGL Graphics Backend")
set(GRAPHICS_D3D11 false CACHE BOOL "Use D3D11 Graphics Backend")

set(LIBS "")

# use the OpenGL Graphics Backend
if (GRAPHICS_OPENGL)

	add_compile_definitions(BLAH_GRAPHICS_OPENGL)

# use the D3D11 Graphics Backend
elseif (GRAPHICS_D3D11)

	add_compile_definitions(BLAH_GRAPHICS_D3D11)
	set(LIBS ${LIBS} d3d11.lib dxguid.lib D3Dcompiler.lib)

endif()

# use the SDL2 Platform Backend
# Link and create SDL2 Definition
if (PLATFORM_SDL2)
	add_compile_definitions(BLAH_PLATFORM_SDL2)

	# Emscripten can import SDL2 directly
	if (EMSCRIPTEN)

		set_target_properties(blah PROPERTIES COMPILE_FLAGS "-s USE_SDL=2")

	# Load SDL2 Normally
	else()

		# Try to find SDL2
		if (DEFINED SDL2_LIBRARIES AND DEFINED SDL2_INCLUDE_DIRS)
			set(SDL2_FOUND true)
		else()
			find_package(SDL2 QUIET)
		endif()
		
		# If CMake cannot find SDL2 library, then it gets downloaded and compiled that way
		if (NOT ${SDL2_FOUND})

			include(FetchContent)
			FetchContent_Declare(
				SDL2
				GIT_REPOSITORY https://github.com/libsdl-org/SDL
				GIT_TAG release-2.0.18 # grab latest stable release
			)
			FetchContent_GetProperties(SDL2)
			if (NOT sdl2_POPULATED)
				FetchContent_Populate(SDL2)
				add_subdirectory(${sdl2_SOURCE_DIR} ${sdl2_BINARY_DIR})
			endif()

			# statically link SDL2 since we're building it ourselves
			set(LIBS ${LIBS} SDL2main SDL2-static)
			target_include_directories(blah PUBLIC ${sdl2_SOURCE_DIRS}/include)

		else()

			# Add Library and Include Dirs
			set(LIBS ${LIBS} ${SDL2_LIBRARIES})
			target_include_directories(blah PUBLIC ${SDL2_INCLUDE_DIRS})

		endif()

	endif()

# use the Win32 Platform Backend
elseif (PLATFORM_WIN32)

	add_compile_definitions(BLAH_PLATFORM_WIN32)

endif()

target_link_libraries(blah PUBLIC ${LIBS})
