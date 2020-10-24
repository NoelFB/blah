## blah
a small C++ game framework for 2D games.

this will likely see breaking changes.

#### prerequisites
 - A C++17 compiler and CMake
 - Only an SDL2 `platform` [is done](https://github.com/NoelFB/blah/blob/master/private/blah/internal/platform_sdl2.cpp). CMake will need to find SDL2 via `SDL2_INCLUDE_DIRS` and `SDL2_LIBRARIES`
 - Only an OpenGL `graphics` [is done](https://github.com/NoelFB/blah/blob/master/private/blah/internal/graphics_opengl.cpp), so it currently requires OpenGL.

#### notes
 - There are probably lots of small bugs as this is highly untested. Best used as a learning resource for now.
 - There's a custom "StackVector" class that tries to implement an std::vector on the stack, but it's probably bug-prone.
 - There's no Shader abstraction, so the [Sprite Batcher](https://github.com/NoelFB/blah/blob/master/public/blah/drawing/batch.h) has hard-coded GLSL. This will need to change.
 - The rendering layer may be replaced with [FNA3D](https://github.com/FNA-XNA/FNA3D), [BGFX](https://github.com/bkaradzic/bgfx), [Sokol](https://github.com/floooh/sokol), or something else.
 - There's no Audio layer implementation yet.
 - No threaded rendering so the OpenGL implementation will explode if you try that


#### a sample application

```cpp
#include <blah.h>
using namespace Blah;

Batch batch;

void render()
{
	Graphics::clear(Graphics::backbuffer, 0x00000000);
	
	Vec2 center = Vec2(App::draw_width(), App::draw_height()) / 2;
	float rotation = Time::elapsed * Calc::TAU;
	Mat3x2 transform = Mat3x2::create_transform(center, Vec2::zero, Vec2::one, rotation);

	batch.push_matrix(transform);
	batch.rect(Rect(-32, -32, 64, 64), 0xff0000);
	batch.pop_matrix();
	
	batch.render(Graphics::backbuffer);
	batch.clear();
}

int main()
{
	Config config;
	config.name = "blah app";
	config.width = 1280;
	config.height = 720;
	config.on_render = render;
	
	App::run(&config);
	return 0;
}

```
