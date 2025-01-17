
#include "Shader.h"

#include "Renderer.h"

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>


#include <glm/gtc/type_ptr.hpp>

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
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

GLuint Shader::CompileShader(unsigned int type, const std::string& source)
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

GLuint Shader::CreateShader(const std::string& vshader, const std::string& fshader)
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

Shader::Shader(const std::string& filepath)
	:m_filepath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath); //以工作目录为相对路径
    std::cout << "=================VERTEX==================" << std::endl;
    std::cout << source.vertexSource << std::endl;
    std::cout << "=================FRAGMENT==================" << std::endl;
    std::cout << source.fragmentSource << std::endl;

    m_RendererID = CreateShader(source.vertexSource, source.fragmentSource);
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

void Shader::UnBind() const
{
    GLCall(glUseProgram(0));
}

//set uniforms
void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    int location = GetUniformLocation(name);
    GLCall(glUniform4f(location, v0, v1, v2, v3));
}

void Shader::SetUniform1f(const std::string& name, float v)
{
    int location = GetUniformLocation(name);
    GLCall(glUniform1f(location, v));
}

void Shader::SetUniform1i(const std::string& name, int v)
{
    int location = GetUniformLocation(name);
    GLCall(glUniform1i(location, v));
}

void Shader::SetUniform2i(const std::string& name, int v0, int v1)
{
    int location = GetUniformLocation(name);
    int v[2] = { v0, v1 };
    GLCall(glUniform1iv(location, 2, v));
}

void Shader::SetUniformMat4f(const std::string& name,const glm::mat4& v)
{
    int location = GetUniformLocation(name);
    GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(v)));
}

unsigned int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    {
        return m_UniformLocationCache[name];
    }

    int location = glGetUniformLocation(m_RendererID, name.c_str());
    ASSERT(location != -1);

    m_UniformLocationCache[name] = location;

    return location;
}