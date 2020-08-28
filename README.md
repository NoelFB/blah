## blah
a small C++ game framework for 2D games.

this will likely see breaking changes.

#### Prerequisites 
 - A C++17 compiler and CMake
 - Only an SDL2 `platform` implementation [is done](https://github.com/NoelFB/blah/blob/master/private/blah/internal/platform_sdl2.cpp). CMake will need to find your own installation of SDL2 through `SDL2_INCLUDE_DIRS` and `SDL2_LIBRARIES`
 - Only an OpenGL `graphics` implementation [is done](https://github.com/NoelFB/blah/blob/master/private/blah/internal/graphics_opengl.cpp), so it currently requires OpenGL.
 
#### Notes
 - There are probably lots of small bugs as I've only tested this for my personal projects. This is probably best used as a learning resource for now.
 - There's a custom "vector" class which is called List and Stacklist. this may later be replaced with std::vector
 - There's no Shader code abstraction, so the current Sprite Batcher has a hard-coded OpenGL shader. At some point this will need to change.
 - The rendering layer may be replaced with [FNA3D](https://github.com/FNA-XNA/FNA3D), [BGFX](https://github.com/bkaradzic/bgfx), [Sokol](https://github.com/floooh/sokol), or something else.
 - There's no Audio layer implementation yet.
