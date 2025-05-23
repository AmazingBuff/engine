//
// Created by AmazingBuff on 2025/5/21.
//

#ifndef EVENT_H
#define EVENT_H

#include <astd/astd.h>

AMAZING_NAMESPACE_BEGIN

enum class EventUserDataType : uint8_t
{
    e_undefined,
    e_orbital_camera
};

// F must have a user data parameter at last of parameter list
template <typename T, typename F>
    requires (std::is_same_v<Trait::tail_type_t<typename Trait::function_traits<std::decay_t<F>>::argument_type>, void*>)
class EventCallbackHandler
{
public:
    EventCallbackHandler() = default;
    ~EventCallbackHandler() = default;

    void register_callback(const T& type, EventUserDataType data_type, F&& callback)
    {
        m_callbacks[type][data_type].push_back(std::forward<F>(callback));
    }

    void unregister_callback(const T& type)
    {
        m_callbacks.erase(type);
    }

    template <typename... Args>
    void call(const T& type, EventUserDataType data_type, Args&&... args)
    {
        auto type_it = m_callbacks.find(type);
        if (type_it != m_callbacks.end())
        {
            auto data_type_it = type_it->second.find(data_type);
            if (data_type_it != type_it->second.end())
            {
                for (auto& callback : data_type_it->second)
                    callback(std::forward<Args>(args)...);
            }
        }
    }

private:
    HashMap<T, HashMap<EventUserDataType, Vector<F>>> m_callbacks;
};

AMAZING_NAMESPACE_END

#endif //EVENT_H
