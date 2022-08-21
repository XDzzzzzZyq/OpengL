#pragma once
#include "ImguiItem.h"

namespace UI {
	class OpaButton : public ImguiItem
	{
	private:
		ImVec2 bt_size;
		int objectID;
	public:
		OpaButton();
		OpaButton(const std::string& name, int id = -1);
		~OpaButton();

		void RenderItem() const override;
		void ResetSize(const ImVec2& size) override;
	};
}

