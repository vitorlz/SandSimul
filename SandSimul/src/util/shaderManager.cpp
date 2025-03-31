#include "shaderManager.h"


void ShaderManager::loadShaders()
{
    shaders.insert({ "cellShader", Shader("shaders/cell.vert", "shaders/cell.frag") });
    shaders.insert({ "cascadesShader", Shader("shaders/radianceCascades.vert", "shaders/radianceCascades.frag") });
    shaders.insert({ "postShader",Shader("shaders/post.vert", "shaders/post.frag") });
    shaders.insert({ "cascadeMipmapShader", Shader("shaders/cascadeMipmap.vert", "shaders/cascadeMipmap.frag") });
}

void ShaderManager::reloadShaders()
{
    shaders.clear();
    loadShaders();
}

Shader& ShaderManager::getShader(std::string name)
{
    return shaders[name];
}
