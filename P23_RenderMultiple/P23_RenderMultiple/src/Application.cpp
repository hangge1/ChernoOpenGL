
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 640;
    int height = 480;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -2;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    //init glew
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        return -3;
    }

    GLCall(glViewport(0, 0, width, height));

    //开启混合后，相当于可以利用纹理alpha通道，带来半透明的效果！
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glEnable(GL_BLEND));

    { //解决在glfwTerminate之前让ib和vb析构
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

        IndexBuffer ib(indices, 6);
        VertexBuffer vb(positions, sizeof(float) * 16);
 
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);

        VertexArray va;
        va.AddBuffer(vb, layout);


        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        //shader.SetUniform4f("u_Color", uColor[0], uColor[1], uColor[2], uColor[3]);
        shader.SetUniform1i("u_Texture", 0);

        glm::mat4 proj = glm::ortho(-5.0f, 5.0f, -3.0f, 3.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, 0.0f)); //模拟的是向右移动相机
        

        va.UnBind();
        ib.UnBind();
        vb.UnBind();
        shader.UnBind();

        Texture texture("res/textures/logo.png");
        texture.Bind();

        Renderer renderer;

        //imgui init
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        const char* glsl_version = "#version 130";
        ImGui_ImplOpenGL3_Init(glsl_version);

        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        glm::vec3 translateA(0.2f, 0.2f, 0.0f);
        glm::vec3 translateB(5.2f, 0.2f, 0.0f);
        
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            shader.Bind();

            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translateA);
                glm::mat4 mvp = proj * view * model;
                shader.SetUniformMat4f("u_MVP", mvp);
                renderer.Draw(va, ib, shader);
            }
            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translateB);
                glm::mat4 mvp = proj * view * model;
                shader.SetUniformMat4f("u_MVP", mvp);
                renderer.Draw(va, ib, shader);
            }


            //至少要绑定的基本状态 shader + vao + ibo

            {
                static float f = 0.0f;
                static int counter = 0;

                ImGui::Begin("Hello, world!");
                ImGui::SliderFloat3("TranslateA", &translateA.x, 0.0f, 10.0f);
                ImGui::SliderFloat3("TranslateB", &translateB.x, 0.0f, 10.0f);

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();
            }

            


            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    //imgui Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}