#include "GraphicsTests.h"

#include "graphics/MathGL.h"
#include "math/Mat.h"

#include "glm/gtc/epsilon.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/matrix.hpp"

#include "catch.hpp"

#include <iostream>

bool glmMatEqual(const glm::mat4 &lhs, const glm::mat4 &rhs)
{
	for (size_t i = 0; i < 4; i++)
	{
		if(glm::epsilonEqual(lhs[i], rhs[i], 0.001f) != 
			glm::bvec4(true, true, true, true)) { return false; }
	}
	return true;
}

TEST_CASE("Matrix Transformations", "[graphics][math][matrix][transformation]") 
{
	SECTION("Inverse")
	{
		Math::fMat4 m(4.f, 2.f, 7.f, 6.f, 5.f, 3.f, -0.324f, 0.2f, 213.f, 43.f, 2.f, 5.f, 2.f, 1.f, 6.f, 3.f);
		glm::mat4 mg(4.f, 2.f, 7.f, 6.f, 5.f, 3.f, -0.324f, 0.2f, 213.f, 43.f, 2.f, 5.f, 2.f, 1.f, 6.f, 3.f);
		m = Math::GL::Inverse(m);
		mg = glm::inverse(mg);
		CHECK(glmMatEqual(glm::make_mat4((float *)&m.e), mg));
	}

	SECTION("Look At")
	{
		Math::fMat4 m = Math::GL::LookAt(fVec3(32.f, 0.432f, -23.f), fVec3(645.f, -0.43f, 2.2f), fVec3(0.34f, 32.f, 543.f));
		glm::mat4 mg = glm::lookAt(glm::vec3(32.f, 0.432f, -23.f), glm::vec3(645.f, -0.43f, 2.2f), glm::vec3(0.34f, 32.f, 543.f));
		CHECK(glmMatEqual(glm::make_mat4((float *)&m.e), mg));
	}

	SECTION("Orthographic Transformation")
	{
		Math::fMat4 m = Math::GL::Orthographic(-2.f, 3.f, -5.f, 6.f, 0.0432f, 987.f);
		glm::mat4 mg = glm::ortho(-2.f, 3.f, -5.f, 6.f, 0.0432f, 987.f);
		CHECK(glmMatEqual(glm::make_mat4((float *)&m.e), mg));
	}

	SECTION("Perspective Transformation")
	{
		Math::fMat4 m = Math::GL::Perspective(Math::Deg2Rad * 60.f, 16.f/9.f, 0.0432f, 987.f);
		glm::mat4 mg = glm::perspective(glm::radians(60.f), 16.f/9.f, 0.0432f, 987.f);
		CHECK(glmMatEqual(glm::make_mat4((float *)&m.e), mg));
	}

	SECTION("Rotation")
	{
		Math::fMat4 m = Math::GL::Rotate(Math::Mat4(), Math::Deg2Rad * 65.f, fVec3(353.f, -643.f, -0.0432f));
		glm::mat4 mg = glm::rotate(glm::identity<glm::mat4>(), glm::radians(65.f), glm::vec3(353.f, -643.f, -0.0432f));
		CHECK(glmMatEqual(glm::make_mat4((float *)&m.e), mg));
	}

	SECTION("Scale")
	{
		Math::fMat4 m = Math::GL::Scale(Math::Mat4(), fVec3(353.f, -643.f, -0.0432f));
		glm::mat4 mg = glm::scale(glm::identity<glm::mat4>(), glm::vec3(353.f, -643.f, -0.0432f));
		CHECK(glmMatEqual(glm::make_mat4((float *)&m.e), mg));
	}

	SECTION("Translation")
	{
		Math::fMat4 m = Math::GL::Translate(Math::Mat4(), fVec3(353.f, -643.f, -0.0432f));
		glm::mat4 mg = glm::translate(glm::identity<glm::mat4>(), glm::vec3(353.f, -643.f, -0.0432f));
		CHECK(glmMatEqual(glm::make_mat4((float *)&m.e), mg));
	}
}