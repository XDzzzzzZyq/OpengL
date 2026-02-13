#include "ImguiManager.h"
#include "macros.h"

#include "menu/ImguiMButton.h"
#include "menu/ImguiMOption.h"
#include "menu/ImguiMSwitch.h"

#include "layer/Viewport.h"
#include "layer/Outliner.h"
#include "layer/ParamControl.h"
#include "layer/ShaderEditor.h"
#include "layer/MaterialViewer.h"
#include "layer/TransformPanel.h"
#include "layer/RenderConfigViewer.h"

#include "item/ParaInput.h"
#include "item/TextureViewer.h"
#include "item/Text.h"
#include "item/Button.h"
#include "item/OpaButton.h"

#include "events/KeyMouseEvents.h"

void ImguiManager::DefultViewports() {


	auto menu1 = CreateImguiMenu("File");
	menu1->PushSubMenu<UI::ImguiMButton>("test", "ctrl + A");
	menu1->PushSubMenu<UI::ImguiMButton>("test2", "ctrl + S");

	auto menu2 = CreateImguiMenu("Edit");
	menu2->PushSubMenu<UI::ImguiMButton>("undo", "ctrl + Z");
	menu2->PushSubMenu<UI::ImguiMButton>("redo", "ctrl + Y");
	menu2->PushSubMenu<UI::ImguiMButton>("preference", "ctrl + P");

	auto menu3 = CreateImguiMenu("Window");

	auto menu4 = CreateImguiMenu("View");
	menu4->PushSubMenu<UI::ImguiMSwitch>("Axis");
	menu4->PushSubMenu<UI::ImguiMSwitch>("Grid");
	menu4->PushSubMenu<UI::ImguiMSwitch>("Transform");
	menu4->PushSubMenu<UI::ImguiMSwitch>("Icons");
	menu4->PushSubMenu<UI::ImguiMOption>("Render Result", OPTIONS("Combine", "Position", "Normal", "MRSE", "Random", "AO"));

	auto menu5 = CreateImguiMenu("Render");
	menu5->PushSubMenu<UI::ImguiMOption>("Rendering PipeLine", OPTIONS("Forward", "Deferred", "Custom (future)"));
	menu5->PushSubMenu<UI::ImguiMOption>("Sampling", OPTIONS("Average", "Increment Average"));
	menu5->PushSubMenu<UI::ImguiMOption>("Optical Flow", OPTIONS("None", "Forward", "Backward"));
	menu5->PushSubMenu<UI::ImguiMOption>("Anti Aliasing", OPTIONS("None", "MSAA (future)", "FXAA"));
	menu5->PushSubMenu<UI::ImguiMOption>("Screen Space Reflection", OPTIONS("None", "Ray Marching", "SDF Ray Marching", "SDF Resolved Ray Marching"));
	menu5->PushSubMenu<UI::ImguiMOption>("Shadow", OPTIONS("None", "Shadow Mapping", "SDF Soft Shadow", "Variance Soft Shadow", "Moment Soft Shadow"));
	menu5->PushSubMenu<UI::ImguiMOption>("Ambient Occlusion", OPTIONS("None", "SSAO", "HBAO (future)"));

	auto layer1 = CreateImguiLayer<ParamControl>("test layer");
	layer1->PushItem<UI::ParaInput>(FLOAT_INP, "testf", 0.0, 1.0);
	layer1->PushItem<UI::ParaInput>(FLOAT_INP, "Metalness", 0.0, 1.0);
	layer1->PushItem<UI::ParaInput>(FLOAT_INP, "Roughness", 0.0, 1.0);
	layer1->PushItem<UI::ParaInput>(FLOAT_INP, "Specular", 0.0, 1.0, 1.0);
	layer1->PushItem<UI::ParaInput>(INT_INP, "test");
	layer1->PushItem<UI::ParaInput>(BOOL_INP, "test");
	layer1->PushItem<UI::ParaInput>(RGB_INP, "test");
	layer1->PushItem<UI::Button>("testB");
	layer1->PushItem<UI::Text>("test[%.1f]");
	FindImguiItem("test layer", "test[%.1f]")->SetArgsList(1, &GetParaValue("test layer", "testf")->Get<float>());
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
	layer2->PushItem<UI::ParaInput>(RGB_INP, "Light Color", glm::vec3{ 0.5,0.5,0.5 });
	layer2->PushItem<UI::ParaInput>(RGB_INP, "Light Position");
	layer2->PushItem<UI::ParaInput>(RGB_INP, "Light Rotation", glm::vec3{ 0.5,0.5,0.5 });
	layer2->PushItem<UI::Button>("Debug");

	auto render_config = CreateImguiLayer<RenderConfigViewer>("Renderer");

	auto viewport = CreateImguiLayer<Viewport>("Viewport");

	auto compshader_result = CreateImguiLayer<Viewport>("CompShader");
	compshader_result->uly_is_rendered = false;

	auto outline = CreateImguiLayer<Outliner>("Outliner");

	auto shaderedit = CreateImguiLayer<ShaderEditor>("Shader Editor");     //ShaderEditor must after Outliner since the order of "Input::is_selected_changed"

	auto material = CreateImguiLayer<MaterialViewer>("Material");

	auto transform = CreateImguiLayer<TransformPanel>("Transform");

	FindImguiMenuItem("View", "Axis")->BindSwitch(&viewport->display_axis);
	FindImguiMenuItem("View", "Grid")->BindSwitch(&viewport->display_grid);
	FindImguiMenuItem("View", "Transform")->BindSwitch(&viewport->display_trans_handle);

	SetActiveImguiLayer("__Parameters__");
}

void ImguiManager::RegisterDefultEvents(EventPool& evt)
{
	// TODO: match the hotkey input
	evt.subscribe<KeyClickEvent>([](KeyClickEvent e) {
		switch (e.norm_key) {
		case 1: // G
			Viewport::MTranslate();
			break;
		case 2: // R
			Viewport::MRotate();
			break;
		case 3: // S
			Viewport::MScale();
			break;
		case 4: // X
			Viewport::XAxis();
			break;
		case 5: // Y
			Viewport::YAxis();
			break;
		case 6: // Z
			Viewport::ZAxis();
			break;
		case 7: // W
			Viewport::WAxis();
			break;
		}
		});

	evt.subscribe<MouseClickEvent>([](MouseClickEvent e) {
		if (e.mouse == Input::MouseStatus::LMB) { // LMB
			ShaderEditor::se_node_editor.LMB();
		}
		else if(e.mouse == Input::MouseStatus::MMB) { // MMB
			if (e.key == Input::SpecialKeys::SHIFT)
				ShaderEditor::se_node_editor.SHIFT_MMB();
			else if (e.key == Input::SpecialKeys::CTRL)
				ShaderEditor::se_node_editor.CTRL_MMB();
		}
		});

	evt.subscribe<MouseScrolledEvent>([](MouseScrolledEvent e) {
		ShaderEditor::se_node_editor.SCRLL();
		});
}