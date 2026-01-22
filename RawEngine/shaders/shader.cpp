#include "shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
void Shader::use() const
{
    glUseProgram(ID);
}

int Shader::getUniformLocation(const std::string& name) const
{
    if (uniformCache.contains(name))
        return uniformCache[name];

    int location = glGetUniformLocation(ID, name.c_str());
    uniformCache[name] = location;
    return location;
}

void Shader::setProperty(const std::string& name, float value) const
{
    glUniform1f(getUniformLocation(name), value);
}

// other setters follow same pattern...
