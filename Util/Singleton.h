#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <memory>
#include <mutex>

template <class T>
class Singleton {
 private:
  static std::unique_ptr<T> instance;
  static std::once_flag initializedFlag;

 private:
  inline static T& GetReference(const std::unique_ptr<T>& instancePtr) {
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
  static T& GetInstance() {
    std::call_once(initializedFlag, [&]() { instance.reset(new T()); });
    return GetReference(instance);
  }

  template <typename... InstanceArgs>
  static T& GetInstance(InstanceArgs&&... args) {
    std::call_once(
        initializedFlag,
        [&]() { instance.reset(new T(std::forward<InstanceArgs>(args)...)); },
        std::forward<InstanceArgs>(args)...);
    return GetReference(instance);
  }
};

template <class T>
std::unique_ptr<T> Singleton<T>::instance;

template <class T>
std::once_flag Singleton<T>::initializedFlag;

#endif  // !_SINGLETON_H_
