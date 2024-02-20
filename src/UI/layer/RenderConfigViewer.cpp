#include "RenderConfigViewer.h"
#include "macros.h"

RenderConfigViewer::RenderConfigViewer()
	:RenderConfigViewer("Renderer")
{}

RenderConfigViewer::RenderConfigViewer(const std::string& _name)
{
	uly_name = _name;
	uly_type = RENDER_CONFIG_ULATER;
}

RenderConfigViewer::~RenderConfigViewer()
{}

void RenderConfigViewer::RenderLayer()
{
	RenderConfigs* active_config = RenderConfigs::GetActiveConfigPtr();

	if (ImGui::Begin(uly_name.c_str(), &uly_is_rendered)) {

		if (active_config == nullptr) {
			ImGui::Text("No active renderer");
			ImGui::End();
			return;
		}

		bool modified = false;
		int flag = 0;

		modified = RenderOption((char*)&active_config->r_pipeline,			"Rendering PipeLine",		OPTIONS("Forward", "Deferred", "Custom (future)"));
		modified = RenderOption((char*)&active_config->r_sampling_average,	"Sampling",					OPTIONS("Average", "Increment Average"));
		modified = RenderOption((char*)&active_config->r_of_algorithm,		"Optical Flow",				OPTIONS("None", "Forward", "Backward"));
		modified = RenderOption((char*)&active_config->r_anti_alias,		"Anti Aliasing",			OPTIONS("None", "MSAA (future)", "FXAA"));
		modified = RenderOption((char*)&active_config->r_ssr_algorithm,		"Screen Space Reflection",	OPTIONS("None", "Ray Marching", "SDF Ray Marching", "SDF Resolved Ray Marching"));
		modified = RenderOption((char*)&active_config->r_shadow_algorithm,	"Shadow",					OPTIONS("None", "Shadow Mapping", "SDF Soft Shadow", "Variance Soft Shadow", "Moment Soft Shadow"));
		if (modified) flag |= RenderConfigs::ShadowChanged;
		modified = RenderOption((char*)&active_config->r_ao_algorithm,		"Ambient Occlusion",		OPTIONS("None", "SSAO", "HBAO (future)"));

		active_config->call_back((RenderConfigs::ModifyFlags)flag);
	}
	ImGui::End();

}

bool RenderConfigViewer::RenderOption(
	char* option, 
	std::string name, 
	const std::vector<std::string>& option_names)
{
	bool modified = false;

	if (ImGui::BeginCombo(name.c_str(), option_names[*option].c_str())) {
		LOOP(option_names.size())
			if (ImGui::Selectable(option_names[i].c_str())) {
				modified = *option != i;
				*option = i;
			}
		ImGui::EndCombo();
	}

	return modified;
}
