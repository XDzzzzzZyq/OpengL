#include "DebugLine.h"
#include "macros.h"

DebugLine::DebugLine(const glm::vec3& start, const glm::vec3& end)
	:DebugLine(std::vector<glm::vec3>{start, end})
{
	is_multi_lines = false;
}

DebugLine::DebugLine(const std::vector<glm::vec3>& vertices)
{
	o_type = GO_DL;

	for (const auto& vert : vertices) 
		dLine_pos_list.push_back(vert);

	is_multi_lines = true;

	SetDLineShader();

	o_name = "Debug Line." + std::to_string(GetObjectID());
}

DebugLine::DebugLine()
{
	SetDLineShader();

	o_name = "Debug Line." + std::to_string(GetObjectID());
}

void DebugLine::SetDebugLineParas(bool stipple, bool smooth, float width, float opacity)
{
	using_stipple = stipple;
	using_smooth = smooth;
	dLine_width = width;
	dLine_opacity = opacity;
}

void DebugLine::PushDebugLine(const glm::vec3& point)
{
	dLine_pos_list.emplace_back(point);
}

void DebugLine::PushDebugLine(float x, float y, float z)
{
	PushDebugLine({ x, y, z });
}

void DebugLine::PushDebugLines(const std::vector<glm::vec3>& points)
{
	dLine_pos_list.reserve(dLine_pos_list.size() + points.size());

	LOOP(points.size())
		dLine_pos_list.emplace_back(points[i]);
}

void DebugLine::RenderDdbugLine(Camera* camera)
{

	if (dLine_pos_list.size() < 2)return;

	dLine_shader.UseShader();
	if (dLine_shader.is_shader_changed)
		dLine_shader.InitShader();

	if(camera->is_invUniform_changed || dLine_shader.is_shader_changed)
		dLine_shader.SetValue("U_cam_trans",camera->o_InvTransform);

	if(camera->is_frustum_changed || dLine_shader.is_shader_changed)
		dLine_shader.SetValue("U_ProjectM", camera->cam_frustum);

	if(is_Uniform_changed || dLine_shader.is_shader_changed)
		dLine_shader.SetValue("U_Trans", o_Transform);

	dLine_shader.SetValue("is_selected", (int)is_selected);

	dLine_shader.SetValue("dlineOpacity", dLine_opacity);
	dLine_shader.SetValue("U_color", dLine_color);

	if (using_smooth)
		glEnable(GL_LINE_SMOOTH);
	if (using_stipple)
		glEnable(GL_LINE_STIPPLE);

	glPushAttrib(GL_LINE_BIT);
	glLineWidth(dLine_width);

	if (using_stipple)
		glLineStipple(5, 0x0c0f);
	glEnableClientState(GL_VERTEX_ARRAY);


	//glDrawElements(GL_LINE, dLine_index.count(), GL_UNSIGNED_INT, nullptr);

	glVertexPointer(3, GL_FLOAT, 0, dLine_pos_list.data());
	glDrawArrays(GL_LINE_STRIP, 0, dLine_pos_list.size());
	//glMultiDrawArrays(GL_LINES,)

	glDisableClientState(GL_VERTEX_ARRAY);
	glPopAttrib();
	if (using_smooth)
		glDisable(GL_LINE_SMOOTH);
	if (using_stipple)
		glDisable(GL_LINE_STIPPLE);

	dLine_shader.UnuseShader();
}

void DebugLine::SetDLineShader()
{
	dLine_shader = RenderShader("LineShader");
	dLine_shader.InitShader = [&] {
		dLine_shader.UseShader();
		dLine_shader.SetValue("blen", 0.5f);
		dLine_shader.SetValue("U_color", 1.0f, 1.0f, 1.0f);
		dLine_shader.SetValue("ID_color", id_color);
		dLine_shader.SetValue("RAND_color", id_color_rand);
		dLine_shader.UnuseShader();
	};
}
