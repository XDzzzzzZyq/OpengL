#pragma once
#include "ImguiLayer.h"
#include "EventListener.h"
#include "Shaders.h"
#include "ITEM/TextEditor.h"

class ShaderEditor : public ImguiLayer, public EventListener
{
private:
	static std::string edit_mode[3];
	static std::string shader_type[2];
	static TextEditor Editor;

	mutable int current_edit = 0, current_shad_type = 0;
	mutable bool is_edit_changed = true, is_shad_type_changed = true;
	mutable bool sel;
public:
	ShaderEditor();
	ShaderEditor(const std::string& name);
	~ShaderEditor();
public:
	bool AddParam();
	bool AddStruct();
	bool AddLink();
	void CompileShader() const;
public:
	void UpdateShaderEditor();
	void UpdateLayer() override;
	void RenderShaderStruct() const;
	void RenderLayer() const override;
};

class MiniPropPanel
{
private:
	bool is_open;
public:
	MiniPropPanel();
	~MiniPropPanel();

	void RenderPanel(const ImVec2& pos) const;
};