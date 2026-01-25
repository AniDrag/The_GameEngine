#pragma once

#include <glad/glad.h>
#include <string>

namespace core {

    class Texture {
    private:
        GLuint id;

    public:
        Texture(const std::string& path);// binds it in memory and all we need is the id to access it

        GLuint getId();
    };

}
