#include "DebugLine.h"

DebugLine::DebugLine(const glm::vec3& start, const glm::vec3& end)
{
	LOOP(3) {
		VertData.push_back(start[i]);
	}
	LOOP(3) {
		VertData.push_back(end[i]);
	}


	SetDLineShader();

	o_name = "Debug Line." + std::to_string(GetObjectID());
	multiLine = false;
}

DebugLine::DebugLine(const std::vector<std::vector<float>>& vertices)
{
	for (const auto& vert : vertices) {
		LOOP(3) {
			VertData.push_back(vert[i]);
		}
	}
	multiLine = true;
	vert_count = vertices.size();
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

void DebugLine::RenderDline(const glm::mat4& cam_Trans, const glm::mat4& cam_projec)
{
	dLine_shader.UseShader();

	dLine_shader.SetValue("U_cam_trans", cam_Trans);
	dLine_shader.SetValue("U_ProjectM", cam_projec);
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
	if (multiLine)
	{
		glVertexPointer(3, GL_FLOAT, 0, VertData.data());
		glDrawArrays(GL_LINES, 0, vert_count);
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, 0, VertData.data());
		glDrawArrays(GL_LINES, 0, 2);
	}


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
