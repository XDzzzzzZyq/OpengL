#pragma once

#include "VertexArray.h"
#include "buffer/IndexBuffer.h"

#include "Camera.h"

#include <numeric>
#include <optional>

class MeshData {

public:

	struct Reading
	{
		std::vector<float> data_array;
		glm::vec3 center = { 0,0,0 };
		int count[4] = { 0,0,0,0 };

		std::string name = "";
	};

	struct Face
	{
		int pos, uv, norm, smo_norm;
		void copy(const Face& inp) {
			pos = inp.pos;
			uv = inp.uv;
			norm = inp.norm;
			smo_norm = inp.smo_norm;
		}
		void print() {
			std::cout << pos << " " << uv << " " << norm << "\n";
		}
	};

	friend class MeshLib;

private:
	static std::string obj_file_root;
private:

	Reading me_read{};
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