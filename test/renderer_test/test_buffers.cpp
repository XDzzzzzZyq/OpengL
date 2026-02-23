#include "pch.h"
#include "macros.h"

#include "buffers/VertexBuffer.h"
#include "buffers/IndexBuffer.h"
#include "buffers/StorageBuffer.h"
#include "buffers/UniformBuffer.h"

// ---------------------------------------------------------------------------
// VertexBuffer
// ---------------------------------------------------------------------------

TEST_F(RendererEnvir, VertexBuffer_RAII) {
	if (gl_version < 4.0)
		GTEST_SKIP();

	const std::vector<float> data = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f };

	// Construction
	VertexBuffer vbo(data);
	EXPECT_NE(vbo.GetID(), 0u);
	EXPECT_EQ(vbo.GetSize(), data.size() * sizeof(float));
	GLERRTEST;

	// Copy constructor - deep copy creates a new GPU buffer
	VertexBuffer vbo_copy = vbo;
	EXPECT_NE(vbo_copy.GetID(), 0u);
	EXPECT_NE(vbo_copy.GetID(), vbo.GetID());
	EXPECT_EQ(vbo_copy.GetSize(), vbo.GetSize());
	GLERRTEST;

	// Move constructor - ownership transferred, source invalidated
	GLuint orig_id = vbo.GetID();
	VertexBuffer vbo_moved = std::move(vbo);
	EXPECT_EQ(vbo_moved.GetID(), orig_id);
	EXPECT_EQ(vbo.GetID(), 0u);
	GLERRTEST;

	// Copy assignment - replaces contents with deep copy
	VertexBuffer vbo_assign;
	vbo_assign = vbo_copy;
	EXPECT_NE(vbo_assign.GetID(), 0u);
	EXPECT_NE(vbo_assign.GetID(), vbo_copy.GetID());
	EXPECT_EQ(vbo_assign.GetSize(), vbo_copy.GetSize());
	GLERRTEST;

	// Move assignment
	GLuint copy_id = vbo_copy.GetID();
	VertexBuffer vbo_mova;
	vbo_mova = std::move(vbo_copy);
	EXPECT_EQ(vbo_mova.GetID(), copy_id);
	EXPECT_EQ(vbo_copy.GetID(), 0u);
	GLERRTEST;

	// Verify deep-copy data integrity
	{
		const std::vector<float> src_data = { 10.0f, 20.0f, 30.0f };
		VertexBuffer src(src_data);
		VertexBuffer dst = src;

		std::vector<float> read_back(src_data.size(), 0.0f);
		glBindBuffer(GL_ARRAY_BUFFER, dst.GetID());
		glGetBufferSubData(GL_ARRAY_BUFFER, 0, dst.GetSize(), read_back.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		GLERRTEST;

		EXPECT_EQ(read_back, src_data);
	}
}

// ---------------------------------------------------------------------------
// IndexBuffer
// ---------------------------------------------------------------------------

TEST_F(RendererEnvir, IndexBuffer_RAII) {
	if (gl_version < 4.0)
		GTEST_SKIP();

	const GLuint indices[] = { 0, 1, 2, 2, 3, 0 };
	const GLuint byte_size = sizeof(indices);

	IndexBuffer ibo(indices, byte_size);
	EXPECT_NE(ibo.GetID(), 0u);
	EXPECT_EQ(ibo.GetSize(), byte_size);
	EXPECT_EQ(ibo.Count(), 6u);
	GLERRTEST;

	// Copy constructor
	IndexBuffer ibo_copy = ibo;
	EXPECT_NE(ibo_copy.GetID(), 0u);
	EXPECT_NE(ibo_copy.GetID(), ibo.GetID());
	EXPECT_EQ(ibo_copy.Count(), ibo.Count());
	GLERRTEST;

	// Move constructor
	GLuint orig_id = ibo.GetID();
	IndexBuffer ibo_moved = std::move(ibo);
	EXPECT_EQ(ibo_moved.GetID(), orig_id);
	EXPECT_EQ(ibo.GetID(), 0u);
	GLERRTEST;

	// Verify deep-copy data integrity
	{
		const GLuint src_idx[] = { 5, 4, 3, 2, 1, 0 };
		IndexBuffer src(src_idx, sizeof(src_idx));
		IndexBuffer dst = src;

		std::vector<GLuint> read_back(6, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dst.GetID());
		glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, dst.GetSize(), read_back.data());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		GLERRTEST;

		for (int i = 0; i < 6; i++)
			EXPECT_EQ(read_back[i], src_idx[i]);
	}
}

// ---------------------------------------------------------------------------
// StorageBuffer
// ---------------------------------------------------------------------------

TEST_F(RendererEnvir, StorageBuffer_RAII) {
	if (gl_version < 4.6)
		GTEST_SKIP();

	const std::vector<float> src_data = { 1.f, 2.f, 3.f, 4.f };

	StorageBuffer ssbo(FLOAT_LIST, 0);
	ssbo.GenStorageBuffer(src_data);
	EXPECT_NE(ssbo.GetID(), 0u);
	EXPECT_EQ(ssbo.GetSize(), src_data.size() * sizeof(float));
	GLERRTEST;

	// Copy constructor - deep copy
	StorageBuffer ssbo_copy = ssbo;
	EXPECT_NE(ssbo_copy.GetID(), 0u);
	EXPECT_NE(ssbo_copy.GetID(), ssbo.GetID());
	EXPECT_EQ(ssbo_copy.GetSize(), ssbo.GetSize());
	EXPECT_EQ(ssbo_copy.GetBase(), ssbo.GetBase());
	EXPECT_EQ(ssbo_copy.GetType(), ssbo.GetType());
	GLERRTEST;

	// Move constructor
	GLuint orig_id = ssbo.GetID();
	StorageBuffer ssbo_moved = std::move(ssbo);
	EXPECT_EQ(ssbo_moved.GetID(), orig_id);
	EXPECT_EQ(ssbo.GetID(), 0u);
	GLERRTEST;

	// Verify deep-copy data integrity
	{
		const std::vector<float> vals = { 7.f, 8.f, 9.f };
		StorageBuffer src(FLOAT_LIST, 1);
		src.GenStorageBuffer(vals);
		StorageBuffer dst = src;

		std::vector<float> read_back;
		dst.ReadStorageBuffer(read_back);
		GLERRTEST;

		ASSERT_EQ(read_back.size(), vals.size());
		for (size_t i = 0; i < vals.size(); i++)
			EXPECT_FLOAT_EQ(read_back[i], vals[i]);
	}
}

// ---------------------------------------------------------------------------
// UniformBuffer
// ---------------------------------------------------------------------------

TEST_F(RendererEnvir, UniformBuffer_RAII) {
	if (gl_version < 4.0)
		GTEST_SKIP();

	struct TestData { float a, b, c, d; };
	TestData td{ 1.f, 2.f, 3.f, 4.f };

	UniformBuffer ubo(0, td);
	EXPECT_NE(ubo.GetID(), 0u);
	EXPECT_EQ(ubo.GetSize(), sizeof(TestData));
	GLERRTEST;

	// Copy constructor - deep copy
	UniformBuffer ubo_copy = ubo;
	EXPECT_NE(ubo_copy.GetID(), 0u);
	EXPECT_NE(ubo_copy.GetID(), ubo.GetID());
	EXPECT_EQ(ubo_copy.GetSize(), ubo.GetSize());
	GLERRTEST;

	// Move constructor
	GLuint orig_id = ubo.GetID();
	UniformBuffer ubo_moved = std::move(ubo);
	EXPECT_EQ(ubo_moved.GetID(), orig_id);
	EXPECT_EQ(ubo.GetID(), 0u);
	GLERRTEST;

	// Verify deep-copy data integrity
	{
		TestData src_td{ 10.f, 20.f, 30.f, 40.f };
		UniformBuffer src(0, src_td);
		UniformBuffer dst = src;

		TestData read_back{};
		glBindBuffer(GL_UNIFORM_BUFFER, dst.GetID());
		glGetBufferSubData(GL_UNIFORM_BUFFER, 0, dst.GetSize(), &read_back);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		GLERRTEST;

		EXPECT_FLOAT_EQ(read_back.a, src_td.a);
		EXPECT_FLOAT_EQ(read_back.b, src_td.b);
		EXPECT_FLOAT_EQ(read_back.c, src_td.c);
		EXPECT_FLOAT_EQ(read_back.d, src_td.d);
	}
}
