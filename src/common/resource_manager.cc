#include "resource_manager.h"

ResourceManager::ResourceManager() {}

std::shared_ptr<QOpenGLTexture> ResourceManager::Texture(const std::string& name,
                                                         const std::string& file, bool alpha)
{
    auto iter = texture_map_.find(name);
    if (iter == texture_map_.end()) {
        texture_map_[name] = std::make_shared<QOpenGLTexture>(QImage(name.c_str()));
        if (alpha) {
            texture_map_[name]->setFormat(QOpenGLTexture::RGBAFormat);
        }
    }

    return texture_map_[name];
}
