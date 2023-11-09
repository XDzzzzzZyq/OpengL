#include "MeshData.h"

std::string MeshData::obj_file_root = "res/obj/";

#include <fstream>
#include <sstream>

inline MeshData::Face Split(std::string_view in) {
	std::string out[3] = { "","","" };
	int count = 0;

	for (int i = 0; i < in.length(); i++)
	{
		if (in[i] == '/') {
			count++;
			continue;
		}
		else {
			out[count] += in[i];
		}
	}
	MeshData::Face res;
	res.pos = 3 * std::stoi(out[0]) - 3;
	res.uv = 2 * std::stoi(out[1]) - 2;
	res.norm = 3 * std::stoi(out[2]) - 3;
	return res;
}

inline MeshData::Reading ReadObj(const std::string& path, bool is_smooth = true) {
	Timer timer("Load OBJ");
	MeshData::Reading result;

	std::vector<std::vector<int>> vertIndex;

	std::vector<float> tempdata[4];

	std::fstream obj;
	obj.open(path);
	std::string a = "";
	std::string last = "";

	MeshData::Face face;
	bool is_face = true;

	int vert_count = 0;

	//timer.Tick();
	//Timer timer2;
	while (!obj.eof())
	{
		getline(obj, a);
		//if (last != a.substr(0, 2)) {
		last = a.substr(0, 2);
		std::istringstream str(a);
		if (last == "v ")
		{
			while (str >> last)
			{
				if (last == "v")continue;
				tempdata[0].emplace_back(atof(last.c_str()));
			}
			result.count[0]++;

		}
		else if (last == "vt")
		{
			while (str >> last)
			{
				if (last == "vt")continue;
				tempdata[1].emplace_back(atof(last.c_str()));

			}
			result.count[1]++;
		}
		else if (last == "vn")
		{
			while (str >> last)
			{
				if (last == "vn")continue;
				tempdata[2].emplace_back(atof(last.c_str()));

			}
			result.count[2]++;
		}
		else if (last == "f ")
		{
			if (is_face)
			{
				//init
				is_face = false;
				vertIndex.resize(result.count[0] * 4);
			}
			while (str >> last)
			{
				if (last == "f")continue;

				face.copy(Split(last));

				result.data_array.emplace_back(tempdata[0][face.pos + 0]);
				result.data_array.emplace_back(tempdata[0][face.pos + 1]);
				result.data_array.emplace_back(tempdata[0][face.pos + 2]);

				result.data_array.emplace_back(tempdata[1][face.uv + 0]);
				result.data_array.emplace_back(tempdata[1][face.uv + 1]);

				result.data_array.emplace_back(tempdata[2][face.norm + 0]);
				result.data_array.emplace_back(tempdata[2][face.norm + 1]);
				result.data_array.emplace_back(tempdata[2][face.norm + 2]);

				result.data_array.emplace_back(0.0f);
				result.data_array.emplace_back(0.0f);
				result.data_array.emplace_back(0.0f);

				vertIndex[(face.pos) / 3].push_back(result.count[3] * 3 + vert_count);

				vert_count++;
				if (vert_count >= 3)
					vert_count = 0;
			}
			result.count[3]++;
		}
		else if (last == "o ") {
			while (str >> last)
			{
				if (last == "o")continue;
				result.name += last.c_str();
			}
		}
	}


	//timer.Tick();
	//Timer timer3;
	if (is_smooth)
	{
		LOOP(vertIndex.size()) {

			if (vertIndex[i] == std::vector<int>{})
				break;

			float SMX = 0.0f, SMY = 0.0f, SMZ = 0.0f;
			for (int j = 0; j < vertIndex[i].size(); j++)
			{
				SMX += result.data_array[vertIndex[i][j] * 11 + 5] / vertIndex[i].size();
				SMY += result.data_array[vertIndex[i][j] * 11 + 6] / vertIndex[i].size();
				SMZ += result.data_array[vertIndex[i][j] * 11 + 7] / vertIndex[i].size();
			}
			for (int j = 0; j < vertIndex[i].size(); j++)
			{
				result.data_array[vertIndex[i][j] * 11 + 8] = SMX;
				result.data_array[vertIndex[i][j] * 11 + 9] = SMY;
				result.data_array[vertIndex[i][j] * 11 + 10] = SMZ;

			}
		}

	}
	else {
		LOOP(vertIndex.size()) {

			if (vertIndex[i] == std::vector<int>{})
				break;
			for (int j = 0; j < vertIndex[i].size(); j++)
			{
				result.data_array[vertIndex[i][j] * 11 + 8] = result.data_array[vertIndex[i][j] * 11 + 5];
				result.data_array[vertIndex[i][j] * 11 + 9] = result.data_array[vertIndex[i][j] * 11 + 6];
				result.data_array[vertIndex[i][j] * 11 + 10] = result.data_array[vertIndex[i][j] * 11 + 7];
			}


		}
	}
	//timer.Tick();
	for (int i = 0; i < result.count[0]; i++)
	{
		result.center.x += tempdata[0][3 * i + 0] / (result.count[0] + 1);
		result.center.y += tempdata[0][3 * i + 1] / (result.count[0] + 1);
		result.center.z += tempdata[0][3 * i + 2] / (result.count[0] + 1);
	}

	return result;
}


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

	glDrawElements(GL_TRIANGLES, me_index.count(), GL_UNSIGNED_INT, NULL);

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
	return me_read.center;
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