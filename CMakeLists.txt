cmake_minimum_required(VERSION 3.12)
project(blah)

# C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(blah

	src/core/app.cpp
	src/core/filesystem.cpp
	src/core/log.cpp
	src/core/time.cpp

	src/graphics/blend.cpp
	src/graphics/framebuffer.cpp
	src/graphics/material.cpp
	src/graphics/mesh.cpp
	src/graphics/renderpass.cpp
	src/graphics/shader.cpp
	src/graphics/texture.cpp

	src/input/input.cpp
	src/input/virtual_stick.cpp
	src/input/virtual_button.cpp
	src/input/virtual_axis.cpp
	
	src/containers/str.cpp
	
	src/drawing/batch.cpp
	src/drawing/spritefont.cpp
	src/drawing/subtexture.cpp

	src/images/aseprite.cpp
	src/images/font.cpp
	src/images/image.cpp
	src/images/packer.cpp

	src/math/calc.cpp
	src/math/circle.cpp
	src/math/color.cpp
	src/math/line.cpp
	src/math/mat3x2.cpp
	src/math/mat4x4.cpp
	src/math/point.cpp
	src/math/quad.cpp
	src/math/rect.cpp
	src/math/rectI.cpp
	src/math/stopwatch.cpp
	src/math/vec2.cpp
	
	src/streams/bufferstream.cpp	
	src/streams/filestream.cpp
	src/streams/memorystream.cpp
	src/streams/stream.cpp


	src/internal/graphics_backend_gl.cpp
	src/internal/graphics_backend_d3d11.cpp
	src/internal/graphics_backend_dummy.cpp
	src/internal/platform_backend_sdl2.cpp
)

target_include_directories(blah 
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

# Platform Variables
set(SDL2_ENABLED true CACHE BOOL "Use SDL2 as the System implementation")
set(OPENGL_ENABLED true CACHE BOOL "Use OpenGL graphics implementation")
set(D3D11_ENABLED false CACHE BOOL "Use D3D11 graphics implementation")

set(LIBS "")

# add OpenGL definition if we're using it
if (OPENGL_ENABLED)
	add_compile_definitions(BLAH_USE_OPENGL)
endif()

# add D3D11 definition if we're using it
if (D3D11_ENABLED)
	add_compile_definitions(BLAH_USE_D3D11)
	set(LIBS ${LIBS} d3d11.lib dxguid.lib D3Dcompiler.lib)
endif()

# Link and create SDL2 Definition if we're using it
if (SDL2_ENABLED)
	add_compile_definitions(BLAH_USE_SDL2)
	if (EMSCRIPTEN)
  		set_target_properties(blah PROPERTIES COMPILE_FLAGS "-s USE_SDL=2")
	else()
		# Attempt to find SDL2
		find_package(SDL2 QUIET)

		# If CMake cannot find SDL2 library, then it gets downloaded and compiled that way 
		if (NOT ${SDL2_FOUND})
			include(FetchContent)
			FetchContent_Declare(
				SDL2
				GIT_REPOSITORY https://github.com/SDL-mirror/SDL
				GIT_TAG release-2.0.14 # grab latest stable release
			)
			FetchContent_GetProperties(SDL2)
			if (NOT sdl2_POPULATED)
				FetchContent_Populate(SDL2)
				add_subdirectory(${sdl2_SOURCE_DIR} ${sdl2_BINARY_DIR})
			endif()
		endif()

		# Either way we are linking to SDL2
		set(LIBS ${LIBS} SDL2)
	endif()
endif()

target_link_libraries(blah PUBLIC ${LIBS})
