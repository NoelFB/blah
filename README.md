## blah
a small C++ game framework for 2D games.
goal is to be simple and use as few dependencies as possible, to maintain easy building and portability.

this will likely see breaking changes.

#### building
 - Requires C++17 and CMake
 - The [SDL2 platform backend](https://github.com/NoelFB/blah/blob/master/private/blah/internal/platform_backend_sdl2.cpp) can be enabled in CMake with `BLAH_USE_SDL2`, and setting `SDL2_INCLUDE_DIRS` and `SDL2_LIBRARIES`
 - The [OpenGL graphics backend](https://github.com/NoelFB/blah/blob/master/private/blah/internal/graphics_backend_gl.cpp) can be enabled in CMake with `BLAH_USE_OPENGL`.
 - Other backends can be added by implementing the [Platform Backend](https://github.com/NoelFB/blah/blob/master/private/blah/internal/platform_backend.h) or [Graphics Backend](https://github.com/NoelFB/blah/blob/master/private/blah/internal/graphics_backend.h).

#### notes
 - There's no Shader abstraction, so the [Sprite Batcher](https://github.com/NoelFB/blah/blob/master/public/blah/drawing/batch.h) has hard-coded GLSL. This will need to change.
 - The rendering layer may be replaced with [FNA3D](https://github.com/FNA-XNA/FNA3D), [BGFX](https://github.com/bkaradzic/bgfx), [Sokol](https://github.com/floooh/sokol), etc, or I may attempt to write custom backends for D3D11/Metal/Vulkan myself, like the OpenGL backend.
 - There's no Audio API or backend implementation yet.
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
