#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

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

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

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

    float positions[] =
    {
        -0.5f, -0.5f,
        0.5f,  -0.5f,
        0.0f,   0.5f
    };

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, positions, GL_STREAM_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::string vertexShader =
        "#version 330 core\n"
        "layout(location = 0) in vec4 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = position;\n"
        "}\n";

    std::string fragmentShader =
        "#version 330 core\n"
        "layout(location = 0) out vec4 color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";

    GLuint shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glDrawArrays(GL_TRIANGLES, 0, 3); //of course , no triangle can be seen; because nothing explain bytes

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteShader(shader);

    glfwTerminate();
    return 0;
}