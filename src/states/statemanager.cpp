#include "statemanager.h"
#include "../app/app.h"

StateManager::StateManager()
{}

StateManager & StateManager::Get()
{
    static StateManager manager;
    return manager;
}

void StateManager::Add(StateRef newstate)
{
    StateManager::Get().IAdd(newstate);
}

void StateManager::Pop()
{
    StateManager::Get().IPop();
}

int StateManager::Size()
{
    return StateManager::Get().m_states.size();
}

StateRef& StateManager::CurrentState()
{
    // Must check Size() before using
    return Get().m_states.front();
}

std::deque<StateRef>* StateManager::GetStates()
{
    return &Get().m_states;
}

void StateManager::IAdd(StateRef newstate)
{
    // If this isn't the first state pause the current state
    if (Size())
        m_states.front()->Pause();

    // Add new state and run Setup
    m_states.push_front(std::move(newstate));
    m_states.front()->Setup();
}

void StateManager::IPop()
{
    // Remove only if exists
    if (Size())
    {
        m_states.pop_front();

        // Resume next state if it exists
        if (Size())
            m_states.front()->Resume();
    }
}