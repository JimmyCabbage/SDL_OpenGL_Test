#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

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
};

#endif