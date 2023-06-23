#include "MeshData.h"

std::string MeshData::obj_file_root = "res/obj/";

MeshData::MeshData()
{}

void MeshData::LoadObj(const std::string& path)
{
	std::string path_ = path.find(MeshData::obj_file_root) == std::string::npos ? MeshData::obj_file_root + path : path;
	me_read = ReadObj(path_);

	//std::cout << VertData[100] << std::endl;
	me_vertBuffer = VertexBuffer(me_read.data_array.data(), me_read.data_array.size() * sizeof(float));

	BufferLayout layout;
	layout.Push<float>(3); //3D position
	layout.Push<float>(2); //UV
	layout.Push<float>(3); //Normal
	layout.Push<float>(3); //Smooth Normal

	me_vertArry.AddBuffer(me_vertBuffer, layout);
	/*o_verts.Unbind();*/

	const int size = me_read.count[3] * 3;

	std::vector<GLuint> indexArray = std::vector<GLuint>(size);
	std::iota(indexArray.begin(), indexArray.end(), 0);

	me_index = IndexBuffer(indexArray.data(), indexArray.size() * sizeof(GLuint));
}

void MeshData::RenderObjProxy() const
{
	me_vertArry.Bind();
	me_index.Bind();

	glDrawElements(GL_TRIANGLES, me_index.count(), GL_UNSIGNED_INT, nullptr);

	me_index.Unbind();
	me_vertArry.Unbind();
}

void MeshData::Delete()
{
	me_index.Unbind();
	me_vertArry.Unbind();
	me_vertBuffer.Unbind();

	me_index.DelIndBuff();
	me_vertBuffer.DelVertBuff();
	me_vertArry.DelVertArr();
}

glm::vec3 MeshData::GetMeshCenter() const
{
	return stdVec3_vec3(me_read.center);
}

std::string MeshData::GetMeshName() const
{
	return me_read.name;
}



/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

std::unordered_map<std::string, MeshLib::MeshResource> MeshLib::mesh_list = {};

MeshLib::MeshLib()
{

}

MeshLib::~MeshLib()
{

}

void MeshLib::MeshLibInit()
{
	MeshLib::Square = LoadMesh("square.obj");
}

MeshLib::MeshResource MeshLib::LoadMesh(const std::string path)
{
	if (mesh_list.find(path) != mesh_list.end())
		return mesh_list[path];

	MeshLib::MeshResource mesh = std::make_shared<MeshData>();
	mesh->LoadObj(path);

	return mesh;
}

MeshLib::MeshResource MeshLib::Square = nullptr;