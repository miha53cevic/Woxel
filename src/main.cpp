#include <memory>
#include "app/app.h"

#include "states/statemanager.h"
#include "states/Playing.h"

int main(int argc, char* argv[])
{
    App::Init("Woxel", 1280, 720);
    StateManager::Add(std::make_shared<Playing>());
    App::Run();
    
    return 0;
}