#include "texture.h"
#include <GL/glew.h>
#include <SDL2/SDL_image.h>
#include "../defs.h"
#include "../globals.h"
Texture::Texture(unsigned int id, const std::string& filepath)
    : id(id) 
    , filepath(filepath)
    , valid(true)
    , widthInPixels(0)
    , heightInPixels(0)
{
}

Texture::Texture()
    : valid(false)
{
}

Texture::~Texture()
{
    GLCALL(glDeleteTextures(1, &id));
}

void Texture::bind(unsigned int slot) const
{
    GLCALL(glActiveTexture(GL_TEXTURE0 + slot));
    GLCALL(glBindTexture(GL_TEXTURE_2D, id));
}

Texture* Texture::loadTexture(const std::string &filepath)
{
    GLuint id;
    GLCALL(glGenTextures(1, &id));
    GLCALL(glBindTexture(GL_TEXTURE_2D, id));

    SDL_Surface *texImage = IMG_Load(filepath.c_str());
    Texture* result = nullptr;
    if (texImage)
    {
        GLCALL(glTexImage2D
               (GL_TEXTURE_2D, 0, GL_RGBA8, texImage->w, texImage->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage->pixels));

        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

        result = new Texture(id, filepath);
        result->widthInPixels = texImage->w;
        result->heightInPixels = texImage->h;
    }
    else
    {
        std::cout << "[ERROR] could not load texture: " << filepath << std::endl;
        return nullptr;
    }
    SDL_FreeSurface(texImage);
    return result;
}
