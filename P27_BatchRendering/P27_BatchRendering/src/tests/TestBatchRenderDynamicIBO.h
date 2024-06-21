#pragma once

#include "Test.h"

#include <memory>

#include "glm/glm.hpp"
#include "Renderer.h"
#include "Texture.h"

namespace test {

	class TestBatchRenderDynamicIBO : public Test
	{
	public:
		TestBatchRenderDynamicIBO();
		~TestBatchRenderDynamicIBO();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

	private:
		std::unique_ptr<IndexBuffer> m_Ibo;
		std::unique_ptr<VertexBuffer> m_Vbo;
		std::unique_ptr<VertexArray> m_Vao;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture1;
		std::unique_ptr<Texture> m_Texture2;
		glm::mat4 m_Proj;
		glm::mat4 m_View;


		glm::vec2 leftQuadPosition;
		glm::vec2 rightQuadPosition;

		glm::vec3 transition;

	};
}