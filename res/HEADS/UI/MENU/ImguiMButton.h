#pragma once
#include "ImguiMenuItem.h"

namespace UI
{
	class ImguiMButton : public ImguiMenuItem
	{
	public:
		ImguiMButton();
		ImguiMButton(std::string _name, std::string _shortcut);
	
		void RenderMenuItem() override;
	};
}

