#include "texture.h"
#include <GL/glew.h>
#include <SDL2/SDL_image.h>
#include "../defs.h"
#include "../globals.h"


Texture::~Texture()
{
}

void Texture::del()
{
    GLCALL(glDeleteTextures(1, &id));
}

void Texture::bind(unsigned int slot) const
{
    GLCALL(glActiveTexture(GL_TEXTURE0 + slot));
    GLCALL(glBindTexture(GL_TEXTURE_2D, id));
}

Texture* Texture::loadTexture(const String &filepath)
{
    GLuint id;
    GLCALL(glGenTextures(1, &id));
    GLCALL(glBindTexture(GL_TEXTURE_2D, id));

    SDL_Surface *texImage = IMG_Load(filepath.c_str());
    Texture* result = nullptr;
    if (texImage)
    {
        GLCALL(glTexImage2D
               (GL_TEXTURE_2D, 0, GL_RGBA8, texImage->w, texImage->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, texImage->pixels));

        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
//        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
//        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

        
        result = (Texture*)g_memory.reserve(sizeof(Texture));
        result->id = id;
        result->filepath = filepath;
        result->valid = true;
        result->widthInPixels = texImage->w;
        result->heightInPixels = texImage->h;
    }
    else
    {
        err("could not load texture: %s\n", filepath);
        return NULL;
    }
    SDL_FreeSurface(texImage);
    return result;
}
