#include <blah/graphics/graphics.h>
#include <blah/internal/graphics.h>
#include <blah/internal/platform.h>
#include <blah/graphics/texture.h>
#include <blah/graphics/framebuffer.h>
#include <blah/graphics/mesh.h>
#include <blah/graphics/shader.h>
#include <blah/graphics/material.h>

#ifdef BLAH_USE_OPENGL

#include <stdio.h>
#include <string.h>
#include <stddef.h>

#ifdef _WIN32
#include <windows.h>
#else
#define APIENTRY
#endif

typedef ptrdiff_t	GLintptr;
typedef ptrdiff_t	GLsizeiptr;
typedef unsigned int	GLenum;
typedef unsigned char	GLboolean;
typedef unsigned int	GLbitfield;
typedef void		GLvoid;
typedef signed char	GLbyte;		/* 1-byte signed */
typedef short		GLshort;	/* 2-byte signed */
typedef int		GLint;		/* 4-byte signed */
typedef unsigned char	GLubyte;	/* 1-byte unsigned */
typedef unsigned short	GLushort;	/* 2-byte unsigned */
typedef unsigned int	GLuint;		/* 4-byte unsigned */
typedef int		GLsizei;	/* 4-byte signed */
typedef float		GLfloat;	/* single precision float */
typedef float		GLclampf;	/* single precision float in [0,1] */
typedef double		GLdouble;	/* double precision float */
typedef double		GLclampd;	/* double precision float in [0,1] */
typedef char		GLchar;

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

typedef void (APIENTRY *DEBUGPROC)(GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar *message,
			const void *userParam);

#define GL_FUNC(name, ret, ...) \
typedef ret (*name ## Func) (__VA_ARGS__); \
name ## Func name

#define GL_BIND(name) gl.name = (GL::name ## Func)(Internal::Platform::gl_get_func("gl" #name));

namespace Blah
{
	namespace OpenGL
	{
		struct GL
		{
			GL_FUNC(DebugMessageCallback, void, DEBUGPROC callback, const void* userParam);
			GL_FUNC(GetString, const GLubyte*, GLenum name);
			GL_FUNC(Flush, void, void);
			GL_FUNC(Enable, void, GLenum mode);
			GL_FUNC(Disable, void, GLenum mode);
			GL_FUNC(Clear, void, GLenum mask);
			GL_FUNC(ClearColor, void, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
			GL_FUNC(ClearDepth, void, GLdouble depth);
			GL_FUNC(ClearStencil, void, GLint stencil);
			GL_FUNC(DepthMask, void, GLboolean enabled);
			GL_FUNC(DepthFunc, void, GLenum func);
			GL_FUNC(Viewport, void, GLint x, GLint y, GLint width, GLint height);
			GL_FUNC(Scissor, void, GLint x, GLint y, GLint width, GLint height);
			GL_FUNC(CullFace, void, GLenum mode);
			GL_FUNC(BlendEquation, void, GLenum eq);
			GL_FUNC(BlendEquationSeparate, void, GLenum modeRGB, GLenum modeAlpha);
			GL_FUNC(BlendFunc, void, GLenum sfactor, GLenum dfactor);
			GL_FUNC(BlendFuncSeparate, void, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
			GL_FUNC(BlendColor, void, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
			GL_FUNC(ColorMask, void, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
			GL_FUNC(GetIntegerv, void, GLenum name, GLint* data);
			GL_FUNC(GenTextures, void, GLint n, void* textures);
			GL_FUNC(GenRenderbuffers, void, GLint n, void* textures);
			GL_FUNC(GenFramebuffers, void, GLint n, void* textures);
			GL_FUNC(ActiveTexture, void, GLuint id);
			GL_FUNC(BindTexture, void, GLenum target, GLuint id);
			GL_FUNC(BindRenderbuffer, void, GLenum target, GLuint id);
			GL_FUNC(BindFramebuffer, void, GLenum target, GLuint id);
			GL_FUNC(TexImage2D, void, GLenum target, GLint level, GLenum internalFormat, GLint width, GLint height, GLint border, GLenum format, GLenum type, void* data);
			GL_FUNC(FramebufferRenderbuffer, void, GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
			GL_FUNC(FramebufferTexture2D, void, GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
			GL_FUNC(TexParameteri, void, GLenum target, GLenum name, GLint param);
			GL_FUNC(RenderbufferStorage, void, GLenum target, GLenum internalformat, GLint width, GLint height);
			GL_FUNC(GetTexImage, void, GLenum target, GLint level, GLenum format, GLenum type, void* data);
			GL_FUNC(DrawElements, void, GLenum mode, GLint count, GLenum type, void* indices);
			GL_FUNC(DrawElementsInstanced, void, GLenum mode, GLint count, GLenum type, void* indices, GLint amount);
			GL_FUNC(DeleteTextures, void, GLint n, GLuint* textures);
			GL_FUNC(DeleteRenderbuffers, void, GLint n, GLuint* renderbuffers);
			GL_FUNC(DeleteFramebuffers, void, GLint n, GLuint* textures);
			GL_FUNC(GenVertexArrays, void, GLint n, GLuint* arrays);
			GL_FUNC(BindVertexArray, void, GLuint id);
			GL_FUNC(GenBuffers, void, GLint n, GLuint* arrays);
			GL_FUNC(BindBuffer, void, GLenum target, GLuint buffer);
			GL_FUNC(BufferData, void, GLenum target, GLsizeiptr size, const void* data, GLenum usage);
			GL_FUNC(BufferSubData, void, GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
			GL_FUNC(DeleteBuffers, void, GLint n, GLuint* buffers);
			GL_FUNC(DeleteVertexArrays, void, GLint n, GLuint* arrays);
			GL_FUNC(EnableVertexAttribArray, void, GLuint location);
			GL_FUNC(DisableVertexAttribArray, void, GLuint location);
			GL_FUNC(VertexAttribPointer, void, GLuint index, GLint size, GLenum type, GLboolean normalized, GLint stride, const void* pointer);
			GL_FUNC(VertexAttribDivisor, void, GLuint index, GLuint divisor);
			GL_FUNC(CreateShader, GLuint, GLenum type);
			GL_FUNC(AttachShader, void, GLuint program, GLuint shader);
			GL_FUNC(DetachShader, void, GLuint program, GLuint shader);
			GL_FUNC(DeleteShader, void, GLuint shader);
			GL_FUNC(ShaderSource, void, GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
			GL_FUNC(CompileShader, void, GLuint shader);
			GL_FUNC(GetShaderiv, void, GLuint shader, GLenum pname, GLint* result);
			GL_FUNC(GetShaderInfoLog, void, GLuint shader, GLint maxLength, GLsizei* length, GLchar* infoLog);
			GL_FUNC(CreateProgram, GLuint, );
			GL_FUNC(DeleteProgram, void, GLuint program);
			GL_FUNC(LinkProgram, void, GLuint program);
			GL_FUNC(GetProgramiv, void, GLuint program, GLenum pname, GLint* result);
			GL_FUNC(GetProgramInfoLog, void, GLuint program, GLint maxLength, GLsizei* length, GLchar* infoLog);
			GL_FUNC(GetActiveUniform, void, GLuint program, GLuint index, GLint bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
			GL_FUNC(GetActiveAttrib, void, GLuint program, GLuint index, GLint bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
			GL_FUNC(UseProgram, void, GLuint program);
			GL_FUNC(GetUniformLocation, GLint, GLuint program, const GLchar* name);
			GL_FUNC(GetAttribLocation, GLint, GLuint program, const GLchar* name);
			GL_FUNC(Uniform1f, void, GLint location, GLfloat v0);
			GL_FUNC(Uniform2f, void, GLint location, GLfloat v0, GLfloat v1);
			GL_FUNC(Uniform3f, void, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
			GL_FUNC(Uniform4f, void, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
			GL_FUNC(Uniform1fv, void, GLint location, GLint count, const GLfloat* value);
			GL_FUNC(Uniform2fv, void, GLint location, GLint count, const GLfloat* value);
			GL_FUNC(Uniform3fv, void, GLint location, GLint count, const GLfloat* value);
			GL_FUNC(Uniform4fv, void, GLint location, GLint count, const GLfloat* value);
			GL_FUNC(Uniform1i, void, GLint location, GLint v0);
			GL_FUNC(Uniform2i, void, GLint location, GLint v0, GLint v1);
			GL_FUNC(Uniform3i, void, GLint location, GLint v0, GLint v1, GLint v2);
			GL_FUNC(Uniform4i, void, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
			GL_FUNC(Uniform1iv, void, GLint location, GLint count, const GLint* value);
			GL_FUNC(Uniform2iv, void, GLint location, GLint count, const GLint* value);
			GL_FUNC(Uniform3iv, void, GLint location, GLint count, const GLint* value);
			GL_FUNC(Uniform4iv, void, GLint location, GLint count, const GLint* value);
			GL_FUNC(Uniform1ui, void, GLint location, GLuint v0);
			GL_FUNC(Uniform2ui, void, GLint location, GLuint v0, GLuint v1);
			GL_FUNC(Uniform3ui, void, GLint location, GLuint v0, GLuint v1, GLuint v2);
			GL_FUNC(Uniform4ui, void, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
			GL_FUNC(Uniform1uiv, void, GLint location, GLint count, const GLint* value);
			GL_FUNC(Uniform2uiv, void, GLint location, GLint count, const GLint* value);
			GL_FUNC(Uniform3uiv, void, GLint location, GLint count, const GLint* value);
			GL_FUNC(Uniform4uiv, void, GLint location, GLint count, const GLint* value);
			GL_FUNC(UniformMatrix2fv, void, GLint location, GLint count, GLboolean transpose, const GLfloat* value);
			GL_FUNC(UniformMatrix3fv, void, GLint location, GLint count, GLboolean transpose, const GLfloat* value);
			GL_FUNC(UniformMatrix4fv, void, GLint location, GLint count, GLboolean transpose, const GLfloat* value);
			GL_FUNC(UniformMatrix2x3fv, void, GLint location, GLint count, GLboolean transpose, const GLfloat* value);
			GL_FUNC(UniformMatrix3x2fv, void, GLint location, GLint count, GLboolean transpose, const GLfloat* value);
			GL_FUNC(UniformMatrix2x4fv, void, GLint location, GLint count, GLboolean transpose, const GLfloat* value);
			GL_FUNC(UniformMatrix4x2fv, void, GLint location, GLint count, GLboolean transpose, const GLfloat* value);
			GL_FUNC(UniformMatrix3x4fv, void, GLint location, GLint count, GLboolean transpose, const GLfloat* value);
			GL_FUNC(UniformMatrix4x3fv, void, GLint location, GLint count, GLboolean transpose, const GLfloat* value);
			GL_FUNC(PixelStorei, void, GLenum pname, GLint param);
		};

		// static state
		// TODO:
		// this should probably be part of the Device implementation ...
		void* context;
		GL gl;
		GLint maxColorAttachments;
		GLint maxElementIndices;
		GLint maxElementVertices;
		GLint maxRenderBufferSize;
		GLint maxSamples;
		GLint maxTextureImageUnits;
		GLint maxTextureSize;

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
				Log::print("GL (%s) %s", typeName, message);
		}

		GLuint gl_mesh_assign_attributes(GLuint buffer, GLenum buffer_type, const VertexAttribute* vertex_attributes, int vertex_attribute_count, int stride, GLint divisor)
		{
			// bind
			gl.BindBuffer(buffer_type, buffer);

			// TODO: disable existing enabled attributes ..
			// ...
			// ...

			// enable attributes
			size_t ptr = 0;
			for (int n = 0; n < vertex_attribute_count; n++)
			{
				const VertexAttribute* attrib = (vertex_attributes + n);

				for (int i = 0, loc = 0; i < (int)attrib->components; i += 4, loc++)
				{
					int components = attrib->components - i;
					if (components > 4)
						components = 4;

					GLenum type = GL_UNSIGNED_BYTE;
					size_t component_size = 0;
					if (attrib->type == VertexAttributeType::Byte)
					{
						type = GL_UNSIGNED_BYTE;
						component_size = 1;
					}
					else if (attrib->type == VertexAttributeType::Short)
					{
						type = GL_SHORT;
						component_size = 2;
					}
					else if (attrib->type == VertexAttributeType::Int)
					{
						type = GL_INT;
						component_size = 4;
					}
					else if (attrib->type == VertexAttributeType::Float)
					{
						type = GL_FLOAT;
						component_size = 4;
					}

					uint32_t location = (uint32_t)(attrib->index + loc);
					gl.EnableVertexAttribArray(location);
					gl.VertexAttribPointer(location, components, type, attrib->normalized, stride, (void*)ptr);
					gl.VertexAttribDivisor(location, divisor);

					ptr += components * component_size;
				}
			}

			return stride;
		}

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
			TextureWrap m_wrap_x;
			TextureWrap m_wrap_y;
			TextureFilter m_filter;
			TextureFormat m_format;
			GLenum m_gl_internal_format;
			GLenum m_gl_format;
			GLenum m_gl_type;

		public:
			bool framebuffer_parent;

			OpenGL_Texture(int width, int height, TextureFilter filter, TextureWrap wrap_x, TextureWrap wrap_y, TextureFormat format)
			{
				m_id = 0;
				m_width = width;
				m_height = height;
				m_wrap_x = wrap_x;
				m_wrap_y = wrap_y;
				m_filter = filter;
				m_format = format;
				framebuffer_parent = false;
				m_gl_internal_format = GL_RED;
				m_gl_format = GL_RED;
				m_gl_type = GL_UNSIGNED_BYTE;

				if (width > maxTextureSize || height > maxTextureSize)
				{
					Log::error("Exceeded Max Texture Size of %i", maxTextureSize);
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
				else if (format == TextureFormat::RGB)
				{
					m_gl_internal_format = GL_RGB;
					m_gl_format = GL_RGB;
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

				gl.GenTextures(1, &m_id);
				gl.ActiveTexture(GL_TEXTURE0);
				gl.BindTexture(GL_TEXTURE_2D, m_id);

				gl.TexImage2D(GL_TEXTURE_2D, 0, m_gl_internal_format, width, height, 0, m_gl_format, m_gl_type, nullptr);
				gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (m_filter == TextureFilter::Nearest ? GL_NEAREST : GL_LINEAR));
				gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (m_filter == TextureFilter::Nearest ? GL_NEAREST : GL_LINEAR));
				gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (m_wrap_x == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT));
				gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (m_wrap_y == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT));
			}

			~OpenGL_Texture()
			{
				if (m_id > 0)
					gl.DeleteTextures(1, &m_id);
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

			virtual void set_filter(TextureFilter filter) override
			{
				m_filter = filter;

				gl.BindTexture(GL_TEXTURE_2D, m_id);
				gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (filter == TextureFilter::Nearest ? GL_NEAREST : GL_LINEAR));
				gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (filter == TextureFilter::Nearest ? GL_NEAREST : GL_LINEAR));
			}

			virtual TextureFilter get_filter() const override
			{
				return m_filter;
			}

			virtual void set_wrap(TextureWrap x, TextureWrap y) override
			{
				m_wrap_x = x;
				m_wrap_y = y;

				gl.BindTexture(GL_TEXTURE_2D, m_id);
				gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (x == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT));
				gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (y == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT));
			}

			virtual TextureWrap get_wrap_x() const override
			{
				return m_wrap_x;
			}

			virtual TextureWrap get_wrap_y() const override
			{
				return m_wrap_y;
			}

			virtual void set_data(unsigned char* data) override
			{
				gl.ActiveTexture(GL_TEXTURE0);
				gl.BindTexture(GL_TEXTURE_2D, m_id);
				gl.TexImage2D(GL_TEXTURE_2D, 0, m_gl_internal_format, m_width, m_height, 0, m_gl_format, m_gl_type, data);
			}

			virtual void get_data(unsigned char* data) override
			{
				gl.ActiveTexture(GL_TEXTURE0);
				gl.BindTexture(GL_TEXTURE_2D, m_id);
				gl.GetTexImage(GL_TEXTURE_2D, 0, m_gl_internal_format, m_gl_type, data);
			}

			virtual bool is_framebuffer() const override
			{
				return framebuffer_parent;
			}

			virtual bool is_valid() const override
			{
				return m_id > 0;
			}

			virtual void dispose() override
			{
				if (m_id > 0)
				{
					gl.DeleteTextures(1, &m_id);
					m_id = 0;
				}
			}

		};

		class OpenGL_FrameBuffer : public FrameBuffer
		{
		private:
			GLuint m_id;
			int m_width;
			int m_height;
			StackList<TextureRef, BLAH_ATTACHMENTS> m_attachments;

		public:

			OpenGL_FrameBuffer(int width, int height, const TextureFormat* attachments, int attachmentCount)
			{
				gl.GenFramebuffers(1, &m_id);
				m_width = width;
				m_height = height;

				gl.BindFramebuffer(GL_FRAMEBUFFER, m_id);

				for (int i = 0; i < attachmentCount; i++)
				{
					auto tex = Graphics::create_texture(width, height, attachments[i]);
					auto gltex = ((OpenGL_Texture*)tex.get());

					gltex->framebuffer_parent = true;
					m_attachments.add(tex);

					if (attachments[i] != TextureFormat::DepthStencil)
					{
						gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, gltex->gl_id(), 0);
					}
					else
					{
						gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, gltex->gl_id(), 0);
					}
				}
			}

			~OpenGL_FrameBuffer()
			{
				if (m_id > 0)
				{
					gl.DeleteFramebuffers(1, &m_id);
					m_id = 0;
				}
			}

			GLuint gl_id() const
			{
				return m_id;
			}

			virtual Attachments& attachments() override
			{
				return m_attachments;
			}

			virtual const Attachments& attachments() const override
			{
				return m_attachments;
			}

			virtual int width() const override
			{
				return m_width;
			}

			virtual int height() const override
			{
				return m_height;
			}

			virtual bool is_valid() const override
			{
				return m_id > 0;
			}

			virtual void dispose() override
			{
				if (m_id > 0)
				{
					m_attachments.clear();
					gl.DeleteFramebuffers(1, &m_id);
					m_id = 0;
				}
			}
		};

		class OpenGL_Shader : public Shader
		{
		private:
			GLuint m_id;
			Attributes m_attributes;
			Uniforms m_uniforms;

		public:
			GLint uniforms_loc[BLAH_UNIFORMS] = { 0 };

			OpenGL_Shader(const ShaderData* data)
			{
				m_id = 0;

				// vertex shader
				if (data->vertex == nullptr)
				{
					Log::error("Vertex Shader is required");
					return;
				}

				if (data->fragment == nullptr)
				{
					Log::error("Fragment Shader is required");
					return;
				}

				GLchar log[1024];
				GLsizei log_length = 0;

				GLuint vertex_shader = gl.CreateShader(GL_VERTEX_SHADER);
				{
					const GLchar* source = (const GLchar*)data->vertex;
					gl.ShaderSource(vertex_shader, 1, &source, nullptr);
					gl.CompileShader(vertex_shader);
					gl.GetShaderInfoLog(vertex_shader, 1024, &log_length, log);

					if (log_length > 0)
					{
						gl.DeleteShader(vertex_shader);
						Log::error(log);
						return;
					}
				}

				GLuint fragment_shader = gl.CreateShader(GL_FRAGMENT_SHADER);
				{
					const GLchar* source = (const GLchar*)data->fragment;
					gl.ShaderSource(fragment_shader, 1, &source, nullptr);
					gl.CompileShader(fragment_shader);
					gl.GetShaderInfoLog(fragment_shader, 1024, &log_length, log);

					if (log_length > 0)
					{
						gl.DeleteShader(fragment_shader);
						Log::error(log);
						return;
					}
				}

				GLuint id = gl.CreateProgram();
				gl.AttachShader(id, vertex_shader);
				gl.AttachShader(id, fragment_shader);
				gl.LinkProgram(id);
				gl.GetProgramInfoLog(id, 1024, &log_length, log);

				if (log_length > 0)
				{
					gl.DetachShader(id, vertex_shader);
					gl.DetachShader(id, fragment_shader);
					gl.DeleteShader(vertex_shader);
					gl.DeleteShader(fragment_shader);

					Log::error(log);
					return;
				}

				// ready to go
				m_id = id;

				// get attributes
				{
					GLint active_attributes = 0;
					gl.GetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &active_attributes);
					if (active_attributes > BLAH_ATTRIBUTES)
					{
						Log::warn("Exceeding maximum shader attributes (%i / %i)", active_attributes, BLAH_ATTRIBUTES);
						active_attributes = BLAH_ATTRIBUTES;
					}

					for (int i = 0; i < active_attributes; i++)
					{
						GLsizei length;
						GLsizei size;
						GLenum type;
						GLchar name[BLAH_ATTRIBUTE_NAME];

						gl.GetActiveAttrib(id, i, BLAH_ATTRIBUTE_NAME - 1, &length, &size, &type, name);
						name[length] = '\0';

						auto attr = m_attributes.expand();
						attr->name.append(name);
						// TOOD: set these?
						attr->semantic_name = "";
						attr->semantic_location = 0;
					}
				}

				// get uniforms
				{
					GLint active_uniforms = 0;
					gl.GetProgramiv(id, GL_ACTIVE_UNIFORMS, &active_uniforms);
					if (active_uniforms > BLAH_UNIFORMS)
					{
						Log::warn("Exceeding maximum shader uniforms (%i / %i)", active_uniforms, BLAH_ATTRIBUTES);
						active_uniforms = BLAH_UNIFORMS;
					}

					for (int i = 0; i < active_uniforms; i++)
					{
						GLsizei length;
						GLsizei size;
						GLenum type;
						GLchar name[BLAH_UNIFORM_NAME];

						gl.GetActiveUniform(id, i, BLAH_UNIFORM_NAME - 1, &length, &size, &type, name);
						name[length] = '\0';

						// array names end with "[0]", and we don't want that
						for (int n = 0; n < BLAH_UNIFORM_NAME; n++)
							if (name[n] == '[')
							{
								if (name[n + 1] == '0' && name[n + 2] == ']')
								{
									name[n] = '\0';
									break;
								}
							}

						auto uniform = m_uniforms.expand();
						uniform->name = name;
						uniform->type = UniformType::None;
						uniform->array_length = size;
						uniforms_loc[i] = gl.GetUniformLocation(id, name);

						if (type == GL_FLOAT)
							uniform->type = UniformType::Float;
						else if (type == GL_FLOAT_VEC2)
							uniform->type = UniformType::Float2;
						else if (type == GL_FLOAT_VEC3)
							uniform->type = UniformType::Float3;
						else if (type == GL_FLOAT_VEC4)
							uniform->type = UniformType::Float4;
						else if (type == GL_FLOAT_MAT3x2)
							uniform->type = UniformType::Mat3x2;
						else if (type == GL_FLOAT_MAT4)
							uniform->type = UniformType::Mat4x4;
						else if (type == GL_SAMPLER_2D)
							uniform->type = UniformType::Texture;
						else
						{
							Log::error("Unsupported Uniform Type. Must be either FLOAT, MAT3x2, MAT4, or SAMPLER_2D");
							break;
						}
					}
				}
			}

			~OpenGL_Shader()
			{
				gl.DeleteProgram(m_id);
				m_id = 0;
			}

			GLuint gl_id() const
			{
				return m_id;
			}

			virtual Uniforms& uniforms() override
			{
				return m_uniforms;
			}

			virtual const Uniforms& uniforms() const override
			{
				return m_uniforms;
			}

			virtual Attributes& attributes() override
			{
				return m_attributes;
			}

			virtual const Attributes& attributes() const override
			{
				return m_attributes;
			}

			virtual bool is_valid() const override
			{
				return m_id > 0;
			}

			virtual void dispose() override
			{
				gl.DeleteProgram(m_id);
				m_id = 0;
			}
		};

		class OpenGL_Mesh : public Mesh
		{
		private:
			GLuint m_id;
			GLuint m_index_buffer;
			GLuint m_vertex_buffer;
			GLuint m_instance_buffer;
			int64_t m_index_count;
			int64_t m_vertex_count;
			int64_t m_instance_count;
			uint16_t m_vertex_size;
			uint16_t m_instance_size;
			uint8_t m_vertex_attribs_enabled;
			uint8_t m_instance_attribs_enabled;
			GLuint m_vertex_attribs[BLAH_ATTRIBUTES];
			GLuint m_instance_attribs[BLAH_ATTRIBUTES];

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

				gl.GenVertexArrays(1, &m_id);
			}

			~OpenGL_Mesh()
			{
				if (m_vertex_buffer != 0)
					gl.DeleteBuffers(1, &m_vertex_buffer);
				if (m_index_buffer != 0)
					gl.DeleteBuffers(1, &m_index_buffer);
				if (m_instance_buffer != 0)
					gl.DeleteBuffers(1, &m_instance_buffer);
				if (m_id != 0)
					gl.DeleteVertexArrays(1, &m_id);
				m_id = 0;
			}

			GLuint gl_id() const
			{
				return m_id;
			}

			virtual void vertex_format_internal(const VertexAttribute* attributes, int attribute_count, int stride = -1) override
			{
				gl.BindVertexArray(m_id);
				{
					if (m_vertex_buffer == 0)
						gl.GenBuffers(1, &(m_vertex_buffer));

					m_vertex_size = gl_mesh_assign_attributes(m_vertex_buffer, GL_ARRAY_BUFFER, attributes, attribute_count, stride, 0);
				}
				gl.BindVertexArray(0);
			}

			virtual void instance_format_internal(const VertexAttribute* attributes, int attribute_count, int stride = -1) override
			{
				gl.BindVertexArray(m_id);
				{
					if (m_instance_buffer == 0)
						gl.GenBuffers(1, &(m_instance_buffer));

					m_instance_size = gl_mesh_assign_attributes(m_instance_buffer, GL_ARRAY_BUFFER, attributes, attribute_count, stride, 1);
				}
				gl.BindVertexArray(0);
			}

			virtual void index_data(const void* indices, int64_t count) override
			{
				m_index_count = count;

				gl.BindVertexArray(m_id);
				{
					if (m_index_buffer == 0)
						gl.GenBuffers(1, &(m_index_buffer));

					gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
					gl.BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * count, indices, GL_DYNAMIC_DRAW);
				}
				gl.BindVertexArray(0);
			}

			virtual void vertex_data(const void* vertices, int64_t count) override
			{
				if (m_vertex_buffer == 0 || m_vertex_size <= 0)
				{
					Log::error("You must assign a Vertex Format before setting Vertex Data");
				}
				else
				{
					m_vertex_count = count;

					gl.BindVertexArray(m_id);
					{
						gl.BindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
						gl.BufferData(GL_ARRAY_BUFFER, m_vertex_size * count, vertices, GL_DYNAMIC_DRAW);
					}
					gl.BindVertexArray(0);
				}
			}

			virtual void instance_data(const void* instances, int64_t count) override
			{
				if (m_instance_buffer == 0 || m_instance_size <= 0)
				{
					Log::error("You must assign an Instance Format before setting Instance Data");
				}
				else
				{
					m_instance_count = count;

					gl.BindVertexArray(m_id);
					{
						gl.BindBuffer(GL_ARRAY_BUFFER, m_instance_buffer);
						gl.BufferData(GL_ARRAY_BUFFER, m_instance_size * count, instances, GL_DYNAMIC_DRAW);
					}
					gl.BindVertexArray(0);
				}
			}

			virtual int64_t index_count() const override
			{
				return m_index_count;
			}

			virtual int64_t vertex_count() const override
			{
				return m_vertex_count;
			}

			virtual int64_t instance_count() const override
			{
				return m_instance_count;
			}

			virtual bool is_valid() const override
			{
				return m_id > 0;
			}

			virtual void dispose() override
			{
				if (m_vertex_buffer != 0)
					gl.DeleteBuffers(1, &m_vertex_buffer);
				if (m_index_buffer != 0)
					gl.DeleteBuffers(1, &m_index_buffer);
				if (m_instance_buffer != 0)
					gl.DeleteBuffers(1, &m_instance_buffer);
				if (m_id != 0)
					gl.DeleteVertexArrays(1, &m_id);
				m_id = 0;
			}
		};

		class Device : public Internal::GraphicsDevice
		{
			virtual void startup() override
			{
				valid = true;

				// create gl context
				context = Internal::Platform::gl_context_create();
				if (context == nullptr)
				{
					Log::error("Failed to create OpenGL Context");
					valid = false;
					return;
				}
				Internal::Platform::gl_context_make_current(context);

				// bind opengl functions
				GL_BIND(DebugMessageCallback);
				GL_BIND(GetString);
				GL_BIND(Flush);
				GL_BIND(Enable);
				GL_BIND(Disable);
				GL_BIND(Clear);
				GL_BIND(ClearColor);
				GL_BIND(ClearDepth);
				GL_BIND(ClearStencil);
				GL_BIND(DepthMask);
				GL_BIND(DepthFunc);
				GL_BIND(Viewport);
				GL_BIND(Scissor);
				GL_BIND(CullFace);
				GL_BIND(BlendEquation);
				GL_BIND(BlendEquationSeparate);
				GL_BIND(BlendFunc);
				GL_BIND(BlendFuncSeparate);
				GL_BIND(BlendColor);
				GL_BIND(ColorMask);
				GL_BIND(GetIntegerv);
				GL_BIND(GenTextures);
				GL_BIND(GenRenderbuffers);
				GL_BIND(GenFramebuffers);
				GL_BIND(ActiveTexture);
				GL_BIND(BindTexture);
				GL_BIND(BindRenderbuffer);
				GL_BIND(BindFramebuffer);
				GL_BIND(TexImage2D);
				GL_BIND(FramebufferRenderbuffer);
				GL_BIND(FramebufferTexture2D);
				GL_BIND(TexParameteri);
				GL_BIND(RenderbufferStorage);
				GL_BIND(GetTexImage);
				GL_BIND(DrawElements);
				GL_BIND(DrawElementsInstanced);
				GL_BIND(DeleteTextures);
				GL_BIND(DeleteRenderbuffers);
				GL_BIND(DeleteFramebuffers);
				GL_BIND(GenVertexArrays);
				GL_BIND(BindVertexArray);
				GL_BIND(GenBuffers);
				GL_BIND(BindBuffer);
				GL_BIND(BufferData);
				GL_BIND(BufferSubData);
				GL_BIND(DeleteBuffers);
				GL_BIND(DeleteVertexArrays);
				GL_BIND(EnableVertexAttribArray);
				GL_BIND(DisableVertexAttribArray);
				GL_BIND(VertexAttribPointer);
				GL_BIND(VertexAttribDivisor);
				GL_BIND(CreateShader);
				GL_BIND(AttachShader);
				GL_BIND(DetachShader);
				GL_BIND(DeleteShader);
				GL_BIND(ShaderSource);
				GL_BIND(CompileShader);
				GL_BIND(GetShaderiv);
				GL_BIND(GetShaderInfoLog);
				GL_BIND(CreateProgram);
				GL_BIND(DeleteProgram);
				GL_BIND(LinkProgram);
				GL_BIND(GetProgramiv);
				GL_BIND(GetProgramInfoLog);
				GL_BIND(GetActiveUniform);
				GL_BIND(GetActiveAttrib);
				GL_BIND(UseProgram);
				GL_BIND(GetUniformLocation);
				GL_BIND(GetAttribLocation);
				GL_BIND(Uniform1f);
				GL_BIND(Uniform2f);
				GL_BIND(Uniform3f);
				GL_BIND(Uniform4f);
				GL_BIND(Uniform1fv);
				GL_BIND(Uniform2fv);
				GL_BIND(Uniform3fv);
				GL_BIND(Uniform4fv);
				GL_BIND(Uniform1i);
				GL_BIND(Uniform2i);
				GL_BIND(Uniform3i);
				GL_BIND(Uniform4i);
				GL_BIND(Uniform1iv);
				GL_BIND(Uniform2iv);
				GL_BIND(Uniform3iv);
				GL_BIND(Uniform4iv);
				GL_BIND(Uniform1ui);
				GL_BIND(Uniform2ui);
				GL_BIND(Uniform3ui);
				GL_BIND(Uniform4ui);
				GL_BIND(Uniform1uiv);
				GL_BIND(Uniform2uiv);
				GL_BIND(Uniform3uiv);
				GL_BIND(Uniform4uiv);
				GL_BIND(UniformMatrix2fv);
				GL_BIND(UniformMatrix3fv);
				GL_BIND(UniformMatrix4fv);
				GL_BIND(UniformMatrix2x3fv);
				GL_BIND(UniformMatrix3x2fv);
				GL_BIND(UniformMatrix2x4fv);
				GL_BIND(UniformMatrix4x2fv);
				GL_BIND(UniformMatrix3x4fv);
				GL_BIND(UniformMatrix4x3fv);
				GL_BIND(PixelStorei);

				// bind debug message callback
				if (gl.DebugMessageCallback != nullptr)
				{
					gl.Enable(GL_DEBUG_OUTPUT);
					gl.Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
					gl.DebugMessageCallback(gl_message_callback, nullptr);
				}

				// get opengl info
				gl.GetIntegerv(0x8CDF, &maxColorAttachments);
				gl.GetIntegerv(0x80E9, &maxElementIndices);
				gl.GetIntegerv(0x80E8, &maxElementVertices);
				gl.GetIntegerv(0x84E8, &maxRenderBufferSize);
				gl.GetIntegerv(0x8D57, &maxSamples);
				gl.GetIntegerv(0x8872, &maxTextureImageUnits);
				gl.GetIntegerv(0x0D33, &maxTextureSize);

				// log
				Log::print("OpenGL %s, %s",
					gl.GetString(GL_VERSION),
					gl.GetString(GL_RENDERER));

				// don't include row padding
				gl.PixelStorei(GL_PACK_ALIGNMENT, 1);
				gl.PixelStorei(GL_UNPACK_ALIGNMENT, 1);

				// assign info
				info.api = GfxAPI::OpenGL;
				info.instancing = true;
				info.origin_bottom_left = true;
				info.max_texture_size = maxTextureSize;
			}

			virtual void shutdown() override
			{
				Internal::Platform::gl_context_destroy(context);
				context = nullptr;
			}

			virtual void update() override {}
			virtual void before_render() override {}
			virtual void after_render() override {}

			virtual TextureRef create_texture(int width, int height, TextureFilter filter, TextureWrap wrap_x, TextureWrap wrap_y, TextureFormat format) override
			{
				return TextureRef(new OpenGL_Texture(width, height, filter, wrap_x, wrap_y, format));
			}

			virtual FrameBufferRef create_framebuffer(int width, int height, const TextureFormat* attachments, int attachmentCount) override
			{
				return FrameBufferRef(new OpenGL_FrameBuffer(width, height, attachments, attachmentCount));
			}

			virtual ShaderRef create_shader(const ShaderData* data) override
			{
				return ShaderRef(new OpenGL_Shader(data));
			}

			virtual MeshRef create_mesh() override
			{
				return MeshRef(new OpenGL_Mesh());
			}

			virtual void render(RenderCall* call) override
			{
				// Bind the Target
				Point size;
				if (!call->target || !call->target->is_valid())
				{
					gl.BindFramebuffer(GL_FRAMEBUFFER, 0);
					size.x = App::draw_width();
					size.y = App::draw_height();
				}
				else
				{
					gl.BindFramebuffer(GL_FRAMEBUFFER, ((OpenGL_FrameBuffer*)call->target.get())->gl_id());
					size.x = call->target->width();
					size.y = call->target->height();
				}

				// Use the Shader
				// TODO: I don't love how material values are assigned or set here
				{
					OpenGL_Shader* shader = (OpenGL_Shader*)(call->material->shader().get());
					gl.UseProgram(shader->gl_id());

					// upload uniform values
					int texture_slot = 0;
					GLint texture_ids[64];

					auto& uniforms = shader->uniforms();
					for (int i = 0; i < uniforms.count(); i++)
					{
						auto location = shader->uniforms_loc[i];
						auto& uniform = uniforms[i];

						// Sampler 2D
						if (uniform.type == UniformType::Texture)
						{
							for (int n = 0; n < uniform.array_length; n++)
							{
								auto tex = call->material->get_texture(i, n);

								gl.ActiveTexture(GL_TEXTURE0 + texture_slot);

								if (!tex)
								{
									gl.BindTexture(GL_TEXTURE_2D, 0);
								}
								else
								{
									gl.BindTexture(GL_TEXTURE_2D, ((OpenGL_Texture*)tex.get())->gl_id());
								}

								texture_ids[n] = texture_slot;
								texture_slot++;
							}

							gl.Uniform1iv(location, (GLint)uniform.array_length, &texture_ids[0]);
						}
						// Float
						else if (uniform.type == UniformType::Float)
						{
							gl.Uniform1fv(location, (GLint)uniform.array_length, (const GLfloat*)call->material->get_value(i));
						}
						// Float2
						else if (uniform.type == UniformType::Float2)
						{
							gl.Uniform2fv(location, (GLint)uniform.array_length, (const GLfloat*)call->material->get_value(i));
						}
						// Float3
						else if (uniform.type == UniformType::Float3)
						{
							gl.Uniform3fv(location, (GLint)uniform.array_length, (const GLfloat*)call->material->get_value(i));
						}
						// Float4
						else if (uniform.type == UniformType::Float4)
						{
							gl.Uniform4fv(location, (GLint)uniform.array_length, (const GLfloat*)call->material->get_value(i));
						}
						// Matrix3x2
						else if (uniform.type == UniformType::Mat3x2)
						{
							gl.UniformMatrix3x2fv(location, (GLint)uniform.array_length, 0, (const GLfloat*)call->material->get_value(i));
						}
						// Matrix4x4
						else if (uniform.type == UniformType::Mat4x4)
						{
							gl.UniformMatrix4fv(location, (GLint)uniform.array_length, 0, (const GLfloat*)call->material->get_value(i));
						}
					}
				}

				// Blend Mode
				{
					GLenum colorOp = gl_get_blend_func(call->blend.colorOp);
					GLenum alphaOp = gl_get_blend_func(call->blend.alphaOp);
					GLenum colorSrc = gl_get_blend_factor(call->blend.colorSrc);
					GLenum colorDst = gl_get_blend_factor(call->blend.colorDst);
					GLenum alphaSrc = gl_get_blend_factor(call->blend.alphaSrc);
					GLenum alphaDst = gl_get_blend_factor(call->blend.alphaDst);

					gl.Enable(GL_BLEND);
					gl.BlendEquationSeparate(colorOp, alphaOp);
					gl.BlendFuncSeparate(colorSrc, colorDst, alphaSrc, alphaDst);

					gl.ColorMask(
						((int)call->blend.mask & (int)BlendMask::Red),
						((int)call->blend.mask & (int)BlendMask::Green),
						((int)call->blend.mask & (int)BlendMask::Blue),
						((int)call->blend.mask & (int)BlendMask::Alpha));


					unsigned char r = call->blend.rgba >> 24;
					unsigned char g = call->blend.rgba >> 16;
					unsigned char b = call->blend.rgba >> 8;
					unsigned char a = call->blend.rgba;

					gl.BlendColor(
						r / 255.0f,
						g / 255.0f,
						b / 255.0f,
						a / 255.0f);
				}

				// Depth Function
				{
					if (call->depth == Compare::None)
					{
						gl.Disable(GL_DEPTH_TEST);
					}
					else
					{
						gl.Enable(GL_DEPTH_TEST);

						switch (call->depth)
						{
						case Compare::None: break;
						case Compare::Always:
							gl.DepthFunc(GL_ALWAYS);
							break;
						case Compare::Equal:
							gl.DepthFunc(GL_EQUAL);
							break;
						case Compare::Greater:
							gl.DepthFunc(GL_GREATER);
							break;
						case Compare::GreatorOrEqual:
							gl.DepthFunc(GL_GEQUAL);
							break;
						case Compare::Less:
							gl.DepthFunc(GL_LESS);
							break;
						case Compare::LessOrEqual:
							gl.DepthFunc(GL_LEQUAL);
							break;
						case Compare::Never:
							gl.DepthFunc(GL_NEVER);
							break;
						case Compare::NotEqual:
							gl.DepthFunc(GL_NOTEQUAL);
							break;
						}
					}
				}

				// Cull Mode
				{
					if (call->cull == Cull::None)
					{
						gl.Disable(GL_CULL_FACE);
					}
					else
					{
						gl.Enable(GL_CULL_FACE);

						if (call->cull == Cull::Back)
							gl.CullFace(GL_BACK);
						else if (call->cull == Cull::Front)
							gl.CullFace(GL_FRONT);
						else
							gl.CullFace(GL_FRONT_AND_BACK);
					}
				}

				// Viewport
				{
					Rect viewport = call->viewport;
					viewport.y = size.y - viewport.y - viewport.h;

					gl.Viewport((GLint)viewport.x, (GLint)viewport.y, (GLint)viewport.w, (GLint)viewport.h);
				}

				// Scissor
				{
					if (!call->has_scissor)
					{
						gl.Disable(GL_SCISSOR_TEST);
					}
					else
					{
						Rect scissor = call->scissor;
						scissor.y = size.y - scissor.y - scissor.h;

						if (scissor.w < 0)
							scissor.w = 0;
						if (scissor.h < 0)
							scissor.h = 0;

						gl.Enable(GL_SCISSOR_TEST);
						gl.Scissor((GLint)scissor.x, (GLint)scissor.y, (GLint)scissor.w, (GLint)scissor.h);
					}
				}

				// Draw the Mesh
				{
					gl.BindVertexArray(((OpenGL_Mesh*)call->mesh.get())->gl_id());

					if (call->instance_count > 0)
					{
						gl.DrawElementsInstanced(
							GL_TRIANGLES,
							(GLint)(call->index_count),
							GL_UNSIGNED_INT,
							(void*)(sizeof(int) * call->index_start),
							(GLint)call->instance_count);
					}
					else
					{
						gl.DrawElements(
							GL_TRIANGLES,
							(GLint)(call->index_count),
							GL_UNSIGNED_INT,
							(void*)(sizeof(int) * call->index_start));
					}

					gl.BindVertexArray(0);
				}
			}

			virtual void clear(const FrameBufferRef& target, uint32_t rgba) override
			{
				if (!target || !target->is_valid())
				{
					gl.BindFramebuffer(GL_FRAMEBUFFER, 0);
				}
				else
				{
					auto framebuffer = (OpenGL_FrameBuffer*)target.get();
					if (framebuffer != nullptr)
						gl.BindFramebuffer(GL_FRAMEBUFFER, framebuffer->gl_id());
				}

				unsigned char r = rgba >> 24;
				unsigned char g = rgba >> 16;
				unsigned char b = rgba >> 8;
				unsigned char a = rgba;

				gl.Disable(GL_SCISSOR_TEST);
				gl.ClearColor(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
				gl.Clear(GL_COLOR_BUFFER_BIT);
			}
		};

		bool supported()
		{
			return true;
		}

		Internal::GraphicsDevice* create()
		{
			return new OpenGL::Device();
		}

		void destroy(Internal::GraphicsDevice* device)
		{
			delete device;
		}
	}

	Internal::GraphicsDeviceInfo Internal::OpenGL_DeviceInfo = {
		GfxAPI::OpenGL,
		OpenGL::supported,
		OpenGL::create,
		OpenGL::destroy
	};
}

#else // BLAH_USE_OPENGL

namespace Blah
{
	namespace OpenGL
	{
		bool supported() { return false; }
		Internal::GraphicsDevice* create() { return nullptr; }
		void destroy(Internal::GraphicsDevice* device) { }
	}

	Internal::GraphicsDeviceInfo Internal::OpenGL_DeviceInfo = {
		GfxAPI::OpenGL,
		OpenGL::supported,
		OpenGL::create,
		OpenGL::destroy
	};
}

#endif // BLAH_USE_OPENGL
