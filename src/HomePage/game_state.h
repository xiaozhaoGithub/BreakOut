#ifndef GAME_STATE_H_
#define GAME_STATE_H_

class GameState
{
public:
    GameState();
    ~GameState() {}

    inline void SetLife(int life);
    inline int Life();

private:
    int life_;
};

inline void GameState::SetLife(int life)
{
    if (life < 0)
        return;

    life_ = life;
}

inline int GameState::Life()
{
    return life_;
}

#endif
