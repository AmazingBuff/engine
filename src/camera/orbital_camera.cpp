//
// Created by AmazingBuff on 2025/5/21.
//

#include "camera/orbital_camera.h"

AMAZING_NAMESPACE_BEGIN

OrbitalCamera::OrbitalCamera(const Vec3f& target, float radius, float theta, float phi)
    : m_target(target), m_radius(radius), m_theta(theta), m_phi(phi), m_is_camera_moved(false)
{

}

void OrbitalCamera::on_mouse_move(float x_pos, float y_pos)
{
    m_theta += x_pos * 0.005f;
    m_phi += y_pos * 0.005f;

    m_phi = std::clamp(m_phi, -PiOver2, PiOver2);

    m_is_camera_moved = true;
}

void OrbitalCamera::on_mouse_scroll(float y_offset)
{
    m_radius -= y_offset * 0.001f;
    m_radius = std::clamp(m_radius, 1.0f, 20.0f);

    m_is_camera_moved = true;
}

bool OrbitalCamera::update_view_matrix(Affine3f& view)
{
    if (m_is_camera_moved)
    {
        Vec3f position;
        position.x() = m_radius * std::cos(m_phi) * std::sin(m_theta);
        position.y() = m_radius * std::sin(m_phi);
        position.z() = m_radius * std::cos(m_phi) * std::cos(m_theta);

        const Vec3f world_up{ 0.0, 1.0, 0.0 };
        // left coordinate
        Vec3f front = -position.normalized();
        Vec3f right = -front.cross(world_up).normalized();
        Vec3f up = front.cross(right).normalized();

        Mat3f r;
        r.row(0) = right;
        r.row(1) = up;
        r.row(2) = front;

        view = Affine3f::Identity();
        view.linear() = r;
        view.translation() = -view.linear() * position;

        m_is_camera_moved = false;
        return true;
    }

    return false;
}

AMAZING_NAMESPACE_END