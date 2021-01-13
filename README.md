## blah
A small C++ game framework for 2D games.
Goal is to be simple and use as few dependencies as possible, to maintain easy building and portability.

**☆ This will likely see breaking changes! Use at your own risk! ☆**

#### building
 - Requires C++17 and CMake 3.12+
 - Platform Backend
	- [SDL2](https://github.com/NoelFB/blah/blob/master/src/internal/platform_backend_sdl2.cpp) can be enabled in CMake with `SDL2_ENABLED`, and setting `SDL2_INCLUDE_DIRS` and `SDL2_LIBRARIES`
 - Graphics Backend
	- [OpenGL](https://github.com/NoelFB/blah/blob/master/src/internal/graphics_backend_gl.cpp) can be enabled in CMake with `OPENGL_ENABLED`.
	- [D3D11](https://github.com/NoelFB/blah/blob/master/src/internal/graphics_backend_d3d11.cpp) (unfinished) can be enabled in CMake with `D3D11_ENABLED`.
 - Other backends can be added by implementing the [Platform Backend](https://github.com/NoelFB/blah/blob/master/src/internal/platform_backend.h) or [Graphics Backend](https://github.com/NoelFB/blah/blob/master/src/internal/graphics_backend.h).
 
#### notes
 - There's no Shader abstraction, so the [Sprite Batcher](https://github.com/NoelFB/blah/blob/master/include/blah/drawing/batch.h) has hard-coded GLSL/HLSL. This will need to change.
 - Only floatN/mat3x2/mat4x4 uniforms are supported.
 - There's no Audio API or backend implementation yet.
 - No threaded rendering so it will explode if you try that.

#### a sample application

```cpp
#include <blah.h>
using namespace Blah;

Batch batch;
TextureRef tex;

void startup()
{
	tex = Texture::create("player.png");
}

void render()
{
	App::backbuffer->clear(Color::black);
	
	auto center = Vec2(App::backbuffer->width(), App::backbuffer->height()) / 2;
	auto rotation = Time::elapsed * Calc::TAU;
	auto transform = Mat3x2::create_transform(center, Vec2::zero, Vec2::one, rotation);

	batch.push_matrix(transform);
	batch.rect(Rect(-32, -32, 64, 64), Color::red);
	batch.tex(tex, Vec2(64, 0), Color::white);
	batch.pop_matrix();
	
	batch.render();
	batch.clear();
}

int main()
{
	Config config;
	config.name = "blah app";
	config.width = 1280;
	config.height = 720;
	config.on_startup = startup;
	config.on_render = render;
	
	App::run(&config);
	return 0;
}

```
