#ifndef GAME_STATE_H_
#define GAME_STATE_H_

#include "text_renderer.h"

class GameState
{
public:
    enum StateFlag
    {
        SF_MENU,
        SF_ACTIVE,
        SF_WIN
    };

    GameState();
    ~GameState() {}

    void Draw(std::shared_ptr<TextRenderer> renderer);

    inline void SetState(StateFlag state);
    inline StateFlag State();

    inline void SetLives(int lives);
    inline int Lives();

private:
    StateFlag state_;
    int lives_;
};

inline void GameState::SetState(StateFlag state)
{
    state_ = state;
}

inline GameState::StateFlag GameState::State()
{
    return state_;
}

inline void GameState::SetLives(int lives)
{
    if (lives < 0)
        return;

    lives_ = lives;
}

inline int GameState::Lives()
{
    return lives_;
}

#endif
