#pragma once
#include "ImguiItem.h"

namespace UI {
	class OpaButton : public ImguiItem
	{
	private:
		ImVec2 bt_size;
	public:
		OpaButton();
		OpaButton(const std::string& name);
		~OpaButton();

		void RenderItem() const override;
		void ResetSize(const ImVec2& size) override;
	};

}

