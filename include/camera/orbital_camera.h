//
// Created by AmazingBuff on 2025/5/21.
//

#ifndef ORBITAL_CAMERA_H
#define ORBITAL_CAMERA_H

#include "core/math/algebra.h"

AMAZING_NAMESPACE_BEGIN

class OrbitalCamera
{
public:
    explicit OrbitalCamera(const Vec3f& target, float radius = 5.0, float theta = 0.0, float phi = 0.0);
    ~OrbitalCamera() = default;

    void on_mouse_move(float x_pos, float y_pos);
    void on_mouse_scroll(float y_offset);

    bool update_view_matrix(Affine3f& view);
private:
    Vec3f m_target;
    float m_radius;
    float m_theta; // [0, 2 * pi]
    float m_phi;  // [0, pi]
    bool m_is_camera_moved;
};

AMAZING_NAMESPACE_END
#endif //ORBITAL_CAMERA_H
