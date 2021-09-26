#pragma once
#include <deque>
#include "state.h"

class StateManager
{
public:
    static StateManager& Get();

    static void Add(StateRef newstate);
    static void Pop();
    static int  Size();

    static StateRef& CurrentState();

    static std::deque<StateRef>* GetStates();

private:
    void IAdd(StateRef newstate);
    void IPop();

private:
    StateManager();

    std::deque<StateRef> m_states;
};