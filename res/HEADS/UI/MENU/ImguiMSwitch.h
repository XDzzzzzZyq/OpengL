#pragma once
#include "ImguiMenuItem.h"

namespace UI
{
	class ImguiMSwitch : public ImguiMenuItem
	{
	private:
	
		std::shared_ptr<bool> mitem_switch;
	
	public:
		ImguiMSwitch();
		ImguiMSwitch(std::string _name);
	
		void BindSwitch(bool* _switch) override;
	
		void RenderMenuItem() override;
	};
}

