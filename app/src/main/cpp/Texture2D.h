#pragma once

#include <memory>
#include <string>

#include <android/asset_manager.h>
#include <GLES3/gl3.h>

namespace cp {

    class Texture2D {
    public:
        using Ptr = std::shared_ptr<Texture2D>;

    public:
        static Ptr Create(AAssetManager *assetManager, const std::string &assetPath, uint8_t textureUnit = 0);

        ~Texture2D();

        void Bind() const;
        void Unbind() const;

    private:
        Texture2D(GLuint textureId, uint8_t textureUnit)
        : textureId(textureId)
        , textureUnit(textureUnit) {}

    private:
        uint8_t textureUnit{0};
        GLuint textureId{0};
    };


    class TextureBindHelper {
    public:
        explicit TextureBindHelper(Texture2D::Ptr _texture)
        : texture(std::move(_texture)) {
            if (texture) {
                texture->Bind();
            }
        }

        ~TextureBindHelper() {
            if (texture) {
                texture->Unbind();
            }
        }

        TextureBindHelper(const TextureBindHelper&) = delete;
        TextureBindHelper(TextureBindHelper&&) = delete;
    private:
        std::shared_ptr<Texture2D> texture;
    };

} // cp