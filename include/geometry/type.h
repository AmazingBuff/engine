//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef TYPE_H
#define TYPE_H

#include "core/math/algebra.h"

AMAZING_NAMESPACE_BEGIN

enum class DirectionDetection : uint8_t
{
    e_left,
    e_right,
    e_collinear,
    e_top = e_left,
    e_bottom = e_right,
    e_coplanar = e_collinear,
    e_inner = e_left,
    e_outer = e_right,
    e_border = e_collinear
};


inline DirectionDetection in_interval(Float v, Float l, Float r)
{
    if (v > l + Max_Allowable_Error && v < r - Max_Allowable_Error)
        return DirectionDetection::e_inner;
    else if (EQUAL_TO_ZERO(v - l) || EQUAL_TO_ZERO(r - v))
        return DirectionDetection::e_border;
    else
        return DirectionDetection::e_outer;
}


AMAZING_NAMESPACE_END

#endif //TYPE_H
