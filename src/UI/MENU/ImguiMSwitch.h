#pragma once
#include "ImguiMenuItem.h"

namespace UI
{
	class ImguiMSwitch : public ImguiMenuItem
	{
	private:
	
		bool* mitem_switch;
	
	public:
		ImguiMSwitch();
		~ImguiMSwitch() {};
		ImguiMSwitch(std::string _name);
	
		void BindSwitch(bool* _switch) override;
	
		void RenderMenuItem() override;
	};
}

