#include "pch.h"
#include "macros.h"
#include "xdz_math.h"
#include "xdz_matrix.h"

#include "Transform.h"
TEST(Transform, Transform2D) {
	{
		Transform2D t2d{};
		t2d.SetPos({ 10.0f, 5.0f });
		t2d.SetRot(45.0f);
		t2d.SetScale({ 2.0f, 3.0f });

		EXPECT_TRUE(t2d.is_TransF_changed);

		EXPECT_TRUE(t2d.ApplyTransform());

		EXPECT_TRUE(!t2d.is_TransF_changed);
		EXPECT_TRUE(t2d.is_invTransF_changed);

		EXPECT_TRUE(t2d.GetInvTransform());
		EXPECT_TRUE(!t2d.is_invTransF_changed);

		EXPECT_TRUE(t2d.o_Transform * t2d.o_InvTransform == glm::mat3(1.0f));
		EXPECT_TRUE(t2d.o_InvTransform * t2d.o_Transform == glm::mat3(1.0f));
	}
}

inline bool mat4_near(
	const glm::mat4& a,
	const glm::mat4& b,
	float eps = 1e-5f)
{
	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			if (fabs(a[c][r] - b[c][r]) > eps)
				return false;
	return true;
}

TEST(Transform, Transform3D) {
	{
		/* Inverse Calculation */
		Transform3D t3d{};
		t3d.SetPos({ 10.0f, 5.0f, 15.0f });
		t3d.SetRot({ 45.0f, -20.0f, 15.0f });
		t3d.SetScale({ 2.0f, 3.0f, 4.0f });

		EXPECT_TRUE(t3d.is_TransF_changed);

		EXPECT_TRUE(t3d.ApplyTransform());

		EXPECT_TRUE(t3d.is_Uniform_changed);
		EXPECT_TRUE(!t3d.is_TransF_changed);
		EXPECT_TRUE(t3d.is_invTransF_changed);

		EXPECT_TRUE(t3d.GetInvTransform());
		EXPECT_TRUE(!t3d.is_invTransF_changed);
		EXPECT_TRUE(t3d.is_invUniform_changed);

		EXPECT_TRUE(mat4_near(t3d.o_Transform * t3d.o_InvTransform, glm::mat4(1.0f)));
		EXPECT_TRUE(mat4_near(t3d.o_InvTransform * t3d.o_Transform, glm::mat4(1.0f)));
	}
	{
		/* Set Transform */
		Transform3D t1{};
		const float dz = 10.0f, rx = 90.0f, sx = 2.0f, sy = 4.0f, sz = 8.0f;
		t1.SetPos({ 0.0f, 0.0f, dz });
		t1.SetRot({ rx, 0.0f, 0.0f });
		t1.SetScale({ sx, sy, sz });

		EXPECT_TRUE(t1.is_TransF_changed);
		EXPECT_TRUE(t1.ApplyTransform());

		Transform3D t2{};
		t2.SetTrans(t1.o_Transform);
		EXPECT_TRUE(!t2.is_TransF_changed);
		EXPECT_TRUE(glm::all(glm::epsilonEqual(t2.o_position, t1.o_position, 1e-5f)));
		EXPECT_TRUE(glm::all(glm::epsilonEqual(t2.o_rot, t1.o_rot, 1e-5f)));
		EXPECT_TRUE(glm::all(glm::epsilonEqual(t2.o_scale, t1.o_scale, 1e-5f)));

		Transform3D t3{};
		t3.SetTrans(t1.o_Transform, false, true, true);
		EXPECT_TRUE(t3.is_TransF_changed);
		EXPECT_TRUE(glm::all(glm::epsilonEqual(t3.o_position, glm::vec3(0.0f), 1e-5f)));
		EXPECT_TRUE(glm::all(glm::epsilonEqual(t3.o_rot, t1.o_rot, 1e-5f)));
		EXPECT_TRUE(glm::all(glm::epsilonEqual(t3.o_scale, t1.o_scale, 1e-5f)));
	}
	{
		/* Parenting */
		Transform3D t1{};
		const float dz = 10.0f, rx = 90.0f, sx = 2.0f, sy = 4.0f, sz = 8.0f;
		t1.SetPos({ 0.0f, 0.0f, dz });
		t1.SetRot({ rx, 0.0f, 0.0f });
		t1.SetScale({ sx, sy, sz });

		EXPECT_TRUE(t1.is_TransF_changed);

		Transform3D t2{};
		t2.SetParent(&t1, true);
		EXPECT_TRUE(!t1.is_TransF_changed);
		EXPECT_TRUE(t1.is_Uniform_changed);
		EXPECT_TRUE(t2.is_TransF_changed);
		EXPECT_TRUE(t2.is_Uniform_changed);

		EXPECT_TRUE(glm::all(glm::epsilonEqual(t2.o_position, glm::vec3(0.0f, -dz / sy, 0.0f), 1e-5f)));
		EXPECT_TRUE(glm::all(glm::epsilonEqual(t2.o_rot, glm::vec3(-rx, 0.0f, 0.0f), 1e-5f)));
		EXPECT_TRUE(glm::all(glm::epsilonEqual(t2.o_scale, glm::vec3(1.0f / sx, 1.0f / sz, 1.0f / sy), 1e-5f)));

		t1.ApplyAllTransform();
		EXPECT_TRUE(mat4_near(t2.o_Transform, glm::mat4(1.0f)));
		EXPECT_TRUE(!t1.is_TransF_changed);
		EXPECT_TRUE(!t2.is_TransF_changed);
		EXPECT_TRUE(t2.is_Uniform_changed);
		EXPECT_TRUE(t2.is_invTransF_changed);
	}
}