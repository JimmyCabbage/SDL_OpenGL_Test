#include "shader.h"

#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// make sure ifstream can throw exceptions
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		//open file
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		//read file's buffer contents into stream
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//close file handles
		vShaderFile.close();
		fShaderFile.close();
		//stream to string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//compile shaders
	unsigned vertex, fragment;

	//vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	//fragment
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	//delete shaders because already linked into program
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use()
{
	glUseProgram(ID);
}

unsigned Shader::getUniformLoc(const std::string& uniformN)
{
	return glGetUniformLocation(ID, uniformN.c_str());
}

void Shader::setUniformVec3(const std::string& uniName, const float x, const float y, const float z)
{
	glUniform3f(getUniformLoc(uniName), x, y, z);
}
void Shader::setUniformVec3(const std::string& uniName, const glm::vec3& vec)
{
	glUniform3fv(getUniformLoc(uniName), 1, &vec[0]);
}
void Shader::setUniformInt(const std::string& uniName, const int v)
{
	glUniform1i(getUniformLoc(uniName), v);
}
void Shader::setUniform4fv(const std::string& uniName, const glm::mat4 mat)
{
	glUniformMatrix4fv(this->getUniformLoc(uniName), 1, GL_FALSE, glm::value_ptr(mat));
}