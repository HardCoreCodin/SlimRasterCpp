#pragma once

#include "./vec3.h"

struct quat {
    vec3 axis;
    f32 amount;

    static quat Identity;

    quat() noexcept : quat{vec3{0}, 1.0f} {}
    quat(f32 axis_x, f32 axis_y, f32 axis_z, f32 amount) noexcept : axis{axis_x, axis_y, axis_z}, amount{amount} {}
    quat(vec3 axis, f32 amount) noexcept : axis{axis}, amount{amount} {}
    quat(quat &other) noexcept : quat{other.axis, other.amount} {}
    quat(const quat &other) noexcept : quat{other.axis, other.amount} {}

    INLINE f32 length() const {
        return sqrtf(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z + amount * amount);
    }

    INLINE quat normalized() const {
        return *this / length();
    }

    INLINE quat operator * (f32 rhs) const {
        return {
                axis * rhs,
                amount * rhs
        };
    }

    INLINE quat operator / (f32 rhs) const {
        return *this * (1.0f / rhs);
    }

    INLINE vec3 operator * (const vec3 &rhs) const {
        vec3 out{axis.cross(rhs)};
        vec3 qqv{axis.cross(out)};
        out = out.scaleAdd(amount, qqv);
        out = out.scaleAdd(2, rhs);
        return out;
    }

    INLINE quat operator * (const quat &rhs) const {
        return {
            {
                amount * rhs.axis.x + axis.x * rhs.amount + axis.y * rhs.axis.z - axis.z * rhs.axis.y,
                amount * rhs.axis.y - axis.x * rhs.axis.z + axis.y * rhs.amount + axis.z * rhs.axis.x,
                amount * rhs.axis.z + axis.x * rhs.axis.y - axis.y * rhs.axis.x + axis.z * rhs.amount
            },
            amount * rhs.amount - axis.x * rhs.axis.x - axis.y * rhs.axis.y - axis.z * rhs.axis.z
        };
    }

    INLINE void operator *= (const quat &rhs) {
        f32 x = axis.x; f32 X = rhs.axis.x;
        f32 y = axis.y; f32 Y = rhs.axis.y;
        f32 z = axis.z; f32 Z = rhs.axis.z;
        f32 w = amount; f32 W = rhs.amount;
        axis.x = X*w + W*x + Z*y - Y*z;
        axis.y = Y*w - Z*x + W*y + X*z;
        axis.z = Z*w + Y*x - X*y + W*z;
        amount = W*w - X*x - Y*y - Z*z;
    }

    INLINE quat conjugate() const {
        return {
            -axis,
            amount
        };
    }

    INLINE quat rotated(const vec3 &Axis, f32 radians) const {
        return *this * AxisAngle(Axis, radians);
    }

    static INLINE quat AxisAngle(const vec3 &axis, f32 radians) {
        radians *= 0.5f;
        return quat{
            axis * sinf(radians),
            cosf(radians)
        }.normalized();
    }

    static INLINE quat RotationAroundX(f32 radians) { return AxisAngle(vec3::X, radians); }
    static INLINE quat RotationAroundY(f32 radians) { return AxisAngle(vec3::Y, radians); }
    static INLINE quat RotationAroundZ(f32 radians) { return AxisAngle(vec3::Z, radians); }
    INLINE void setRotationAroundX(f32 radians) { *this = RotationAroundX(radians); }
    INLINE void setRotationAroundY(f32 radians) { *this = RotationAroundY(radians); }
    INLINE void setRotationAroundZ(f32 radians) { *this = RotationAroundZ(radians); }

    INLINE void setXYZ(vec3 &X, vec3 &Y, vec3 &Z) const {
        f32 q0 = amount;
        f32 q1 = -axis.x;
        f32 q2 = -axis.y;
        f32 q3 = -axis.z;

        X.x = 2 * (q0 * q0 + q1 * q1) - 1;
        X.y = 2 * (q1 * q2 - q0 * q3);
        X.z = 2 * (q1 * q3 + q0 * q2);

        Y.x = 2 * (q1 * q2 + q0 * q3);
        Y.y = 2 * (q0 * q0 + q2 * q2) - 1;
        Y.z = 2 * (q2 * q3 - q0 * q1);

        Z.x = 2 * (q1 * q3 - q0 * q2);
        Z.y = 2 * (q2 * q3 + q0 * q1);
        Z.z = 2 * (q0 * q0 + q3 * q3) - 1;
    }
};
quat quat::Identity = {};