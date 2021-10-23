//
//  Singleton.hpp
//  MiniRPG
//
//  Created by nuttachai on 13/9/20.
//

#ifndef __NXS_SINGLETON_H__
#define __NXS_SINGLETON_H__

#include "NxsDefine.h"

NXS_NAMESPACE {
    template <typename _Class>
    class Singleton
    {
    public:
        static _Class& getInstance()
        {
            static _Class* _instance = nullptr;
            
            if (!_instance) {
                _instance = new _Class();
            }
            
            return *_instance;
        }
        
    protected:
        Singleton() {}
        Singleton(const Singleton&) {}
        virtual ~Singleton() {}
    };
}

#endif // __NXS_SINGLETON_H__
