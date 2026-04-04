#pragma once
#include "../shaders/shader.h"
// fbo.h
#pragma once
#include <glad/glad.h>
#include <vector>

namespace core {

    class FBO {
    public:
        FBO(int width, int height);
        ~FBO();

        void bind() const;
        void unbind() const;
        void resize(int width, int height);

        // Attach a texture as colour attachment
        void attachColorTexture(GLenum attachment, GLenum internalFormat = GL_RGBA16F);
        // Attach a renderbuffer for depth/stencil
        void attachDepthStencilRenderbuffer();

        GLuint getColorTexture(unsigned int index = 0) const { return colorTextures_[index]; }
        int getWidth() const { return width_; }
        int getHeight() const { return height_; }

    private:
        GLuint fbo_ = 0;
        std::vector<GLuint> colorTextures_;
        GLuint rbo_ = 0;
        int width_, height_;
    };

} // namespace core