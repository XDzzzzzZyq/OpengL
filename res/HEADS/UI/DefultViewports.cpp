#include "ImguiManager.h"

void ImguiManager::DefultViewports() {

	ImguiLayer layer1;
	layer1.uly_name = "test layer";
	layer1.uly_type = PARAS_UILAYER;
	layer1.PushItem(ImguiItem(FLOAT_INP, "test"));
	layer1.PushItem(ImguiItem(FLOAT_INP, "test"));
	layer1.PushItem(ImguiItem(FLOAT_INP, "test"));
	layer1.PushItem(ImguiItem(RGB_INP, "test"));
	layer1.PushItem(ImguiItem(BUTTON_INP, "testB"));

	PushImguiLayer(layer1);

	ImguiMenu menu1("FILE");
	menu1.PushSubMenu(ImguiMenuItem("test", "ctrl + A"));
	menu1.PushSubMenu(ImguiMenuItem("test2", "ctrl + S"));

	PushImguiMenu(menu1);

}