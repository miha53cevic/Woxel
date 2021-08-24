#pragma once
#include <memory>

#ifdef _WIN32
#include <SDL.h>
#elif __linux__
#include <SDL2/SDL.h>
#endif

class App;

class State
{
public:
    State(App* app)
        : m_app(app)
    {}

    virtual void Event(SDL_Event& e) {}
    virtual void Setup()             = 0;
    virtual void Loop(float elapsed) = 0;

    virtual void Pause()  {}
    virtual void Resume() {}

protected:
    App* m_app;
};

typedef std::unique_ptr<State> StateRef;