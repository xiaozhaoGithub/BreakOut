#ifndef TEXT_LOADER_H_
#define TEXT_LOADER_H_

#include <QOpenGLExtraFunctions>

#include "shader.h"
#include "glm.hpp"

struct Character
{
    GLuint textureId;   // 字形纹理的ID
    glm::ivec2 size;    // 字形大小
    glm::ivec2 bearing; // 从基准线到字形左部/顶部的偏移值
    GLuint advance;     // 原点距下一个字形原点的距离
};

class TextRenderer : protected QOpenGLExtraFunctions
{
public:
    TextRenderer();
    ~TextRenderer();

    void Load(const char* font_file, GLuint font_size);
    void Resize(GLint w, GLint h);

    void RenderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
    float TextWidth(const std::string& text, GLfloat scale);

    inline void SetOriginBottom(bool state);

    inline GLuint FontHeight();
    inline glm::vec2 WindowSize();

private:
    void InitRenderData();

private:
    glm::vec2 window_size_;
    std::map<char, Character> characters_;

    std::unique_ptr<AbstractShader> shader_;
    GLuint vao_;
    GLuint vbo_;

    bool is_origin_bottom_;
    GLuint font_height_;
};

inline void TextRenderer::SetOriginBottom(bool state)
{
    is_origin_bottom_ = state;
}

inline GLuint TextRenderer::FontHeight()
{
    return font_height_;
}

inline glm::vec2 TextRenderer::WindowSize()
{
    return window_size_;
}

#endif
