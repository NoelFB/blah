cmake_minimum_required(VERSION 3.6)
project(blah)

# C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(blah

	public/blah.h
	public/blah/app.cpp
	public/blah/app.h
	public/blah/filesystem.cpp
	public/blah/filesystem.h
	public/blah/log.cpp
	public/blah/log.h
	public/blah/time.cpp
	public/blah/time.h

	public/blah/graphics/blend.h
	public/blah/graphics/blend.cpp
	public/blah/graphics/framebuffer.h
	public/blah/graphics/framebuffer.cpp
	public/blah/graphics/material.h
	public/blah/graphics/material.cpp
	public/blah/graphics/mesh.h
	public/blah/graphics/mesh.cpp
	public/blah/graphics/renderpass.h
	public/blah/graphics/renderpass.cpp
	public/blah/graphics/shader.h
	public/blah/graphics/shader.cpp
	public/blah/graphics/texture.h
	public/blah/graphics/texture.cpp

	public/blah/input/input.cpp
	public/blah/input/virtual_stick.cpp
	public/blah/input/virtual_stick.h
	public/blah/input/virtual_button.cpp
	public/blah/input/virtual_button.h
	public/blah/input/virtual_axis.cpp
	public/blah/input/virtual_axis.h
	
	public/blah/containers/vector.h
	public/blah/containers/stackvector.h
	public/blah/containers/str.cpp
	public/blah/containers/str.h
	
	public/blah/drawing/batch.cpp
	public/blah/drawing/batch.h
	public/blah/drawing/spritefont.cpp
	public/blah/drawing/spritefont.h
	public/blah/drawing/subtexture.cpp
	public/blah/drawing/subtexture.h

	public/blah/images/aseprite.cpp
	public/blah/images/aseprite.h
	public/blah/images/font.cpp
	public/blah/images/font.h
	public/blah/images/image.cpp
	public/blah/images/image.h
	public/blah/images/packer.cpp
	public/blah/images/packer.h

	public/blah/math/calc.cpp
	public/blah/math/calc.h
	public/blah/math/circle.cpp
	public/blah/math/circle.h
	public/blah/math/color.cpp
	public/blah/math/color.h
	public/blah/math/ease.h
	public/blah/math/line.cpp
	public/blah/math/line.h
	public/blah/math/mat3x2.cpp
	public/blah/math/mat3x2.h
	public/blah/math/mat4x4.cpp
	public/blah/math/mat4x4.h
	public/blah/math/point.cpp
	public/blah/math/point.h
	public/blah/math/quad.h
	public/blah/math/quad.cpp
	public/blah/math/rect.cpp
	public/blah/math/rect.h
	public/blah/math/rectI.cpp
	public/blah/math/rectI.h
	public/blah/math/stopwatch.cpp
	public/blah/math/stopwatch.h
	public/blah/math/vec2.cpp
	public/blah/math/vec2.h
	public/blah/math/vec4.h
	
	public/blah/streams/endian.h
	public/blah/streams/bufferstream.cpp
	public/blah/streams/bufferstream.h	
	public/blah/streams/filestream.cpp
	public/blah/streams/filestream.h
	public/blah/streams/memorystream.cpp
	public/blah/streams/memorystream.h
	public/blah/streams/stream.cpp
	public/blah/streams/stream.h

	private/blah/third_party/stb_image.h
	private/blah/third_party/stb_image_write.h
	private/blah/third_party/stb_truetype.h
	private/blah/internal/graphics_backend.h
	private/blah/internal/graphics_backend_gl.cpp
	private/blah/internal/graphics_backend_d3d11.cpp
	private/blah/internal/graphics_backend_dummy.cpp
	private/blah/internal/input_backend.h
	private/blah/internal/platform_backend.h
	private/blah/internal/platform_backend_sdl2.cpp
)

target_include_directories(blah 
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/private>
)

# Platform Variables
set(SDL2_ENABLED true CACHE BOOL "Use SDL2 as the System implementation")
set(SDL2_INCLUDE_DIRS "" CACHE FILEPATH "SDL2 Headers")
set(SDL2_LIBRARIES "" CACHE FILEPATH "SDL2 Headers")
set(OPENGL_ENABLED true CACHE BOOL "Use OpenGL graphics implementation")
set(D3D11_ENABLED false CACHE BOOL "Use D3D11 graphics implementation")

set(LIBS "")

# add OpenGL definition if we're using OpenGL
if (OPENGL_ENABLED)
	add_compile_definitions(BLAH_USE_OPENGL)
endif()

# add D3D11 definition if we're using D3D11
if (D3D11_ENABLED)
	add_compile_definitions(BLAH_USE_D3D11)
	set(LIBS ${LIBS} d3d11.lib dxguid.lib D3Dcompiler.lib)
endif()

# Link and create SDL2 Definition if we're using SDL2
if (SDL2_ENABLED)
	add_compile_definitions(BLAH_USE_SDL2)
	target_include_directories(blah PUBLIC "$<BUILD_INTERFACE:${SDL2_INCLUDE_DIRS}>")
	set(LIBS ${LIBS} ${SDL2_LIBRARIES})
endif()

target_link_libraries(blah PUBLIC ${LIBS})
