#pragma once
#include <memory>

#ifdef _WIN32
#include <SDL.h>
#elif __linux__
#include <SDL2/SDL.h>
#endif

class State
{
public:
    State()
    {}

    virtual void Event(SDL_Event& e) {}
    virtual void Setup()             = 0;
    virtual void Loop(float elapsed) = 0;

    virtual void Pause()  {}
    virtual void Resume() {}

protected:
};

typedef std::unique_ptr<State> StateRef;