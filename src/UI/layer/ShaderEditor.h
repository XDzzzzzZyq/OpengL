#pragma once
#include "ImguiLayer.h"

#include "ITEM/TextEditor.h"
#include "ITEM/NodeEditor.h"

#include <string>

class ShaderEditor : public ImguiLayer
{
public:
	struct MiniPropPanel
	{
		bool is_open{ false };
		int datatype{ 0 };
		int prop_count{ 1 };
		ImVec2 panel_pos{ ImVec2(0,0) };
	};

private:

	TextEditor se_code_editor{};
	NodeEditor se_node_editor{};
	MiniPropPanel se_panel;

private:

	enum ShaderEditMode {
		CODE_EDITOR, STRUCT_EDITOR, NODE_EDITOR
	};

	int active_func{0};

	ShaderEditMode current_edit = NODE_EDITOR;
	int current_shad_type = 0;
	bool sel;
	char add_name[CHAR_MAX];

public:

	ShaderEditor();
	ShaderEditor(const std::string& name);
	~ShaderEditor();

private:

	
	bool AddParam(const char* c_name = "", const char* c_sld_name = "");
	bool AddStruct(bool def_type = false);
	bool AddLink();
	bool CompileShader(ObjectID* active_obj);

public:

	void RegisterEvents(EventPool& evt) override;
	void RenderLayer(const SceneContext& ctx, const EventPool& evt) override;

private:
	void UpdateShaderEditor(ObjectID* active_obj);
	void UpdateKeyword();

	void RenderName(const std::string& _label, std::string* _name, float _width = 0.0f, bool read_only = true) const;
	void RenderName(const char* _label, std::string* _name, float _width = 0.0f, bool read_only = true) const;
	void RenderShaderStruct(ObjectID* active_obj);
};