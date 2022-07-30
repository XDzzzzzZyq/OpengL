#include "DebugLine.h"

DebugLine::DebugLine(const glm::vec3& start, const glm::vec3& end)
{
	LOOP(3) {
		dLine_pos_list.push_back(start[i]);
	}
	LOOP(3) {
		dLine_pos_list.push_back(end[i]);
	}


	SetDLineShader();

	o_name = "Debug Line." + std::to_string(GetObjectID());
	multiLine = false;
}

DebugLine::DebugLine(const std::vector<glm::vec3>& vertices)
{
	for (const auto& vert : vertices) {
		LOOP(3) {
			dLine_pos_list.push_back(vert[i]);
		}
	}
	multiLine = true;
	vert_count = vertices.size();

	SetDLineShader();

	o_name = "Debug Line." + std::to_string(GetObjectID());
}

DebugLine::DebugLine()
{
	SetDLineShader();

	o_name = "Debug Line." + std::to_string(GetObjectID());
}

DebugLine::~DebugLine()
{
	DeleteDLine();
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
	dLine_pos_list.push_back(point[0]);
	dLine_pos_list.push_back(point[1]);
	dLine_pos_list.push_back(point[2]);
}

void DebugLine::PushDebugLine(float x, float y, float z)
{
	dLine_pos_list.push_back(x);
	dLine_pos_list.push_back(y);
	dLine_pos_list.push_back(z);
}

void DebugLine::PushDebugLines(const std::vector<glm::vec3>& points)
{
	dLine_pos_list.reserve(dLine_pos_list.size() + points.size() * 3);

	LOOP(points.size()) {

		dLine_pos_list.emplace_back(points[i][0]);
		dLine_pos_list.emplace_back(points[i][1]);
		dLine_pos_list.emplace_back(points[i][2]);

	}
}

void DebugLine::RenderDdbugLine(const Camera& camera)
{
	if (dLine_pos_list.size() < 2)return;

	dLine_shader.UseShader();

	if(camera.is_invUniform_changed)
		dLine_shader.SetValue("U_cam_trans",camera.o_InvTransform);

	if(camera.is_frustum_changed)
		dLine_shader.SetValue("U_ProjectM", camera.cam_frustum);

	if(is_Uniform_changed)
		dLine_shader.SetValue("U_Trans", o_Transform);

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
	glDrawArrays(GL_LINE_STRIP, 0, dLine_pos_list.size()/3);
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
	dLine_shader = Shaders("res/shaders/LineShader.shader");

	dLine_shader.UseShader();
	//dLine_shader.SetValue("blen", 0.5f);
	dLine_shader.SetValue("U_color", 1.0f, 1.0f, 1.0f);
}

void DebugLine::DeleteDLine()
{
	dLine_shader.UnuseShader();
	dLine_index.Unbind();
	dLine_vertArry.Unbind();
	dLine_vertBuffer.Unbind();

	dLine_shader.DelShad();
	dLine_index.DelIndBuff();
	dLine_vertBuffer.DelVertBuff();
	dLine_vertArry.DelVertArr();
}
