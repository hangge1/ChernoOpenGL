
#include "TestBatchRenderDynamicIBO.h"

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

    static Vertex* CreatreQuead(Vertex* v, float x, float y, float texIndex)
    {
        float size = 1.0f;

        //-1.5f -0.5f

        v->position = { x, y, 0.0f };
        v->color = { 0.8f, 0.1f, 0.1f, 1.0f };
        v->texCoord = { 0.0f, 0.0f };
        v->texIndex = texIndex;
        v++;

        v->position = { x + size, y, 0.0f };
        v->color = { 0.8f, 0.1f, 0.1f, 1.0f };
        v->texCoord = { 1.0f, 0.0f };
        v->texIndex = texIndex;
        v++;

        
        v->position = { x + size,  y + size, 0.0f };
        v->color = { 0.8f, 0.1f, 0.1f, 1.0f };
        v->texCoord = { 1.0f, 1.0f };
        v->texIndex = texIndex;
        v++;

        v->position = { x,  y + size, 0.0f };
        v->color = { 0.8f, 0.1f, 0.1f, 1.0f };
        v->texCoord = { 0.0f, 1.0f };
        v->texIndex = texIndex;
        v++;

        return v;
    }

	TestBatchRenderDynamicIBO::TestBatchRenderDynamicIBO()
        : leftQuadPosition({ -1.5f, -0.5f }), rightQuadPosition({ 0.5f, -0.5f }), transition({0.0f,0.0f,0.0f})
	{
        
        m_Vbo = std::make_unique<VertexBuffer>(nullptr, sizeof(Vertex) * MaxVerticeCount, true);

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

        m_Proj = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, -1.0f, 1.0f);
        m_View = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));


        m_Texture1 = std::make_unique<Texture>("res/textures/logo.jpg");
        m_Texture2 = std::make_unique<Texture>("res/textures/logo2.png"); //注意这里，必须先构造完毕再同时绑定。构造绑定+构造绑定是行不通的
        m_Texture1->Bind(0);
        m_Texture2->Bind(1);
	}

    TestBatchRenderDynamicIBO::~TestBatchRenderDynamicIBO()
	{

	}

	void TestBatchRenderDynamicIBO::OnUpdate(float deltaTime)
	{
        const int MaxQuadCount = 1000;
        const int MaxVerticeCount = 4 * MaxQuadCount;
        const int MaxIndiceCount = 6 * MaxQuadCount;


        //构造动态索引数组
        unsigned int indices[MaxIndiceCount];
        int offset = 0;
        for (int i = 0; i < MaxQuadCount; i++)
        {
            int index = 6 * i;
            indices[index + 0] = offset + 0;
            indices[index + 1] = offset + 1;
            indices[index + 2] = offset + 2;

            indices[index + 3] = offset + 2;
            indices[index + 4] = offset + 3;
            indices[index + 5] = offset + 0;

            offset += 4;
        }

        m_Ibo = std::make_unique<IndexBuffer>(indices, MaxIndiceCount);
        //构造顶点
        Vertex vertices[MaxVerticeCount];
        Vertex* p = vertices;
        for (float x = -4.0f; x < 4.0f; x += 1.0)
        {
            for (float y = -4.0f; y < 4.0f; y += 1.0)
            {
                p = CreatreQuead(p, x, y, (int)std::abs(x + y) % 2);
            }
        }

       /* Vertex qs[8];
        memcpy(qs, q0.data(), q0.size() * sizeof(Vertex));
        memcpy(qs + q0.size(), q1.data(), q1.size() * sizeof(Vertex));*/

        m_Vbo->DynamicWriteData(0, sizeof(vertices), vertices);
	}

	void TestBatchRenderDynamicIBO::OnRender()
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

	void TestBatchRenderDynamicIBO::OnImGuiRender()
	{
        ImGui::Begin("Move");

        ImGui::SliderFloat3("Left", &leftQuadPosition.x, -2.0f, 1.0f);
        ImGui::SliderFloat3("Right", &rightQuadPosition.x, -2.0f, 1.5f);
        ImGui::SliderFloat3("Transilate", &transition.x, -1.0f, 1.0f);

        ImGui::End();


        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}

