#include <memory>
#include "app/app.h"

#include "states/statemanager.h"
#include "states/playing.h"

class Woxel : public App
{
public:
    Woxel(const char* title, int width, int height)
        : App(title, width, height)
    {
        StateManager::Add(std::make_unique<Playing>(this));
    }

private:
    virtual bool Event(SDL_Event& e) override
    {
        StateManager::CurrentState()->Event(e);

        return true;
    }
    virtual bool Setup() override
    {
        StateManager::CurrentState()->Setup();

        return true;
    }
    virtual bool Loop(float elapsed) override
    {
        StateManager::CurrentState()->Loop(elapsed);

        return true;
    }

private:
};

int main(int argc, char* argv[])
{
    Woxel game("Woxel", 1280, 720);
    game.Run();
    return 0;
}