#include "SceneManager.h"


std::shared_ptr<SceneResource> SceneManager::SceneConfig1()
{

	if (SceneManager::sce_configs.find("config 1") != SceneManager::sce_configs.end())
		return SceneManager::sce_configs["config 1"];

	std::shared_ptr<SceneResource> config1 = std::make_shared<SceneResource>();

	DEBUG("\n---------------CAMERA----------------")
		std::shared_ptr<Camera> camera = std::make_shared<Camera>(10.0f, 10.0f, 70, 0.1f, 300.0f);
	camera->SetPos({ 0.0f, 20.0f, 0.0f });
	camera->SetRot({ 90, 0, 180 });
	camera->ApplyTransform();
	camera->GetInvTransform();
	config1->UseCamera(camera);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> go1 = std::make_shared<Mesh>("monkey2.obj");
	go1->SetObjShader("testS", "Rasterization");
	go1->SetTex(MAT_ALBEDO, "avatar2.png");
	go1->SetCenter();
	go1->SetScale(glm::vec3(0.3));
	config1->UseMesh(go1);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> go2 = std::make_shared<Mesh>("torus.obj");
	go2->SetObjShader("testS", "Rasterization");
	go2->SetTex(MAT_ALBEDO, "avatar1.png");
	go2->SetCenter();
	go2->SetPos({ 8, 0, 0 });
	go2->SetScale(glm::vec3(1.5f));
	go1->SetParent(go2->GetTransformPtr());
	config1->UseMesh(go2);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> go3 = std::make_shared<Mesh>("UVsphere.obj");
	go3->SetObjShader("testS", "Rasterization");
	go3->SetTex(MAT_ROUGH, "avatar1.png");
	go3->SetMatColor(MAT_ALBEDO, glm::vec3(0.1));
	go3->SetMatColor(MAT_METAL, 0.1);
	go3->SetPos({ -8,0,0 });
	go3->SetScale({ 3,3,3 });
	config1->UseMesh(go3);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> go4 = std::make_shared<Mesh>("plane.obj");
	go4->SetObjShader("testS", "Rasterization");
	go4->SetTex(MAT_ALBEDO, "rough.png");
	go4->SetMatColor(MAT_ROUGH, 0.9);
	;	go4->SetPos({ 0,-7,0 });
	go4->SetScale({ 2,2,2 });
	go4->SetRot({ 0,90,90 });
	config1->UseMesh(go4);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> go5 = std::make_shared<Mesh>("square.obj");
	go5->SetObjShader("testS", "Rasterization");
	go5->SetShadow(false);
	config1->UseMesh(go5);

	DEBUG("\n---------------LIGHT----------------")
		std::shared_ptr<Light> pointLight1 = std::make_shared<Light>(POINTLIGHT, 1.0f, glm::vec3(1.0f));
	pointLight1->SetPos({ 2.0f, 2.0f, 2.0f });
	pointLight1->ApplyTransform();
	//config1->UseLight(pointLight1);

	DEBUG("\n---------------LIGHT----------------")
		std::shared_ptr<Light> pointLight2 = std::make_shared<Light>(POINTLIGHT, 1.0f, glm::vec3(1.0f));
	pointLight2->SetRadius(2);
	//config1->UseLight(pointLight2);

	DEBUG("\n---------------LIGHT----------------")
		std::shared_ptr<Light> sunLight1 = std::make_shared<Light>(SUNLIGHT, 1.0f, glm::vec3(1.0f));
	sunLight1->SetRot(glm::vec3(0, 90, 0));
	sunLight1->SetPos(glm::vec3(2));
	sunLight1->SetPower(20);
	//config1->UseLight(sunLight1);

	DEBUG("\n---------------LIGHT----------------")
		std::shared_ptr<Light> spotLight1 = std::make_shared<Light>(SPOTLIGHT, 1.0f, glm::vec3(1.0f));
	spotLight1->SetRot(glm::vec3(45));
	spotLight1->SetPos({ 6,-6,0 });
	spotLight1->SetCutoff(60);
	spotLight1->SetOuterCutoff(80);
	spotLight1->SetPower(50);
	//config1->UseLight(spotLight1);

	DEBUG("\n-------------AREA LIGHT-------------")
		std::shared_ptr<Light> areaLight1 = std::make_shared<Light>(AREALIGHT, 1.0f, glm::vec3(2));
	areaLight1->SetRot({ 0, 90, 45 });
	areaLight1->SetPower(50);
	areaLight1->SetRatio(1.5f);
	go5->SetParent(areaLight1->GetTransformPtr(), false);
	config1->UseLight(areaLight1);

	DEBUG("\n------------POLYGON LIGHT-------------")
		std::vector<float> plVertData = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			2.0f, 2.0f,
			0.0f, 2.0f
	};
	std::shared_ptr<PolygonLight> polyLight1 = std::make_shared<PolygonLight>(plVertData, glm::vec3(1.0f, 0.0f, 0.0f), 20.0f);
	polyLight1->SetPos({ 0.0f, -3.0f, -4.0f });
	polyLight1->SetRot(glm::vec3(-30.0f, 0.0f, 0.0f));
	//config1->UsePolygonLight(polyLight1);

	DEBUG("\n---------------LINE----------------")
		std::shared_ptr<DebugLine> line = std::make_shared<DebugLine>();
	line->PushDebugLine(5, 5, 5);
	config1->UseDebugLine(line);

	DEBUG("\n---------------LINE----------------")
		std::shared_ptr<DebugLine> line2 = std::make_shared<DebugLine>();
	line2->PushDebugLines({ {0,0,0} , {0,0,1} });
	line2->SetParent(spotLight1->GetTransformPtr(), false);
	config1->UseDebugLine(line2);

	DEBUG("\n---------------ENVIR----------------")
		std::shared_ptr<Environment> environment = std::make_shared<Environment>("hdr/room.hdr");
	environment->SetPos(glm::vec3(0.0f, 7.0f, 7.0f));
	config1->UseEnvironment(environment);

	DEBUG("\n---------------POINT----------------")
		std::shared_ptr<DebugPoints> points = std::make_shared<DebugPoints>();
	points->PushDebugPoint(5, 5, 5);
	config1->UseDebugPoints(points);

	DEBUG("\n---------------POSTPRCS----------------")
		std::shared_ptr<PostProcessing> pps1 = std::make_shared<PostProcessing>("PBR");
	pps1->AddBinding("U_color", BUFFER_TEXTURE + COMBINE_FB);
	pps1->AddBinding("U_pos", BUFFER_TEXTURE + POS_FB);
	pps1->AddBinding("U_normal", BUFFER_TEXTURE + NORMAL_FB);
	pps1->AddBinding("U_albedo", BUFFER_TEXTURE + ALBEDO_FB);
	pps1->AddBinding("U_mrse", BUFFER_TEXTURE + MRSE_FB);
	pps1->AddBinding("U_emission", BUFFER_TEXTURE + EMIS_COL_FB);
	pps1->AddBinding("U_alpha", BUFFER_TEXTURE + MASK_FB);
	pps1->AddBinding("Envir_Texture_diff", IBL_TEXTURE);
	pps1->AddBinding("Envir_Texture_spec", IBL_TEXTURE + 1);
	pps1->AddBinding("LUT", PNG_TEXTURE);
	pps1->AddBinding("LTC1", 13);	// Pass LTC matrix lookup tables for poly & area lights
	pps1->AddBinding("LTC2", 14);	// Texture slot 0-12 are currently occupied, so 13 and 14 are used for these two tables
	config1->UsePostProcessing(pps1);

	DEBUG("\n---------------POSTPRCS----------------")
		std::shared_ptr<PostProcessing> pps2 = std::make_shared<PostProcessing>("Post_Visual");
	pps2->AddBinding("U_combine", BUFFER_TEXTURE + COMBINE_FB);
	pps2->AddBinding("U_select", BUFFER_TEXTURE + MASK_FB);
	config1->UsePostProcessing(pps2);

	SceneManager::sce_configs["config 1"] = config1;
	return config1;
}

std::shared_ptr<SceneResource> SceneManager::SceneConfig2()
{
	return nullptr;
}