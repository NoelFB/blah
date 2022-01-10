## blah
A small 2D C++ Game Framework, using few dependencies and simple code to mainain easy building and portability.

**☆ This will likely see breaking changes! Use at your own risk! ☆**

#### building
 - Requires C++17 and CMake 3.14+
 - A single *Platform* implementation must be enabled:
	- [SDL2](https://github.com/NoelFB/blah/blob/master/src/internal/platform_sdl2.cpp) can be enabled in CMake with `BLAH_PLATFORM_SDL2` (default)
	- [WIN32](https://github.com/NoelFB/blah/blob/master/src/internal/platform_win32.cpp) (UNFINISHED) can be enabled in CMake with `BLAH_PLATFORM_WIN32`
	- Additional platforms can be added by implementing the [Platform Backend](https://github.com/NoelFB/blah/blob/master/src/internal/platform.h)
 - A single *Graphics* implementation must be enabled:
	- [OpenGL](https://github.com/NoelFB/blah/blob/master/src/internal/graphics_gl.cpp) can be enabled in CMake with `BLAH_GRAPHICS_OPENGL` (default)
	- [D3D11](https://github.com/NoelFB/blah/blob/master/src/internal/graphics_d3d11.cpp) can be enabled in CMake with `BLAH_GRAPHICS_D3D11`
	- Additional graphics can be added by implementing the [Graphics Backend](https://github.com/NoelFB/blah/blob/master/src/internal/graphics.h)
 
#### notes
 - There's no Shader abstraction, so the [Sprite Batcher](https://github.com/NoelFB/blah/blob/master/src/graphics/batch.cpp) has hard-coded GLSL/HLSL. This will need to change.
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
	
	auto center = Vec2f(App::backbuffer->width(), App::backbuffer->height()) / 2;
	auto rotation = Time::seconds * Calc::TAU;
	auto transform = Mat3x2f::create_transform(center, Vec2f::zero, Vec2f::one, rotation);

	batch.push_matrix(transform);
	batch.rect(Rectf(-32, -32, 64, 64), Color::red);
	batch.tex(tex, Vec2f(64, 0), Color::white);
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
