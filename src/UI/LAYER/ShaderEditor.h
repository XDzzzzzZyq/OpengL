#pragma once
#include "ImguiLayer.h"
#include "EventListener.h"
#include "Shaders.h"

#include "ITEM/TextEditor.h"
#include "ITEM/NodeEditor.h"

#include <string>

enum ShEditMode {
	CODE_EDITOR, STRUCT_EDITOR, NODE_EDITOR
};

class ShaderEditor : public ImguiLayer
{
private:

	class MiniPropPanel final
	{
	private:
		bool is_open{ false };
		int datatype{ 0 };
		int prop_count{ 1 };
		ImVec2 panel_pos{ ImVec2(0,0) };
		Args prop_args;
		static char prop_name;
		char prop_content;
	public:
		bool RenderPanel(const ImVec2& pos, bool* state, S_U* out, const char* c_name = "", const char* c_sld_name = "");

		//true for function | false for structure
		bool RenderDefPanel(bool type, const ImVec2& pos, bool* state, S_func* _struct);

		void RenderArguPanel(bool* b);
	};

private:

	static TextEditor SE_CodeEditor;
	static NodeEditor SE_NodeEditor;

private:

	static const std::string edit_mode[3];
	static const std::string shader_type[2];
	int active_func{0};

	int current_edit = 0, current_shad_type = 0;
	bool is_mode_changed = true, is_shad_type_changed = true;
	bool add_button = false;
	bool sel;

public:

	ShaderEditor();
	ShaderEditor(const std::string& name);
	static void InitEditors();
	~ShaderEditor();

public:

	mutable MiniPropPanel Panel;
	mutable S_U add_prop;
	mutable S_func add_args;
	mutable char add_name[CHAR_MAX];

	bool AddParam(const char* c_name = "", const char* c_sld_name = "");
	bool AddStruct(bool def_type = false);
	bool AddLink();
	void CompileShader();

public:

	void UpdateShaderEditor(const std::string& _code = "") const;
	void UpdateLayer() override;
	static void UpdateKeyword();
	static Shaders* GetActiveShaderPtr();
	static Shaders::ShaderUnit* GetShaderUnitPtr(ShaderType tar);

public:
	void RenderName(const std::string& _label, std::string* _name, float _width = 0.0f, bool read_only = true) const;
	void RenderName(const char* _label, std::string* _name, float _width = 0.0f, bool read_only = true) const;
	void RenderShaderStruct();
	void RenderLayout(int* _loc, std::string* _name, ParaType* _type) const;
	void RenderSSBO(int* _loc, std::string* _name, ParaType* _type, Args* _args) const;
	void RenderArg(Arg& _arg, int _index, bool _is_editable = true) const;
	void RenderArg(ParaType& _type, std::string& _name, int _index, bool _is_editable = true) const;
	void RenderArgs(Args& args, int _type) const;

	void RenderLayer() override;
};