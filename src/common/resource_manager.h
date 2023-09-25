#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_

#include <QOpenGLTexture>
#include <unordered_map>

#include "singleton.h"

class ResourceManager
{
    SINGLETON_DECLARE(ResourceManager)
public:
    ResourceManager();

    std::shared_ptr<QOpenGLTexture> Texture(const std::string& name, const std::string& file,
                                            bool alpha);

private:
    std::unordered_map<std::string, std::shared_ptr<QOpenGLTexture>> texture_map_;
};

#endif
