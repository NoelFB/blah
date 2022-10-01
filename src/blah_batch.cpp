#include <blah_batch.h>
#include <blah_calc.h>
#include <blah_app.h>
#include "internal/blah_internal.h"

using namespace Blah;

namespace
{
	const VertexFormat format = VertexFormat(
	{
		{ 0, VertexType::Float2, false },
		{ 1, VertexType::Float2, false },
		{ 2, VertexType::UByte4, true },
		{ 3, VertexType::UByte4, true },
	});

	Vec2f batch_shape_intersection(const Vec2f& p0, const Vec2f& p1, const Vec2f& q0, const Vec2f& q1)
	{
		const auto aa = p1 - p0;
		const auto bb = q0 - q1;
		const auto cc = q0 - p0;
		const auto t = (bb.x * cc.y - bb.y * cc.x) / (aa.y * bb.x - aa.x * bb.y);

		return Vec2f(p0.x + t * (p1.x - p0.x), p0.y + t * (p1.y - p0.y));
	}
}

#define MAKE_VERTEX(vert, mat, px, py, tx, ty, c, m, w, f, cast) \
	(vert)->pos.x = cast(((px) * (mat).m11) + ((py) * (mat).m21) + (mat).m31); \
	(vert)->pos.y = cast(((px) * (mat).m12) + ((py) * (mat).m22) + (mat).m32); \
	(vert)->tex.x = (tx); \
	(vert)->tex.y = m_batch.flip_vertically ? 1.0f - (ty) : (ty); \
	(vert)->col = c; \
	(vert)->mult = m; \
	(vert)->wash = w; \
	(vert)->fill = f;
	
#define PUSH_QUAD(px0, py0, px1, py1, px2, py2, px3, py3, tx0, ty0, tx1, ty1, tx2, ty2, tx3, ty3, col0, col1, col2, col3, mult, fill, wash) \
	{ \
		m_batch.elements += 2; \
		auto _i = m_indices.expand(6); \
		*_i++ = (u32)m_vertices.size() + 0; \
		*_i++ = (u32)m_vertices.size() + 1; \
		*_i++ = (u32)m_vertices.size() + 2; \
		*_i++ = (u32)m_vertices.size() + 0; \
		*_i++ = (u32)m_vertices.size() + 2; \
		*_i++ = (u32)m_vertices.size() + 3; \
		Vertex* _v = m_vertices.expand(4); \
		if (integerize) { \
			MAKE_VERTEX(_v, m_matrix, px0, py0, tx0, ty0, col0, mult, fill, wash, Calc::floor); _v++; \
			MAKE_VERTEX(_v, m_matrix, px1, py1, tx1, ty1, col1, mult, fill, wash, Calc::floor); _v++; \
			MAKE_VERTEX(_v, m_matrix, px2, py2, tx2, ty2, col2, mult, fill, wash, Calc::floor); _v++; \
			MAKE_VERTEX(_v, m_matrix, px3, py3, tx3, ty3, col3, mult, fill, wash, Calc::floor); \
		} else { \
			MAKE_VERTEX(_v, m_matrix, px0, py0, tx0, ty0, col0, mult, fill, wash, float); _v++; \
			MAKE_VERTEX(_v, m_matrix, px1, py1, tx1, ty1, col1, mult, fill, wash, float); _v++; \
			MAKE_VERTEX(_v, m_matrix, px2, py2, tx2, ty2, col2, mult, fill, wash, float); _v++; \
			MAKE_VERTEX(_v, m_matrix, px3, py3, tx3, ty3, col3, mult, fill, wash, float); \
		} \
	}

#define PUSH_TRIANGLE(px0, py0, px1, py1, px2, py2, tx0, ty0, tx1, ty1, tx2, ty2, col0, col1, col2, mult, fill, wash) \
	{ \
		m_batch.elements += 1; \
		auto* _i = m_indices.expand(3); \
		*_i++ = (u32)m_vertices.size() + 0; \
		*_i++ = (u32)m_vertices.size() + 1; \
		*_i++ = (u32)m_vertices.size() + 2; \
		Vertex* _v = m_vertices.expand(3); \
		if (integerize) { \
			MAKE_VERTEX(_v, m_matrix, px0, py0, tx0, ty0, col0, mult, fill, wash, Calc::floor); _v++; \
			MAKE_VERTEX(_v, m_matrix, px1, py1, tx1, ty1, col1, mult, fill, wash, Calc::floor); _v++; \
			MAKE_VERTEX(_v, m_matrix, px2, py2, tx2, ty2, col2, mult, fill, wash, Calc::floor); \
		} else { \
			MAKE_VERTEX(_v, m_matrix, px0, py0, tx0, ty0, col0, mult, fill, wash, float); _v++; \
			MAKE_VERTEX(_v, m_matrix, px1, py1, tx1, ty1, col1, mult, fill, wash, float); _v++; \
			MAKE_VERTEX(_v, m_matrix, px2, py2, tx2, ty2, col2, mult, fill, wash, float); \
		} \
	}

#define INSERT_BATCH() \
do { \
	m_batches.expand(); \
	for (auto i = m_batches.size() - 1; i > m_batch_insert; i --) \
		m_batches[i] = std::move(m_batches[i - 1]); \
	m_batches[m_batch_insert++] = m_batch; \
	m_batch.offset += m_batch.elements; \
	m_batch.elements = 0; \
} while (0)

// Compares a Batcher variable, and starts a new batch if it has changed
#define SET_BATCH_VAR(variable) \
	if (m_batch.elements > 0 && (variable) != m_batch.variable) \
		INSERT_BATCH(); \
	m_batch.variable = variable;

void Batch::push_matrix(const Mat3x2f& matrix, bool absolute)
{
	m_matrix_stack.push_back(m_matrix);
	if (absolute)
		m_matrix = matrix;
	else
		m_matrix = matrix * m_matrix;
}

Mat3x2f Batch::pop_matrix()
{
	auto was = m_matrix;
	m_matrix = m_matrix_stack.pop();
	return was;
}

Mat3x2f Batch::peek_matrix() const
{
	return m_matrix;
}

void Batch::push_scissor(const Rectf& scissor)
{
	m_scissor_stack.push_back(m_batch.scissor);
	SET_BATCH_VAR(scissor);
}

Rectf Batch::pop_scissor()
{
	Rectf was = m_batch.scissor;
	Rectf scissor = m_scissor_stack.pop();
	SET_BATCH_VAR(scissor);
	return was;
}

Rectf Batch::peek_scissor() const
{
	return m_batch.scissor;
}

void Batch::push_blend(const BlendMode& blend)
{
	m_blend_stack.push_back(m_batch.blend);
	SET_BATCH_VAR(blend);
}

BlendMode Batch::pop_blend()
{
	BlendMode was = m_batch.blend;
	BlendMode blend = m_blend_stack.pop();
	SET_BATCH_VAR(blend);
	return was;
}

BlendMode Batch::peek_blend() const
{
	return m_batch.blend;
}

void Batch::push_material(const MaterialRef& material)
{
	m_material_stack.push_back(m_batch.material);
	SET_BATCH_VAR(material);
}

MaterialRef Batch::pop_material()
{
	MaterialRef was = m_batch.material;
	MaterialRef material = m_material_stack.pop();
	SET_BATCH_VAR(material);
	return was;
}

MaterialRef Batch::peek_material() const
{
	return m_batch.material;
}

void Batch::push_layer(int layer)
{
	m_layer_stack.push_back(m_batch.layer);

	if (layer != m_batch.layer)
	{
		INSERT_BATCH();

		// TODO:
		// binary search
		auto insert = 0;
		while (insert < m_batches.size() && m_batches[insert].layer >= layer)
			insert++;

		m_batch.layer = layer;
		m_batch_insert = insert;
	}
}

int Batch::pop_layer()
{
	int was = m_batch.layer;
	int layer = m_layer_stack.pop();

	if (layer != was)
	{
		INSERT_BATCH();

		// TODO:
		// binary search
		auto insert = 0;
		while (insert < m_batches.size() && m_batches[insert].layer >= layer)
			insert++;

		m_batch.layer = layer;
		m_batch_insert = insert;
	}

	return was;
}

int Batch::peek_layer() const
{
	return m_batch.layer;
}

void Batch::push_color_mode(ColorMode mode)
{
	m_color_mode_stack.push_back(m_color_mode);
	m_color_mode = mode;

	m_tex_mult = (m_color_mode == ColorMode::Normal ? 255 : 0);
	m_tex_wash = (m_color_mode == ColorMode::Wash ? 255 : 0);
}

Batch::ColorMode Batch::pop_color_mode()
{
	ColorMode was = m_color_mode;
	m_color_mode = m_color_mode_stack.pop();
	m_tex_mult = (m_color_mode == ColorMode::Normal ? 255 : 0);
	m_tex_wash = (m_color_mode == ColorMode::Wash ? 255 : 0);
	return was;
}

Batch::ColorMode Batch::peek_color_mode() const
{
	return m_color_mode;
}

void Batch::set_texture(const TextureRef& texture)
{
	if (m_batch.elements > 0 && texture != m_batch.texture && m_batch.texture)
		INSERT_BATCH();

	if (m_batch.texture != texture)
	{
		m_batch.texture = texture;
		m_batch.flip_vertically = App::renderer().origin_bottom_left && texture && texture->is_framebuffer();
	}
}

void Batch::set_sampler(const TextureSampler& sampler)
{
	if (m_batch.elements > 0 && sampler != m_batch.sampler)
		INSERT_BATCH();

	m_batch.sampler = sampler;
}

void Batch::render(const TargetRef& target)
{
	TargetRef ref = (target ? target : App::backbuffer());
	render(ref, Mat4x4f::create_ortho_offcenter(0, (float)ref->width(), (float)ref->height(), 0, 0.01f, 1000.0f));
}

void Batch::render(const TargetRef& target, const Mat4x4f& matrix)
{
	// nothing to draw
	if ((m_batches.size() <= 0 && m_batch.elements <= 0) || m_indices.size() <= 0)
		return;

	// define defaults
	{
		if (!m_mesh)
			m_mesh = Mesh::create();

		if (!m_default_material)
		{
			BLAH_ASSERT_RENDERER();
			m_default_material = Material::create(App::Internal::renderer->default_batcher_shader);
		}
	}

	// upload data
	m_mesh->index_data(IndexFormat::UInt32, m_indices.data(), m_indices.size());
	m_mesh->vertex_data(format, m_vertices.data(), m_vertices.size());

	DrawCall pass;
	pass.target = target;
	pass.mesh = m_mesh;
	pass.has_viewport = false;
	pass.viewport = Rectf();
	pass.instance_count = 0;
	pass.depth = Compare::None;
	pass.cull = Cull::None;

	// render batches
	for (int i = 0, n = m_batches.size(); i < n; i++)
	{
		// remaining elements in the current batch
		if (m_batch_insert == i && m_batch.elements > 0)
			render_single_batch(pass, m_batch, matrix);

		// render the batch
		render_single_batch(pass, m_batches[i], matrix);
	}

	// remaining elements in the current batch
	if (m_batch_insert == m_batches.size() && m_batch.elements > 0)
		render_single_batch(pass, m_batch, matrix);
}

void Batch::render_single_batch(DrawCall& pass, const DrawBatch& b, const Mat4x4f& matrix)
{
	// get the material
	pass.material = b.material;
	if (!pass.material)
		pass.material = m_default_material;
	
	// assign texture & sampler, fallback to whatever the first one is if the names are different
	if (pass.material->has_value(texture_uniform))
		pass.material->set_texture(texture_uniform, b.texture);
	else
		pass.material->set_texture(0, b.texture);

	if (pass.material->has_value(sampler_uniform))
		pass.material->set_sampler(sampler_uniform, b.sampler);
	else
		pass.material->set_sampler(0, b.sampler);

	// assign the matrix uniform
	pass.material->set_value(matrix_uniform, matrix);
	
	pass.blend = b.blend;
	pass.has_scissor = b.scissor.w >= 0 && b.scissor.h >= 0;
	pass.scissor = b.scissor;
	pass.index_start = (i64)b.offset * 3;
	pass.index_count = (i64)b.elements * 3;

	pass.perform();
}

void Batch::clear()
{
	m_matrix = Mat3x2f::identity;
	m_color_mode = ColorMode::Normal;
	m_tex_mult = 255;
	m_tex_wash = 0;

	m_vertices.clear();
	m_indices.clear();

	m_batch.layer = 0;
	m_batch.elements = 0;
	m_batch.offset = 0;
	m_batch.blend = BlendMode::Normal;
	m_batch.material.reset();
	m_batch.texture.reset();
	m_batch.sampler = default_sampler;
	m_batch.scissor.w = m_batch.scissor.h = -1;
	m_batch.flip_vertically = false;

	m_matrix_stack.clear();
	m_scissor_stack.clear();
	m_blend_stack.clear();
	m_material_stack.clear();
	m_color_mode_stack.clear();
	m_layer_stack.clear();
	m_batches.clear();

	m_batch_insert = 0;
}

void Batch::dispose()
{
	clear();

	m_vertices.dispose();
	m_indices.dispose();
	m_matrix_stack.dispose();
	m_scissor_stack.dispose();
	m_blend_stack.dispose();
	m_material_stack.dispose();
	m_color_mode_stack.dispose();
	m_layer_stack.dispose();
	m_batches.dispose();

	m_default_material.reset();
	m_mesh.reset();
}

void Batch::line(const Vec2f& from, const Vec2f& to, float t, Color color)
{
	line(from, to, t, color, color);
}

void Batch::line(const Vec2f& from, const Vec2f& to, float t, Color fromColor, Color toColor)
{
	if (from.x == to.x && from.y == to.y)
		return;

	Vec2f perp;

	if (from.x == to.x)
	{
		perp = Vec2f::unit_x;
	}
	else if (from.y == to.y)
	{
		perp = Vec2f::unit_y;
	}
	else
	{
		Vec2f normal = (to - from).normal();
		perp = Vec2f(normal.y, -normal.x);
	}
	
	Vec2f pos0 = from + perp * t * 0.5f;
	Vec2f pos1 = to + perp * t * 0.5f;
	Vec2f pos2 = to - perp * t * 0.5f;
	Vec2f pos3 = from - perp * t * 0.5f;

	PUSH_QUAD(
		pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y,
		0, 0, 0, 0, 0, 0, 0, 0,
		fromColor, toColor, toColor, fromColor,
		0, 0, 255);
}

void Batch::bezier_line(const Vec2f& from, const Vec2f& b, const Vec2f& to, int steps, float t, Color color)
{
	Vec2f prev = from;
	float add = 1.0f / steps;

	for (int i = 1; i < steps; i++)
	{
		Vec2f at = Vec2f::lerp_bezier(from, b, to, add * i);
		line(prev, at, t, color);
		prev = at;
	}

	line(prev, to, t, color);
}

void Batch::bezier_line(const Vec2f& from, const Vec2f& b, const Vec2f& c, const Vec2f& to, int steps, float t, Color color)
{
	Vec2f prev = from;
	float add = 1.0f / steps;

	for (int i = 1; i < steps; i++)
	{
		Vec2f at = Vec2f::lerp_bezier(from, b, c, to, add * i);
		line(prev, at, t, color);
		prev = at;
	}

	line(prev, to, t, color);
}

void Batch::tri(const Vec2f& pos0, const Vec2f& pos1, const Vec2f& pos2, Color color)
{
	PUSH_TRIANGLE(
		pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y,
		0, 0, 0, 0, 0, 0,
		color, color, color,
		0, 0, 255);
}

void Batch::tri(const Vec2f& pos0, const Vec2f& pos1, const Vec2f& pos2, Color col0, Color col1, Color col2)
{
	PUSH_TRIANGLE(
		pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y,
		0, 0, 0, 0, 0, 0,
		col0, col1, col2,
		0, 0, 255);
}

void Batch::tri(const Vec2f& pos0, const Vec2f& pos1, const Vec2f& pos2, const Vec2f& tex0, const Vec2f& tex1, const Vec2f& tex2, Color color)
{
	PUSH_TRIANGLE(
		pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y,
		tex0.x, tex0.y, tex1.x, tex1.y, tex2.x, tex2.y,
		color, color, color,
		m_tex_mult, m_tex_wash, 0);
}

void Batch::tri(const Vec2f& pos0, const Vec2f& pos1, const Vec2f& pos2, const Vec2f& tex0, const Vec2f& tex1, const Vec2f& tex2, Color col0, Color col1, Color col2)
{
	PUSH_TRIANGLE(
		pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y,
		tex0.x, tex0.y, tex1.x, tex1.y, tex2.x, tex2.y,
		col0, col1, col2,
		m_tex_mult, m_tex_wash, 0);
}

void Batch::tri_line(const Vec2f& a, const Vec2f& b, const Vec2f& c, float t, Color color)
{
	// TODO:
	// Detect if the thickness of the line fills the entire shape
	// (in which case, draw a triangle instead)

	const float len_ab = (a - b).length();
	const float len_bc = (b - c).length();
	const float len_ca = (c - a).length();

	const auto off_ab = ((b - a) / len_ab).turn_left() * t;
	const auto off_bc = ((c - b) / len_bc).turn_left() * t;
	const auto off_ca = ((a - c) / len_ca).turn_left() * t;

	const auto aa = batch_shape_intersection(a + off_ca, a + off_ca, a + off_ab, b + off_ab);
	const auto bb = batch_shape_intersection(a + off_ab, b + off_ab, b + off_bc, c + off_bc);
	const auto cc = batch_shape_intersection(b + off_bc, c + off_bc, c + off_ca, a + off_ca);

	quad(aa, a, b, bb, color);
	quad(bb, b, c, cc, color);
	quad(cc, c, a, aa, color);
}

void Batch::rect(const Rectf& rect, Color color)
{
	PUSH_QUAD(
		rect.x, rect.y,
		rect.x + rect.w, rect.y,
		rect.x + rect.w, rect.y + rect.h,
		rect.x, rect.y + rect.h,
		0, 0, 0, 0, 0, 0, 0, 0,
		color, color, color, color,
		0, 0, 255);
}

void Batch::rect_line(const Rectf& rect, float t, Color color)
{
	if (t >= rect.w || t >= rect.h)
	{
		this->rect(rect, color);
	}
	else
	{
		PUSH_QUAD(
			rect.x, rect.y,
			rect.x + rect.w - t, rect.y,
			rect.x + rect.w - t, rect.y + t,
			rect.x, rect.y + t,
			0, 0, 0, 0, 0, 0, 0, 0,
			color, color, color, color,
			0, 0, 255);

		PUSH_QUAD(
			rect.x + rect.w - t, rect.y,
			rect.x + rect.w, rect.y,
			rect.x + rect.w, rect.y + rect.h - t,
			rect.x + rect.w - t, rect.y + rect.h - t,
			0, 0, 0, 0, 0, 0, 0, 0,
			color, color, color, color,
			0, 0, 255);

		PUSH_QUAD(
			rect.x + t, rect.y + rect.h - t,
			rect.x + rect.w, rect.y + rect.h - t,
			rect.x + rect.w, rect.y + rect.h,
			rect.x, rect.y + rect.h,
			0, 0, 0, 0, 0, 0, 0, 0,
			color, color, color, color,
			0, 0, 255);

		PUSH_QUAD(
			rect.x, rect.y + t,
			rect.x + t, rect.y + t,
			rect.x + t, rect.y + rect.h - t,
			rect.x, rect.y + rect.h,
			0, 0, 0, 0, 0, 0, 0, 0,
			color, color, color, color,
			0, 0, 255);
	}
}

void Batch::rect_rounded(const Rectf& rect, float radius, int steps, Color color)
{
	rect_rounded(rect, radius, steps, radius, steps, radius, steps, radius, steps, color);
}

void Batch::rect_rounded(const Rectf& rect, float rtl, int rtl_steps, float rtr, int rtr_steps, float rbr, int rbr_steps, float rbl, int rbl_steps, Color color)
{
	// clamp
	rtl = Calc::min(Calc::min(Calc::max(0.0f, rtl), rect.w / 2.0f), rect.h / 2.0f);
	rtr = Calc::min(Calc::min(Calc::max(0.0f, rtr), rect.w / 2.0f), rect.h / 2.0f);
	rbr = Calc::min(Calc::min(Calc::max(0.0f, rbr), rect.w / 2.0f), rect.h / 2.0f);
	rbl = Calc::min(Calc::min(Calc::max(0.0f, rbl), rect.w / 2.0f), rect.h / 2.0f);

	if (rtl <= 0 && rtr <= 0 && rbr <= 0 && rbl <= 0)
	{
		this->rect(rect, color);
	}
	else
	{
		// get corners
		Rectf tl = Rectf(rect.x, rect.y, rtl, rtl);
		Rectf tr = Rectf(rect.x + rect.w - rtr, rect.y, rtr, rtr);
		Rectf bl = Rectf(rect.x, rect.y + rect.h - rbl, rbl, rbl);
		Rectf br = Rectf(rect.x + rect.w - rbr, rect.y + rect.h - rbr, rbr, rbr);

		// rounded corners
		semi_circle(tl.bottom_right(), Calc::UP, Calc::LEFT, rtl, rtl_steps, color);
		semi_circle(tr.bottom_left(), Calc::UP, Calc::RIGHT, rtr, rtr_steps, color);
		semi_circle(bl.top_right(), Calc::DOWN, Calc::LEFT, rbl, rbl_steps, color);
		semi_circle(br.top_left(), Calc::DOWN, Calc::RIGHT, rbr, rbr_steps, color);

		// center quads
		quad(tl.top_right(), tr.top_left(), tr.bottom_left(), tl.bottom_right(), color);
		quad(tr.bottom_left(), tr.bottom_right(), br.top_right(), br.top_left(), color);
		quad(bl.top_right(), br.top_left(), br.bottom_left(), bl.bottom_right(), color);
		quad(tl.bottom_left(), tl.bottom_right(), bl.top_right(), bl.top_left(), color);
		quad(tl.bottom_right(), tr.bottom_left(), br.top_left(), bl.top_right(), color);
	}
}

void Batch::rect_rounded_line(const Rectf & rect, float radius, int steps, float t, Color color)
{
	rect_rounded_line(rect, radius, steps, radius, steps, radius, steps, radius, steps, t, color);
}

void Batch::rect_rounded_line(const Rectf& r, float rtl, int rtl_steps, float rtr, int rtr_steps, float rbr, int rbr_steps, float rbl, int rbl_steps, float t, Color color)
{
	// clamp
	rtl = Calc::min(Calc::min(Calc::max(0.0f, rtl), r.w / 2.0f), r.h / 2.0f);
	rtr = Calc::min(Calc::min(Calc::max(0.0f, rtr), r.w / 2.0f), r.h / 2.0f);
	rbr = Calc::min(Calc::min(Calc::max(0.0f, rbr), r.w / 2.0f), r.h / 2.0f);
	rbl = Calc::min(Calc::min(Calc::max(0.0f, rbl), r.w / 2.0f), r.h / 2.0f);

	if (rtl <= 0 && rtr <= 0 && rbr <= 0 && rbl <= 0)
	{
		rect_line(r, t, color);
	}
	else
	{
		// rounded corners
		semi_circle_line(Vec2f(r.x + rtl, r.y + rtl), Calc::UP, Calc::LEFT, rtl, rtl_steps, t, color);
		semi_circle_line(Vec2f(r.x + r.w - rtr, r.y + rtr), Calc::UP, Calc::UP + Calc::TAU * 0.25f, rtr, rtr_steps, t, color);
		semi_circle_line(Vec2f(r.x + rbl, r.y + r.h - rbl), Calc::DOWN, Calc::LEFT, rbl, rbl_steps, t, color);
		semi_circle_line(Vec2f(r.x + r.w - rbr, r.y + r.h - rbr), Calc::DOWN, Calc::RIGHT, rbr, rbr_steps, t, color);

		// connect sides that aren't touching
		if (r.h > rtl + rbl)
			rect(Rectf(r.x, r.y + rtl, t, r.h - rtl - rbl), color);
		if (r.h > rtr + rbr)
			rect(Rectf(r.x + r.w - t, r.y + rtr, t, r.h - rtr - rbr), color);
		if (r.w > rtl + rtr)
			rect(Rectf(r.x + rtl, r.y, r.w - rtl - rtr, t), color);
		if (r.w > rbl + rbr)
			rect(Rectf(r.x + rbl, r.y + r.h - t, r.w - rbl - rbr, t), color);
	}
}

void Batch::semi_circle(const Vec2f& center, float start_radians, float end_radians, float radius, int steps, Color centerColor, Color edgeColor)
{
	Vec2f last = Vec2f::from_angle(start_radians, radius);
	float add = Calc::angle_diff(start_radians, end_radians);

	for (int i = 1; i <= steps; i++)
	{
		Vec2f next = Vec2f::from_angle(start_radians + add * (i / (float)steps), radius);
		tri(center + last, center + next, center, edgeColor, edgeColor, centerColor);
		last = next;
	}
}

void Batch::semi_circle(const Vec2f& center, float start_radians, float end_radians, float radius, int steps, Color color)
{
	semi_circle(center, start_radians, end_radians, radius, steps, color, color);
}

void Batch::semi_circle_line(const Vec2f& center, float start_radians, float end_radians, float radius, int steps, float t, Color color)
{
	if (t >= radius)
	{
		semi_circle(center, start_radians, end_radians, radius, steps, color, color);
	}
	else
	{
		const auto add = Calc::angle_diff(start_radians, end_radians);

		Vec2f last_inner = Vec2f::from_angle(start_radians, radius - t);
		Vec2f last_outer = Vec2f::from_angle(start_radians, radius);

		for (int i = 1; i <= steps; i++)
		{
			const auto next_inner = Vec2f::from_angle(start_radians + add * (i / (float)steps), radius - t);
			const auto next_outer = Vec2f::from_angle(start_radians + add * (i / (float)steps), radius);

			quad(center + last_inner, center + last_outer, center + next_outer, center + next_inner, color);

			last_inner = next_inner;
			last_outer = next_outer;
		}
	}
}

void Batch::circle(const Vec2f& center, float radius, int steps, Color color)
{
	circle(center, radius, steps, color, color);
}

void Batch::circle(const Vec2f& center, float radius, int steps, Color center_color, Color outer_color)
{
	Vec2f last = Vec2f(center.x + radius, center.y);

	for (int i = 1; i <= steps; i++)
	{
		const auto radians = (i / (float)steps) * Calc::TAU;
		const auto next = Vec2f(center.x + Calc::cos(radians) * radius, center.y + Calc::sin(radians) * radius);

		tri(last, next, center, outer_color, outer_color, center_color);

		last = next;
	}
}

void Batch::circle_line(const Vec2f& center, float radius, float t, int steps, Color color)
{
	if (t >= radius)
	{
		circle(center, radius, steps, color);
	}
	else
	{
		Vec2f last_inner = Vec2f(center.x + radius - t, center.y);
		Vec2f last_outer = Vec2f(center.x + radius, center.y);

		for (int i = 1; i <= steps; i++)
		{
			const auto radians = (i / (float)steps) * Calc::TAU;
			const auto normal = Vec2f(Calc::cos(radians), Calc::sin(radians));

			const auto next_inner = Vec2f(center.x + normal.x * (radius - t), center.y + normal.y * (radius - t));
			const auto next_outer = Vec2f(center.x + normal.x * radius, center.y + normal.y * radius);

			quad(last_inner, last_outer, next_outer, next_inner, color);

			last_outer = next_outer;
			last_inner = next_inner;
		}
	}
}

void Batch::quad(const Vec2f& pos0, const Vec2f& pos1, const Vec2f& pos2, const Vec2f& pos3, Color color)
{
	PUSH_QUAD(
		pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y,
		0, 0, 0, 0, 0, 0, 0, 0,
		color, color, color, color,
		0, 0, 255);
}

void Batch::quad(const Vec2f& pos0, const Vec2f& pos1, const Vec2f& pos2, const Vec2f& pos3, Color col0, Color col1, Color col2, Color col3)
{
	PUSH_QUAD(
		pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y,
		0, 0, 0, 0, 0, 0, 0, 0,
		col0, col1, col2, col3,
		0, 0, 255);
}

void Batch::quad(const Vec2f& pos0, const Vec2f& pos1, const Vec2f& pos2, const Vec2f& pos3, const Vec2f& tex0, const Vec2f& tex1, const Vec2f& tex2, const Vec2f& tex3, Color color)
{
	PUSH_QUAD(
		pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y,
		tex0.x, tex0.y, tex1.x, tex1.y, tex2.x, tex2.y, tex3.x, tex3.y,
		color, color, color, color,
		m_tex_mult, m_tex_wash, 0);
}

void Batch::quad(const Vec2f& pos0, const Vec2f& pos1, const Vec2f& pos2, const Vec2f& pos3, const Vec2f& tex0, const Vec2f& tex1, const Vec2f& tex2, const Vec2f& tex3, Color col0, Color col1, Color col2, Color col3)
{
	PUSH_QUAD(
		pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y,
		tex0.x, tex0.y, tex1.x, tex1.y, tex2.x, tex2.y, tex3.x, tex3.y,
		col0, col1, col2, col3,
		m_tex_mult, m_tex_wash, 0);
}

void Batch::quad_line(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, float t, Color color)
{
	// TODO:
	// Detect if the thickness of the line fills the entire shape
	// (in which case, draw a quad instead)

	const float len_ab = (a - b).length();
	const float len_bc = (b - c).length();
	const float len_cd = (c - d).length();
	const float len_da = (d - a).length();
	
	const auto off_ab = ((b - a) / len_ab).turn_left() * t;
	const auto off_bc = ((c - b) / len_bc).turn_left() * t;
	const auto off_cd = ((d - c) / len_cd).turn_left() * t;
	const auto off_da = ((a - d) / len_da).turn_left() * t;

	const auto aa = batch_shape_intersection(d + off_da, a + off_da, a + off_ab, b + off_ab);
	const auto bb = batch_shape_intersection(a + off_ab, b + off_ab, b + off_bc, c + off_bc);
	const auto cc = batch_shape_intersection(b + off_bc, c + off_bc, c + off_cd, d + off_cd);
	const auto dd = batch_shape_intersection(c + off_cd, d + off_cd, d + off_da, a + off_da);

	quad(aa, a, b, bb, color);
	quad(bb, b, c, cc, color);
	quad(cc, c, d, dd, color);
	quad(dd, d, a, aa, color);
}

void Batch::arrow_head(const Vec2f& point_pos, float radians, float side_len, Color color)
{
	arrow_head(point_pos, point_pos - Vec2f::from_angle(radians), side_len, color);
}

void Batch::arrow_head(const Vec2f& point_pos, const Vec2f& from_pos, float side_len, Color color)
{
	float height = sqrtf(side_len * side_len - powf(side_len / 2, 2));
	Vec2f dir = (point_pos - from_pos).normal();
	Vec2f perp = dir.turn_right();
	Vec2f base = point_pos - dir * height;
	
	tri(point_pos, base + perp * side_len / 2, base - perp * side_len / 2, color);
}

void Batch::tex(const TextureRef& texture, const Vec2f& pos, Color color)
{
	set_texture(texture);

	const auto w = texture->width();
	const auto h = texture->height();

	PUSH_QUAD(
		pos.x, pos.y, pos.x + w, pos.y, pos.x + w, pos.y + h, pos.x, pos.y + h,
		0, 0, 1, 0, 1, 1, 0, 1,
		color, color, color, color,
		m_tex_mult, m_tex_wash, 0);
}

void Batch::tex(const TextureRef& texture, const Vec2f& pos, const Vec2f& origin, const Vec2f& scale, float rotation, Color color)
{
	push_matrix(Mat3x2f::create_transform(pos, origin, scale, rotation));

	set_texture(texture);

	const auto w = texture->width();
	const auto h = texture->height();

	PUSH_QUAD(
		0, 0, w, 0, w, h, 0, h,
		0, 0, 1, 0, 1, 1, 0, 1,
		color, color, color, color,
		m_tex_mult, m_tex_wash, 0);

	pop_matrix();
}

void Batch::tex(const TextureRef& texture, const Rectf& clip, const Vec2f& pos, const Vec2f& origin, const Vec2f& scale, float rotation, Color color)
{
	push_matrix(Mat3x2f::create_transform(pos, origin, scale, rotation));

	set_texture(texture);

	const auto tw = texture->width();
	const auto th = texture->height();
	const auto tx0 = clip.x / tw;
	const auto tx1 = (clip.x + clip.w) / tw;
	const auto ty0 = clip.y / th;
	const auto ty1 = (clip.y + clip.h) / th;

	PUSH_QUAD(
		0, 0, clip.w, 0, clip.w, clip.h, 0, clip.h,
		tx0, ty0, tx1, ty0, tx1, ty1, tx0, ty1,
		color, color, color, color,
		m_tex_mult, m_tex_wash, 0);

	pop_matrix();
}

void Batch::tex(const Subtexture& sub, const Vec2f& pos, Color color)
{
	if (!sub.texture)
	{
		PUSH_QUAD(
			pos.x + sub.draw_coords[0].x, pos.y + sub.draw_coords[0].y,
			pos.x + sub.draw_coords[1].x, pos.y + sub.draw_coords[1].y,
			pos.x + sub.draw_coords[2].x, pos.y + sub.draw_coords[2].y,
			pos.x + sub.draw_coords[3].x, pos.y + sub.draw_coords[3].y,
			0, 0, 0, 0, 0, 0, 0, 0,
			color, color, color, color,
			0, 0, 255);
	}
	else
	{
		set_texture(sub.texture);

		PUSH_QUAD(
			pos.x + sub.draw_coords[0].x, pos.y + sub.draw_coords[0].y,
			pos.x + sub.draw_coords[1].x, pos.y + sub.draw_coords[1].y,
			pos.x + sub.draw_coords[2].x, pos.y + sub.draw_coords[2].y,
			pos.x + sub.draw_coords[3].x, pos.y + sub.draw_coords[3].y,
			sub.tex_coords[0].x, sub.tex_coords[0].y,
			sub.tex_coords[1].x, sub.tex_coords[1].y,
			sub.tex_coords[2].x, sub.tex_coords[2].y,
			sub.tex_coords[3].x, sub.tex_coords[3].y,
			color, color, color, color,
			m_tex_mult, m_tex_wash, 0);
	}
}

void Batch::tex(const Subtexture& sub, const Vec2f& pos, const Vec2f& origin, const Vec2f& scale, float rotation, Color color)
{
	push_matrix(Mat3x2f::create_transform(pos, origin, scale, rotation));

	if (!sub.texture)
	{
		PUSH_QUAD(
			sub.draw_coords[0].x, sub.draw_coords[0].y,
			sub.draw_coords[1].x, sub.draw_coords[1].y,
			sub.draw_coords[2].x, sub.draw_coords[2].y,
			sub.draw_coords[3].x, sub.draw_coords[3].y,
			0, 0, 0, 0, 0, 0, 0, 0,
			color, color, color, color,
			0, 0, 255);
	}
	else
	{
		set_texture(sub.texture);

		PUSH_QUAD(
			sub.draw_coords[0].x, sub.draw_coords[0].y,
			sub.draw_coords[1].x, sub.draw_coords[1].y,
			sub.draw_coords[2].x, sub.draw_coords[2].y,
			sub.draw_coords[3].x, sub.draw_coords[3].y,
			sub.tex_coords[0].x, sub.tex_coords[0].y,
			sub.tex_coords[1].x, sub.tex_coords[1].y,
			sub.tex_coords[2].x, sub.tex_coords[2].y,
			sub.tex_coords[3].x, sub.tex_coords[3].y,
			color, color, color, color,
			m_tex_mult, m_tex_wash, 0);
	}

	pop_matrix();
}

void Batch::tex(const Subtexture& sub, const Rectf& clip, const Vec2f& pos, const Vec2f& origin, const Vec2f& scale, float rotation, Color color)
{
	tex(sub.crop(clip), pos, origin, scale, rotation, color);
}

void Batch::str(const SpriteFont& font, const String& text, const Vec2f& pos, Color color)
{
	str(font, text, pos, Vec2f::zero, font.size, color);
}

void Batch::str(const SpriteFont& font, const String& text, const Vec2f& pos, const Vec2f& justify, float size, Color color)
{
	push_matrix(
		Mat3x2f::create_scale(size / font.size) *
		Mat3x2f::create_translation(pos)
	);

	Vec2f offset = Vec2f(0, font.ascent + font.descent);
	if (justify.x != 0)
		offset.x -= font.width_of_line(text) * justify.x;
	if (justify.y != 0)
		offset.y -= font.height_of(text) * justify.y;

	u32 last = 0;
	Utf8 utf8(text.cstr());
	int i = 0;
	while (utf8.character)
	{
		if (utf8.character == '\n')
		{
			offset.x = 0;
			offset.y += font.line_height();

			if (justify.x != 0)
				offset.x -= font.width_of_line(text, i + 1) * justify.x;

			last = 0;
		}
		else
		{
			const auto& ch = font[utf8.character];
			if (ch.subtexture.texture)
			{
				Vec2f at = offset + ch.offset;
				if (last)
					at.x += font.get_kerning(last, utf8.character);
				tex(ch.subtexture, at, color);
			}

			offset.x += ch.advance;
			last = utf8.character;
		}

		i += utf8.character_size;
		utf8.next();
	}

	pop_matrix();
}
