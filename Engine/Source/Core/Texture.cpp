#include "Core/Texture.h"
#include <stdexcept>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const std::string& path, TextureFilter filter)
{
    int width, height, nrChannels;

    // Configura stb_image per capovolgere l'immagine sull'asse Y
    stbi_set_flip_vertically_on_load(true);

    // Carica i dati dell'immagine
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        // Genera la texture OpenGL
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        // Imposta i parametri di wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // --- LOGICA DEL FILTRO AGGIUNTA ---
        if (filter == TextureFilter::PIXEL_PERFECT)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        else
        { // TextureFilter::SMOOTH
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        // ------------------------------------

        // Determina il formato dell'immagine
        GLenum format = GL_RGB;
        if (nrChannels == 4)
        {
            format = GL_RGBA;
        }

        // Invia i dati dell'immagine alla GPU
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        //glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "ERROR::TEXTURE::FAILED_TO_LOAD_IMAGE at path: " << path << std::endl;
        throw std::runtime_error("Failed to load texture file.");
    }

    // Libera la memoria della CPU
    stbi_image_free(data);
}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

unsigned int Texture::GetID() const
{
    return id;
}