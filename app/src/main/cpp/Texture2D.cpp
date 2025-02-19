#include <memory>
#include <vector>

#include <android/imagedecoder.h>

#include "Texture2D.h"
#include "LogBuffer.h"

namespace cp {

    std::shared_ptr<Texture2D> Texture2D::Create(AAssetManager *assetManager, const std::string &assetPath, uint8_t textureUnit) {
        AAsset* encodedImage = AAssetManager_open(assetManager, assetPath.c_str(), AASSET_MODE_BUFFER);
        if (!encodedImage) {
            out << "AAssetManager_open failed for " << assetPath << std::endl;
            return nullptr;
        }

        AImageDecoder *pImageDecoder = nullptr;
        int result = AImageDecoder_createFromAAsset(encodedImage, &pImageDecoder);
        if (result != ANDROID_IMAGE_DECODER_SUCCESS) {
            AAsset_close(encodedImage);
            out << "AImageDecoder_createFromAAsset failed for " << assetPath << ", error " << result << std::endl;
            return nullptr;
        }

        AImageDecoder_setAndroidBitmapFormat(pImageDecoder, ANDROID_BITMAP_FORMAT_RGBA_8888);

        const AImageDecoderHeaderInfo *pHeaderInfo = nullptr;
        pHeaderInfo = AImageDecoder_getHeaderInfo(pImageDecoder);
        if (!pHeaderInfo) {
            AAsset_close(encodedImage);
            out << "AImageDecoder_getHeaderInfo failed for " << assetPath << std::endl;
            return nullptr;
        }

        const int32_t width = AImageDecoderHeaderInfo_getWidth(pHeaderInfo);
        const int32_t height = AImageDecoderHeaderInfo_getHeight(pHeaderInfo);
        const size_t stride = AImageDecoder_getMinimumStride(pImageDecoder);

        std::vector<uint8_t> imageData(height * stride);
        int decodeResult = AImageDecoder_decodeImage(
                pImageDecoder,
                imageData.data(),
                stride,
                imageData.size());
        if (decodeResult != ANDROID_IMAGE_DECODER_SUCCESS) {
            AImageDecoder_delete(pImageDecoder);
            AAsset_close(encodedImage);
            out << "AImageDecoder_decodeImage failed for " << assetPath << ", error " << decodeResult << std::endl;
            return nullptr;
        }

        GLuint textureId;
        glGenTextures(1, &textureId);

        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGBA,
                width,
                height,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                imageData.data());

        glGenerateMipmap(GL_TEXTURE_2D);

        AImageDecoder_delete(pImageDecoder);
        AAsset_close(encodedImage);

        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, 0);

        return std::shared_ptr<Texture2D>(new Texture2D(textureId, textureUnit));
    }


    Texture2D::~Texture2D() {
        glDeleteTextures(1, &textureId);
    }


    void Texture2D::Bind() const {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, textureId);
    }


    void Texture2D::Unbind() const {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

}