#include "ImguiManager.h"

void ImguiManager::DefultViewports() {

	ImguiLayer* layer1 = new ParamControl("test layer");
	layer1->uly_type = PARAS_UILAYER;
	layer1->PushItem(new UI::ParaInput(FLOAT_INP, "test"));
	layer1->PushItem(new UI::ParaInput(INT_INP, "test"));
	layer1->PushItem(new UI::ParaInput(BOOL_INP, "test"));
	layer1->PushItem(new UI::ParaInput(RGB_INP, "test"));
	layer1->PushItem(new UI::Button("testB"));
	layer1->PushItem(new UI::Text("test[%.1f]"));
	PushImguiLayer(layer1);
	FindImguiItem("test layer", "test[%.1f]")->SetArgsList(1, &GetParaValue("test layer", "test")->para_data.fdata);

	ImguiLayer* layer2=new ParamControl("__Parameters__");
	layer2->uly_type = PARAS_UILAYER;
	layer2->PushItem(new UI::Text("Frame Rate %.3f ms/frame (%.1f FPS)"));
	layer2->PushItem(new UI::Text("MOUSE_POS : [%.1f : %.1f]"));
	layer2->PushItem(new UI::ParaInput(FLOAT_INP, "SCALE", 0.0f, 1.0f, 0.3f));
	layer2->PushItem(new UI::ParaInput(FLOAT_INP, "POWER", 0.0f, 1.0f, 0.5f));
	layer2->PushItem(new UI::ParaInput(FLOAT_INP, "X", -90.0f, 90.0f));
	layer2->PushItem(new UI::ParaInput(FLOAT_INP, "Y", -90.0f, 90.0f));
	layer2->PushItem(new UI::ParaInput(FLOAT_INP, "Z", -90.0f, 90.0f));
	layer2->PushItem(new UI::ParaInput(FLOAT_INP, "GAMMA", 0.0f, 30.0f, 3.3f));
	layer2->PushItem(new UI::ParaInput(RGB_INP, "Light Color", glm::vec4(0.5, 0.5, 0.5, 1.0)));
	layer2->PushItem(new UI::ParaInput(RGB_INP, "Light Position"));
	layer2->PushItem(new UI::ParaInput(RGB_INP, "Light Rotation", glm::vec4(0.5, 0.5, 0.5, 1.0)));
	layer2->PushItem(new UI::Button("Debug"));
	PushImguiLayer(layer2);

	ImguiLayer* viewport = new Viewport("Viewport");
	PushImguiLayer(viewport);

	ImguiLayer* shaderedit = new ShaderEditor();
	PushImguiLayer(shaderedit);

	ImguiLayer* outline = new Outliner("Outliner");
	PushImguiLayer(outline);

	ImguiMenu menu1("FILE");
	menu1.PushSubMenu(ImguiMenuItem("test", "ctrl + A"));
	menu1.PushSubMenu(ImguiMenuItem("test2", "ctrl + S"));

	PushImguiMenu(menu1);

	SetActiveImguiLayer("__Parameters__");
}