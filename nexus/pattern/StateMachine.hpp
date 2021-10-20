//
//  StateMachine.hpp
//  MiniRPG
//
//  Created by nuttachai on 13/9/20.
//

#ifndef __NXS_STATE_MACHINE_H__
#define __NXS_STATE_MACHINE_H__

#include "NxsMacros.h"

NXS_NAMESPACE {
    class StateMachine;

    class State
    {
        friend class StateMachine;
    public:
        virtual void onEnter() {}
        virtual void update(float dt) = 0;
        virtual void onExit() {}
        
    private:
        StateMachine* _owner = nullptr;
    };

    class StateMachine
    {
    public:
        StateMachine();
        
        void changeState(State& next);
        
        virtual void update(float dt) final;
        
    protected:
        /// Update global state-machine's logics. This will be called after _current->update()
        virtual void onUpdate(float dt) = 0;
        
    protected:
        State* _next = nullptr;
        State* _current = nullptr;
        
        bool _enableLog = false;
        
    private:
        void logStateChange();
    };
}

#endif // __NXS_STATE_MACHINE_H__
