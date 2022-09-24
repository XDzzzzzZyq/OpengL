#pragma once
#include "ImguiLayer.h"
#include "EventListener.h"
#include "Shaders.h"
#include "ITEM/TextEditor.h"

#include <string>

class MiniPropPanel final
{
private:
	bool is_open{ false };
	int datatype{ 0 };
	int prop_count{ 1 };
	ImVec2 panel_pos{ ImVec2(0,0) };
	Args prop_args;
	char prop_name;
	char prop_content;
public:
	bool RenderPanel(const ImVec2& pos, bool* state, S_U* out, const std::string& c_name = "");

	//true for function | false for structure
	bool RenderDefPanel(bool type, const ImVec2& pos, bool* state, S_func* _struct);

	void RenderArguPanel();
};

class ShaderEditor : public ImguiLayer, public EventListener
{
private:
	static std::string edit_mode[3];
	static std::string shader_type[2];
	static TextEditor Editor;


	mutable int current_edit = 0, current_shad_type = 0;
	mutable bool is_edit_changed = true, is_shad_type_changed = true;
	mutable bool add_button = false;
	mutable bool sel;
public:
	ShaderEditor();
	ShaderEditor(const std::string& name);
	~ShaderEditor();
public:
	mutable MiniPropPanel Panel;
	mutable S_U add_prop;
	mutable S_func add_args;
	bool AddParam(const std::string& c_name = "") const;
	bool AddStruct(bool def_type = false) const;
	bool AddLink();
	void CompileShader() const;
public:
	void UpdateShaderEditor();
	void UpdateLayer() override;
	void RenderShaderStruct() const;
	void RenderArgs(Args& args, int type) const;
	void RenderLayer() const override;
};