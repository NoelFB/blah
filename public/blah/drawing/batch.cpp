#include <blah/drawing/batch.h>
#include <blah/graphics/texture.h>
#include <blah/graphics/framebuffer.h>
#include <blah/graphics/mesh.h>
#include <blah/graphics/shader.h>
#include <blah/graphics/material.h>
#include <blah/math/calc.h>

using namespace Blah;

// TODO:
// the line drawings methods aren't consistent
// ex. some draw outside the shape, some draw within it, etc.
// -> make them all draw within the provided shape

// TODO:
// This needs to be graphics API agnostic

const ShaderData data = {
	// vertex shader
	"#version 330\n"
	"uniform mat4 u_matrix;\n"
	"layout(location=0) in vec2 a_position;\n"
	"layout(location=1) in vec2 a_tex;\n"
	"layout(location=2) in vec4 a_color;\n"
	"layout(location=3) in vec3 a_type;\n"
	"out vec2 v_tex;\n"
	"out vec4 v_col;\n"
	"out vec3 v_type;\n"
	"void main(void)\n"
	"{\n"
	"	gl_Position = u_matrix * vec4(a_position.xy, 0, 1);\n"
	"	v_tex = a_tex;\n"
	"	v_col = a_color;\n"
	"	v_type = a_type;\n"
	"}",

	// fragment shader
	"#version 330\n"
	"uniform sampler2D u_texture;\n"
	"in vec2 v_tex;\n"
	"in vec4 v_col;\n"
	"in vec3 v_type;\n"
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

const VertexAttribute attributes[4] = {
	{ 0, VertexSemantics::Position, VertexAttributeType::Float, 2, false },
	{ 1, VertexSemantics::Texcoord0, VertexAttributeType::Float, 2, false },
	{ 2, VertexSemantics::Color0, VertexAttributeType::Byte, 4, true },
	{ 3, VertexSemantics::Texcoord1, VertexAttributeType::Byte, 3, true },
};

#define MAKE_VERTEX(vert, mat, px, py, tx, ty, c, m, w, f) \
	(vert)->pos.x = ((px) * mat.m11) + ((py) * mat.m21) + mat.m31; \
	(vert)->pos.y = ((px) * mat.m12) + ((py) * mat.m22) + mat.m32; \
	(vert)->tex.x = tx;  \
	if (m_batch.flip_vertically) \
		(vert)->tex.y = 1.0f - ty; \
	else \
		(vert)->tex.y = ty; \
	(vert)->col = c; \
	(vert)->mult = m; \
	(vert)->wash = w; \
	(vert)->fill = f;
	
#define PUSH_QUAD(px0, py0, px1, py1, px2, py2, px3, py3, tx0, ty0, tx1, ty1, tx2, ty2, tx3, ty3, col0, col1, col2, col3, mult, fill, wash) \
	{ \
		m_batch.elements += 2; \
		int* _i = m_indices.expand(6); \
		*_i++ = m_vertices.count() + 0; \
		*_i++ = m_vertices.count() + 1; \
		*_i++ = m_vertices.count() + 2; \
		*_i++ = m_vertices.count() + 0; \
		*_i++ = m_vertices.count() + 2; \
		*_i++ = m_vertices.count() + 3; \
		Vertex* _v = m_vertices.expand(4); \
		MAKE_VERTEX(_v, m_matrix, px0, py0, tx0, ty0, col0, mult, fill, wash); _v++; \
		MAKE_VERTEX(_v, m_matrix, px1, py1, tx1, ty1, col1, mult, fill, wash); _v++; \
		MAKE_VERTEX(_v, m_matrix, px2, py2, tx2, ty2, col2, mult, fill, wash); _v++; \
		MAKE_VERTEX(_v, m_matrix, px3, py3, tx3, ty3, col3, mult, fill, wash); \
	}

#define PUSH_TRIANGLE(px0, py0, px1, py1, px2, py2, tx0, ty0, tx1, ty1, tx2, ty2, col0, col1, col2, mult, fill, wash) \
	{ \
		m_batch.elements += 1; \
		int* _i = m_indices.expand(3); \
		*_i++ = m_vertices.count() + 0; \
		*_i++ = m_vertices.count() + 1; \
		*_i++ = m_vertices.count() + 2; \
		Vertex* _v = m_vertices.expand(3); \
		MAKE_VERTEX(_v, m_matrix, px0, py0, tx0, ty0, col0, mult, fill, wash); _v++; \
		MAKE_VERTEX(_v, m_matrix, px1, py1, tx1, ty1, col1, mult, fill, wash); _v++; \
		MAKE_VERTEX(_v, m_matrix, px2, py2, tx2, ty2, col2, mult, fill, wash); \
	}

// Compares a Batcher variable, and starts a new batch if it has changed
#define SET_BATCH_VAR(variable) \
	if (m_batch.elements > 0 && variable != m_batch.variable) \
	{ \
		m_batches.add(m_batch); \
		m_batch.offset += m_batch.elements; \
		m_batch.elements = 0; \
	} \
	m_batch.variable = variable;

ShaderRef Batch::m_default_shader;

Batch::Batch()
{
	matrix_uniform = "u_matrix";
	texture_uniform = "u_texture";

	clear();
}

Batch::~Batch()
{
	dispose();
}

void Batch::push_matrix(const Mat3x2& matrix)
{
	m_matrix_stack.add(m_matrix);
	m_matrix = matrix * m_matrix;
}

void Batch::pop_matrix()
{
	m_matrix = m_matrix_stack.pop();
}

void Batch::push_scissor(const Rect& scissor)
{
	m_scissor_stack.add(m_batch.scissor);
	SET_BATCH_VAR(scissor);
}

void Batch::pop_scissor()
{
	Rect scissor = m_scissor_stack.pop();
	SET_BATCH_VAR(scissor);
}

void Batch::push_blend(const BlendMode& blend)
{
	m_blend_stack.add(m_batch.blend);
	SET_BATCH_VAR(blend);
}

void Batch::pop_blend()
{
	BlendMode blend = m_blend_stack.pop();
	SET_BATCH_VAR(blend);
}

void Batch::push_material(const MaterialRef& material)
{
	m_material_stack.add(m_batch.material);
	SET_BATCH_VAR(material);
}

void Batch::pop_material()
{
	MaterialRef material = m_material_stack.pop();
	SET_BATCH_VAR(material);
}

void Batch::push_layer(int layer)
{
	m_layer_stack.add(m_batch.layer);
	SET_BATCH_VAR(layer);
}

void Batch::pop_layer()
{
	int layer = m_layer_stack.pop();
	SET_BATCH_VAR(layer);
}

void Batch::push_color_mode(ColorMode mode)
{
	m_color_mode_stack.add(m_color_mode);
	m_color_mode = mode;

	m_tex_mult = (m_color_mode == ColorMode::Normal ? 255 : 0);
	m_tex_wash = (m_color_mode == ColorMode::Wash ? 255 : 0);
}

void Batch::pop_color_mode()
{
	m_color_mode = m_color_mode_stack.pop();
	m_tex_mult = (m_color_mode == ColorMode::Normal ? 255 : 0);
	m_tex_wash = (m_color_mode == ColorMode::Wash ? 255 : 0);
}

void Batch::set_texture(const TextureRef& texture)
{
	if (m_batch.elements > 0 && texture != m_batch.texture && m_batch.texture && m_batch.texture->is_valid())
	{
		m_batches.add(m_batch);
		m_batch.offset += m_batch.elements;
		m_batch.elements = 0;
	}

	if (m_batch.texture != texture)
	{
		m_batch.texture = texture;
		m_batch.flip_vertically = Graphics::info()->origin_bottom_left && texture && texture->is_valid() && texture->is_framebuffer();
	}
}

void Batch::render(const FrameBufferRef& target)
{
	Point size;
	if (!target || !target->is_valid())
		size = Point(App::draw_width(), App::draw_height());
	else
		size = Point(target->width(), target->height());

	render(target, Mat4x4::create_ortho_offcenter(0, (float)size.x, (float)size.y, 0, 0.01f, 1000.0f));
}

void Batch::render(const FrameBufferRef& target, const Mat4x4& matrix)
{
	// nothing to draw
	if ((m_batches.count() <= 0 && m_batch.elements <= 0) || m_indices.count() <= 0)
		return;

	// define defaults
	{
		if (!m_mesh || !m_mesh->is_valid())
		{
			m_mesh = Graphics::create_mesh();
			m_mesh->vertex_format(attributes, 4, sizeof(Vertex));
		}

		if (!m_default_shader)
			m_default_shader = Graphics::create_shader(&data);

		if (!m_default_material)
			m_default_material = Graphics::create_material(m_default_shader);
	}

	// upload data
	m_mesh->index_data(m_indices.begin(), m_indices.count());
	m_mesh->vertex_data(m_vertices.begin(), m_vertices.count());

	RenderCall call;
	call.target = target;
	call.mesh = m_mesh;
	call.has_viewport = false;
	call.viewport = Rect();
	call.instance_count = 0;
	call.depth = Compare::None;
	call.cull = Cull::None;

	for (auto& b : m_batches)
		render_single_batch(call, b, matrix);

	if (m_batch.elements > 0)
		render_single_batch(call, m_batch, matrix);
}

void Batch::render_single_batch(RenderCall& call, const DrawBatch& b, const Mat4x4& matrix)
{
	call.material = b.material;
	if (!call.material || !call.material->is_valid())
		call.material = m_default_material;

	call.material->set_texture(texture_uniform, b.texture, 0);
	call.material->set_value(matrix_uniform, &matrix.m11, 16);
	
	call.blend = b.blend;
	call.has_scissor = b.scissor.w >= 0 && b.scissor.h >= 0;
	call.scissor = b.scissor;
	call.index_start = (int64_t)b.offset * 3;
	call.index_count = (int64_t)b.elements * 3;

	Graphics::render(call);
}

void Batch::clear()
{
	m_matrix = Mat3x2::identity;
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
	m_batch.scissor.w = m_batch.scissor.h = -1;
	m_batch.flip_vertically = false;

	m_matrix_stack.clear();
	m_scissor_stack.clear();
	m_blend_stack.clear();
	m_material_stack.clear();
	m_color_mode_stack.clear();
	m_layer_stack.clear();
	m_batches.clear();
}

void Batch::dispose()
{
	m_matrix_stack.dispose();
	m_scissor_stack.dispose();
	m_blend_stack.dispose();
	m_material_stack.dispose();
	m_color_mode_stack.dispose();
	m_layer_stack.dispose();
	m_batches.dispose();
	m_default_material.reset();
	m_mesh.reset();
	m_vertices.dispose();
	m_indices.dispose();
}

void Batch::line(const Vec2& from, const Vec2& to, float t, Color color)
{
	line(from, to, t, color, color);
}

void Batch::line(const Vec2& from, const Vec2& to, float t, Color fromColor, Color toColor)
{
	if (from.x == to.x && from.y == to.y)
		return;

	Vec2 normal = (to - from).normal();
	Vec2 perp = Vec2(normal.y, -normal.x);
	Vec2 pos0 = from + perp * t * 0.5f;
	Vec2 pos1 = to + perp * t * 0.5f;
	Vec2 pos2 = to - perp * t * 0.5f;
	Vec2 pos3 = from - perp * t * 0.5f;

	PUSH_QUAD(
		pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y,
		0, 0, 0, 0, 0, 0, 0, 0,
		fromColor, toColor, toColor, fromColor,
		0, 0, 255);
}

void Batch::bezier_line(const Vec2& from, const Vec2& b, const Vec2& to, int steps, float t, Color color)
{
	Vec2 prev = from;
	float add = 1.0f / steps;

	for (int i = 1; i < steps; i++)
	{
		Vec2 at = Vec2::bezier_lerp(from, b, to, add * i);
		line(prev, at, t, color);
		prev = at;
	}

	line(prev, to, t, color);
}

void Batch::bezier_line(const Vec2& from, const Vec2& b, const Vec2& c, const Vec2& to, int steps, float t, Color color)
{
	Vec2 prev = from;
	float add = 1.0f / steps;

	for (int i = 1; i < steps; i++)
	{
		Vec2 at = Vec2::bezier_lerp(from, b, c, to, add * i);
		line(prev, at, t, color);
		prev = at;
	}

	line(prev, to, t, color);
}

void Batch::tri(const Vec2& pos0, const Vec2& pos1, const Vec2& pos2, Color color)
{
	PUSH_TRIANGLE(
		pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y,
		0, 0, 0, 0, 0, 0,
		color, color, color,
		0, 0, 255);
}

void Batch::tri(const Vec2& pos0, const Vec2& pos1, const Vec2& pos2, Color col0, Color col1, Color col2)
{
	PUSH_TRIANGLE(
		pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y,
		0, 0, 0, 0, 0, 0,
		col0, col1, col2,
		0, 0, 255);
}

void Batch::tri(const Vec2& pos0, const Vec2& pos1, const Vec2& pos2, const Vec2& tex0, const Vec2& tex1, const Vec2& tex2, Color color)
{
	PUSH_TRIANGLE(
		pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y,
		tex0.x, tex0.y, tex1.x, tex1.y, tex2.x, tex2.y,
		color, color, color,
		m_tex_mult, m_tex_wash, 0);
}

void Batch::tri(const Vec2& pos0, const Vec2& pos1, const Vec2& pos2, const Vec2& tex0, const Vec2& tex1, const Vec2& tex2, Color col0, Color col1, Color col2)
{
	PUSH_TRIANGLE(
		pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y,
		tex0.x, tex0.y, tex1.x, tex1.y, tex2.x, tex2.y,
		col0, col1, col2,
		m_tex_mult, m_tex_wash, 0);
}

void Batch::tri_line(const Vec2& a, const Vec2& b, const Vec2& c, float t, Color color)
{
	
}

void Batch::rect(const Rect& rect, Color color)
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

void Batch::rect_line(const Rect& rect, float t, Color color)
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
		rect.x + t, rect.y + rect.h,
		rect.x, rect.y + rect.h,
		0, 0, 0, 0, 0, 0, 0, 0,
		color, color, color, color,
		0, 0, 255);
}

void Batch::rect_rounded(const Rect& rect, float radius, int steps, Color color)
{
	rect_rounded(rect, radius, steps, radius, steps, radius, steps, radius, steps, color);
}

void Batch::rect_rounded(const Rect& rect, float rtl, int rtl_steps, float rtr, int rtr_steps, float rbr, int rbr_steps, float rbl, int rbl_steps, Color color)
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
		Rect tl = Rect(rect.top_left(), Vec2(rtl, rtl));
		Rect tr = Rect(rect.top_right() + Vec2(-rtr, 0), Vec2(rtr, rtr));
		Rect bl = Rect(rect.bottom_left() + Vec2(0, -rbl), Vec2(rbl, rbl));
		Rect br = Rect(rect.bottom_right() + Vec2(-rbr, -rbr), Vec2(rbr, rbr));

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

void Batch::rect_rounded_line(const Rect & rect, float radius, int steps, float t, Color color)
{
	rect_rounded_line(rect, radius, steps, radius, steps, radius, steps, radius, steps, t, color);
}

void Batch::rect_rounded_line(const Rect& rect, float rtl, int rtl_steps, float rtr, int rtr_steps, float rbr, int rbr_steps, float rbl, int rbl_steps, float t, Color color)
{
	Rect r = rect;
	r.x += t * 0.5f;
	r.y += t * 0.5f;
	r.w -= t;
	r.h -= t;

	// clamp
	rtl = Calc::min(Calc::min(Calc::max(0.0f, rtl), r.w / 2.0f), r.h / 2.0f);
	rtr = Calc::min(Calc::min(Calc::max(0.0f, rtr), r.w / 2.0f), r.h / 2.0f);
	rbr = Calc::min(Calc::min(Calc::max(0.0f, rbr), r.w / 2.0f), r.h / 2.0f);
	rbl = Calc::min(Calc::min(Calc::max(0.0f, rbl), r.w / 2.0f), r.h / 2.0f);

	if (rtl <= 0 && rtr <= 0 && rbr <= 0 && rbl <= 0)
	{
		this->rect_line(r, t, color);
	}
	else
	{
		// get corners
		Rect tl = Rect(r.top_left(), Vec2(rtl, rtl));
		Rect tr = Rect(r.top_right() + Vec2(-rtr, 0), Vec2(rtr, rtr));
		Rect bl = Rect(r.bottom_left() + Vec2(0, -rbl), Vec2(rbl, rbl));
		Rect br = Rect(r.bottom_right() + Vec2(-rbr, -rbr), Vec2(rbr, rbr));

		// rounded corners
		semi_circle_line(tl.bottom_right(), Calc::UP, Calc::LEFT, rtl, rtl_steps, t, color);
		semi_circle_line(tr.bottom_left(), Calc::UP, Calc::RIGHT, rtr, rtr_steps, t, color);
		semi_circle_line(bl.top_right(), Calc::DOWN, Calc::LEFT, rbl, rbl_steps, t, color);
		semi_circle_line(br.top_left(), Calc::DOWN, Calc::RIGHT, rbr, rbr_steps, t, color);

		// connect sides that aren't touching
		if (tl.bottom() < bl.top())
			line(tl.bottom_left(), bl.top_left(), t, color);
		if (tl.right() < tr.left())
			line(tl.top_right(), tr.top_left(), t, color);
		if (tr.bottom() < br.top())
			line(tr.bottom_right(), br.top_right(), t, color);
		if (bl.right() < br.left())
			line(bl.bottom_right(), br.bottom_left(), t, color);
	}
}

void Batch::semi_circle(Vec2 center, float start_radians, float end_radians, float radius, int steps, Color centerColor, Color edgeColor)
{
	Vec2 last = Vec2::from_angle(start_radians, radius);
	float add = Calc::angle_diff(start_radians, end_radians);

	for (int i = 1; i <= steps; i++)
	{
		Vec2 next = Vec2::from_angle(start_radians + add * (i / (float)steps), radius);
		tri(center + last, center + next, center, edgeColor, edgeColor, centerColor);
		last = next;
	}
}

void Batch::semi_circle(Vec2 center, float start_radians, float end_radians, float radius, int steps, Color color)
{
	semi_circle(center, start_radians, end_radians, radius, steps, color, color);
}

void Batch::semi_circle_line(Vec2 center, float start_radians, float end_radians, float radius, int steps, float t, Color color)
{
	float add = Calc::angle_diff(start_radians, end_radians);

	Vec2 last_inner = Vec2::from_angle(start_radians, radius - t / 2);
	Vec2 last_outer = Vec2::from_angle(start_radians, radius + t / 2);

	for (int i = 1; i <= steps; i++)
	{
		Vec2 next_inner = Vec2::from_angle(start_radians + add * (i / (float)steps), radius - t / 2);
		Vec2 next_outer = Vec2::from_angle(start_radians + add * (i / (float)steps), radius + t / 2);

		quad(center + last_inner, center + last_outer, center + next_outer, center + next_inner, color);

		last_inner = next_inner;
		last_outer = next_outer;
	}
}

void Batch::circle(const Vec2 center, float radius, int steps, Color color)
{
	Vec2 last = Vec2(radius, 0);

	for (int i = 1; i <= steps; i++)
	{
		float radians = (i / (float)steps) * Calc::TAU;
		Vec2 next = Vec2::from_angle(radians, radius);
		tri(center + last, center + next, center, color);
		last = next;
	}
}

void Batch::circle_line(const Vec2 center, float radius, float t, int steps, Color color)
{
	radius -= t * 0.5f;
	Vec2 last = Vec2(radius, 0);

	for (int i = 1; i <= steps; i++)
	{
		float radians = (i / (float)steps) * Calc::TAU;
		Vec2 next = Vec2(Calc::cos(radians), Calc::sin(radians)) * radius;
		line(center + last, center + next, t, color);
		last = next;
	}
}

void Batch::quad(const Vec2& pos0, const Vec2& pos1, const Vec2& pos2, const Vec2& pos3, Color color)
{
	PUSH_QUAD(
		pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y,
		0, 0, 0, 0, 0, 0, 0, 0,
		color, color, color, color,
		0, 0, 255);
}

void Batch::quad(const Vec2& pos0, const Vec2& pos1, const Vec2& pos2, const Vec2& pos3, Color col0, Color col1, Color col2, Color col3)
{
	PUSH_QUAD(
		pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y,
		0, 0, 0, 0, 0, 0, 0, 0,
		col0, col1, col2, col3,
		0, 0, 255);
}

void Batch::quad(const Vec2& pos0, const Vec2& pos1, const Vec2& pos2, const Vec2& pos3, const Vec2& tex0, const Vec2& tex1, const Vec2& tex2, const Vec2& tex3, Color color)
{
	PUSH_QUAD(
		pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y,
		tex0.x, tex0.y, tex1.x, tex1.y, tex2.x, tex2.y, tex3.x, tex3.y,
		color, color, color, color,
		m_tex_mult, m_tex_wash, 0);
}

void Batch::quad(const Vec2& pos0, const Vec2& pos1, const Vec2& pos2, const Vec2& pos3, const Vec2& tex0, const Vec2& tex1, const Vec2& tex2, const Vec2& tex3, Color col0, Color col1, Color col2, Color col3)
{
	PUSH_QUAD(
		pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y,
		tex0.x, tex0.y, tex1.x, tex1.y, tex2.x, tex2.y, tex3.x, tex3.y,
		col0, col1, col2, col3,
		m_tex_mult, m_tex_wash, 0);
}

void Batch::quad_line(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d, float t, Color color)
{
	line(a, b, t, color);
	line(b, c, t, color);
	line(c, d, t, color);
	line(d, a, t, color);
}

void Batch::arrow_head(const Vec2& point_pos, float radians, float side_len, Color color)
{
	arrow_head(point_pos, point_pos - Vec2::from_angle(radians), side_len, color);
}

void Batch::arrow_head(const Vec2& point_pos, const Vec2& from_pos, float side_len, Color color)
{
	float height = sqrtf(side_len * side_len - powf(side_len / 2, 2));
	Vec2 dir = (point_pos - from_pos).normal();
	Vec2 perp = dir.perpendicular();
	Vec2 base = point_pos - dir * height;
	
	tri(point_pos, base + perp * side_len / 2, base - perp * side_len / 2, color);
}

void Batch::tex()
{

}

void Batch::tex(const Subtexture& sub, const Vec2& pos, Color color)
{
	if (!sub.texture || !sub.texture->is_valid())
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

void Batch::tex(const Subtexture& sub, const Vec2& pos, const Vec2& origin, const Vec2& scale, float rotation, Color color)
{
	push_matrix(Mat3x2::create_transform(pos, origin, scale, rotation));

	if (!sub.texture || !sub.texture->is_valid())
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

void Batch::str(const SpriteFont& font, const String& text, const Vec2& pos, Color color)
{
	str(font, text, pos, TextAlign::TopLeft, font.size(), color);
}

void Batch::str(const SpriteFont& font, const String& text, const Vec2& pos, TextAlign align, float size, Color color)
{
	push_matrix(
		Mat3x2::create_scale(size / font.size()) *
		Mat3x2::create_translation(pos)
		);

	Vec2 offset;

	if ((align & TextAlign::Left) == TextAlign::Left)
		offset.x = 0;
	else if ((align & TextAlign::Right) == TextAlign::Right)
		offset.x -= font.width_of_line(text);
	else
		offset.x -= font.width_of_line(text) * 0.5f;

	if ((align & TextAlign::Top) == TextAlign::Top)
		offset.y = font.ascent() + font.descent();
	else if ((align & TextAlign::Bottom) == TextAlign::Bottom)
		offset.y = font.height() - font.height_of(text);
	else
		offset.y = (font.ascent() + font.descent() + font.height() - font.height_of(text)) * 0.5f;

	for (int i = 0, l = text.length(); i < l; i++)
	{
		if (text[i] == '\n')
		{
			// increment y
			offset.y += font.line_height();

			// re-align X for this line
			if ((align & TextAlign::Left) == TextAlign::Left)
				offset.x = 0;
			else if ((align & TextAlign::Right) == TextAlign::Right)
				offset.x = -font.width_of_line(text, i + 1);
			else
				offset.x = -font.width_of_line(text, i + 1) * 0.5f;

			continue;
		}

		// TODO:
		// This doesn't parse Unicode!
		// It will assume it's a 1-byte ASCII char which is incorrect
		auto ch = font[text[i]];

		if (!ch.subtexture.texture || !ch.subtexture.texture->is_valid())
		{
			Vec2 at = offset + ch.offset;

			if (i > 0 && text[i - 1] != '\n')
			{
				// TODO:
				// This doesn't parse Unicode!
				at.x += font.get_kerning(text[i], text[i - 1]);
			}

			tex(ch.subtexture, at, color);
		}

		offset.x += ch.advance;
	}

	pop_matrix();
}
