#include "pch.h"
#include "AssetManager.h"

// Dummy asset types used only in these tests to avoid polluting real asset caches.
struct DummyMesh  { std::string path; };
struct DummyTex   { int width = 0; };

TEST(AssetManager, RegisterAndGet)
{
	AssetManager::Clear<DummyMesh>();

	auto mesh = std::make_shared<DummyMesh>();
	mesh->path = "test.obj";
	AssetManager::Register<DummyMesh>("test.obj", mesh);

	auto result = AssetManager::Get<DummyMesh>("test.obj");
	ASSERT_NE(result, nullptr);
	EXPECT_EQ(result->path, "test.obj");
	EXPECT_EQ(result.get(), mesh.get()); // same instance
}

TEST(AssetManager, GetMissing)
{
	AssetManager::Clear<DummyMesh>();

	auto result = AssetManager::Get<DummyMesh>("nonexistent.obj");
	EXPECT_EQ(result, nullptr);
}

TEST(AssetManager, Contains)
{
	AssetManager::Clear<DummyTex>();

	EXPECT_FALSE(AssetManager::Contains<DummyTex>("a.png"));

	AssetManager::Register<DummyTex>("a.png", std::make_shared<DummyTex>());

	EXPECT_TRUE(AssetManager::Contains<DummyTex>("a.png"));
}

TEST(AssetManager, Load_CachesOnFirstCall)
{
	AssetManager::Clear<DummyMesh>();

	int factory_calls = 0;
	auto factory = [&]() -> std::shared_ptr<DummyMesh> {
		++factory_calls;
		auto m = std::make_shared<DummyMesh>();
		m->path = "cached.obj";
		return m;
	};

	auto first  = AssetManager::Load<DummyMesh>("cached.obj", factory);
	auto second = AssetManager::Load<DummyMesh>("cached.obj", factory);

	EXPECT_EQ(factory_calls, 1); // factory called only once
	EXPECT_EQ(first.get(), second.get()); // same instance returned
	EXPECT_EQ(first->path, "cached.obj");
}

TEST(AssetManager, Load_DifferentKeys)
{
	AssetManager::Clear<DummyMesh>();

	auto a = AssetManager::Load<DummyMesh>("a.obj", []() {
		auto m = std::make_shared<DummyMesh>(); m->path = "a.obj"; return m;
	});
	auto b = AssetManager::Load<DummyMesh>("b.obj", []() {
		auto m = std::make_shared<DummyMesh>(); m->path = "b.obj"; return m;
	});

	EXPECT_NE(a.get(), b.get());
	EXPECT_EQ(a->path, "a.obj");
	EXPECT_EQ(b->path, "b.obj");
}

TEST(AssetManager, Clear_ReleasesManagerReference)
{
	AssetManager::Clear<DummyMesh>();

	std::weak_ptr<DummyMesh> weak;
	{
		auto mesh = AssetManager::Load<DummyMesh>("weak.obj", []() {
			return std::make_shared<DummyMesh>();
		});
		weak = mesh;
		EXPECT_FALSE(weak.expired());
	}
	// mesh local is gone; manager still holds it
	EXPECT_FALSE(weak.expired());

	AssetManager::Clear<DummyMesh>();
	// after Clear the manager's reference is dropped
	EXPECT_TRUE(weak.expired());
}

TEST(AssetManager, TypesAreIndependent)
{
	AssetManager::Clear<DummyMesh>();
	AssetManager::Clear<DummyTex>();

	AssetManager::Register<DummyMesh>("asset", std::make_shared<DummyMesh>());

	// DummyTex cache should not be affected
	EXPECT_FALSE(AssetManager::Contains<DummyTex>("asset"));
	EXPECT_TRUE(AssetManager::Contains<DummyMesh>("asset"));

	AssetManager::Clear<DummyMesh>();
	EXPECT_FALSE(AssetManager::Contains<DummyMesh>("asset"));
}
