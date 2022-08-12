#pragma once
#include "ImguiLayer.h"

namespace UI {
	class Outline : public ImguiLayer
	{
	public:
		Outline();
		~Outline();



		void SetOutLine(const std::vector<std::tuple<int, std::string>>& name_list, const std::vector<int>& depth_list) override;
		void RenderLayer() const override;
	};
}


