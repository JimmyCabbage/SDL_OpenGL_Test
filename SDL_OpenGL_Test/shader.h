#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>

class Shader
{
public:
	//program ID
	unsigned ID;

	//constructor reads and builds the shader
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	//uses/activates the shader
	void use();

	unsigned getUniformLoc(const std::string& uniformN);

	void setUniformVec3(const std::string& uniName, const glm::vec3& vec);
	void setUniformVec3(const std::string& uniName, const float x, const float y, const float z);
	void setUniformInt(const std::string& uniName, const int v);
	void setUniform4fv(const std::string& uniName, const glm::mat4 mat);
};

#endif