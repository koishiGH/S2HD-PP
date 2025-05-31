#pragma once
#include <SDL2/SDL.h>

class GameState {
public:
    virtual ~GameState() = default;
 
    virtual bool Initialize() = 0;
    virtual void Update() = 0;
    virtual void Render() = 0;
    virtual void HandleEvent(const SDL_Event& event) = 0;
}; 