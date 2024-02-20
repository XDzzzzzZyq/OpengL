#pragma once
#include "ImguiMenuItem.h"

namespace UI
{
	class ImguiMOption : public ImguiMenuItem
	{
	private:
	
		std::vector<std::pair<std::string, bool>> mitem_options;
		char* mitem_option;
	
		bool is_single_selection{ true };
	
		void ResetOptions();
	
	public:
		ImguiMOption();
		~ImguiMOption() {};
		ImguiMOption(std::string _name);
		ImguiMOption(std::string _name, const std::vector<std::string>& _options);
	
		void BindOption(char* _tar) override;
	
		void RenderMenuItem() override;
	};
}

