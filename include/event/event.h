//
// Created by AmazingBuff on 2025/5/21.
//

#ifndef EVENT_H
#define EVENT_H

#include <astd/astd.h>

AMAZING_NAMESPACE_BEGIN

// F must be a function wrapper
template <typename T, typename F>
class EventCallbackHandler
{
public:
    EventCallbackHandler() = default;
    ~EventCallbackHandler() = default;

    void register_callback(const T& type, F&& callback)
    {
        m_callbacks[type].push_back(std::forward<F>(callback));
    }

    void unregister_callback(const T& type)
    {
        m_callbacks.erase(type);
    }

    template <typename... Args>
    void call(const T& type, Args&&... args)
    {
        auto type_it = m_callbacks.find(type);
        if (type_it != m_callbacks.end())
        {
            for (auto& callback : type_it->second)
                callback(std::forward<Args>(args)...);
        }
    }

private:
    HashMap<T, Vector<F>> m_callbacks;
};

AMAZING_NAMESPACE_END

#endif //EVENT_H
