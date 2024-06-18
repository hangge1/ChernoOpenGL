
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"



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


    { //解决在glfwTerminate之前让ib和vb析构
        float positions[] =
        {
           -0.5f, -0.5f,
            0.5f, -0.5f,
            0.5f,  0.5f,
           -0.5f,  0.5f
        };

        unsigned int indices[] =
        {
            0, 1, 2,
            2, 3, 0
        };

        IndexBuffer ib(indices, 6);
        VertexBuffer vb(positions, sizeof(float) * 8);
 
        VertexBufferLayout layout;
        layout.Push<float>(2);

        VertexArray va;
        va.AddBuffer(vb, layout);

        float uColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
        float increment = 0.01f;

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", uColor[0], uColor[1], uColor[2], uColor[3]);

        va.UnBind();
        ib.UnBind();
        vb.UnBind();
        shader.UnBind();

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT));


            //至少要绑定的基本状态 shader + vao + ibo
            shader.Bind();
            shader.SetUniform4f("u_Color", uColor[0], uColor[1], uColor[2], uColor[3]);
            va.Bind();
            ib.Bind();

            if (uColor[0] >= 1.0f)
            {
                increment = -0.01f;
            }
            else if (uColor[0] <= 0.0f)
            {
                increment = 0.01f;
            }
            uColor[0] += increment;


            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    glfwTerminate();
    return 0;
}