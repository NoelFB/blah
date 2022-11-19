#pragma once
#include <blah_vector.h>
#include <blah_string.h>
#include <blah_spritefont.h>
#include <blah_subtexture.h>
#include <blah_spatial.h>
#include <blah_color.h>
#include <blah_graphics.h>

namespace Blah
{
	// A 2D sprite batcher, used for drawing shapes and textures
	class Batch
	{
	public:

		// Spritebatcher Color Mode
		enum class ColorMode
		{
			// Draws textures and shapes normally
			Normal,

			// Ignores the texture color but still uses transparency, essentially
			// drawing the "shape" of the texture as a solid color
			Wash
		};

		// The name of the default uniforms to set
		String texture_uniform = "u_texture";
		String sampler_uniform = "u_texture_sampler";
		String matrix_uniform = "u_matrix";

		// Snaps all drawing coordinates to integer values
		// This is useful for drawing Pixel Art stuff
		bool integerize = false;

		// Default Sampler, set on clear
		TextureSampler default_sampler;

		// Pushes a new matrix onto the stack, and uses it for transforming all drawing.
		// `absolute` means the matrix provided will not be transformed by the current stack.
		void push_matrix(const Mat3x2f& matrix, bool absolute = false);

		// Pops the matrix from the stack
		Mat3x2f pop_matrix();

		// Gets the current matrix from the top of the stackKO
		Mat3x2f peek_matrix() const;

		// Pushes a Scissor rectangle. Note this is not transformed by the matrix stack
		// or other scissors. Each push is screen-space.
		void push_scissor(const Rectf& scissor);

		// Pops a Scissor rectangle from the stack
		Rectf pop_scissor();

		// Gets the current Scissor rectangle from the top of the stack
		Rectf peek_scissor() const;

		// Pushes a blend mode
		void push_blend(const BlendMode& blend);

		// Pops a blend mode
		BlendMode pop_blend();

		// Gets the current BlendMode from the top of the stack
		BlendMode peek_blend() const;

		// Pushes a Material to use for all drawing. Note that the state of the Material
		// is not copied - it will be drawn with the values of the Material at render.
		void push_material(const MaterialRef& material);

		// Pops a Material
		MaterialRef pop_material();

		// Gets the current Material from the top of the stack
		MaterialRef peek_material() const;

		// Pushes a render layer. Lower values are rendered first. This is not super optimized
		// and should generally be avoided.
		void push_layer(int layer);

		// Pops a Layer
		int pop_layer();

		// Gets the current Layer from the top of the stack
		int peek_layer() const;

		// Pushes a Color Mode for drawing Textures
		void push_color_mode(ColorMode mode);

		// Pops a Color MOde
		ColorMode pop_color_mode();

		// Gets the current ColorMode from the top of the stack
		ColorMode peek_color_mode() const;

		// Sets the current texture used for drawing. Note that certain functions will override
		// this (ex the `str` and `tex` methods)
		void set_texture(const TextureRef& texture);

		// Sets the current texture sampler for drawing.
		void set_sampler(const TextureSampler& sampler);

		// Draws the batch to the given target
		void render(const TargetRef& target = TargetRef());

		// Draws the batch to the given target, with the provided matrix
		void render(const TargetRef& target, const Mat4x4f& matrix);

		// Clears the batch
		void clear();

		// Clears and disposes all resources that the batch is using
		void dispose();

		void line(const Vec2f& from, const Vec2f& to, float t, Color color);
		void line(const Vec2f& from, const Vec2f& to, float t, Color fromColor, Color toColor);

		void bezier_line(const Vec2f& from, const Vec2f& b, const Vec2f& to, int steps, float t, Color color);
		void bezier_line(const Vec2f& from, const Vec2f& b, const Vec2f& c, const Vec2f& to, int steps, float t, Color color);

		void tri(const Vec2f& pos0, const Vec2f& pos1, const Vec2f& pos2, Color color);
		void tri(const Vec2f& pos0, const Vec2f& pos1, const Vec2f& pos2, Color col0, Color col1, Color col2);
		void tri(const Vec2f& pos0, const Vec2f& pos1, const Vec2f& pos2, const Vec2f& tex0, const Vec2f& tex1, const Vec2f& tex2, Color color);
		void tri(const Vec2f& pos0, const Vec2f& pos1, const Vec2f& pos2, const Vec2f& tex0, const Vec2f& tex1, const Vec2f& tex2, Color col0, Color col1, Color col2);

		void tri_line(const Vec2f& a, const Vec2f& b, const Vec2f& c, float t, Color color);

		void rect(const Rectf& rect, Color color);
		void rect_line(const Rectf& rect, float t, Color color);
		void rect_rounded(const Rectf& rect, float radius, int steps, Color color);
		void rect_rounded(const Rectf& rect, float rtl, int rtl_steps, float rtr, int rtr_steps, float rbr, int rbr_steps, float rbl, int rbl_steps, Color color);
		void rect_rounded_line(const Rectf& rect, float radius, int steps, float t, Color color);
		void rect_rounded_line(const Rectf& rect, float rtl, int rtl_steps, float rtr, int rtr_steps, float rbr, int rbr_steps, float rbl, int rbl_steps, float t, Color color);

		void semi_circle(const Vec2f& center, float start_radians, float end_radians, float radius, int steps, Color centerColor, Color edgeColor);
		void semi_circle(const Vec2f& center, float start_radians, float end_radians, float radius, int steps, Color color);
		void semi_circle_line(const Vec2f& center, float start_radians, float end_radians, float radius, int steps, float t, Color color);

		void circle(const Vec2f& center, float radius, int steps, Color color);
		void circle(const Vec2f& center, float radius, int steps, Color center_color, Color outer_color);
		void circle_line(const Vec2f& center, float radius, float t, int steps, Color color);

		void quad(const Vec2f& pos0, const Vec2f& pos1, const Vec2f& pos2, const Vec2f& pos3, Color color);
		void quad(const Vec2f& pos0, const Vec2f& pos1, const Vec2f& pos2, const Vec2f& pos3, Color col0, Color col1, Color col2, Color col3);
		void quad(const Vec2f& pos0, const Vec2f& pos1, const Vec2f& pos2, const Vec2f& pos3, const Vec2f& tex0, const Vec2f& tex1, const Vec2f& tex2, const Vec2f& tex3, Color color);
		void quad(const Vec2f& pos0, const Vec2f& pos1, const Vec2f& pos2, const Vec2f& pos3, const Vec2f& tex0, const Vec2f& tex1, const Vec2f& tex2, const Vec2f& tex3, Color col0, Color col1, Color col2, Color col3);
		void quad_line(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, float t, Color color);

		void arrow_head(const Vec2f& point_pos, float radians, float side_len, Color color);
		void arrow_head(const Vec2f& point_pos, const Vec2f& from_pos, float side_len, Color color);

		void tex(const TextureRef& texture, const Vec2f& position = Vec2f::zero, Color color = Color::white);
		void tex(const TextureRef& texture, const Vec2f& position, const Vec2f& origin, const Vec2f& scale, float rotation, Color color);
		void tex(const TextureRef& texture, const Rectf& clip, const Vec2f& position, const Vec2f& origin, const Vec2f& scale, float rotation, Color color);

		void tex(const Subtexture& subtexture, const Vec2f& position = Vec2f::zero, Color color = Color::white);
		void tex(const Subtexture& subtexture, const Vec2f& pos, const Vec2f& origin, const Vec2f& scale, float rotation, Color color);
		void tex(const Subtexture& subtexture, const Rectf& clip, const Vec2f& pos, const Vec2f& origin, const Vec2f& scale, float rotation, Color color);

		void str(const SpriteFont& font, const String& text, const Vec2f& pos, Color color);
		void str(const SpriteFont& font, const String& text, const Vec2f& pos, const Vec2f& justify, float size, Color color);

	private:

		struct Vertex
		{
			Vec2f pos;
			Vec2f tex;
			Color col;

			u8 mult;
			u8 wash;
			u8 fill;
			u8 pad;
		};

		struct DrawBatch
		{
			int layer;
			int offset;
			int elements;
			MaterialRef material;
			BlendMode blend;
			TextureRef texture;
			TextureSampler sampler;
			bool flip_vertically;
			Rectf scissor;

			DrawBatch() :
				layer(0),
				offset(0),
				elements(0),
				blend(BlendMode::Normal),
				flip_vertically(false),
				scissor(0, 0, -1, -1) {}
		};

		MaterialRef m_default_material;
		MeshRef m_mesh;
		Mat3x2f m_matrix = Mat3x2f::identity;
		ColorMode m_color_mode = ColorMode::Normal;
		u8 m_tex_mult = 255;
		u8 m_tex_wash = 0;
		DrawBatch m_batch;
		Vector<Vertex> m_vertices;
		Vector<u32> m_indices;
		Vector<Mat3x2f> m_matrix_stack;
		Vector<Rectf> m_scissor_stack;
		Vector<BlendMode> m_blend_stack;
		Vector<MaterialRef> m_material_stack;
		Vector<ColorMode> m_color_mode_stack;
		Vector<int> m_layer_stack;
		Vector<DrawBatch> m_batches;
		int m_batch_insert = 0;

		void render_single_batch(DrawCall& pass, const DrawBatch& b, const Mat4x4f& matrix);
	};
}