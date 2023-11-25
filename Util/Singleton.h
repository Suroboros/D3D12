#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <memory>
#include <mutex>

template <class T, typename... Args>
class Singleton
{
private:
    inline static std::unique_ptr<T> instance;
    inline static std::once_flag initializedFlag;

private:
    inline void CreateInstance(Args&&... args)
    {
        instance.reset(new T(std::forward<Args>(args)...));
    }

    inline static T& GetReference(const std::unique_ptr<T>& instancePtr)
    {
        return *instancePtr;
    }

    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton& operator=(Singleton&&) = delete;

protected:
    Singleton() = default;
    virtual ~Singleton() = default;

public:
    template <typename... InstanceArgs>
    static T& GetInstance(InstanceArgs&&... args)
    {
        std::call_once(initializedFlag, &Singleton::CreateInstance, std::forward<InstanceArgs>(args)...);
        return GetReference(instance);
    }
};

template <class T, typename... Args>
inline std::unique_ptr<T> Singleton<T, Args...>::instance;

template <class T, typename... Args>
inline std::once_flag Singleton<T, Args...>::initializedFlag;

#endif // !_SINGLETON_H_
