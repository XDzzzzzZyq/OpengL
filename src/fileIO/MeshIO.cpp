#include "MeshIO.h"

#include "macros.h"
#include "structs.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <execution>

namespace
{

// Parses a face token of the form "v/vt/vn" and returns byte-offset indices
// into the flat position, UV, and normal arrays.
MeshData::VertexIdx Split(std::string_view in)
{
	std::string out[3] = { "", "", "" };
	int count = 0;

	for (int i = 0; i < (int)in.length(); i++)
	{
		if (in[i] == '/') {
			count++;
			continue;
		}
		out[count] += in[i];
	}

	MeshData::VertexIdx res;
	res.pos  = 3 * std::stoi(out[0]) - 3;
	res.uv   = 2 * std::stoi(out[1]) - 2;
	res.norm = 3 * std::stoi(out[2]) - 3;
	return res;
}

} // anonymous namespace

namespace MeshIO
{

MeshData::ByteArray Read(const std::string& path, bool smooth /*= true*/)
{
	Timer timer("Load OBJ");
	MeshData::ByteArray result;

	std::vector<std::vector<int>> vertIndex;
	std::vector<float> tempdata[4];

	std::fstream obj;
	obj.open(path);
	if (!obj.is_open())
	{
		std::cout << "MeshIO::Read failed to open: " << path << "\n";
		return result;
	}

	std::string a;
	std::string last;
	bool is_face = true;
	int vert_count = 0;

	while (!obj.eof())
	{
		getline(obj, a);
		last = a.substr(0, 2);
		std::istringstream str(a);

		if (last == "v ")
		{
			while (str >> last)
			{
				if (last == "v") continue;
				tempdata[0].emplace_back(float(atof(last.c_str())));
			}
			result.count[0]++;
		}
		else if (last == "vt")
		{
			while (str >> last)
			{
				if (last == "vt") continue;
				tempdata[1].emplace_back(float(atof(last.c_str())));
			}
			result.count[1]++;
		}
		else if (last == "vn")
		{
			while (str >> last)
			{
				if (last == "vn") continue;
				tempdata[2].emplace_back(float(atof(last.c_str())));
			}
			result.count[2]++;
		}
		else if (last == "f ")
		{
			if (is_face)
			{
				is_face = false;
				vertIndex.resize(result.count[0] * 4);
			}
			while (str >> last)
			{
				if (last == "f") continue;

				MeshData::VertexIdx face = Split(last);

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
		else if (last == "o ")
		{
			while (str >> last)
			{
				if (last == "o") continue;
				result.name += last.c_str();
			}
		}
	}

	if (smooth)
	{
		std::for_each(std::execution::par_unseq, vertIndex.begin(), vertIndex.end(),
			[&](std::vector<int>& indices)
			{
				if (indices.empty()) return;

				float SMX = 0.0f, SMY = 0.0f, SMZ = 0.0f;
				for (int j = 0; j < (int)indices.size(); j++)
				{
					SMX += result.data_array[indices[j] * 11 + 5] / indices.size();
					SMY += result.data_array[indices[j] * 11 + 6] / indices.size();
					SMZ += result.data_array[indices[j] * 11 + 7] / indices.size();
				}
				for (int j = 0; j < (int)indices.size(); j++)
				{
					result.data_array[indices[j] * 11 + 8]  = SMX;
					result.data_array[indices[j] * 11 + 9]  = SMY;
					result.data_array[indices[j] * 11 + 10] = SMZ;
				}
			});
	}
	else
	{
		std::for_each(std::execution::par_unseq, vertIndex.begin(), vertIndex.end(),
			[&](std::vector<int>& indices)
			{
				if (indices.empty()) return;
				for (int j = 0; j < (int)indices.size(); j++)
				{
					result.data_array[indices[j] * 11 + 8]  = result.data_array[indices[j] * 11 + 5];
					result.data_array[indices[j] * 11 + 9]  = result.data_array[indices[j] * 11 + 6];
					result.data_array[indices[j] * 11 + 10] = result.data_array[indices[j] * 11 + 7];
				}
			});
	}

	for (int i = 0; i < result.count[0]; i++)
	{
		glm::vec3 pos = { tempdata[0][3 * i + 0], tempdata[0][3 * i + 1], tempdata[0][3 * i + 2] };
		result.center += pos;
	}
	result.center /= result.count[0];

	return result;
}

} // namespace MeshIO
