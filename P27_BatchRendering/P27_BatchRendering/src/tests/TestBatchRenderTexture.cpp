
#include "TestBatchRenderTexture.h"

#include "Renderer.h"
#include "imgui/imgui.h"

#include "VertexBufferLayout.h"

#include "glm/ext.hpp"
#include "glm/glm.hpp"

namespace test {

	TestBatchRenderTexture::TestBatchRenderTexture()
	{
        //float positions[] =
        //{
        //   -1.5f, -0.5f, 0.0f,
        //   -0.5f, -0.5f, 0.0f,
        //   -0.5f,  0.5f, 0.0f,
        //   -1.5f,  0.5f, 0.0f,

        //    0.5f, -0.5f, 0.0f,
        //    1.5f, -0.5f, 0.0f,
        //    1.5f,  0.5f, 0.0f,
        //    0.5f,  0.5f, 0.0f
        //};

        float positions[] =
        {
           -1.5f, -0.5f, 0.0f, 0.8f, 0.1f, 0.1f, 1.0f, 0.0f, 0.0f, 0.0f,
           -0.5f, -0.5f, 0.0f, 0.8f, 0.1f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f,
           -0.5f,  0.5f, 0.0f, 0.8f, 0.1f, 0.1f, 1.0f, 1.0f, 1.0f, 0.0f,
           -1.5f,  0.5f, 0.0f, 0.8f, 0.1f, 0.1f, 1.0f, 0.0f, 1.0f, 0.0f,

            0.5f, -0.5f, 0.0f, 0.2f, 0.6f, 0.1f, 1.0f, 0.0f, 0.0f, 1.0f,
            1.5f, -0.5f, 0.0f, 0.2f, 0.6f, 0.1f, 1.0f, 1.0f, 0.0f, 1.0f,
            1.5f,  0.5f, 0.0f, 0.2f, 0.6f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f,
            0.5f,  0.5f, 0.0f, 0.2f, 0.6f, 0.1f, 1.0f, 0.0f, 1.0f, 1.0f
        };

        unsigned int indices[] =
        {
            0, 1, 2,
            2, 3, 0,

            4, 5, 6,
            6, 7, 4
        };

        m_Ibo = std::make_unique<IndexBuffer>(indices, 6 * 2);
        m_Vbo = std::make_unique<VertexBuffer>(positions, sizeof(float) * 10 * 8);

        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(4);
        layout.Push<float>(2);
        layout.Push<float>(1);


        m_Vao = std::make_unique<VertexArray>();
        m_Vao->AddBuffer(*m_Vbo, layout);

        m_Vao->Bind();

        m_Shader = std::make_unique<Shader>("res/shaders/BatchRenderTexture.shader");
        m_Shader->Bind();

        m_Proj = glm::ortho(-4.0f, 4.0f, -2.0f, 2.0f, -1.0f, 1.0f);
        m_View = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));


        m_Texture1 = std::make_unique<Texture>("res/textures/logo.jpg");
        m_Texture2 = std::make_unique<Texture>("res/textures/logo2.png"); //注意这里，必须先构造完毕再同时绑定。构造绑定+构造绑定是行不通的
        m_Texture1->Bind(0);
        m_Texture2->Bind(1);
	}

    TestBatchRenderTexture::~TestBatchRenderTexture()
	{

	}

	void TestBatchRenderTexture::OnUpdate(float deltaTime)
	{

	}

	void TestBatchRenderTexture::OnRender()
	{
		GLCall(glClearColor(0.3f, 0.0f, 0.2f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 mvp = m_Proj * m_View * model;

        Renderer renderer;
        m_Shader->Bind();
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        m_Shader->SetUniform2i("u_Textures", 0, 1);

        renderer.Draw(*m_Vao, *m_Ibo, *m_Shader);
	}

	void TestBatchRenderTexture::OnImGuiRender()
	{
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}

