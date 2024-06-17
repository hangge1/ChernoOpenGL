#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

#include <fstream>
#include <sstream>

#define ASSERT(x) if(!(x)) __debugbreak();

#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << ")" << function  << " " << file << " " << line << std::endl;
        return false;
    }
    return true;
}

struct ShaderProgramSource
{
    std::string vertexSource;
    std::string fragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << "\n";
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static GLuint CompileShader(unsigned int type, const std::string& source)
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();

    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    //Error handing
    GLint result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(shader, length, &length, message);

        std::string desc = (type == GL_VERTEX_SHADER) ? "vertex shader" : "framgent shader";
        std::cout << "Failed to compile " << desc << std::endl;
        std::cout << "Error info: " << message << std::endl;

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

static GLuint CreateShader(const std::string& vshader, const std::string& fshader)
{
    GLuint program = glCreateProgram();
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vshader);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fshader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


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

    //重复
    //float positions[] =
    //{
    //   -0.5f, -0.5f,
    //    0.5f, -0.5f,
    //    0.5f,  0.5f,

    //    0.5f,   0.5f, //重复存储
    //   -0.5f,   0.5f,
    //   -0.5f,  -0.5f  //重复存储
    //};
    
    //不重复
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

    GLuint ibo = 0;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices, GL_STREAM_DRAW));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    GLuint vbo = 0;
    GLCall(glGenBuffers(1, &vbo));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, positions, GL_STREAM_DRAW));
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

    
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader"); //以工作目录为相对路径
    std::cout << "=================VERTEX==================" << std::endl;
    std::cout << source.vertexSource << std::endl;
    std::cout << "=================FRAGMENT==================" << std::endl;
    std::cout << source.fragmentSource << std::endl;

    GLuint shader = CreateShader(source.vertexSource, source.fragmentSource);
    GLCall(glUseProgram(shader));


    float uColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    float increment = 0.01;
    int location = glGetUniformLocation(shader, "u_Color");
    ASSERT(location != -1);
    glUniform4f(location, uColor[0], uColor[1], uColor[2], uColor[3]);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        //glBindBuffer(GL_ARRAY_BUFFER, vbo);
        //glDrawArrays(GL_TRIANGLES, 0, 6); //of course , no triangle can be seen; because nothing explain bytes
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

        /*GLClearError();
        glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr);
        ASSERT(GLLogCall());*/

        
        if (uColor[0] >= 1.0f)
        {
            increment = -0.01f;
        }
        else if (uColor[0] <= 0.0f)
        {
            increment = 0.01f;
        }
        uColor[0] += increment;
        glUniform4f(location, uColor[0], uColor[1], uColor[2], uColor[3]);

        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteShader(shader);

    glfwTerminate();
    return 0;
}