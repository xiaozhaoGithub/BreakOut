#ifndef SHADER_H_
#define SHADER_H_

#include <QOpenGLExtraFunctions>
#include <string>

#include "gtc/type_ptr.hpp"

/**
 * @brief ��ɫ������
 */
class AbstractShader : protected QOpenGLExtraFunctions
{
public:
    AbstractShader(const char* vertexPath, const char* geomertyPath, const char* fragmentPath);
    virtual ~AbstractShader();

    virtual void bind() = 0;

    void setBool(const std::string& name, bool value);

    void setInt(const std::string& name, int value);

    void setFloat(const std::string& name, float value);
    void setFloat(const std::string& name, float v1, float v2, float v3, float v4);

    void setVec(const std::string& name, float v1, float v2);
    void setVec(const std::string& name, float v1, float v2, float v3);
    void setVec(const std::string& name, float v1, float v2, float v3, float v4);
    void setVec(const std::string& name, glm::vec2 vec);
    void setVec(const std::string& name, glm::vec3 vec);
    void setVec(const std::string& name, glm::vec4 vec);

    void setMatrix(const std::string& name, float* value);
    void setMatrix(const std::string& name, glm::mat3 value);
    void setMatrix(const std::string& name, glm::mat4 value);

    void setUniformBlockBinding(const std::string& name, unsigned int value);

protected:
    unsigned int m_id;
};

/**
 * @brief ���붥����ɫ����������ɫ����Ƭ����ɫ��
 */
class SimpleShader : public AbstractShader
{
public:
    SimpleShader(const char* vertexPath, const char* geometryPath, const char* fragmentPath);
    void bind() override;
};

#endif
