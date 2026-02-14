#pragma once

#include "ImGui/imgui.h"

#include "Context.h"
#include "Events.h"

#include "ImguiMenu.h"
#include "ImguiLayer.h"
#include "ImguiItem.h"

#include <vector>
#include <unordered_map>

/*#define ParaUpdate ParaUpdate*/

class ImguiManager
{
private:
	std::vector<std::shared_ptr<ImguiLayer>> layer_list;
	std::vector<std::shared_ptr<ImguiMenu>> menu_list;
	
	ImGuiIO* io = nullptr;
	ImGuiStyle* m_style= nullptr;

	mutable std::unordered_map<std::string, int> layer_name_buffer;  //name | ID
	mutable std::unordered_map<std::string, int> menu_name_buffer;   //name | ID

public:
	static bool is_prefW_open;

public:

	ImguiManager();
	void Init(EventPool& evt);
	void _debug() const;

public:
	void SetConfigFlag(ImGuiConfigFlags_ flag) const { io->ConfigFlags |= flag; };
	void SetBackendFlag(ImGuiBackendFlags_ flag) const { io->BackendFlags |= flag; };
	ImGuiIO* GetIO()const { return io; }
	ImGuiStyle* GetStyle()const { return m_style; }

	void DefultViewports();
	void RegisterLayerEvents(EventPool& evt);
	void RegistarMenuEvents(EventPool& evt);

public:
	void NewFrame() const;
	void RenderUI(const SceneContext& ctx, const EventPool& evt, bool rend = true);
	void Terminate() const;

public:
	void PushImguiMenu(std::shared_ptr<ImguiMenu> _menu);
	std::shared_ptr<ImguiMenu> CreateImguiMenu(std::string name);

	ImguiMenu* FindImguiMenu(const std::string& name) const;
	ImguiMenuItem* FindImguiMenuItem(const std::string& menu, const std::string& submenu) const;

public:
	void PushImguiLayer(std::shared_ptr<ImguiLayer> layer);
	template<class LayerType>
	std::shared_ptr<LayerType> CreateImguiLayer(std::string name);

	ImguiLayer* FindImguiLayer(const std::string& name)const;
	ImguiLayer* FindImguiLayer(int id)const;
	template<ImguiLayerType T, typename... Args>
	T* FindImguiLayerAs(Args&&... args) const
	{
		ImguiLayer* base = FindImguiLayer(std::forward<Args>(args)...);
		return static_cast<T*>(base);
	}

	ImguiItem* FindImguiItem(const std::string& layer, const std::string& name) const;
	ImguiItem* FindImguiItem(int id, const std::string& name) const;
	ImguiItem* FindImguiItem(int id, int item_id) const;
	template<ImguiItemType T, typename... Args>
	T* FindImguiItemAs(Args&&... args) const
	{
		ImguiItem* base = FindImguiItem(std::forward<Args>(args)...);
		return static_cast<T*>(base);
	}

public:
	void ActivateLayer(const std::string& name);

public:
	void SetButtonFunc(const std::string& ly_name, const std::string& it_name, const std::function<void(void)>& func);
	Parameters* GetParaValue(const std::string& ly_name, const std::string& it_name);

public:
	mutable std::function<void(void)> ParaUpdate = [] {};

};

template<class LayerType>
std::shared_ptr<LayerType> ImguiManager::CreateImguiLayer(std::string name)
{
	std::shared_ptr<LayerType> layer = std::make_shared<LayerType>(name);
	PushImguiLayer(std::dynamic_pointer_cast<ImguiLayer>(layer));

	return layer;
}

