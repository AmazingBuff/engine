//
// Created by AmazingBuff on 2025/4/30.
//

#pragma once

#include "core/base.h"
#include <Eigen/Eigen>

AMAZING_NAMESPACE_BEGIN

using Vec2f = Eigen::Vector2<Float>;
using Vec3f = Eigen::Vector3<Float>;
using Vec4f = Eigen::Vector4<Float>;

using Vec2i = Eigen::Vector2<int32_t>;
using Vec2u = Eigen::Vector2<uint32_t>;

using Mat3f = Eigen::Matrix<Float, 3, 3>;
using Mat4f = Eigen::Matrix<Float, 4, 4>;

using Affine3f = Eigen::Affine3f;

static constexpr Float Pi = EIGEN_PI;
static constexpr Float PiOver2 = EIGEN_PI / 2;
static constexpr Float PiOver4 = EIGEN_PI / 4;
static constexpr Float Max_Allowable_Error = 1e-6;

#define EQUAL_TO_ZERO(expr)  (std::abs(expr) < Max_Allowable_Error)

AMAZING_NAMESPACE_END

namespace std
{
    template <typename Scalar, int Rows, int Cols>
    Eigen::Matrix<Scalar, Rows, Cols> min(const Eigen::Matrix<Scalar, Rows, Cols>& l, const Eigen::Matrix<Scalar, Rows, Cols>& r)
    {
        Eigen::Matrix<Scalar, Rows, Cols> ret;
        for (int i = 0; i < Rows; i++)
            for (int j = 0; j < Cols; j++)
                ret(i, j) = min(l(i, j), r(i, j));
        return ret;
    }

    template <typename Scalar, int Rows, int Cols>
    Eigen::Matrix<Scalar, Rows, Cols> max(const Eigen::Matrix<Scalar, Rows, Cols>& l, const Eigen::Matrix<Scalar, Rows, Cols>& r)
    {
        Eigen::Matrix<Scalar, Rows, Cols> ret;
        for (int i = 0; i < Rows; i++)
            for (int j = 0; j < Cols; j++)
                ret(i, j) = max(l(i, j), r(i, j));
        return ret;
    }

    template <typename Scalar, int Rows, int Cols>
    struct hash<Eigen::Matrix<Scalar, Rows, Cols>>
    {
        // https://wjngkoh.wordpress.com/2015/03/04/c-hash-function-for-eigen-matrix-and-vector/
        NODISCARD constexpr size_t operator()(const Eigen::Matrix<Scalar, Rows, Cols>& matrix) const
        {
            size_t seed = 0;
            for (size_t i = 0; i < matrix.size(); i++)
            {
                Scalar elem = *(matrix.data() + i);
                seed = Amazing::hash_combine(seed, hash<Scalar>()(elem));
            }
            return seed;
        }
    };
}