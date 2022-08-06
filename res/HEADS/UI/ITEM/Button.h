#pragma once
#include "ImguiItem.h"
namespace UI {
	class Button : public ImguiItem
	{
	public:
		Button(const std::string& name);
		~Button();

		//mutable std::function<void(void)> ButtonFunc = [] {};
		void CallButtonFunc() const override { ButtonFunc(); }
		void RenderItem() const override;
	};
}


