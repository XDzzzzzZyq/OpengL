
#include "ImguiManager.h"

void ImguiManager::DefultViewports() {


	auto menu1 = CreateImguiMenu("File");
	menu1->PushSubMenu(std::make_shared<ImguiMenuItem>("test", "ctrl + A"));
	menu1->PushSubMenu(std::make_shared<ImguiMenuItem>("test2", "ctrl + S"));

	auto menu2 = CreateImguiMenu("Edit");
	menu2->PushSubMenu(std::make_shared<ImguiMenuItem>("undo", "ctrl + Z"));
	menu2->PushSubMenu(std::make_shared<ImguiMenuItem>("redo", "ctrl + Y"));
	menu2->PushSubMenu(std::make_shared<ImguiMenuItem>("preference", "ctrl + P"));

	auto menu3 = CreateImguiMenu("Window");

	auto menu4 = CreateImguiMenu("View");
	menu4->PushSubMenu(std::make_shared<ImguiMenuItem>("Axis", "", BOOL_MITEM));
	menu4->PushSubMenu(std::make_shared<ImguiMenuItem>("Grid", "", BOOL_MITEM));
	menu4->PushSubMenu(std::make_shared<ImguiMenuItem>("Transform", "", BOOL_MITEM));

	auto menu5 = CreateImguiMenu("Render");
	menu5->PushSubMenu(std::make_shared<ImguiMenuItem>("Optical Flow", "", BOOL_MITEM));
	menu5->PushSubMenu(std::make_shared<ImguiMenuItem>("Forward Optical Flow", "", BOOL_MITEM));
	menu5->PushSubMenu(std::make_shared<ImguiMenuItem>("Deferred Rendering", "", BOOL_MITEM));
	menu5->PushSubMenu(std::make_shared<ImguiMenuItem>("FXAA", "", BOOL_MITEM));
	menu5->PushSubMenu(std::make_shared<ImguiMenuItem>("Screen Space Reflection", "", BOOL_MITEM));
	menu5->PushSubMenu(std::make_shared<ImguiMenuItem>("Shadow Map", "", BOOL_MITEM));
	menu5->PushSubMenu(std::make_shared<ImguiMenuItem>("SDF Field", "", BOOL_MITEM));
	menu5->PushSubMenu(std::make_shared<ImguiMenuItem>("Icons", "", BOOL_MITEM));

	auto layer1 = CreateImguiLayer<ParamControl>("test layer");
	layer1->PushItem<UI::ParaInput>(FLOAT_INP, "testf", 0.0, 1.0);
	layer1->PushItem<UI::ParaInput>(FLOAT_INP, "Metalness", 0.0, 1.0);
	layer1->PushItem<UI::ParaInput>(FLOAT_INP, "Roughness", 0.0, 1.0);
	layer1->PushItem<UI::ParaInput>(FLOAT_INP, "Specular", 0.0, 1.0, 1.0);
	layer1->PushItem<UI::ParaInput>(INT_INP  , "test");
	layer1->PushItem<UI::ParaInput>(BOOL_INP , "test");
	layer1->PushItem<UI::ParaInput>(RGB_INP  , "test");
	layer1->PushItem<UI::Button>   ("testB");
	layer1->PushItem<UI::Text>	   ("test[%.1f]");
	FindImguiItem("test layer", "test[%.1f]")->SetArgsList(1, &GetParaValue("test layer", "testf")->para_data.fdata);
	layer1->uly_is_rendered = false;

	auto layer2 = CreateImguiLayer<ParamControl>("__Parameters__");
	layer2->PushItem<UI::Text>("Frame Rate %.3f ms/frame (%.1f FPS)");
	layer2->PushItem<UI::Text>("MOUSE_POS : [%.1f : %.1f]");
	layer2->PushItem<UI::ParaInput>(FLOAT_INP, "SCALE", 0.0f, 1.0f, 0.3f);
	layer2->PushItem<UI::ParaInput>(FLOAT_INP, "POWER", 0.0f, 1.0f, 0.5f);
	layer2->PushItem<UI::ParaInput>(FLOAT_INP, "X", -90.0f, 90.0f);
	layer2->PushItem<UI::ParaInput>(FLOAT_INP, "Y", -90.0f, 90.0f);
	layer2->PushItem<UI::ParaInput>(FLOAT_INP, "Z", -90.0f, 90.0f);
	layer2->PushItem<UI::ParaInput>(FLOAT_INP, "W", 0.0f, 10.0f);
	layer2->PushItem<UI::ParaInput>(FLOAT_INP, "GAMMA", 0.0f, 30.0f, 1.5f);
	layer2->PushItem<UI::ParaInput>(RGB_INP,   "Light Color", glm::vec3{ 0.5,0.5,0.5 });
	layer2->PushItem<UI::ParaInput>(RGB_INP,   "Light Position");
	layer2->PushItem<UI::ParaInput>(RGB_INP,   "Light Rotation", glm::vec3{ 0.5,0.5,0.5 });
	layer2->PushItem<UI::Button>("Debug");

	auto viewport = CreateImguiLayer<Viewport>("Viewport");

	auto compshader_result = CreateImguiLayer<Viewport>("CompShader");
	compshader_result->uly_is_rendered = false;

	auto outline = CreateImguiLayer<Outliner>("Outliner");

	auto shaderedit = CreateImguiLayer<ShaderEditor>("Shader Editor");     //ShaderEditor must after Outliner since the order of "EventListener::is_selected_changed"

	auto material = CreateImguiLayer<MaterialViewer>("Material");

	auto transform = CreateImguiLayer<TransformPanel>("Transform");

	FindImguiMenuItem("View", "Axis")->BindSwitch(viewport, &viewport->display_axis);
	FindImguiMenuItem("View", "Grid")->BindSwitch(viewport, &viewport->display_grid);
	FindImguiMenuItem("View", "Transform")->BindSwitch(viewport, &viewport->display_trans_handle);

	SetActiveImguiLayer("__Parameters__");
}