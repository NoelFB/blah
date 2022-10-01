#ifdef BLAH_RENDERER_OPENGL

#include "blah_renderer.h"
#include "blah_internal.h"
#include "blah_platform.h"
#include <blah_common.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#define APIENTRY
#endif

// OpenGL Value Types
typedef ptrdiff_t        GLintptr;
typedef ptrdiff_t        GLsizeiptr;
typedef unsigned int     GLenum;
typedef unsigned char    GLboolean;
typedef unsigned int     GLbitfield;
typedef void             GLvoid;
typedef signed char      GLbyte;      /* 1-byte signed */
typedef short            GLshort;     /* 2-byte signed */
typedef int              GLint;       /* 4-byte signed */
typedef unsigned char    GLubyte;     /* 1-byte unsigned */
typedef unsigned short   GLushort;    /* 2-byte unsigned */
typedef unsigned int     GLuint;      /* 4-byte unsigned */
typedef int              GLsizei;     /* 4-byte signed */
typedef float            GLfloat;     /* single precision float */
typedef float            GLclampf;    /* single precision float in [0,1] */
typedef double           GLdouble;    /* double precision float */
typedef double           GLclampd;    /* double precision float in [0,1] */
typedef char             GLchar;

// OpenGL Constants
#define GL_DONT_CARE 0x1100
#define GL_ZERO 0x0000
#define GL_ONE 0x0001
#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406
#define GL_HALF_FLOAT 0x140B
#define GL_UNSIGNED_SHORT_4_4_4_4_REV 0x8365
#define GL_UNSIGNED_SHORT_5_5_5_1_REV 0x8366
#define GL_UNSIGNED_INT_2_10_10_10_REV 0x8368
#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#define GL_UNSIGNED_INT_24_8 0x84FA
#define GL_VENDOR 0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02
#define GL_EXTENSIONS 0x1F03
#define GL_COLOR_BUFFER_BIT 0x4000
#define GL_DEPTH_BUFFER_BIT 0x0100
#define GL_STENCIL_BUFFER_BIT 0x0400
#define GL_SCISSOR_TEST 0x0C11
#define GL_DEPTH_TEST 0x0B71
#define GL_STENCIL_TEST 0x0B90
#define GL_LINE 0x1B01
#define GL_FILL 0x1B02
#define GL_CW 0x0900
#define GL_CCW 0x0901
#define GL_FRONT 0x0404
#define GL_BACK 0x0405
#define GL_FRONT_AND_BACK 0x0408
#define GL_CULL_FACE 0x0B44
#define GL_POLYGON_OFFSET_FILL 0x8037
#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE_3D 0x806F
#define GL_TEXTURE_CUBE_MAP 0x8513
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define GL_BLEND 0x0BE2
#define GL_SRC_COLOR 0x0300
#define GL_ONE_MINUS_SRC_COLOR 0x0301
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_DST_ALPHA 0x0304
#define GL_ONE_MINUS_DST_ALPHA 0x0305
#define GL_DST_COLOR 0x0306
#define GL_ONE_MINUS_DST_COLOR 0x0307
#define GL_SRC_ALPHA_SATURATE 0x0308
#define GL_CONSTANT_COLOR 0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR 0x8002
#define GL_CONSTANT_ALPHA 0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA 0x8004
#define GL_SRC1_ALPHA 0x8589
#define GL_SRC1_COLOR 0x88F9
#define GL_ONE_MINUS_SRC1_COLOR 0x88FA
#define GL_ONE_MINUS_SRC1_ALPHA 0x88FB
#define GL_MIN 0x8007
#define GL_MAX 0x8008
#define GL_FUNC_ADD 0x8006
#define GL_FUNC_SUBTRACT 0x800A
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#define GL_NEVER 0x0200
#define GL_LESS 0x0201
#define GL_EQUAL 0x0202
#define GL_LEQUAL 0x0203
#define GL_GREATER 0x0204
#define GL_NOTEQUAL 0x0205
#define GL_GEQUAL 0x0206
#define GL_ALWAYS 0x0207
#define GL_INVERT 0x150A
#define GL_KEEP 0x1E00
#define GL_REPLACE 0x1E01
#define GL_INCR 0x1E02
#define GL_DECR 0x1E03
#define GL_INCR_WRAP 0x8507
#define GL_DECR_WRAP 0x8508
#define GL_REPEAT 0x2901
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_MIRRORED_REPEAT 0x8370
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_STENCIL_ATTACHMENT 0x8D20
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#define GL_RED 0x1903
#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_LUMINANCE 0x1909
#define GL_RGB8 0x8051
#define GL_RGBA8 0x8058
#define GL_RGBA4 0x8056
#define GL_RGB5_A1 0x8057
#define GL_RGB10_A2_EXT 0x8059
#define GL_RGBA16 0x805B
#define GL_BGRA 0x80E1
#define GL_DEPTH_COMPONENT16 0x81A5
#define GL_DEPTH_COMPONENT24 0x81A6
#define GL_RG 0x8227
#define GL_RG8 0x822B
#define GL_RG16 0x822C
#define GL_R16F 0x822D
#define GL_R32F 0x822E
#define GL_RG16F 0x822F
#define GL_RG32F 0x8230
#define GL_RGBA32F 0x8814
#define GL_RGBA16F 0x881A
#define GL_DEPTH24_STENCIL8 0x88F0
#define GL_COMPRESSED_TEXTURE_FORMATS 0x86A3
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#define GL_DEPTH_COMPONENT 0x1902
#define GL_DEPTH_STENCIL 0x84F9
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_TEXTURE_WRAP_R 0x8072
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_TEXTURE_BASE_LEVEL 0x813C
#define GL_TEXTURE_MAX_LEVEL 0x813D
#define GL_TEXTURE_LOD_BIAS 0x8501
#define GL_PACK_ALIGNMENT 0x0D05
#define GL_UNPACK_ALIGNMENT 0x0CF5
#define GL_TEXTURE0 0x84C0
#define GL_MAX_TEXTURE_IMAGE_UNITS 0x8872
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS 0x8B4C
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_STREAM_DRAW 0x88E0
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_MAX_VERTEX_ATTRIBS 0x8869
#define GL_FRAMEBUFFER 0x8D40
#define GL_READ_FRAMEBUFFER 0x8CA8
#define GL_DRAW_FRAMEBUFFER 0x8CA9
#define GL_RENDERBUFFER 0x8D41
#define GL_MAX_DRAW_BUFFERS 0x8824
#define GL_POINTS 0x0000
#define GL_LINES 0x0001
#define GL_LINE_STRIP 0x0003
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_QUERY_RESULT 0x8866
#define GL_QUERY_RESULT_AVAILABLE 0x8867
#define GL_SAMPLES_PASSED 0x8914
#define GL_MULTISAMPLE 0x809D
#define GL_MAX_SAMPLES 0x8D57
#define GL_SAMPLE_MASK 0x8E51
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_ACTIVE_UNIFORMS 0x8B86
#define GL_ACTIVE_ATTRIBUTES 0x8B89
#define GL_FLOAT_VEC2 0x8B50
#define GL_FLOAT_VEC3 0x8B51
#define GL_FLOAT_VEC4 0x8B52
#define GL_SAMPLER_2D 0x8B5E
#define GL_FLOAT_MAT3x2 0x8B67
#define GL_FLOAT_MAT4 0x8B5C
#define GL_NUM_EXTENSIONS 0x821D
#define GL_DEBUG_SOURCE_API 0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM 0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER 0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY 0x8249
#define GL_DEBUG_SOURCE_APPLICATION 0x824A
#define GL_DEBUG_SOURCE_OTHER 0x824B
#define GL_DEBUG_TYPE_ERROR 0x824C
#define GL_DEBUG_TYPE_PUSH_GROUP 0x8269
#define GL_DEBUG_TYPE_POP_GROUP 0x826A
#define GL_DEBUG_TYPE_MARKER 0x8268
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 0x824E
#define GL_DEBUG_TYPE_PORTABILITY 0x824F
#define GL_DEBUG_TYPE_PERFORMANCE 0x8250
#define GL_DEBUG_TYPE_OTHER 0x8251
#define GL_DEBUG_SEVERITY_HIGH 0x9146
#define GL_DEBUG_SEVERITY_MEDIUM 0x9147
#define GL_DEBUG_SEVERITY_LOW 0x9148
#define GL_DEBUG_SEVERITY_NOTIFICATION 0x826B
#define GL_DEBUG_OUTPUT 0x92E0
#define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242

// OpenGL Functions
#define GL_FUNCTIONS \
	GL_FUNC(DebugMessageCallback, void, DEBUGPROC callback, const void* userParam) \
	GL_FUNC(GetString, const GLubyte*, GLenum name) \
	GL_FUNC(Flush, void, void) \
	GL_FUNC(Enable, void, GLenum mode) \
	GL_FUNC(Disable, void, GLenum mode) \
	GL_FUNC(Clear, void, GLenum mask) \
	GL_FUNC(ClearColor, void, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) \
	GL_FUNC(ClearDepth, void, GLdouble depth) \
	GL_FUNC(ClearStencil, void, GLint stencil) \
	GL_FUNC(DepthMask, void, GLboolean enabled) \
	GL_FUNC(DepthFunc, void, GLenum func) \
	GL_FUNC(Viewport, void, GLint x, GLint y, GLint width, GLint height) \
	GL_FUNC(Scissor, void, GLint x, GLint y, GLint width, GLint height) \
	GL_FUNC(CullFace, void, GLenum mode) \
	GL_FUNC(BlendEquation, void, GLenum eq) \
	GL_FUNC(BlendEquationSeparate, void, GLenum modeRGB, GLenum modeAlpha) \
	GL_FUNC(BlendFunc, void, GLenum sfactor, GLenum dfactor) \
	GL_FUNC(BlendFuncSeparate, void, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) \
	GL_FUNC(BlendColor, void, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) \
	GL_FUNC(ColorMask, void, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) \
	GL_FUNC(GetIntegerv, void, GLenum name, GLint* data) \
	GL_FUNC(GenTextures, void, GLint n, void* textures) \
	GL_FUNC(GenRenderbuffers, void, GLint n, void* textures) \
	GL_FUNC(GenFramebuffers, void, GLint n, void* textures) \
	GL_FUNC(ActiveTexture, void, GLuint id) \
	GL_FUNC(BindTexture, void, GLenum target, GLuint id) \
	GL_FUNC(BindRenderbuffer, void, GLenum target, GLuint id) \
	GL_FUNC(BindFramebuffer, void, GLenum target, GLuint id) \
	GL_FUNC(TexImage2D, void, GLenum target, GLint level, GLenum internalFormat, GLint width, GLint height, GLint border, GLenum format, GLenum type, const void* data) \
	GL_FUNC(FramebufferRenderbuffer, void, GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) \
	GL_FUNC(FramebufferTexture2D, void, GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) \
	GL_FUNC(TexParameteri, void, GLenum target, GLenum name, GLint param) \
	GL_FUNC(RenderbufferStorage, void, GLenum target, GLenum internalformat, GLint width, GLint height) \
	GL_FUNC(GetTexImage, void, GLenum target, GLint level, GLenum format, GLenum type, void* data) \
	GL_FUNC(DrawElements, void, GLenum mode, GLint count, GLenum type, void* indices) \
	GL_FUNC(DrawElementsInstanced, void, GLenum mode, GLint count, GLenum type, void* indices, GLint amount) \
	GL_FUNC(DeleteTextures, void, GLint n, GLuint* textures) \
	GL_FUNC(DeleteRenderbuffers, void, GLint n, GLuint* renderbuffers) \
	GL_FUNC(DeleteFramebuffers, void, GLint n, GLuint* textures) \
	GL_FUNC(GenVertexArrays, void, GLint n, GLuint* arrays) \
	GL_FUNC(BindVertexArray, void, GLuint id) \
	GL_FUNC(GenBuffers, void, GLint n, GLuint* arrays) \
	GL_FUNC(BindBuffer, void, GLenum target, GLuint buffer) \
	GL_FUNC(BufferData, void, GLenum target, GLsizeiptr size, const void* data, GLenum usage) \
	GL_FUNC(BufferSubData, void, GLenum target, GLintptr offset, GLsizeiptr size, const void* data) \
	GL_FUNC(DeleteBuffers, void, GLint n, GLuint* buffers) \
	GL_FUNC(DeleteVertexArrays, void, GLint n, GLuint* arrays) \
	GL_FUNC(EnableVertexAttribArray, void, GLuint location) \
	GL_FUNC(DisableVertexAttribArray, void, GLuint location) \
	GL_FUNC(VertexAttribPointer, void, GLuint index, GLint size, GLenum type, GLboolean normalized, GLint stride, const void* pointer) \
	GL_FUNC(VertexAttribDivisor, void, GLuint index, GLuint divisor) \
	GL_FUNC(CreateShader, GLuint, GLenum type) \
	GL_FUNC(AttachShader, void, GLuint program, GLuint shader) \
	GL_FUNC(DetachShader, void, GLuint program, GLuint shader) \
	GL_FUNC(DeleteShader, void, GLuint shader) \
	GL_FUNC(ShaderSource, void, GLuint shader, GLsizei count, const GLchar** string, const GLint* length) \
	GL_FUNC(CompileShader, void, GLuint shader) \
	GL_FUNC(GetShaderiv, void, GLuint shader, GLenum pname, GLint* result) \
	GL_FUNC(GetShaderInfoLog, void, GLuint shader, GLint maxLength, GLsizei* length, GLchar* infoLog) \
	GL_FUNC(CreateProgram, GLuint, ) \
	GL_FUNC(DeleteProgram, void, GLuint program) \
	GL_FUNC(LinkProgram, void, GLuint program) \
	GL_FUNC(GetProgramiv, void, GLuint program, GLenum pname, GLint* result) \
	GL_FUNC(GetProgramInfoLog, void, GLuint program, GLint maxLength, GLsizei* length, GLchar* infoLog) \
	GL_FUNC(GetActiveUniform, void, GLuint program, GLuint index, GLint bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name) \
	GL_FUNC(GetActiveAttrib, void, GLuint program, GLuint index, GLint bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name) \
	GL_FUNC(UseProgram, void, GLuint program) \
	GL_FUNC(GetUniformLocation, GLint, GLuint program, const GLchar* name) \
	GL_FUNC(GetAttribLocation, GLint, GLuint program, const GLchar* name) \
	GL_FUNC(Uniform1f, void, GLint location, GLfloat v0) \
	GL_FUNC(Uniform2f, void, GLint location, GLfloat v0, GLfloat v1) \
	GL_FUNC(Uniform3f, void, GLint location, GLfloat v0, GLfloat v1, GLfloat v2) \
	GL_FUNC(Uniform4f, void, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) \
	GL_FUNC(Uniform1fv, void, GLint location, GLint count, const GLfloat* value) \
	GL_FUNC(Uniform2fv, void, GLint location, GLint count, const GLfloat* value) \
	GL_FUNC(Uniform3fv, void, GLint location, GLint count, const GLfloat* value) \
	GL_FUNC(Uniform4fv, void, GLint location, GLint count, const GLfloat* value) \
	GL_FUNC(Uniform1i, void, GLint location, GLint v0) \
	GL_FUNC(Uniform2i, void, GLint location, GLint v0, GLint v1) \
	GL_FUNC(Uniform3i, void, GLint location, GLint v0, GLint v1, GLint v2) \
	GL_FUNC(Uniform4i, void, GLint location, GLint v0, GLint v1, GLint v2, GLint v3) \
	GL_FUNC(Uniform1iv, void, GLint location, GLint count, const GLint* value) \
	GL_FUNC(Uniform2iv, void, GLint location, GLint count, const GLint* value) \
	GL_FUNC(Uniform3iv, void, GLint location, GLint count, const GLint* value) \
	GL_FUNC(Uniform4iv, void, GLint location, GLint count, const GLint* value) \
	GL_FUNC(Uniform1ui, void, GLint location, GLuint v0) \
	GL_FUNC(Uniform2ui, void, GLint location, GLuint v0, GLuint v1) \
	GL_FUNC(Uniform3ui, void, GLint location, GLuint v0, GLuint v1, GLuint v2) \
	GL_FUNC(Uniform4ui, void, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3) \
	GL_FUNC(Uniform1uiv, void, GLint location, GLint count, const GLint* value) \
	GL_FUNC(Uniform2uiv, void, GLint location, GLint count, const GLint* value) \
	GL_FUNC(Uniform3uiv, void, GLint location, GLint count, const GLint* value) \
	GL_FUNC(Uniform4uiv, void, GLint location, GLint count, const GLint* value) \
	GL_FUNC(UniformMatrix2fv, void, GLint location, GLint count, GLboolean transpose, const GLfloat* value) \
	GL_FUNC(UniformMatrix3fv, void, GLint location, GLint count, GLboolean transpose, const GLfloat* value) \
	GL_FUNC(UniformMatrix4fv, void, GLint location, GLint count, GLboolean transpose, const GLfloat* value) \
	GL_FUNC(UniformMatrix2x3fv, void, GLint location, GLint count, GLboolean transpose, const GLfloat* value) \
	GL_FUNC(UniformMatrix3x2fv, void, GLint location, GLint count, GLboolean transpose, const GLfloat* value) \
	GL_FUNC(UniformMatrix2x4fv, void, GLint location, GLint count, GLboolean transpose, const GLfloat* value) \
	GL_FUNC(UniformMatrix4x2fv, void, GLint location, GLint count, GLboolean transpose, const GLfloat* value) \
	GL_FUNC(UniformMatrix3x4fv, void, GLint location, GLint count, GLboolean transpose, const GLfloat* value) \
	GL_FUNC(UniformMatrix4x3fv, void, GLint location, GLint count, GLboolean transpose, const GLfloat* value) \
	GL_FUNC(PixelStorei, void, GLenum pname, GLint param)

// Debug Function Delegate
typedef void (APIENTRY* DEBUGPROC)(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam);

// shorthand to our internal state
#define renderer ((Renderer_OpenGL*)App::Internal::renderer)

namespace Blah
{
	const ShaderData opengl_batch_shader_data = {
		// vertex shader
#ifdef __EMSCRIPTEN__
		"#version 300 es\n"
#else
		"#version 330\n"
#endif
		"uniform mat4 u_matrix;\n"
		"layout(location=0) in vec2 a_position;\n"
		"layout(location=1) in vec2 a_tex;\n"
		"layout(location=2) in vec4 a_color;\n"
		"layout(location=3) in vec4 a_type;\n"
		"out vec2 v_tex;\n"
		"out vec4 v_col;\n"
		"out vec4 v_type;\n"
		"void main(void)\n"
		"{\n"
		"	gl_Position = u_matrix * vec4(a_position.xy, 0, 1);\n"
		"	v_tex = a_tex;\n"
		"	v_col = a_color;\n"
		"	v_type = a_type;\n"
		"}",

		// fragment shader
#ifdef __EMSCRIPTEN__
		"#version 300 es\n"
		"precision mediump float;\n"
#else
		"#version 330\n"
#endif
		"uniform sampler2D u_texture;\n"
		"in vec2 v_tex;\n"
		"in vec4 v_col;\n"
		"in vec4 v_type;\n"
		"out vec4 o_color;\n"
		"void main(void)\n"
		"{\n"
		"	vec4 color = texture(u_texture, v_tex);\n"
		"	o_color = \n"
		"		v_type.x * color * v_col + \n"
		"		v_type.y * color.a * v_col + \n"
		"		v_type.z * v_col;\n"
		"}"
	};

	class Renderer_OpenGL : public Renderer
	{
	public:

		struct Bindings
		{
			// GL function pointers
			#define GL_FUNC(name, ret, ...) typedef ret (*name ## Func) (__VA_ARGS__); name ## Func name;
			GL_FUNCTIONS
			#undef GL_FUNC
		} gl;

		// state
		void* context;

		// info
		int max_color_attachments;
		int max_element_indices;
		int max_element_vertices;
		int max_renderbuffer_size;
		int max_samples;
		int max_texture_image_units;
		int max_texture_size;

		bool init() override;
		void shutdown() override;
		void update() override;
		void before_render() override;
		void after_render() override;
		void render(const DrawCall& pass) override;
		void clear_backbuffer(Color color, float depth, u8 stencil, ClearMask mask) override;
		TextureRef create_texture(int width, int height, TextureFormat format) override;
		TargetRef create_target(int width, int height, const TextureFormat* attachments, int attachment_count) override;
		ShaderRef create_shader(const ShaderData* data) override;
		MeshRef create_mesh() override;
	};

	// debug callback
	void APIENTRY gl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		// these are basically never useful
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION &&
			type == GL_DEBUG_TYPE_OTHER)
			return;

		const char* typeName = "";
		const char* severityName = "";

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: typeName = "ERROR"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeName = "DEPRECATED BEHAVIOR"; break;
		case GL_DEBUG_TYPE_MARKER: typeName = "MARKER"; break;
		case GL_DEBUG_TYPE_OTHER: typeName = "OTHER"; break;
		case GL_DEBUG_TYPE_PERFORMANCE: typeName = "PEROFRMANCE"; break;
		case GL_DEBUG_TYPE_POP_GROUP: typeName = "POP GROUP"; break;
		case GL_DEBUG_TYPE_PORTABILITY: typeName = "PORTABILITY"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP: typeName = "PUSH GROUP"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeName = "UNDEFINED BEHAVIOR"; break;
		}

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH: severityName = "HIGH"; break;
		case GL_DEBUG_SEVERITY_MEDIUM: severityName = "MEDIUM"; break;
		case GL_DEBUG_SEVERITY_LOW: severityName = "LOW"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: severityName = "NOTIFICATION"; break;
		}

		if (type == GL_DEBUG_TYPE_ERROR)
			Log::error("GL (%s:%s) %s", typeName, severityName, message);
		else if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
			Log::warn("GL (%s:%s) %s", typeName, severityName, message);
		else
			Log::info("GL (%s) %s", typeName, message);
	}

	// assign attributes
	GLuint gl_mesh_assign_attributes(GLuint buffer, GLenum buffer_type, const VertexFormat& format, GLint divisor)
	{
		// bind
		renderer->gl.BindBuffer(buffer_type, buffer);

		// TODO: disable existing enabled attributes ..
		// ...
		// ...

		// enable attributes
		size_t ptr = 0;
		for (int n = 0; n < format.attributes.size(); n++)
		{
			auto& attribute = format.attributes[n];
			GLenum type = GL_UNSIGNED_BYTE;
			size_t component_size = 0;
			int components = 1;

			if (attribute.type == VertexType::Float)
			{
				type = GL_FLOAT;
				component_size = 4;
				components = 1;
			}
			else if (attribute.type == VertexType::Float2)
			{
				type = GL_FLOAT;
				component_size = 4;
				components = 2;
			}
			else if (attribute.type == VertexType::Float3)
			{
				type = GL_FLOAT;
				component_size = 4;
				components = 3;
			}
			else if (attribute.type == VertexType::Float4)
			{
				type = GL_FLOAT;
				component_size = 4;
				components = 4;
			}
			else if (attribute.type == VertexType::Byte4)
			{
				type = GL_BYTE;
				component_size = 1;
				components = 4;
			}
			else if (attribute.type == VertexType::UByte4)
			{
				type = GL_UNSIGNED_BYTE;
				component_size = 1;
				components = 4;
			}
			else if (attribute.type == VertexType::Short2)
			{
				type = GL_SHORT;
				component_size = 2;
				components = 2;
			}
			else if (attribute.type == VertexType::UShort2)
			{
				type = GL_UNSIGNED_SHORT;
				component_size = 2;
				components = 2;
			}
			else if (attribute.type == VertexType::Short4)
			{
				type = GL_SHORT;
				component_size = 2;
				components = 4;
			}
			else if (attribute.type == VertexType::UShort4)
			{
				type = GL_UNSIGNED_SHORT;
				component_size = 2;
				components = 4;
			}

			u32 location = (u32)(attribute.index);
			renderer->gl.EnableVertexAttribArray(location);
			renderer->gl.VertexAttribPointer(location, components, type, attribute.normalized, format.stride, (void*)ptr);
			renderer->gl.VertexAttribDivisor(location, divisor);

			ptr += components * component_size;
		}

		return format.stride;
	}

	// convert blend op enum
	GLenum gl_get_blend_func(BlendOp operation)
	{
		switch (operation)
		{
		case BlendOp::Add:				return GL_FUNC_ADD;
		case BlendOp::Subtract:			return GL_FUNC_SUBTRACT;
		case BlendOp::ReverseSubtract:	return GL_FUNC_REVERSE_SUBTRACT;
		case BlendOp::Min:				return GL_MIN;
		case BlendOp::Max:				return GL_MAX;
		};
		return GL_FUNC_ADD;
	}

	// convert blend factor enum
	GLenum gl_get_blend_factor(BlendFactor factor)
	{
		switch (factor)
		{
		case BlendFactor::Zero:						return GL_ZERO;
		case BlendFactor::One:						return GL_ONE;
		case BlendFactor::SrcColor:					return GL_SRC_COLOR;
		case BlendFactor::OneMinusSrcColor:			return GL_ONE_MINUS_SRC_COLOR;
		case BlendFactor::DstColor:					return GL_DST_COLOR;
		case BlendFactor::OneMinusDstColor:			return GL_ONE_MINUS_DST_COLOR;
		case BlendFactor::SrcAlpha:					return GL_SRC_ALPHA;
		case BlendFactor::OneMinusSrcAlpha:			return GL_ONE_MINUS_SRC_ALPHA;
		case BlendFactor::DstAlpha:					return GL_DST_ALPHA;
		case BlendFactor::OneMinusDstAlpha:			return GL_ONE_MINUS_DST_ALPHA;
		case BlendFactor::ConstantColor:			return GL_CONSTANT_COLOR;
		case BlendFactor::OneMinusConstantColor:	return GL_ONE_MINUS_CONSTANT_COLOR;
		case BlendFactor::ConstantAlpha:			return GL_CONSTANT_ALPHA;
		case BlendFactor::OneMinusConstantAlpha:	return GL_ONE_MINUS_CONSTANT_ALPHA;
		case BlendFactor::SrcAlphaSaturate:			return GL_SRC_ALPHA_SATURATE;
		case BlendFactor::Src1Color:				return GL_SRC1_COLOR;
		case BlendFactor::OneMinusSrc1Color:		return GL_ONE_MINUS_SRC1_COLOR;
		case BlendFactor::Src1Alpha:				return GL_SRC1_ALPHA;
		case BlendFactor::OneMinusSrc1Alpha:		return GL_ONE_MINUS_SRC1_ALPHA;
		};

		return GL_ZERO;
	}

	class OpenGL_Texture : public Texture
	{
	private:
		GLuint m_id;
		int m_width;
		int m_height;
		TextureSampler m_sampler;
		TextureFormat m_format;
		GLenum m_gl_internal_format;
		GLenum m_gl_format;
		GLenum m_gl_type;

	public:
		bool framebuffer_parent;

		OpenGL_Texture(int width, int height, TextureFormat format)
		{
			m_id = 0;
			m_width = width;
			m_height = height;
			m_sampler = TextureSampler(TextureFilter::None, TextureWrap::None, TextureWrap::None);
			m_format = format;
			framebuffer_parent = false;
			m_gl_internal_format = GL_RED;
			m_gl_format = GL_RED;
			m_gl_type = GL_UNSIGNED_BYTE;

			if (width > renderer->max_texture_size || height > renderer->max_texture_size)
			{
				Log::error("Exceeded Max Texture Size of %i", renderer->max_texture_size);
				return;
			}

			if (format == TextureFormat::R)
			{
				m_gl_internal_format = GL_RED;
				m_gl_format = GL_RED;
				m_gl_type = GL_UNSIGNED_BYTE;
			}
			else if (format == TextureFormat::RG)
			{
				m_gl_internal_format = GL_RG;
				m_gl_format = GL_RG;
				m_gl_type = GL_UNSIGNED_BYTE;
			}
			else if (format == TextureFormat::RGBA)
			{
				m_gl_internal_format = GL_RGBA;
				m_gl_format = GL_RGBA;
				m_gl_type = GL_UNSIGNED_BYTE;
			}
			else if (format == TextureFormat::DepthStencil)
			{
				m_gl_internal_format = GL_DEPTH24_STENCIL8;
				m_gl_format = GL_DEPTH_STENCIL;
				m_gl_type = GL_UNSIGNED_INT_24_8;
			}
			else
			{
				Log::error("Invalid Texture Format %i", format);
				return;
			}

			renderer->gl.GenTextures(1, &m_id);
			renderer->gl.ActiveTexture(GL_TEXTURE0);
			renderer->gl.BindTexture(GL_TEXTURE_2D, m_id);
			renderer->gl.TexImage2D(GL_TEXTURE_2D, 0, m_gl_internal_format, width, height, 0, m_gl_format, m_gl_type, nullptr);
		}

		~OpenGL_Texture()
		{
			if (m_id > 0 && renderer)
				renderer->gl.DeleteTextures(1, &m_id);
		}

		GLuint gl_id() const
		{
			return m_id;
		}

		virtual int width() const override
		{
			return m_width;
		}

		virtual int height() const override
		{
			return m_height;
		}

		virtual TextureFormat format() const override
		{
			return m_format;
		}

		void update_sampler(const TextureSampler& sampler)
		{
			if (m_sampler != sampler)
			{
				m_sampler = sampler;

				renderer->gl.BindTexture(GL_TEXTURE_2D, m_id);
				renderer->gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (m_sampler.filter == TextureFilter::Nearest ? GL_NEAREST : GL_LINEAR));
				renderer->gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (m_sampler.filter == TextureFilter::Nearest ? GL_NEAREST : GL_LINEAR));
				renderer->gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (m_sampler.wrap_x == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT));
				renderer->gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (m_sampler.wrap_y == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT));
			}
		}

		virtual void set_data(const u8* data) override
		{
			renderer->gl.ActiveTexture(GL_TEXTURE0);
			renderer->gl.BindTexture(GL_TEXTURE_2D, m_id);
			renderer->gl.TexImage2D(GL_TEXTURE_2D, 0, m_gl_internal_format, m_width, m_height, 0, m_gl_format, m_gl_type, data);
		}

		virtual void get_data(u8* data) override
		{
			renderer->gl.ActiveTexture(GL_TEXTURE0);
			renderer->gl.BindTexture(GL_TEXTURE_2D, m_id);
			renderer->gl.GetTexImage(GL_TEXTURE_2D, 0, m_gl_internal_format, m_gl_type, data);
		}

		virtual bool is_framebuffer() const override
		{
			return framebuffer_parent;
		}

	};

	class OpenGL_Target : public Target
	{
	private:
		GLuint m_id;
		int m_width;
		int m_height;
		StackVector<TextureRef, Attachments::capacity> m_attachments;

	public:

		OpenGL_Target(int width, int height, const TextureFormat* attachments, int attachmentCount)
		{
			renderer->gl.GenFramebuffers(1, &m_id);
			m_width = width;
			m_height = height;

			renderer->gl.BindFramebuffer(GL_FRAMEBUFFER, m_id);

			for (int i = 0; i < attachmentCount; i++)
			{
				auto tex = Texture::create(width, height, attachments[i]);
				auto gltex = ((OpenGL_Texture*)tex.get());

				gltex->framebuffer_parent = true;
				m_attachments.push_back(tex);

				if (attachments[i] != TextureFormat::DepthStencil)
				{
					renderer->gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, gltex->gl_id(), 0);
				}
				else
				{
					renderer->gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, gltex->gl_id(), 0);
				}
			}
		}

		~OpenGL_Target()
		{
			if (m_id > 0 && renderer)
			{
				renderer->gl.DeleteFramebuffers(1, &m_id);
				m_id = 0;
			}
		}

		GLuint gl_id() const
		{
			return m_id;
		}

		virtual Attachments& textures() override
		{
			return m_attachments;
		}

		virtual const Attachments& textures() const override
		{
			return m_attachments;
		}

		virtual void clear(Color color, float depth, u8 stencil, ClearMask mask) override
		{
			renderer->gl.BindFramebuffer(GL_FRAMEBUFFER, m_id);
			renderer->gl.Disable(GL_SCISSOR_TEST);

			int clear = 0;

			if (((int)mask & (int)ClearMask::Color) == (int)ClearMask::Color)
			{
				clear |= GL_COLOR_BUFFER_BIT;
				renderer->gl.ColorMask(true, true, true, true);
				renderer->gl.ClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
			}
			
			if (((int)mask & (int)ClearMask::Depth) == (int)ClearMask::Depth)
			{
				clear |= GL_DEPTH_BUFFER_BIT;
				if (renderer->gl.ClearDepth)
					renderer->gl.ClearDepth(depth);
			}

			if (((int)mask & (int)ClearMask::Stencil) == (int)ClearMask::Stencil)
			{
				clear |= GL_STENCIL_BUFFER_BIT;
				if (renderer->gl.ClearStencil)
					renderer->gl.ClearStencil(stencil);
			}

			renderer->gl.Clear(clear);
		}
	};

	class OpenGL_Shader : public Shader
	{
	private:
		GLuint m_id;
		Vector<UniformInfo> m_uniforms;

	public:
		Vector<GLint> uniform_locations;

		OpenGL_Shader(const ShaderData* data)
		{
			m_id = 0;

			if (data->vertex.length() <= 0)
			{
				Log::error("Vertex Shader is required");
				return;
			}

			if (data->fragment.length() <= 0)
			{
				Log::error("Fragment Shader is required");
				return;
			}

			GLchar log[1024] = { 0 };
			GLsizei log_length = 0;

			GLuint vertex_shader = renderer->gl.CreateShader(GL_VERTEX_SHADER);
			{
				const GLchar* source = (const GLchar*)data->vertex.cstr();
				renderer->gl.ShaderSource(vertex_shader, 1, &source, nullptr);
				renderer->gl.CompileShader(vertex_shader);
				renderer->gl.GetShaderInfoLog(vertex_shader, 1024, &log_length, log);

				if (log_length > 0)
				{
					renderer->gl.DeleteShader(vertex_shader);
					Log::error(log);
					return;
				}
			}

			GLuint fragment_shader = renderer->gl.CreateShader(GL_FRAGMENT_SHADER);
			{
				const GLchar* source = (const GLchar*)data->fragment.cstr();
				renderer->gl.ShaderSource(fragment_shader, 1, &source, nullptr);
				renderer->gl.CompileShader(fragment_shader);
				renderer->gl.GetShaderInfoLog(fragment_shader, 1024, &log_length, log);

				if (log_length > 0)
				{
					renderer->gl.DeleteShader(vertex_shader);
					renderer->gl.DeleteShader(fragment_shader);
					Log::error(log);
					return;
				}
			}

			// create actual shader program
			GLuint id = renderer->gl.CreateProgram();
			renderer->gl.AttachShader(id, vertex_shader);
			renderer->gl.AttachShader(id, fragment_shader);
			renderer->gl.LinkProgram(id);
			renderer->gl.GetProgramInfoLog(id, 1024, &log_length, log);
			renderer->gl.DetachShader(id, vertex_shader);
			renderer->gl.DetachShader(id, fragment_shader);
			renderer->gl.DeleteShader(vertex_shader);
			renderer->gl.DeleteShader(fragment_shader);

			if (log_length > 0)
			{
				Log::error(log);
				return;
			}

			// get uniforms
			bool valid_uniforms = true;
			{
				const int max_name_length = 256;

				GLint active_uniforms = 0;
				GLint sampler_uniforms = 0;
				renderer->gl.GetProgramiv(id, GL_ACTIVE_UNIFORMS, &active_uniforms);

				for (int i = 0; i < active_uniforms; i++)
				{
					GLsizei length;
					GLsizei size;
					GLenum type;
					GLchar name[max_name_length + 1] = { 0 };

					renderer->gl.GetActiveUniform(id, i, max_name_length, &length, &size, &type, name);
					name[length] = '\0';

					// array names end with "[0]", and we don't want that
					for (int n = 0; n < max_name_length; n++)
						if (name[n] == '[')
						{
							if (name[n + 1] == '0' && name[n + 2] == ']')
							{
								name[n] = '\0';
								break;
							}
						}

					if (type == GL_SAMPLER_2D)
					{
						UniformInfo tex_uniform;
						tex_uniform.name = name;
						tex_uniform.register_index = sampler_uniforms;
						tex_uniform.buffer_index = 0;
						tex_uniform.array_length = size;
						tex_uniform.type = UniformType::Texture2D;
						tex_uniform.shader = ShaderType::Fragment;
						uniform_locations.push_back(renderer->gl.GetUniformLocation(id, name));
						m_uniforms.push_back(tex_uniform);

						UniformInfo sampler_uniform;
						sampler_uniform.name = String(name) + "_sampler";
						sampler_uniform.register_index = sampler_uniforms;
						sampler_uniform.buffer_index = 0;
						sampler_uniform.array_length = size;
						sampler_uniform.type = UniformType::Sampler2D;
						sampler_uniform.shader = ShaderType::Fragment;
						uniform_locations.push_back(renderer->gl.GetUniformLocation(id, name));
						m_uniforms.push_back(sampler_uniform);

						sampler_uniforms += size;
					}
					else
					{
						UniformInfo uniform;
						uniform.name = name;
						uniform.type = UniformType::None;
						uniform.register_index = 0;
						uniform.buffer_index = 0;
						uniform.array_length = size;
						uniform_locations.push_back(renderer->gl.GetUniformLocation(id, name));
						uniform.shader = (ShaderType)((int)ShaderType::Vertex | (int)ShaderType::Fragment);

						if (type == GL_FLOAT)
							uniform.type = UniformType::Float;
						else if (type == GL_FLOAT_VEC2)
							uniform.type = UniformType::Float2;
						else if (type == GL_FLOAT_VEC3)
							uniform.type = UniformType::Float3;
						else if (type == GL_FLOAT_VEC4)
							uniform.type = UniformType::Float4;
						else if (type == GL_FLOAT_MAT3x2)
							uniform.type = UniformType::Mat3x2;
						else if (type == GL_FLOAT_MAT4)
							uniform.type = UniformType::Mat4x4;
						else
						{
							valid_uniforms = false;
							Log::error("Unsupported Uniform Type");
							break;
						}

						m_uniforms.push_back(uniform);
					}

				}
			}

			// assign ID if the uniforms were valid
			if (!valid_uniforms)
				renderer->gl.DeleteProgram(id);
			else
				m_id = id;
		}

		~OpenGL_Shader()
		{
			if (m_id > 0 && renderer)
				renderer->gl.DeleteProgram(m_id);
			m_id = 0;
		}

		GLuint gl_id() const
		{
			return m_id;
		}

		virtual Vector<UniformInfo>& uniforms() override
		{
			return m_uniforms;
		}

		virtual const Vector<UniformInfo>& uniforms() const override
		{
			return m_uniforms;
		}
	};

	class OpenGL_Mesh : public Mesh
	{
	private:
		GLuint m_id;
		GLuint m_index_buffer;
		GLuint m_vertex_buffer;
		GLuint m_instance_buffer;
		i64 m_index_count;
		i64 m_vertex_count;
		i64 m_instance_count;
		u16 m_vertex_size;
		u16 m_instance_size;
		u8 m_vertex_attribs_enabled;
		u8 m_instance_attribs_enabled;
		Vector<GLuint> m_vertex_attribs;
		Vector<GLuint> m_instance_attribs;
		GLenum m_index_format;
		int m_index_size;

	public:

		OpenGL_Mesh()
		{
			m_id = 0;
			m_index_buffer = 0;
			m_vertex_buffer = 0;
			m_instance_buffer = 0;
			m_index_count = 0;
			m_vertex_count = 0;
			m_instance_count = 0;
			m_vertex_size = 0;
			m_instance_size = 0;
			m_vertex_attribs_enabled = 0;
			m_instance_attribs_enabled = 0;

			renderer->gl.GenVertexArrays(1, &m_id);
		}

		~OpenGL_Mesh()
		{
			if (renderer)
			{
				if (m_vertex_buffer != 0)
					renderer->gl.DeleteBuffers(1, &m_vertex_buffer);
				if (m_index_buffer != 0)
					renderer->gl.DeleteBuffers(1, &m_index_buffer);
				if (m_instance_buffer != 0)
					renderer->gl.DeleteBuffers(1, &m_instance_buffer);
				if (m_id != 0)
					renderer->gl.DeleteVertexArrays(1, &m_id);
			}
			m_id = 0;
		}

		GLuint gl_id() const
		{
			return m_id;
		}

		GLenum gl_index_format() const
		{
			return m_index_format;
		}

		int gl_index_size() const
		{
			return m_index_size;
		}

		virtual void index_data(IndexFormat format, const void* indices, i64 count) override
		{
			m_index_count = count;

			renderer->gl.BindVertexArray(m_id);
			{
				if (m_index_buffer == 0)
					renderer->gl.GenBuffers(1, &(m_index_buffer));

				switch (format)
				{
				case IndexFormat::UInt16:
					m_index_format = GL_UNSIGNED_SHORT;
					m_index_size = 2;
					break;
				case IndexFormat::UInt32:
					m_index_format = GL_UNSIGNED_INT;
					m_index_size = 4;
					break;
				}

				renderer->gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
				renderer->gl.BufferData(GL_ELEMENT_ARRAY_BUFFER, m_index_size * count, indices, GL_DYNAMIC_DRAW);
			}
			renderer->gl.BindVertexArray(0);
		}

		virtual void vertex_data(const VertexFormat& format, const void* vertices, i64 count) override
		{
			m_vertex_count = count;

			renderer->gl.BindVertexArray(m_id);
			{
				// Create Buffer if it doesn't exist yet
				if (m_vertex_buffer == 0)
					renderer->gl.GenBuffers(1, &(m_vertex_buffer));

				// TODO:
				// Cache this
				m_vertex_size = gl_mesh_assign_attributes(m_vertex_buffer, GL_ARRAY_BUFFER, format, 0);

				// Upload Buffer
				renderer->gl.BindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
				renderer->gl.BufferData(GL_ARRAY_BUFFER, m_vertex_size * count, vertices, GL_DYNAMIC_DRAW);
			}
			renderer->gl.BindVertexArray(0);
		}

		virtual void instance_data(const VertexFormat& format, const void* instances, i64 count) override
		{
			m_instance_count = count;

			renderer->gl.BindVertexArray(m_id);
			{
				// Create Buffer if it doesn't exist yet
				if (m_instance_buffer == 0)
					renderer->gl.GenBuffers(1, &(m_instance_buffer));

				// TODO:
				// Cache this
				m_instance_size = gl_mesh_assign_attributes(m_instance_buffer, GL_ARRAY_BUFFER, format, 1);

				// Upload Buffer
				renderer->gl.BindBuffer(GL_ARRAY_BUFFER, m_instance_buffer);
				renderer->gl.BufferData(GL_ARRAY_BUFFER, m_instance_size * count, instances, GL_DYNAMIC_DRAW);
			}
			renderer->gl.BindVertexArray(0);
		}

		virtual i64 index_count() const override
		{
			return m_index_count;
		}

		virtual i64 vertex_count() const override
		{
			return m_vertex_count;
		}

		virtual i64 instance_count() const override
		{
			return m_instance_count;
		}
	};

	bool Renderer_OpenGL::init()
	{
		// create gl context
		context = App::Internal::platform->gl_context_create();
		if (context == nullptr)
		{
			Log::error("Failed to create OpenGL Context");
			return false;
		}
		App::Internal::platform->gl_context_make_current(context);

		// bind opengl functions
		#define GL_FUNC(name, ...) gl.name = (Renderer_OpenGL::Bindings::name ## Func)(App::Internal::platform->gl_get_func("gl" #name));
		GL_FUNCTIONS
		#undef GL_FUNC

		// bind debug message callback
		if (gl.DebugMessageCallback != nullptr)
		{
			gl.Enable(GL_DEBUG_OUTPUT);
			gl.Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			gl.DebugMessageCallback(gl_message_callback, nullptr);
		}

		// get opengl info
		gl.GetIntegerv(0x8CDF, &max_color_attachments);
		gl.GetIntegerv(0x80E9, &max_element_indices);
		gl.GetIntegerv(0x80E8, &max_element_vertices);
		gl.GetIntegerv(0x84E8, &max_renderbuffer_size);
		gl.GetIntegerv(0x8D57, &max_samples);
		gl.GetIntegerv(0x8872, &max_texture_image_units);
		gl.GetIntegerv(0x0D33, &max_texture_size);

		// log
		Log::info("OpenGL %s, %s",
			gl.GetString(GL_VERSION),
			gl.GetString(GL_RENDERER));

		// don't include row padding
		gl.PixelStorei(GL_PACK_ALIGNMENT, 1);
		gl.PixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// assign info
		info.type = RendererType::OpenGL;
		info.instancing = true;
		info.origin_bottom_left = true;
		info.max_texture_size = max_texture_size;

		// create the default batch shader
		default_batcher_shader = Shader::create(opengl_batch_shader_data);

		return true;
	}

	void Renderer_OpenGL::shutdown()
	{
		App::Internal::platform->gl_context_destroy(context);
		context = nullptr;
	}

	void Renderer_OpenGL::update() {}
	void Renderer_OpenGL::before_render() {}
	void Renderer_OpenGL::after_render() {}

	TextureRef Renderer_OpenGL::create_texture(int width, int height, TextureFormat format)
	{
		auto resource = new OpenGL_Texture(width, height, format);

		if (resource->gl_id() <= 0)
		{
			delete resource;
			return TextureRef();
		}

		return TextureRef(resource);
	}

	TargetRef Renderer_OpenGL::create_target(int width, int height, const TextureFormat* attachments, int attachmentCount)
	{
		auto resource = new OpenGL_Target(width, height, attachments, attachmentCount);

		if (resource->gl_id() <= 0)
		{
			delete resource;
			return TargetRef();
		}

		return TargetRef(resource);
	}

	ShaderRef Renderer_OpenGL::create_shader(const ShaderData* data)
	{
		auto resource = new OpenGL_Shader(data);

		if (resource->gl_id() <= 0)
		{
			delete resource;
			return ShaderRef();
		}

		return ShaderRef(resource);
	}

	MeshRef Renderer_OpenGL::create_mesh()
	{
		auto resource = new OpenGL_Mesh();

		if (resource->gl_id() <= 0)
		{
			delete resource;
			return MeshRef();
		}

		return MeshRef(resource);
	}

	void Renderer_OpenGL::render(const DrawCall& pass)
	{
		// Bind the Target
		if (pass.target == App::backbuffer())
		{
			renderer->gl.BindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else if (pass.target)
		{
			auto framebuffer = (OpenGL_Target*)pass.target.get();
			renderer->gl.BindFramebuffer(GL_FRAMEBUFFER, framebuffer->gl_id());
		}

		auto size = Point(pass.target->width(), pass.target->height());
		auto shader_ref = pass.material->shader();
		auto shader = (OpenGL_Shader*)shader_ref.get();
		auto mesh = (OpenGL_Mesh*)pass.mesh.get();

		// Use the Shader
		// TODO: I don't love how material values are assigned or set here
		// TODO: this should be cached?
		{
			renderer->gl.UseProgram(shader->gl_id());

			int texture_slot = 0;
			GLint texture_ids[64];
			auto& uniforms = shader->uniforms();
			auto data = pass.material->data();

			for (int i = 0; i < uniforms.size(); i++)
			{
				auto location = shader->uniform_locations[i];
				auto& uniform = uniforms[i];

				// Sampler 2D's are assigned by updating texture values
				if (uniform.type == UniformType::Sampler2D)
					continue;

				// Texture2D
				if (uniform.type == UniformType::Texture2D)
				{
					for (int n = 0; n < uniform.array_length; n++)
					{
						auto tex = pass.material->get_texture(texture_slot);
						auto sampler = pass.material->get_sampler(texture_slot);

						renderer->gl.ActiveTexture(GL_TEXTURE0 + texture_slot);

						if (!tex)
						{
							renderer->gl.BindTexture(GL_TEXTURE_2D, 0);
						}
						else
						{
							auto gl_tex = ((OpenGL_Texture*)tex.get());
							gl_tex->update_sampler(sampler);
							renderer->gl.BindTexture(GL_TEXTURE_2D, gl_tex->gl_id());
						}

						texture_ids[n] = texture_slot;
						texture_slot++;
					}

					renderer->gl.Uniform1iv(location, (GLint)uniform.array_length, &texture_ids[0]);
					continue;
				}

				// Float
				if (uniform.type == UniformType::Float)
				{
					renderer->gl.Uniform1fv(location, (GLint)uniform.array_length, data);
					data += uniform.array_length;
				}
				// Float2
				else if (uniform.type == UniformType::Float2)
				{
					renderer->gl.Uniform2fv(location, (GLint)uniform.array_length, data);
					data += 2 * uniform.array_length;
				}
				// Float3
				else if (uniform.type == UniformType::Float3)
				{
					renderer->gl.Uniform3fv(location, (GLint)uniform.array_length, data);
					data += 3 * uniform.array_length;
				}
				// Float4
				else if (uniform.type == UniformType::Float4)
				{
					renderer->gl.Uniform4fv(location, (GLint)uniform.array_length, data);
					data += 4 * uniform.array_length;
				}
				// Matrix3x2
				else if (uniform.type == UniformType::Mat3x2)
				{
					renderer->gl.UniformMatrix3x2fv(location, (GLint)uniform.array_length, 0, data);
					data += 6 * uniform.array_length;
				}
				// Matrix4x4
				else if (uniform.type == UniformType::Mat4x4)
				{
					renderer->gl.UniformMatrix4fv(location, (GLint)uniform.array_length, 0, data);
					data += 16 * uniform.array_length;
				}
			}
		}

		// Blend Mode
		{
			GLenum colorOp = gl_get_blend_func(pass.blend.color_op);
			GLenum alphaOp = gl_get_blend_func(pass.blend.alpha_op);
			GLenum colorSrc = gl_get_blend_factor(pass.blend.color_src);
			GLenum colorDst = gl_get_blend_factor(pass.blend.color_dst);
			GLenum alphaSrc = gl_get_blend_factor(pass.blend.alpha_src);
			GLenum alphaDst = gl_get_blend_factor(pass.blend.alpha_dst);

			renderer->gl.Enable(GL_BLEND);
			renderer->gl.BlendEquationSeparate(colorOp, alphaOp);
			renderer->gl.BlendFuncSeparate(colorSrc, colorDst, alphaSrc, alphaDst);

			renderer->gl.ColorMask(
				((int)pass.blend.mask & (int)BlendMask::Red),
				((int)pass.blend.mask & (int)BlendMask::Green),
				((int)pass.blend.mask & (int)BlendMask::Blue),
				((int)pass.blend.mask & (int)BlendMask::Alpha));

			unsigned char r = pass.blend.rgba >> 24;
			unsigned char g = pass.blend.rgba >> 16;
			unsigned char b = pass.blend.rgba >> 8;
			unsigned char a = pass.blend.rgba;

			renderer->gl.BlendColor(
				r / 255.0f,
				g / 255.0f,
				b / 255.0f,
				a / 255.0f);
		}

		// Depth Function
		{
			if (pass.depth == Compare::None)
			{
				renderer->gl.Disable(GL_DEPTH_TEST);
			}
			else
			{
				renderer->gl.Enable(GL_DEPTH_TEST);

				switch (pass.depth)
				{
				case Compare::None: break;
				case Compare::Always:
					renderer->gl.DepthFunc(GL_ALWAYS);
					break;
				case Compare::Equal:
					renderer->gl.DepthFunc(GL_EQUAL);
					break;
				case Compare::Greater:
					renderer->gl.DepthFunc(GL_GREATER);
					break;
				case Compare::GreatorOrEqual:
					renderer->gl.DepthFunc(GL_GEQUAL);
					break;
				case Compare::Less:
					renderer->gl.DepthFunc(GL_LESS);
					break;
				case Compare::LessOrEqual:
					renderer->gl.DepthFunc(GL_LEQUAL);
					break;
				case Compare::Never:
					renderer->gl.DepthFunc(GL_NEVER);
					break;
				case Compare::NotEqual:
					renderer->gl.DepthFunc(GL_NOTEQUAL);
					break;
				}
			}
		}

		// Cull Mode
		{
			if (pass.cull == Cull::None)
			{
				renderer->gl.Disable(GL_CULL_FACE);
			}
			else
			{
				renderer->gl.Enable(GL_CULL_FACE);

				if (pass.cull == Cull::Back)
					renderer->gl.CullFace(GL_BACK);
				else if (pass.cull == Cull::Front)
					renderer->gl.CullFace(GL_FRONT);
				else
					renderer->gl.CullFace(GL_FRONT_AND_BACK);
			}
		}

		// Viewport
		{
			Rectf viewport = pass.viewport;
			viewport.y = size.y - viewport.y - viewport.h;

			renderer->gl.Viewport((GLint)viewport.x, (GLint)viewport.y, (GLint)viewport.w, (GLint)viewport.h);
		}

		// Scissor
		{
			if (!pass.has_scissor)
			{
				renderer->gl.Disable(GL_SCISSOR_TEST);
			}
			else
			{
				Rectf scissor = pass.scissor;
				scissor.y = size.y - scissor.y - scissor.h;

				if (scissor.w < 0)
					scissor.w = 0;
				if (scissor.h < 0)
					scissor.h = 0;

				renderer->gl.Enable(GL_SCISSOR_TEST);
				renderer->gl.Scissor((GLint)scissor.x, (GLint)scissor.y, (GLint)scissor.w, (GLint)scissor.h);
			}
		}

		// Draw the Mesh
		{
			renderer->gl.BindVertexArray(mesh->gl_id());

			GLenum index_format = mesh->gl_index_format();
			int index_size = mesh->gl_index_size();

			if (pass.instance_count > 0)
			{
				renderer->gl.DrawElementsInstanced(
					GL_TRIANGLES,
					(GLint)(pass.index_count),
					index_format,
					(void*)(index_size* pass.index_start),
					(GLint)pass.instance_count);
			}
			else
			{
				renderer->gl.DrawElements(
					GL_TRIANGLES,
					(GLint)(pass.index_count),
					index_format,
					(void*)(index_size * pass.index_start));
			}

			renderer->gl.BindVertexArray(0);
		}
	}

	void Renderer_OpenGL::clear_backbuffer(Color color, float depth, u8 stencil, ClearMask mask)
	{
			renderer->gl.BindFramebuffer(GL_FRAMEBUFFER, 0);
			renderer->gl.Disable(GL_SCISSOR_TEST);

			int clear = 0;

			if (((int)mask & (int)ClearMask::Color) == (int)ClearMask::Color)
			{
				clear |= GL_COLOR_BUFFER_BIT;
				renderer->gl.ColorMask(true, true, true, true);
				renderer->gl.ClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
			}
			
			if (((int)mask & (int)ClearMask::Depth) == (int)ClearMask::Depth)
			{
				clear |= GL_DEPTH_BUFFER_BIT;
				if (renderer->gl.ClearDepth)
					renderer->gl.ClearDepth(depth);
			}

			if (((int)mask & (int)ClearMask::Stencil) == (int)ClearMask::Stencil)
			{
				clear |= GL_STENCIL_BUFFER_BIT;
				if (renderer->gl.ClearStencil)
					renderer->gl.ClearStencil(stencil);
			}

			renderer->gl.Clear(clear);
	}
}

Blah::Renderer* Blah::Renderer::try_make_opengl()
{
	return new Blah::Renderer_OpenGL();
}

#else // BLAH_RENDERER_OPENGL

#include "renderer.h"
Blah::Renderer* Blah::Renderer::try_make_opengl()
{
	return nullptr;
}

#endif


