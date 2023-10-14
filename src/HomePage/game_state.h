#ifndef GAME_STATE_H_
#define GAME_STATE_H_

class GameState
{
public:
    GameState();
    ~GameState() {}

    inline void SetLives(int lives);
    inline int Lives();

private:
    int lives_;
};

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
