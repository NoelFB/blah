## blah
A small 2D C++ Game Framework, using few dependencies and simple code to maintain easy building and portability.

**☆ This will likely see breaking changes! Use at your own risk! ☆**

#### a sample application

```cpp
#include <blah.h>
using namespace Blah;

Batch batch;

int main()
{
    Config config;
    config.name = "blah app";
    config.on_render = []()
    {
        App::backbuffer()->clear(Color::black);

        auto center = App::get_backbuffer_size() / 2;
        auto rotation = Time::seconds * Calc::TAU;
        auto transform = Mat3x2f::create_transform(center, Vec2f::zero, Vec2f::one, rotation);

        batch.push_matrix(transform);
        batch.rect(Rectf(-32, -32, 64, 64), Color::red);
        batch.pop_matrix();

        batch.render();
        batch.clear();
    };

    App::run(&config);
    return 0;
}

```

#### building
 - Requires C++17 and CMake 3.14+
 - A single **Platform** implementation must be enabled in CMake:
	- [SDL2](https://github.com/NoelFB/blah/blob/master/src/internal/platform_sdl2.cpp) (Default) `BLAH_PLATFORM_SDL2`
	- [WIN32](https://github.com/NoelFB/blah/blob/master/src/internal/platform_win32.cpp) (Unfinished) `BLAH_PLATFORM_WIN32`
	- Additional platforms can be added by implementing the [Platform Backend](https://github.com/NoelFB/blah/blob/master/src/internal/platform.h)
 - At least one **Renderer** implementation must be enabled in CMake:
	- [OpenGL](https://github.com/NoelFB/blah/blob/master/src/internal/renderer_gl.cpp) (Default on Linux/macOS) `BLAH_RENDERER_OPENGL`
	- [D3D11](https://github.com/NoelFB/blah/blob/master/src/internal/renderer_d3d11.cpp) (Default on Windows) `BLAH_RENDERER_D3D11`
	- Additional renderers can be added by implementing the [Renderer Backend](https://github.com/NoelFB/blah/blob/master/src/internal/renderer.h)
 
#### notes
 - There's no Shader abstraction, so you need to swap between GLSL/HLSL depending on the Renderer.
 - Only floatN/mat3x2/mat4x4 uniforms are supported.
 - No threaded rendering so it will explode if you try that.
