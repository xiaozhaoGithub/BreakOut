#include "resource_manager.h"

ResourceManager::ResourceManager() {}

std::shared_ptr<QOpenGLTexture> ResourceManager::Texture(const std::string& name,
                                                         const std::string& file, bool alpha)
{
    auto iter = texture_map_.find(name);
    if (iter == texture_map_.end()) {
        auto texture = std::make_shared<QOpenGLTexture>(QOpenGLTexture::Target2D);
        if (alpha) {
            texture->setFormat(QOpenGLTexture::RGBAFormat);
        }
        texture->setData(QImage(file.c_str()));
        texture_map_[name] = texture;
    }

    return texture_map_[name];
}
