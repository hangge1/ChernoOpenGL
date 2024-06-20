
#include "TestTexture2D.h"

#include "Renderer.h"
#include "imgui/imgui.h"

#include "VertexBufferLayout.h"

#include "glm/ext.hpp"
#include "glm/glm.hpp"

namespace test {

	TestTexture2D::TestTexture2D()
        : m_TranslateA(0.2f, 0.2f, 0.0f), 
          m_TranslateB(5.2f, 0.2f, 0.0f)
	{
        float positions[] =
        {
           -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, 1.0f, 0.0f,
            0.5f,  0.5f, 1.0f, 1.0f,
           -0.5f,  0.5f, 0.0f, 1.0f
        };

        unsigned int indices[] =
        {
            0, 1, 2,
            2, 3, 0
        };

        m_Ibo = std::make_unique<IndexBuffer>(indices, 6);
        m_Vbo = std::make_unique<VertexBuffer>(positions, sizeof(float) * 16);

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);

        m_Vao = std::make_unique<VertexArray>();
        m_Vao->AddBuffer(*m_Vbo, layout);


        m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        m_Shader->Bind();
        //shader.SetUniform4f("u_Color", uColor[0], uColor[1], uColor[2], uColor[3]);
        m_Shader->SetUniform1i("u_Texture", 0);

        m_Proj = glm::ortho(-5.0f, 5.0f, -3.0f, 3.0f, -1.0f, 1.0f);
        m_View = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, 0.0f)); //模拟的是向右移动相机


        m_Texture = std::make_unique<Texture>("res/textures/logo.png");
        m_Texture->Bind();
	}

	TestTexture2D::~TestTexture2D()
	{

	}

	void TestTexture2D::OnUpdate(float deltaTime)
	{

	}

	void TestTexture2D::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

        Renderer renderer;
        m_Shader->Bind();

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslateA);
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_Vao, *m_Ibo, *m_Shader);
        }
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslateB);
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_Vao, *m_Ibo, *m_Shader);
        }
	}

	void TestTexture2D::OnImGuiRender()
	{
        ImGui::SliderFloat3("TranslateA", &m_TranslateA.x, 0.0f, 10.0f);
        ImGui::SliderFloat3("TranslateB", &m_TranslateB.x, 0.0f, 10.0f);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}

