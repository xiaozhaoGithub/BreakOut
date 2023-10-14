#ifndef TEXT_LOADER_H_
#define TEXT_LOADER_H_

#include <QOpenGLExtraFunctions>

#include "shader.h"
#include "glm.hpp"

struct Character
{
    GLuint textureId;   // ���������ID
    glm::ivec2 size;    // ���δ�С
    glm::ivec2 bearing; // �ӻ�׼�ߵ�������/������ƫ��ֵ
    GLuint advance;     // ԭ�����һ������ԭ��ľ���
};

class TextRenderer : protected QOpenGLExtraFunctions
{
public:
    TextRenderer();
    ~TextRenderer();

    void Load(const char* font_file, GLuint font_size);
    void Resize(GLint w, GLint h);
    void SetOriginBottom(bool state);

    void RenderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

private:
    void InitRenderData();

private:
    glm::vec2 viewport_;
    std::map<char, Character> characters_;

    std::unique_ptr<AbstractShader> shader_;
    GLuint vao_;
    GLuint vbo_;

    bool is_origin_bottom_;
};

#endif
