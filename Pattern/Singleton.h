#ifndef RENDEREVALUATOR_PATTERN_SINGLETON_H_
#define RENDEREVALUATOR_PATTERN_SINGLETON_H_

#include <utils/Mutex.h>

namespace RenderEvaluator
{

template <typename TYPE>
class Singleton
{
public:
    /*Double-Checked Locking*/
    static TYPE& getInstance()
    {
        TYPE* instance = sInstance;
        if(instance == 0)
            {
                android::Mutex::Autolock _lock(sLock);
                if(instance == 0)
                    {
                        instance = new TYPE();
                        sInstance = instance;
                    }
            }
        return *instance;
    }

protected:
    Singleton() {}

private:
    static android::Mutex sLock;
    static TYPE* sInstance;
};

#define RENDEREVALUATOR_SINGLETON_STATIC_VAR_INIT(TYPE)                 \
    template<> android::Mutex Singleton< TYPE >::sLock(android::Mutex::PRIVATE);  \
    template<> TYPE* Singleton< TYPE >::sInstance(0);           \
    template class Singleton< TYPE >;

};

#endif
