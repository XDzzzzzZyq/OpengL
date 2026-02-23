/**
 * @file SceneManager.h
 * @brief Factory for creating preset scenes.
 *
 * Provides SceneManager (factory for preset scenes). The Scene class has been
 * moved to src/scene/Scene.h.
 *
 * Architecture:
 * - SceneManager: Static factory for creating preset scenes (Main, CornellBox, etc.)
 * - Scenes are cached by name for reuse across sessions
 *
 * @note Preset scenes are hardcoded (not loaded from files).
 */

#pragma once

#include "Scene.h"

/**
 * @brief Factory for creating preset scenes.
 *
 * SceneManager provides static factory methods to instantiate common test scenes
 * (Main, CornellBox, SDF_Test, Shadow). Scenes are cached by name to enable
 * reuse across sessions.
 *
 * Caching Strategy:
 * - Scenes stored in sce_configs map (name -> Scene)
 * - First call creates and caches scene
 * - Subsequent calls return existing scene
 * - Useful for quick scene switching without reload
 *
 * @note All methods are static (no instance required).
 * @note Preset scenes are hardcoded (not loaded from files).
 */
class SceneManager
{
private:

	static std::unordered_map<std::string, std::shared_ptr<Scene>> sce_configs; ///< Scene cache (name -> scene)

public:

	/**
	 * @brief Creates or retrieves the "Main" test scene.
	 * @param _name Scene name (default: "scene1")
	 * @return Shared pointer to Scene
	 * @note Contains basic test geometry, camera, and lighting.
	 */
	static std::shared_ptr<Scene> Main(std::string _name="scene1");

	/**
	 * @brief Creates or retrieves the Cornell Box scene.
	 * @param _name Scene name (default: "cornellbox")
	 * @return Shared pointer to Scene
	 * @note Classic Cornell Box setup for testing global illumination.
	 */
	static std::shared_ptr<Scene> CornellBox(std::string _name="cornellbox");

	/**
	 * @brief Creates or retrieves the SDF test scene.
	 * @param _name Scene name (default: "SDF test")
	 * @return Shared pointer to Scene
	 * @note Scene for testing Signed Distance Field soft shadows.
	 */
	static std::shared_ptr<Scene> SDF_Test(std::string _name="SDF test");

	/**
	 * @brief Creates or retrieves the shadow test scene.
	 * @param _name Scene name (default: "shadow test")
	 * @return Shared pointer to Scene
	 * @note Scene for testing shadow mapping techniques.
	 */
	static std::shared_ptr<Scene> Shadow(std::string _name = "shadow test");

	/**
	 * @brief Retrieves cached scene by name.
	 * @param _name Scene name to lookup
	 * @return Shared pointer to Scene, or nullptr if not cached
	 */
	static std::shared_ptr<Scene> GetSceneConfig(std::string _name);
};

