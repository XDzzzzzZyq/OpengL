#pragma once

#include "VertexArray.h"
#include "buffer/IndexBuffer.h"

#include "Camera.h"

#include <numeric>

class MeshData {

public:

	struct ByteArray
	{
		std::vector<float> data_array;
		glm::vec3 center = { 0,0,0 };
		int count[4] = { 0,0,0,0 };

		std::string name = "";
	};

	struct VertexIdx
	{
		int pos, uv, norm, smo_norm;
		void _debug() {
#ifdef _DEBUG
			std::cout << pos << " " << uv << " " << norm << "\n";
#endif
		}
	};

	friend class MeshLib;

private:
	static std::string obj_file_root;
private:

	ByteArray me_read{};
	VertexArray me_vertArry{};
	VertexBuffer me_vertBuffer{};
	IndexBuffer me_index{};

public:

	MeshData();
	void LoadObj(const std::string& path);
	void RenderObjProxy() const;
	void BindVBO(GLuint slot) const;

public:

	glm::vec3 GetMeshCenter() const;
	std::string GetMeshName() const;

};



class MeshLib
{
private:
	using MeshResource = std::shared_ptr<MeshData>;
private:
	static std::unordered_map<std::string, MeshResource> mesh_list;
public:

	MeshLib();
	~MeshLib();

	static void MeshLibInit();

public:

	static MeshResource LoadMesh(const std::string path);

public:

	static MeshResource Square;

public:

	static void ToGeoCenter(MeshResource _tar);

};