#pragma once

#include "Renderer.h"
#include <string>
#include <unordered_map>

struct ShaderProgramSource
{
	std::string vertexSource;
	std::string fragmentSource;
};

class Shader
{
private:
	std::string m_filepath;
	unsigned int m_RendererID;
	//caching for uniforms
	std::unordered_map<std::string, unsigned int> m_UniformLocationCache;
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void UnBind() const;

	//set uniforms
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

private:
	unsigned int GetUniformLocation(const std::string& name);

	ShaderProgramSource ParseShader(const std::string& filepath);
	GLuint CompileShader(unsigned int type, const std::string& source);
	GLuint CreateShader(const std::string& vshader, const std::string& fshader);
};