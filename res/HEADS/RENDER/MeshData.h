#pragma once

#include "GL/glew.h"

#include "VertexArray.h"
#include "IndexBuffer.h"

#include "GameObject.h"
#include "Camera.h"

#include "Transform.h"
#include "support.h"
#include <numeric>
#include <optional>


class MeshData {

private:
	static std::string obj_file_root;
private:

	Reading me_read;
	VertexArray me_vertArry;
	VertexBuffer me_vertBuffer;
	IndexBuffer me_index;

public:

	MeshData();
	void LoadObj(const std::string& path);
	void RenderObjProxy() const;

	void Delete();

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

};