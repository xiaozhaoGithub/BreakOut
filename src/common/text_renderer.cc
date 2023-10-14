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

    // 宽度值设为0, 从字体面通过给定的高度中动态计算出字形的宽度
    FT_Set_Pixel_Sizes(face, 0, font_size);

    // OpenGL要求所有的纹理都是4字节对齐的，即纹理的大小永远是4字节的倍数。通常这并不会出现什么问题，因为大部分纹理的宽度都为4的倍数或
    // 每像素使用4个字节，但是现在我们每个像素只用了一个字节，它可以是任意的宽度。通过将纹理解压对齐参数设为1，这样才能确保不会有对齐问题
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //禁用字节对齐限制

    for (GLubyte c = 0; c < 128; c++) {
        // 加载字符的字形(8位的灰度位图)
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
    // 注：绘制字符需要经常更新VBO的内存，设置内存类型为动态绘制：GL_DYNAMIC_DRAW
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::Resize(GLint w, GLint h)
{
    viewport_.x = static_cast<GLfloat>(w);
    viewport_.y = static_cast<GLfloat>(h);
}

void TextRenderer::SetOriginBottom(bool state)
{
    is_origin_bottom_ = state;
}

void TextRenderer::RenderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale,
                              glm::vec3 color)
{
    shader_->bind();
    // use floats to make ortho mat.
    shader_->setMatrix("projectionMat", glm::ortho(0.0f, viewport_.x, 0.0f, viewport_.y));
    shader_->setVec("textColor", color);

    glBindVertexArray(vao_);

    for (auto c = text.begin(); c != text.end(); c++) {
        Character ch = characters_[*c];

        GLfloat xpos = x + ch.bearing.x * scale;
        GLfloat ypos;
        if (is_origin_bottom_) {
            ypos = y - (ch.size.y - ch.bearing.y) * scale; // 基准线以下的部分需要考虑
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

        // 更新位置到下一个字形的原点，advance单位是1/64像素
        x += (ch.advance >> 6) * scale; // 位偏移6个单位来获取单位为像素的值 (2^6 = 64)
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
