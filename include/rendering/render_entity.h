//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef RENDER_ENTITY_H
#define RENDER_ENTITY_H

#include "rendering/render_type.h"

AMAZING_NAMESPACE_BEGIN

// a unique identifier for the object in render scene
class RenderEntity
{
public:
    RenderEntity() : m_id(0) {}
    explicit RenderEntity(const uint32_t id) : m_id(id) {}
    ~RenderEntity() = default;

    NODISCARD uint32_t id() const { return *m_id; }
    NODISCARD uint32_t use_count() const { return m_id.use_count(); }
    NODISCARD bool is_valid() const { return *m_id != 0; }

    NODISCARD bool operator==(RenderEntity const& other) const { return m_id == other.m_id; }
    NODISCARD bool operator!=(RenderEntity const& other) const { return m_id != other.m_id; }
private:
    SharedPtr<uint32_t> m_id;
};

AMAZING_NAMESPACE_END

template <>
struct std::hash<Amazing::RenderEntity>
{
    size_t operator()(Amazing::RenderEntity const& entity) const noexcept
    {
        return entity.id();
    }
};

#endif //RENDER_ENTITY_H
