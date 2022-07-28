#include "Mesh.h"
#include "support.h"
#include <numeric>


Mesh::Mesh(const char* path)
{

	read = ReadObj(path,true);

	o_name = read.name;

	std::cout << "\n";
	VertData = read.vertex.data();
	center = stdVec3_vec3(read.center);
	//std::cout << VertData[100] << std::endl;
	o_vertBuffer = VertexBuffer(VertData, read.vertex.size() * sizeof(float));

	BufferLayout layout;
	layout.Push<float>(3); //3D position
	layout.Push<float>(2); //UV
	layout.Push<float>(3); //Normal
	layout.Push<float>(3); //Smooth Normal

	o_vertArry.AddBuffer(o_vertBuffer, layout);
	/*o_verts.Unbind();*/

	const int size = read.count[3] * 3;

	std::vector<GLuint>* indexArray = new std::vector<GLuint>(size);
	std::iota(indexArray->begin(), indexArray->end(), 0);

	GLuint* index = indexArray->data();

	o_index = IndexBuffer(index, indexArray->size() * sizeof(GLuint));
}

Mesh::Mesh()
{
	DEBUG("mesh c")
}

Mesh::~Mesh()
{
	DeleteObj();
}

void Mesh::RenderObj(const Camera& cam, const std::unordered_map<int, Light*>& light_list)
{
	o_vertArry.Bind();
	o_index.Bind();
	o_shader.UseShader();
	o_tex.Bind(o_tex.Tex_slot);


	//std::cout << o_Transform;
	if(is_Uniform_changed)
		o_shader.SetValue("U_obj_trans", o_Transform);

	if (cam.is_invUniform_changed)
		o_shader.SetValue("U_cam_trans", cam.o_InvTransform);

	if(cam.is_frustum_changed)
		o_shader.SetValue("U_ProjectM", cam.cam_frustum);

	if(cam.is_Uniform_changed || cam.is_frustum_changed)
		o_shader.SetValue("Scene_data",8 , cam.cam_floatData.data());

	if (!light_list.empty())
	{
		LightFloatArray lightdata(light_list);
		o_shader.SetValue("L_point", lightdata.point_count * (5 + 3) + 1, lightdata.point.data());
		//o_shader.SetValue("L_sun", lightdata.sun_count * (5 + 6) + 1, lightdata.sun.data());
		//o_shader.SetValue("L_spot", lightdata.spot_count * (5 + 6 + 2) + 1, lightdata.spot.data());
	}

	//light settings


	glDrawElements(GL_TRIANGLES, o_index.count(), GL_UNSIGNED_INT, nullptr);

	//o_Transform = glm::mat4(1.0f);

#if 0
	o_index.Unbind();
	o_shader.UnuseShader();
	o_vertArry.Unbind();
	o_tex.Unbind();
#endif


}

void Mesh::SetObjShader(std::string path)
{
	o_shader = Shaders(path);
	//std::cout << "Shader:" << (glGetError()) << "\n";
	o_shader.UseShader();
	//matrix = glm::translate(matrix, o_position);
	o_shader.SetValue("U_ProjectM", o_Transform);
	//o_shader.SetVal1f("is", 0.5f);
	//std::cout<<"aaa\n";
}

void Mesh::SetTex(std::string path, TextureType slot)
{
	o_tex = Texture(path, IMAGE_TEXTURE ,GL_REPEAT);
	o_tex.Bind(slot);
	o_tex.Tex_slot = slot;

	o_shader.UseShader();
	//o_shader.SetValue("blen", 0.5f);
	o_shader.SetValue("U_color", 1.0f, 0.0f, 1.0f, 1.0f);
	o_shader.SetValue("U_Texture", slot);
	o_shader.SetValue("Envir_Texture", HDR_TEXTURE);

	o_shader.UnuseShader();
	o_tex.Unbind();
}

void Mesh::SetCenter()
{
	SetPos(-center);
}

void Mesh::DeleteObj()
{
	o_tex.Unbind();
	o_shader.UnuseShader();
	o_index.Unbind();
	o_vertArry.Unbind();
	o_vertBuffer.Unbind();

	o_tex.DelTexture();
	o_shader.DelShad();
	o_index.DelIndBuff();
	o_vertBuffer.DelVertBuff();
	o_vertArry.DelVertArr();

}
