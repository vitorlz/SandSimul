#pragma once

#include "unordered_map"
#include "iostream"
#include "Shader.h"

class ShaderManager
{
public:
	ShaderManager() = default;

	void loadShaders();
	void reloadShaders();

	Shader& getShader(std::string name);

	std::unordered_map<std::string, Shader> shaders;
};