/**
 * @file AssetManager.h
 * @brief Unified asset cache and file-based resource management system.
 *
 * AssetManager provides a centralized, type-keyed cache for GPU-backed assets
 * such as MeshData, Texture, and ComputeShader. It replaces the per-class static
 * maps previously scattered across MeshLib, TextureLib, and ComputeShader, and
 * serves as the single source of truth for loaded-resource lifetimes.
 *
 * Design:
 * - Per-type function-local statics avoid the static-initialization-order problem.
 * - Resources are stored as shared_ptr; callers share ownership via returned handles.
 * - Clear<T>() drops the manager's references; externally-held shared_ptrs remain valid.
 * - Load<T>() accepts a factory callable to keep asset creation logic in the owning layer.
 *
 * Usage:
 * @code
 * // Load or retrieve a cached MeshData
 * auto mesh = AssetManager::Load<MeshData>("monkey.obj", []() {
 *     auto m = std::make_shared<MeshData>();
 *     m->LoadObj("monkey.obj");
 *     return m;
 * });
 *
 * // Retrieve without loading
 * auto tex = AssetManager::Get<Texture>("albedo.png");  // nullptr if not cached
 *
 * // Register a resource directly
 * AssetManager::Register<Texture>("noise", noise_ptr);
 *
 * // Release all cached textures
 * AssetManager::Clear<Texture>();
 * @endcode
 *
 * @note Not thread-safe. All access must occur on the OpenGL/main thread.
 * @note Header-only; no compilation unit required.
 */

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

/**
 * @brief Unified asset cache for file-based GPU resources.
 *
 * All methods are static. The cache is partitioned per asset type T via
 * function-local statics inside GetRegistry<T>(), avoiding SIOF issues.
 */
class AssetManager
{
private:

	/**
	 * @brief Returns the per-type cache map.
	 * @tparam T Asset type
	 * @return Reference to the type-specific cache map
	 * @note Each instantiation owns an independent static map.
	 */
	template<typename T>
	static std::unordered_map<std::string, std::shared_ptr<T>>& GetRegistry()
	{
		static std::unordered_map<std::string, std::shared_ptr<T>> s_registry;
		return s_registry;
	}

public:

	/**
	 * @brief Returns a cached asset by path key, or nullptr if not found.
	 * @tparam T Asset type
	 * @param path Asset path key (typically the file path used to load)
	 * @return Shared pointer to the cached asset, or nullptr
	 */
	template<typename T>
	static std::shared_ptr<T> Get(const std::string& path)
	{
		auto& reg = GetRegistry<T>();
		auto it = reg.find(path);
		return it != reg.end() ? it->second : nullptr;
	}

	/**
	 * @brief Returns true if an asset is currently cached under the given path.
	 * @tparam T Asset type
	 * @param path Asset path key
	 */
	template<typename T>
	static bool Contains(const std::string& path)
	{
		return GetRegistry<T>().count(path) != 0;
	}

	/**
	 * @brief Stores a pre-created asset in the cache.
	 * @tparam T Asset type
	 * @param path Asset path key
	 * @param asset Shared pointer to the asset to cache
	 * @note Overwrites any existing entry with the same key.
	 */
	template<typename T>
	static void Register(const std::string& path, std::shared_ptr<T> asset)
	{
		GetRegistry<T>()[path] = std::move(asset);
	}

	/**
	 * @brief Loads or retrieves a cached asset using a factory callable.
	 *
	 * If an asset with @p path is already cached, returns the cached instance.
	 * Otherwise calls @p factory to create a new asset, caches it, and returns it.
	 *
	 * @tparam T       Asset type
	 * @tparam Factory Callable type with signature `std::shared_ptr<T>()`
	 * @param path     Asset path key
	 * @param factory  Callable that produces a std::shared_ptr<T>
	 * @return         Shared pointer to the asset (cached or newly created)
	 */
	template<typename T, typename Factory>
	static std::shared_ptr<T> Load(const std::string& path, Factory factory)
	{
		auto& reg = GetRegistry<T>();
		auto it = reg.find(path);
		if (it != reg.end())
			return it->second;

		auto asset = factory();
		reg[path] = asset;
		return asset;
	}

	/**
	 * @brief Evicts all cached assets of type T.
	 * @tparam T Asset type to clear
	 * @note Externally-held shared_ptrs remain valid after this call.
	 */
	template<typename T>
	static void Clear()
	{
		GetRegistry<T>().clear();
	}
};
