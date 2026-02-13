#pragma once
#include "ImguiLayer.h"

#include "ITEM/TextEditor.h"
#include "ITEM/NodeEditor.h"

#include <string>

enum ShEditMode {
	CODE_EDITOR, STRUCT_EDITOR, NODE_EDITOR
};

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

public: // TODO: use private

	static TextEditor se_code_editor;
	static NodeEditor se_node_editor;

private:

	static const std::string edit_mode[3];
	int active_func{0};

	int current_edit = 0, current_shad_type = 0;
	bool is_mode_changed = true, is_shad_type_changed = true;
	bool sel;

public:

	ShaderEditor();
	ShaderEditor(const std::string& name);
	static void InitEditors(); // TODO: not necessary
	~ShaderEditor();

public:

	mutable MiniPropPanel se_panel;
	mutable char add_name[CHAR_MAX];
	
	bool AddParam(const char* c_name = "", const char* c_sld_name = "");
	bool AddStruct(bool def_type = false);
	bool AddLink();
	void CompileShader(const SceneContext& ctx);

public:

	void UpdateShaderEditor(const SceneContext& ctx, const std::string& _code = "") const;
	void UpdateLayer(const SceneContext& ctx) override;
	static void UpdateKeyword();

public:
	void RenderName(const std::string& _label, std::string* _name, float _width = 0.0f, bool read_only = true) const;
	void RenderName(const char* _label, std::string* _name, float _width = 0.0f, bool read_only = true) const;
	void RenderShaderStruct(const SceneContext& ctx);

	void RenderLayer(const SceneContext& ctx, const EventPool& evt) override;
};