#include "ImguiManager.h"

void ImguiManager::DefultViewports() {

	ParamControl* layer1 = new ParamControl("test layer");
	layer1->uly_type = PARAS_UILAYER;
	layer1->PushItem(new UI::ParaInput(FLOAT_INP, "test"));
	layer1->PushItem(new UI::ParaInput(INT_INP, "test"));
	layer1->PushItem(new UI::ParaInput(BOOL_INP, "test"));
	layer1->PushItem(new UI::ParaInput(RGB_INP, "test"));
	layer1->PushItem(new UI::Button("testB"));
	layer1->PushItem(new UI::Text("test[%.1f]"));
	PushImguiLayer(layer1);

	FindImguiItem("test layer", "test[%.1f]")->SetArgsList(1, &GetParaValue("test layer", "test")->para_data.fdata);

	ParamControl* layer2=new ParamControl("__Parameters__");
	layer2->PushItem(new UI::Text("Application average %.3f ms/frame (%.1f FPS)"));
	layer2->PushItem(new UI::Text("MOUSE_POS : [%.1f : %.1f]"));
	layer2->PushItem(new UI::ParaInput(FLOAT_INP, "SCALE", 0.0f, 1.0f, 0.3f));
	layer2->PushItem(new UI::ParaInput(FLOAT_INP, "POWER", 0.0f, 1.0f, 0.5f));
	layer2->PushItem(new UI::ParaInput(FLOAT_INP, "X", -90.0f, 90.0f));
	layer2->PushItem(new UI::ParaInput(FLOAT_INP, "Y", -90.0f, 90.0f));
	layer2->PushItem(new UI::ParaInput(FLOAT_INP, "Z", -90.0f, 90.0f));
	layer2->PushItem(new UI::ParaInput(RGB_INP, "Light Color"));
	layer2->PushItem(new UI::ParaInput(RGB_INP, "Light Position"));
	layer2->PushItem(new UI::Button("Debug"));
	PushImguiLayer(layer2);

	Viewport* layer3 = new Viewport("Viewport");
	layer3->PushItem(new UI::TextureView("Viewport", 0, ImVec2(SCREEN_W, SCREEN_H)));
	PushImguiLayer(layer3);

	ImguiMenu menu1("FILE");
	menu1.PushSubMenu(ImguiMenuItem("test", "ctrl + A"));
	menu1.PushSubMenu(ImguiMenuItem("test2", "ctrl + S"));

	PushImguiMenu(menu1);

	SetActiveImguiLayer("__Parameters__");
}