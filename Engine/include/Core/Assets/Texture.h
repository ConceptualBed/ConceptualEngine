#pragma once

#include <glad/gl.h>
#include <string>
#include "Core/Assets/Asset.h"

enum class TextureFilter
{
    PIXEL_PERFECT,
    SMOOTH
};

class Texture : public Asset
{
public:
    /**
     * @brief Carica una texture da un file immagine.
     * @param path Percorso del file immagine.
     */
    Texture(const std::string& path, TextureFilter filter = TextureFilter::SMOOTH);

    /**
     * @brief Distruttore che dealloca la risorsa OpenGL.
     */
    ~Texture();

    /**
     * @brief Restituisce l'ID della texture OpenGL.
     */
    unsigned int GetID() const;

private:
    unsigned int id;
};