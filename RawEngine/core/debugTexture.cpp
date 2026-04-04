#include <iostream>
#include <glad/glad.h>
namespace utility{
    void debugTexture(const char* name, GLuint tex, bool enabled)
    {
        std::cout << name << ": " << (enabled ? "ON" : "OFF");

        if (enabled)
            std::cout << " (ID: " << tex << ")";
        else
            std::cout << " (FALLBACK)";

        std::cout << "\n";
    }

}