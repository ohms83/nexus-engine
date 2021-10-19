//
//  StateMachine.cpp
//  MiniRPG
//
//  Created by nuttachai on 13/9/20.
//

#include "StateMachine.hpp"

// #include "engine/util/Common.hpp"
// #include "engine/debug/Log.hpp"

USING_NAMESPACE_NXS;
// using namespace std;

StateMachine::StateMachine()
{
#ifndef DEBUG
    _enableLog = false;
#endif
}

void StateMachine::changeState(State& next)
{
    _next = &next;
}

void StateMachine::update(float dt)
{
    if (_next)
    {
        logStateChange();
        
        if (_current)
        {
            _current->onExit();
            _current->_owner = nullptr;
        }
        
        _current = _next;
        _current->_owner = this;
        _current->onEnter();
        
        _next = nullptr;
    }

    if (_current) {
        _current->update(dt);
    }
    
    onUpdate(dt);
}

void StateMachine::logStateChange()
{
    if (_enableLog)
    {
        // const string clasName = util::common::getClassName(*this);
        // const string currentState = (_current ? util::common::getClassName(_current) : "NONE");
        // const string nextState = (_next ? util::common::getClassName(_next) : "NONE");
        // Log::debug("%s is changing state from %s => %s", clasName.c_str(), currentState.c_str(), nextState.c_str());
    }
}
