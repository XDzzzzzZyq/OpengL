#include "Spirit.h"

Spirit::Spirit()
{
	
	//std::cout << VertData[100] << std::endl;
	r_vertBuffer = VertexBuffer(VertData.data(), VertData.size() * sizeof(float));

	BufferLayout layout;
	layout.Push<float>(3); //3D position
	layout.Push<float>(2); //UV

	r_vertArry.AddBuffer(r_vertBuffer, layout);
	/*o_verts.Unbind();*/



	std::vector<GLuint>* indexArray = new std::vector<GLuint>{ 0,2,1,1,2,3 };
	GLuint* index = indexArray->data();

	r_index = IndexBuffer(index, indexArray->size() * sizeof(GLuint));
	SetSpiritShader();

	o_name = "Spirit." + std::to_string(GetObjectID());
	
}

Spirit::~Spirit()
{
	DeleteSpirit();
}

void Spirit::RenderSpirit(const std::vector<float>& light_data, const Camera& cam)
{
	r_vertArry.Bind();
	r_index.Bind();
	r_shader.UseShader();
	r_tex.Bind(r_tex.Tex_slot);

	//transform settings

	//std::cout << o_Transform;
	r_shader.SetValue("Light_data",6 ,light_data.data());

	if(cam.is_invUniform_changed)
		r_shader.SetValue("U_cam_trans", cam.o_InvTransform);

	if(cam.is_frustum_changed)
		r_shader.SetValue("U_ProjectM", cam.cam_frustum);

	r_shader.SetValue("SpiritOpacity", spirit_opacity);
	r_shader.SetValue("U_Scale", SPIRIT_SIZE);
	//light settings



	glDrawElements(GL_TRIANGLES, r_index.count(), GL_UNSIGNED_INT, nullptr);


	//o_Transform = glm::mat4(1.0f);
	r_index.Unbind();
	r_shader.UnuseShader();
	r_vertArry.Unbind();
	r_tex.Unbind();


}

void Spirit::SetSpiritShader()
{
	r_shader = Shaders("res/shaders/SpiritShader.shader");

}

void Spirit::SetTex()
{
	r_tex = Texture(ParsePath(), PNG_TEXTURE, GL_REPEAT);
	r_tex.Tex_slot = PNG_TEXTURE;
	//r_tex.Bind(spr_type);

	r_shader.UseShader();
	//o_shader.SetValue("blen", 0.5f);
	r_shader.SetValue("U_color", 1.0f, 0.0f, 1.0f, 1.0f);
	r_shader.SetValue("U_Texture", spr_type);
	r_shader.UnuseShader();
}



void Spirit::DeleteSpirit()
{
	r_tex.Unbind();
	r_shader.UnuseShader();
	r_index.Unbind();
	r_vertArry.Unbind();
	r_vertBuffer.Unbind();

	r_tex.DelTexture();
	r_shader.DelShad();
	r_index.DelIndBuff();
	r_vertBuffer.DelVertBuff();
	r_vertArry.DelVertArr();

}

std::string Spirit::ParsePath() const
{
	switch (spr_type)
	{
	case NONE_SPIRIT:
		//DEBUG(111111)
		return "res/tex/spirit/BAKED.png";
	case LIGHT_SPIRIT:
		//DEBUG(222222)
		return "res/tex/spirit/light.png";
	case CAM_SPIRIT:
		return "res/tex/spirit/BAKED.png";
	case ENVIRN_SPIRIT:
		return "res/tex/spirit/BAKED.png";

	}
	return "???";
	DEBUG("???")
}
