## blah
a small C++ game framework for 2D games.
this will likely see breaking changes.

### Prerequisites 
 - A C++17 compiler and CMake
 - Only an SDL2 "platform" implementation is done. CMake will need to find your own installation of SDL2 through `SDL2_INCLUDE_DIRS` and `SDL2_LIBRARIES`
 - Only an OpenGL "graphics" implementation is done, so it currently requires OpenGL.
 
### Other Notes
 - I use my own "vector" class which is called List and Stacklist. I may later replace with with std::vector
 - There are probably lots of small bugs as I've only tested this for my personal projects. This is probably best used as a learning resource for now.
