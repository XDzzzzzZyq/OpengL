#include "ImguiMOption.h"

void UI::ImguiMOption::ResetOptions()
{
	for (auto& option : mitem_options)
		option.second = false;
}

UI::ImguiMOption::ImguiMOption(){}

UI::ImguiMOption::ImguiMOption(std::string _name)
	:ImguiMOption(_name, {})
{
	assert(false && "Empty option error");
}

UI::ImguiMOption::ImguiMOption(std::string _name, const std::vector<std::string>& _options)
	:ImguiMenuItem(_name, MenuItemType::OPTION_MITEM)
{
	assert(_options.size() > 0);

	for (auto& option : _options)
		mitem_options.push_back({ option, false });

	mitem_options[0].second = true;
}

void UI::ImguiMOption::BindOption(char* _tar)
{
	mitem_option = _tar;

	ResetOptions();
	mitem_options[*_tar].second = true;
}

void UI::ImguiMOption::RenderMenuItem()
{
	if (ImGui::BeginMenu(mitem_name.c_str())) {

		if (mitem_option == nullptr) {
			ImGui::EndMenu();
			return;
		}

		char index = 0;
		for (auto & option : mitem_options) {
		
			const bool selected_b = option.second;
			if (ImGui::MenuItem(
				option.first.c_str(),
				mitem_shortcut.c_str(),
				&option.second,
				mitem_enable
			)) {
				if (is_single_selection)
					*mitem_option = index;
			}
		
			index++;
		}
	
		ResetOptions();
		mitem_options[*mitem_option].second = true;
		ImGui::EndMenu();
	}
}
