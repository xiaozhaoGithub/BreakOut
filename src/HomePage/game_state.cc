#include "game_state.h"

#include <sstream>

GameState::GameState()
    : state_(SF_MENU)
    , lives_(3)
{}

void GameState::Draw(std::shared_ptr<TextRenderer> renderer)
{
    float window_w = renderer->WindowSize().x;
    float window_h = renderer->WindowSize().y;

    unsigned int font_height = renderer->FontHeight();

    std::stringstream ss;
    ss << Lives();
    float scale = 1.0f;
    renderer->RenderText("Lives:" + ss.str(), 0.0f, window_h - font_height * scale, scale,
                         glm::vec3(5.0f, 5.0f, 1.0f));

    float scale_factor = 0.7f;

    int spacing = 5;
    int total_text_height = (int)(font_height + spacing + font_height * scale_factor);
    int y = (window_h - total_text_height) / 2;

    switch (state_) {
    case GameState::SF_MENU: {
        std::string text = "Press UP or DOWN to select level";
        renderer->RenderText(text, (window_w - renderer->TextWidth(text, scale_factor)) / 2, y,
                             scale_factor, glm::vec3(0.75f));

        text = "Press ENTER to start";
        scale_factor = 1.0f;
        renderer->RenderText(text, (window_w - renderer->TextWidth(text, scale_factor)) / 2,
                             y + font_height * scale_factor + spacing, scale, glm::vec3(1.0f));
    } break;
    case GameState::SF_ACTIVE:
        break;
    case GameState::SF_WIN: {
        scale_factor = 1.0f;
        std::string text = "Press ENTER to retry or ESC to quit";
        renderer->RenderText(text, (window_w - renderer->TextWidth(text, scale_factor)) / 2, y,
                             scale_factor, glm::vec3(1.0f, 1.0f, 0.0f));

        text = "You WON!!!";
        renderer->RenderText(text, (window_w - renderer->TextWidth(text, scale_factor)) / 2,
                             y + font_height * scale_factor + spacing, scale,
                             glm::vec3(0.0f, 1.0f, 0.0f));
    } break;
    default:
        break;
    }
}
