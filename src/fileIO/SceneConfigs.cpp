#include "SceneManager.h"
#include "operator.h"

std::shared_ptr<SceneResource> SceneManager::SceneConfig1(std::string _name/*="scene1"*/)
{

	if (SceneManager::sce_configs.find(_name) != SceneManager::sce_configs.end())
		return SceneManager::sce_configs[_name];

	std::shared_ptr<SceneResource> config1 = std::make_shared<SceneResource>();
	SceneManager::sce_configs[_name] = config1;

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
	go4->SetPos({ 0,-7,0 });
	go4->SetScale({ 2,2,2 });
	go4->SetRot({ 0,90,90 });
	config1->UseMesh(go4);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> go5 = std::make_shared<Mesh>("square.obj");
	go5->SetObjShader("testS", "Rasterization");
	go5->EnableShadow(false);
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

	return config1;
}

std::shared_ptr<SceneResource> SceneManager::SceneConfig2(std::string _name/*="cornellbox"*/)
{
	if (SceneManager::sce_configs.find(_name) != SceneManager::sce_configs.end())
		return SceneManager::sce_configs[_name];

	std::shared_ptr<SceneResource> config2 = std::make_shared<SceneResource>();
	SceneManager::sce_configs[_name] = config2;

	DEBUG("\n---------------CAMERA----------------")
		std::shared_ptr<Camera> camera = std::make_shared<Camera>(10.0f, 10.0f, 39.5978f, 0.1f, 300.0f);
	camera->SetPos({ -3.76f, 0.0f, 1.0f });
	camera->SetTarPos({ 0, 0, 1 });
	camera->SetRot({ 90, 0, -90 });
	camera->ApplyTransform();
	camera->GetInvTransform();
	config2->UseCamera(camera);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> monkey = std::make_shared<Mesh>("cornellbox/monkey.obj");
	monkey->SetObjShader("testS", "Rasterization");
	monkey->SetMatColor(MAT_ROUGH, 0.0f);
	monkey->SetMatColor(MAT_METAL, 1.0f);
	monkey->SetLowPoly("cornellbox/monkey_low.obj");
	config2->UseMesh(monkey);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> back_wall = std::make_shared<Mesh>("cornellbox/back_wall.obj");
	back_wall->SetObjShader("testS", "Rasterization");
	back_wall->SetMatColor(MAT_ALBEDO, glm::vec3(0.9));
	back_wall->SetMatColor(MAT_ROUGH, 0.90f);
	back_wall->SetMatColor(MAT_METAL, 0.05f);
	back_wall->is_closure = false;
	config2->UseMesh(back_wall);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> left_wall = std::make_shared<Mesh>("cornellbox/left_wall.obj");
	left_wall->SetObjShader("testS", "Rasterization");
	left_wall->SetMatColor(MAT_ALBEDO, glm::vec3(0.8, 0.22, 0.2));
	left_wall->SetMatColor(MAT_ROUGH, 0.90f);
	left_wall->SetMatColor(MAT_METAL, 0.05f);
	left_wall->is_closure = false;
	config2->UseMesh(left_wall);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> right_wall = std::make_shared<Mesh>("cornellbox/right_wall.obj");
	right_wall->SetObjShader("testS", "Rasterization");
	right_wall->SetMatColor(MAT_ALBEDO, glm::vec3(0.26, 0.8, 0.29));
	right_wall->SetMatColor(MAT_ROUGH, 0.90f);
	right_wall->SetMatColor(MAT_METAL, 0.05f);
	right_wall->is_closure = false;
	config2->UseMesh(right_wall);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> left_box = std::make_shared<Mesh>("cornellbox/left_box.obj");
	left_box->SetObjShader("testS", "Rasterization");
	left_box->SetMatColor(MAT_ALBEDO, glm::vec3(0.9));
	left_box->SetMatColor(MAT_ROUGH, 0.90f);
	left_box->SetMatColor(MAT_METAL, 0.05f);
	config2->UseMesh(left_box);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> right_box = std::make_shared<Mesh>("cornellbox/right_box.obj");
	right_box->SetObjShader("testS", "Rasterization");
	right_box->SetMatColor(MAT_ALBEDO, glm::vec3(0.9));
	right_box->SetMatColor(MAT_ROUGH, 0.90f);
	right_box->SetMatColor(MAT_METAL, 0.05f);
	config2->UseMesh(right_box);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> light = std::make_shared<Mesh>("cornellbox/light_up.obj");
	light->SetObjShader("testS", "Rasterization");
	light->SetMatColor(MAT_ALBEDO, glm::vec3(0));
	light->SetMatColor(MAT_ROUGH, 1.0);
	light->SetMatColor(MAT_METAL, 1.0);
	light->SetMatColor(MAT_EMIS_COL, glm::vec3(1));
	light->SetMatColor(MAT_EMIS_STR, 1.0);
	light->is_closure = false;
	light->EnableSDF(false);
	config2->UseMesh(light);

	DEBUG("\n----- ------SDF VISUAL--------------")
		std::shared_ptr<Mesh> go4 = std::make_shared<Mesh>("plane.obj");
	go4->EnableMaterial(false);
	go4->EnableSDF(false);
	go4->EnableShadow(false);
	go4->SetObjShader("testS", "SDF_Visual");
	go4->is_viewport = false;
	go4->SetPos({ 0,0,0 });
	go4->SetScale({ 2,2,2 });
	go4->SetRot({ 0,0,90 });
	config2->UseMesh(go4);

	DEBUG("\n-------------AREA LIGHT-------------")
		std::shared_ptr<Light> areaLight1 = std::make_shared<Light>(AREALIGHT);
	areaLight1->SetColor({ 1, 1, 0.7 });
	areaLight1->SetPos({ 0, 0, 1.95 });
	areaLight1->SetScale(glm::vec3(0.25));
	areaLight1->SetRot({ 180, 0, 0 });
	areaLight1->SetPower(15);
	DEBUG(light->o_position);
	light->SetParent(areaLight1->GetTransformPtr(), true);
	config2->UseLight(areaLight1);

	Light::point_blur_range = 0.005;

	DEBUG("\n---------------ENVIR----------------")
		std::shared_ptr<Environment> environment = std::make_shared<Environment>("hdr/room.png");
	environment->SetPos(glm::vec3(0.0f, 7.0f, 7.0f));
	config2->UseEnvironment(environment);

	DEBUG("\n---------------POSTPRCS----------------")
		std::shared_ptr<PostProcessing> pps1 = std::make_shared<PostProcessing>("PBR", COMPUTE_SHADER);
	pps1->pps_field.SetPos({ 5, 5, 5 });
	pps1->AddBinding("U_color", BUFFER_TEXTURE + COMBINE_FB);
	pps1->AddBinding("U_pos", BUFFER_TEXTURE + POS_FB);
	pps1->AddBinding("U_normal", BUFFER_TEXTURE + NORMAL_FB);
	pps1->AddBinding("U_albedo", BUFFER_TEXTURE + ALBEDO_FB);
	pps1->AddBinding("U_mrse", BUFFER_TEXTURE + MRSE_FB);
	pps1->AddBinding("U_emission", BUFFER_TEXTURE + EMIS_COL_FB);
	pps1->AddBinding("U_alpha", BUFFER_TEXTURE + MASK_FB);
	pps1->AddBinding("Envir_Texture_diff", IBL_TEXTURE);
	pps1->AddBinding("Envir_Texture_spec", IBL_TEXTURE + 1);
	pps1->AddBinding("LTC1", 13);	// Pass LTC matrix lookup tables for poly & area lights
	pps1->AddBinding("LTC2", 14);	// Texture slot 0-12 are currently occupied, so 13 and 14 are used for these two tables
	config2->UsePostProcessing(pps1);

	return config2;
}

std::shared_ptr<SceneResource> SceneManager::SceneConfig3(std::string _name/*="SDF test"*/)
{
	if (SceneManager::sce_configs.find(_name) != SceneManager::sce_configs.end())
		return SceneManager::sce_configs[_name];

	std::shared_ptr<SceneResource> config3 = std::make_shared<SceneResource>();
	SceneManager::sce_configs[_name] = config3;

	DEBUG("\n---------------CAMERA----------------")
		std::shared_ptr<Camera> camera = std::make_shared<Camera>(10.0f, 10.0f, 70, 0.1f, 300.0f);
	camera->SetPos({ 10, 0, 10 });
	camera->SetRot({ 45, 0, 90 });
	camera->ApplyTransform();
	camera->GetInvTransform();
	config3->UseCamera(camera);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> go4 = std::make_shared<Mesh>("plane.obj");
	go4->EnableMaterial(false);
	go4->EnableSDF(false);
	go4->SetObjShader("testS", "SDF_Visual");
	go4->SetPos({ 0,0,0 });
	go4->SetScale({ 2,2,2 });
	go4->SetRot({ 0,0,90 });
	config3->UseMesh(go4);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> go2 = std::make_shared<Mesh>("torus.obj");
	go2->SetObjShader("testS", "Rasterization");
	go2->SetTex(MAT_ALBEDO, "avatar1.png");
	go2->SetCenter();
	go2->SetPos({ 0, 0, 0 });
	go2->SetScale(glm::vec3(1.5f));
	config3->UseMesh(go2);

	DEBUG("\n---------------ENVIR----------------")
		std::shared_ptr<Environment> environment = std::make_shared<Environment>("hdr/room.hdr");
	environment->SetPos(glm::vec3(0.0f, 7.0f, 7.0f));
	config3->UseEnvironment(environment);

	DEBUG("\n---------------POSTPRCS----------------")
		std::shared_ptr<PostProcessing> pps1 = std::make_shared<PostProcessing>("PBR", COMPUTE_SHADER);
	pps1->pps_field.SetPos({ 5, 5, 5 });
	pps1->AddBinding("U_color", BUFFER_TEXTURE + COMBINE_FB);
	pps1->AddBinding("U_pos", BUFFER_TEXTURE + POS_FB);
	pps1->AddBinding("U_normal", BUFFER_TEXTURE + NORMAL_FB);
	pps1->AddBinding("U_albedo", BUFFER_TEXTURE + ALBEDO_FB);
	pps1->AddBinding("U_mrse", BUFFER_TEXTURE + MRSE_FB);
	pps1->AddBinding("U_emission", BUFFER_TEXTURE + EMIS_COL_FB);
	pps1->AddBinding("U_alpha", BUFFER_TEXTURE + MASK_FB);
	pps1->AddBinding("Envir_Texture_diff", IBL_TEXTURE);
	pps1->AddBinding("Envir_Texture_spec", IBL_TEXTURE + 1);
	pps1->AddBinding("LTC1", 13);	// Pass LTC matrix lookup tables for poly & area lights
	pps1->AddBinding("LTC2", 14);	// Texture slot 0-12 are currently occupied, so 13 and 14 are used for these two tables
	config3->UsePostProcessing(pps1);

	return config3;
}

std::shared_ptr<SceneResource> SceneManager::SceneConfig4(std::string _name /*= "shadow test"*/)
{
	if (SceneManager::sce_configs.find(_name) != SceneManager::sce_configs.end())
		return SceneManager::sce_configs[_name];

	std::shared_ptr<SceneResource> config4 = std::make_shared<SceneResource>();
	SceneManager::sce_configs[_name] = config4;

	DEBUG("\n---------------CAMERA----------------")
		std::shared_ptr<Camera> camera = std::make_shared<Camera>(10.0f, 10.0f, 70, 0.1f, 300.0f);
	camera->SetCamPos({ 4, -4, 2 });
	camera->ApplyTransform();
	camera->GetInvTransform();
	config4->UseCamera(camera);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> go1 = std::make_shared<Mesh>("shadow/grid1.obj");
	go1->SetObjShader("testS", "Rasterization");
	config4->UseMesh(go1);	

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> go2 = std::make_shared<Mesh>("shadow/grid2.obj");
	go2->SetObjShader("testS", "Rasterization");
	config4->UseMesh(go2);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> go3 = std::make_shared<Mesh>("shadow/ground.obj");
	go3->SetObjShader("testS", "Rasterization");
	config4->UseMesh(go3);

	DEBUG("\n---------------MESH----------------")
		std::shared_ptr<Mesh> go4 = std::make_shared<Mesh>("shadow/torus.obj");
	go4->SetObjShader("testS", "Rasterization");
	config4->UseMesh(go4);
	
	DEBUG("\n---------------LIGHT----------------")
		std::shared_ptr<Light> pointLight1 = std::make_shared<Light>(POINTLIGHT, 1.0f, glm::vec3(1.0f));
	pointLight1->SetPos({ 2.0f, 2.0f, 3.0f });
	pointLight1->SetColor({ 0.7,0.7, 1 });
	pointLight1->SetPower(30);
	pointLight1->ApplyTransform();
	config4->UseLight(pointLight1);

	DEBUG("\n-------------AREA LIGHT-------------")
		std::shared_ptr<Light> pointLight2 = std::make_shared<Light>(AREALIGHT, 1.0f, glm::vec3(1.0f));
	pointLight2->SetPos({ -2.0f, -1.5f, 0.7f });
	pointLight2->SetRot({ 135, 0, 135 });
	pointLight2->SetColor({ 1,0.7,0.7 });
	pointLight2->SetPower(10);
	pointLight2->ApplyTransform();
	config4->UseLight(pointLight2);

	DEBUG("\n---------------ENVIR----------------")
		std::shared_ptr<Environment> environment = std::make_shared<Environment>("hdr/room.png");
	environment->SetPos(glm::vec3(0.0f, 7.0f, 7.0f));
	config4->UseEnvironment(environment);

	DEBUG("\n---------------POSTPRCS----------------")
		std::shared_ptr<PostProcessing> pps1 = std::make_shared<PostProcessing>("PBR", COMPUTE_SHADER);
	pps1->pps_field.SetPos({ 5, 5, 5 });
	pps1->AddBinding("U_color", BUFFER_TEXTURE + COMBINE_FB);
	pps1->AddBinding("U_pos", BUFFER_TEXTURE + POS_FB);
	pps1->AddBinding("U_normal", BUFFER_TEXTURE + NORMAL_FB);
	pps1->AddBinding("U_albedo", BUFFER_TEXTURE + ALBEDO_FB);
	pps1->AddBinding("U_mrse", BUFFER_TEXTURE + MRSE_FB);
	pps1->AddBinding("U_emission", BUFFER_TEXTURE + EMIS_COL_FB);
	pps1->AddBinding("U_alpha", BUFFER_TEXTURE + MASK_FB);
	pps1->AddBinding("Envir_Texture_diff", IBL_TEXTURE);
	pps1->AddBinding("Envir_Texture_spec", IBL_TEXTURE + 1);
	pps1->AddBinding("LTC1", 13);	// Pass LTC matrix lookup tables for poly & area lights
	pps1->AddBinding("LTC2", 14);	// Texture slot 0-12 are currently occupied, so 13 and 14 are used for these two tables
	config4->UsePostProcessing(pps1);

	return config4;
}



