
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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

        float uColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
        float increment = 0.01f;

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        //shader.SetUniform4f("u_Color", uColor[0], uColor[1], uColor[2], uColor[3]);
        shader.SetUniform1i("u_Texture", 0);

        va.UnBind();
        ib.UnBind();
        vb.UnBind();
        shader.UnBind();

        Texture texture("res/textures/logo.png");
        texture.Bind();

        Renderer renderer;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            //至少要绑定的基本状态 shader + vao + ibo
            if (uColor[0] >= 1.0f)
            {
                increment = -0.01f;
            }
            else if (uColor[0] <= 0.0f)
            {
                increment = 0.01f;
            }
            uColor[0] += increment;

            renderer.Draw(va, ib, shader);
            //shader.SetUniform4f("u_Color", uColor[0], uColor[1], uColor[2], uColor[3]);

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    glfwTerminate();
    return 0;
}