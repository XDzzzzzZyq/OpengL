#pragma once

#include "support.h"

#include "Mesh.h"
#include "Camera.h"
#include "Light.h"
#include "Environment.h"

#include "Spirit.h"

#include "GL/glew.h"
#include <vector>
#include <unordered_map>


class Renderer
{
private:
	std::unordered_map<int, Light*> emptyLight;
public:
	Renderer();
	~Renderer();

	//state
	bool is_light_changed = false;

	std::unordered_map<int, Camera*>		   cam_list;
	std::unordered_map<int, Mesh* >		      mesh_list;
	std::unordered_map<int, Light*>			 light_list; //num | pointer
	std::unordered_map<int, Environment*>	 envir_list;
	std::unordered_map<int, Spirit*>        spirit_list;

	void UseCamera(Camera* camera);
	void UseCamera(const int& cam_id);
	Camera* GetActiveCamera();

	void UseMesh(Mesh* mesh);

	void UseLight(Light* light);

	void UseEnvironment(Environment* envir);
	void UseEnvironment(const int& envir_id);

	void FrameClean() const;
	void Render();
};

