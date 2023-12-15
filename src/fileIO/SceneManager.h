#pragma once

#include "Mesh.h"
#include "Camera.h"
#include "Light.h"
#include "PolygonLight.h"
#include "Environment.h"
#include "DebugLine.h"
#include "DebugPoints.h"
#include "PostProcessing.h"

class SceneResource
{
public:

	SceneResource();
	~SceneResource();

public:

	template<class Object>
	using Resource = std::shared_ptr<Object>;

	template<class Object>
	using ResPool = std::unordered_map<int, Resource<Object>>;

	template<class Object>
	using ResList = std::vector<Resource<Object>>;

public:
	
	enum SceneModifStatus
	{
		NoChanges			= 0,
		ObjectTransChanged	= 1 << 0,
		LightChanged		= 1 << 1,
		ShaderChanged		= 1 << 2,
		SDFChanged			= 1 << 3,
		CameraChanged		= 1 << 4,

		SceneChanged = ObjectTransChanged | LightChanged | ShaderChanged | CameraChanged
	};

	void UpdateSceneStatus(int tar, bool mask);
	void SetSceneStatus(int tar, bool value);
	bool CheckStatus(SceneModifStatus tar);
	void _debugStatus();

private:
	SceneModifStatus status = SceneModifStatus::SceneChanged;
public:

	ResPool<GameObject>			obj_list;

	ResPool<Camera>				cam_list;
	ResPool<Mesh>				mesh_list;
	ResPool<Light>				light_list;
	ResPool<PolygonLight>		poly_light_list;
	ResPool<Environment>		envir_list;
	ResPool<Sprite>				sprite_list;
	ResPool<DebugLine>			dLine_list;
	ResPool<DebugPoints>		dPoints_list;
	ResList<PostProcessing>		pps_list;

public:

	void UseCamera(Resource<Camera> camera);
	void UseMesh(Resource<Mesh> mesh);
	void UseLight(Resource<Light> light);
	void UsePolygonLight(Resource<PolygonLight> polyLight);
	void UseEnvironment(Resource<Environment> envir);
	void UseDebugLine(Resource<DebugLine> dline);
	void UseDebugPoints(Resource<DebugPoints> dpoints);
	void UsePostProcessing(Resource<PostProcessing> pps);

	std::shared_ptr<Camera> GetActiveCamera();
	std::shared_ptr<Environment> GetActiveEnvironment();
	std::shared_ptr<PostProcessing> GetPPS(int _tar);

public:

	void UpdateObjTransforms();

	void ResetStatus();

};

class SceneManager
{
private: 

	static std::unordered_map<std::string, std::shared_ptr<SceneResource>> sce_configs;

public:                        

	static std::shared_ptr<SceneResource> SceneConfig1(std::string _name="scene1");
	static std::shared_ptr<SceneResource> SceneConfig2(std::string _name="cornellbox");
	static std::shared_ptr<SceneResource> SceneConfig3(std::string _name="SDF test");

	static std::shared_ptr<SceneResource> GetSceneConfig(std::string _name);
};

