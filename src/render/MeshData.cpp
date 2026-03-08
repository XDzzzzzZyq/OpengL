#include "MeshData.h"
#include "AssetManager.h"
#include "MeshIO.h"
#include "macros.h"
#include "structs.h"
#include "operator.h"

std::string MeshData::obj_file_root = "res/obj/";

#include <algorithm>
#include <execution>


MeshData::MeshData()
{}

void MeshData::LoadObj(const std::string& path)
{
	std::string path_ = path.find(MeshData::obj_file_root) == std::string::npos ? MeshData::obj_file_root + path : path;
	me_read = MeshIO::Read(path_);

	//std::cout << VertData[100] << std::endl;
	me_vertBuffer = VertexBuffer(me_read.data_array);

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

	glDrawElements(GL_TRIANGLES, me_index.Count(), GL_UNSIGNED_INT, NULL);

	me_index.Unbind();
	me_vertArry.Unbind();
}

void MeshData::BindVBO(GLuint slot) const
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, me_vertBuffer.GetID());
}

glm::vec3 MeshData::GetMeshCenter() const
{
	return me_read.center;
}

std::string MeshData::GetMeshName() const
{
	return me_read.name;
}



/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

MeshLib::MeshResource MeshLib::Square = nullptr;

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
	return AssetManager::Load<MeshData>(path, [path]() {
		auto mesh = std::make_shared<MeshData>();
		mesh->LoadObj(path);
		return mesh;
	});
}

void MeshLib::ToGeoCenter(MeshResource _tar)
{
	_tar->me_vertBuffer.Bind();
	GLfloat* pData = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

	if (pData == nullptr)
		return;

	for (int i = 0; i < _tar->me_read.data_array.size(); i += _tar->me_vertArry.GetStride()) {
		LOOP_N(3, j)
			pData[i + j] -= _tar->me_read.center[j];
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	_tar->me_read.center = glm::vec3{ 0 };
	DEBUG("Done");
}
