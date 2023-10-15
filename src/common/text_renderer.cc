#include "text_renderer.h"

#include <iostream>

#include "ft2build.h"
#include FT_FREETYPE_H
#include "gtc/matrix_transform.hpp"

TextRenderer::TextRenderer()
    : is_origin_bottom_(true)
{
    initializeOpenGLFunctions();

    shader_ =
        std::make_unique<SimpleShader>("res/shaders/text.vert", nullptr, "res/shaders/text.frag");

    InitRenderData();
}

TextRenderer::~TextRenderer() {}

void TextRenderer::Load(const char* font_file, GLuint font_size)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft) != 0)
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    FT_Face face;
    if (FT_New_Face(ft, font_file, 0, &face) != 0)
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    // ���ֵ��Ϊ0, ��������ͨ�������ĸ߶��ж�̬��������εĿ��
    FT_Set_Pixel_Sizes(face, 0, font_size);
    font_height_ = font_size;

    // OpenGLҪ�����е�������4�ֽڶ���ģ�������Ĵ�С��Զ��4�ֽڵı�����ͨ���Ⲣ�������ʲô���⣬��Ϊ�󲿷�����Ŀ�ȶ�Ϊ4�ı�����
    // ÿ����ʹ��4���ֽڣ�������������ÿ������ֻ����һ���ֽڣ�������������Ŀ�ȡ�ͨ���������ѹ���������Ϊ1����������ȷ�������ж�������
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //�����ֽڶ�������

    for (GLubyte c = 0; c < 128; c++) {
        // �����ַ�������(8λ�ĻҶ�λͼ)
        if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows,
                     0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {textureId,
                               glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                               glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                               static_cast<unsigned int>(face->glyph->advance.x)};
        characters_.insert(std::pair<GLchar, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void TextRenderer::InitRenderData()
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    // ע�������ַ���Ҫ��������VBO���ڴ棬�����ڴ�����Ϊ��̬���ƣ�GL_DYNAMIC_DRAW
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::Resize(GLint w, GLint h)
{
    window_size_.x = static_cast<GLfloat>(w);
    window_size_.y = static_cast<GLfloat>(h);
}


float TextRenderer::TextWidth(const std::string& text, GLfloat scale)
{
    float width = 0.0f;

    for (auto c = text.begin(); c != text.end(); c++) {
        Character ch = characters_[*c];
        GLfloat w = ch.size.x * scale;

        width += (ch.advance >> 6) * scale; // λƫ��6����λ����ȡ��λΪ���ص�ֵ (2^6 = 64)
    }

    return width;
}

void TextRenderer::RenderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale,
                              glm::vec3 color)
{
    shader_->bind();
    // use floats to make ortho mat.
    shader_->setMatrix("projectionMat", glm::ortho(0.0f, window_size_.x, 0.0f, window_size_.y));
    shader_->setVec("textColor", color);

    glBindVertexArray(vao_);

    for (auto c = text.begin(); c != text.end(); c++) {
        Character ch = characters_[*c];

        GLfloat xpos = x + ch.bearing.x * scale;
        GLfloat ypos;
        if (is_origin_bottom_) {
            ypos = y - (ch.size.y - ch.bearing.y) * scale; // ��׼�����µĲ�����Ҫ����
        } else {
            ypos = y + (characters_['H'].bearing.y - ch.bearing.y) * scale;
        }

        GLfloat w = ch.size.x * scale;
        GLfloat h = ch.size.y * scale;

        // clang-format off
        GLfloat vertices[6][4] = {
			{xpos, ypos + h, 0.0, 0.0},    
			{xpos, ypos, 0.0, 1.0},
			{xpos + w, ypos, 1.0, 1.0},
			{xpos, ypos + h, 0.0, 0.0},   
			{xpos + w, ypos, 1.0, 1.0},
			{xpos + w, ypos + h, 1.0, 0.0}
		};
        // clang-format on

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ch.textureId);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        // ����λ�õ���һ�����ε�ԭ�㣬advance��λ��1/64����
        x += (ch.advance >> 6) * scale; // λƫ��6����λ����ȡ��λΪ���ص�ֵ (2^6 = 64)
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
