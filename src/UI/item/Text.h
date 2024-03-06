#pragma once

#include "ImguiItem.h"
namespace UI {

	class Text : public ImguiItem
	{
	public:
		Text();
		Text(const std::string& name);

		mutable float args_list[5];
		mutable float* args_ptr;
		mutable int args_count = 0;

		mutable bool mode;

		void RenderItem() const override;

		void SetArgsList(int length, ...) const override {
			args_count = length;
			va_list arg_ptr;
			float data = 0;
			va_start(arg_ptr, length);
			LOOP(length) {
				data = (float)va_arg(arg_ptr, double);
				args_list[i] = data;
			}
			va_end(arg_ptr);
			mode = false;
		}

		virtual void SetArgsList(int length, float* f1) const {
			args_count = length;
			args_ptr = f1;
			mode = true;
		}
	};
}


