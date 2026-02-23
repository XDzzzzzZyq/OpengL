#include "SceneManager.h"

std::unordered_map<std::string, std::shared_ptr<Scene>> SceneManager::sce_configs;

std::shared_ptr<Scene> SceneManager::GetSceneConfig(std::string _name)
{
	if (SceneManager::sce_configs.find(_name) == SceneManager::sce_configs.end())
		return nullptr;

	return SceneManager::sce_configs[_name];
}
