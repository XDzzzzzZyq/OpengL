#pragma once
#include "ImguiItem.h"

namespace UI {
	class OpaButton : public ImguiItem
	{
	private:
		ImVec2 obt_size;
		int obt_objectID{ -1 };
		bool obt_swap_per_click{ true };
	public:
		OpaButton();
		OpaButton(const std::string& name, int id = -1, bool spc = true);
		~OpaButton();

		void RenderItem() const override;
		void ResetSize(const ImVec2& size) override;
	};
}

