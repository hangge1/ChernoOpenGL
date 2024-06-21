
#include "TestBatchRenderDynamic.h"

#include <array>

#include "Renderer.h"
#include "imgui/imgui.h"

#include "VertexBufferLayout.h"

#include "glm/ext.hpp"
#include "glm/glm.hpp"

namespace test {

    struct Vertex
    {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 texCoord;
        float texIndex;
    };

    static std::array<Vertex, 4> CreatreQuead(float x, float y, float texIndex)
    {
        float size = 1.0f;

        //-1.5f -0.5f

        Vertex v0;
        v0.position = { x, y, 0.0f };
        v0.color = { 0.8f, 0.1f, 0.1f, 1.0f };
        v0.texCoord = { 0.0f, 0.0f };
        v0.texIndex = texIndex;

        Vertex v1;
        v1.position = { x + size, y, 0.0f };
        v1.color = { 0.8f, 0.1f, 0.1f, 1.0f };
        v1.texCoord = { 1.0f, 0.0f };
        v1.texIndex = texIndex;

        Vertex v2;
        v2.position = { x + size,  y + size, 0.0f };
        v2.color = { 0.8f, 0.1f, 0.1f, 1.0f };
        v2.texCoord = { 1.0f, 1.0f };
        v2.texIndex = texIndex;

        Vertex v3;
        v3.position = { x,  y + size, 0.0f };
        v3.color = { 0.8f, 0.1f, 0.1f, 1.0f };
        v3.texCoord = { 0.0f, 1.0f };
        v3.texIndex = texIndex;

        return { v0, v1, v2, v3 };
    }

	TestBatchRenderDynamic::TestBatchRenderDynamic()
        : leftQuadPosition({ -1.5f, -0.5f }), rightQuadPosition({ 0.5f, -0.5f }), transition({0.0f,0.0f,0.0f})
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

        /*float positions[] =
        {
           -1.5f, -0.5f, 0.0f, 0.8f, 0.1f, 0.1f, 1.0f, 0.0f, 0.0f, 0.0f,
           -0.5f, -0.5f, 0.0f, 0.8f, 0.1f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f,
           -0.5f,  0.5f, 0.0f, 0.8f, 0.1f, 0.1f, 1.0f, 1.0f, 1.0f, 0.0f,
           -1.5f,  0.5f, 0.0f, 0.8f, 0.1f, 0.1f, 1.0f, 0.0f, 1.0f, 0.0f,

            0.5f, -0.5f, 0.0f, 0.2f, 0.6f, 0.1f, 1.0f, 0.0f, 0.0f, 1.0f,
            1.5f, -0.5f, 0.0f, 0.2f, 0.6f, 0.1f, 1.0f, 1.0f, 0.0f, 1.0f,
            1.5f,  0.5f, 0.0f, 0.2f, 0.6f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f,
            0.5f,  0.5f, 0.0f, 0.2f, 0.6f, 0.1f, 1.0f, 0.0f, 1.0f, 1.0f
        };*/

        unsigned int indices[] =
        {
            0, 1, 2,
            2, 3, 0,

            4, 5, 6,
            6, 7, 4
        };

        m_Ibo = std::make_unique<IndexBuffer>(indices, 6 * 2);
        m_Vbo = std::make_unique<VertexBuffer>(nullptr, sizeof(Vertex) * 1000, true);

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

    TestBatchRenderDynamic::~TestBatchRenderDynamic()
	{

	}

	void TestBatchRenderDynamic::OnUpdate(float deltaTime)
	{
        auto q0 = CreatreQuead(leftQuadPosition.x, leftQuadPosition.y, 0.0f);
        auto q1 = CreatreQuead(rightQuadPosition.x, rightQuadPosition.y, 1.0f);

        Vertex qs[8];
        memcpy(qs, q0.data(), q0.size() * sizeof(Vertex));
        memcpy(qs + q0.size(), q1.data(), q1.size() * sizeof(Vertex));

        m_Vbo->DynamicWriteData(0, sizeof(qs), qs);
	}

	void TestBatchRenderDynamic::OnRender()
	{
		GLCall(glClearColor(0.3f, 0.0f, 0.2f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

        glm::mat4 model = glm::translate(glm::mat4(1.0f), transition);
        glm::mat4 mvp = m_Proj * m_View * model;

        Renderer renderer;
        m_Shader->Bind();
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        m_Shader->SetUniform2i("u_Textures", 0, 1);

        renderer.Draw(*m_Vao, *m_Ibo, *m_Shader);
	}

	void TestBatchRenderDynamic::OnImGuiRender()
	{
        ImGui::Begin("Move");

        ImGui::SliderFloat3("Left", &leftQuadPosition.x, -2.0f, 1.0f);
        ImGui::SliderFloat3("Right", &rightQuadPosition.x, -2.0f, 1.5f);
        ImGui::SliderFloat3("Transilate", &transition.x, -1.0f, 1.0f);

        ImGui::End();


        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}

