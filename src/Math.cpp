#include "Math.hpp"
#include <cmath>

Vec3 make_vec3(float x, float y, float z)
{
    Vec3 v;

    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

Vec2 make_vec2(float x, float y)
{
    Vec2 v;

    v.x = x;
    v.y = y;
    return v;
}

Vec3 sub_vec3(const Vec3 &a, const Vec3 &b)
{
    Vec3 r;

    r.x = a.x - b.x;
    r.y = a.y - b.y;
    r.z = a.z - b.z;
    return r;
}

Vec3 mul_vec3(const Vec3 &v, float s)
{
    Vec3 r;

    r.x = v.x * s;
    r.y = v.y * s;
    r.z = v.z * s;
    return r;
}

float dot_vec3(const Vec3 &a, const Vec3 &b)
{
    float d;

    d = a.x * b.x + a.y * b.y + a.z * b.z;
    return d;
}

Vec3 cross_vec3(const Vec3 &a, const Vec3 &b)
{
    Vec3 r;

    r.x = a.y * b.z - a.z * b.y;
    r.y = a.z * b.x - a.x * b.z;
    r.z = a.x * b.y - a.y * b.x;
    return r;
}

Vec3 normalize_vec3(const Vec3 &v)
{
    float len;
    float invLen;
    Vec3 r;

    len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len == 0.0f)
        return make_vec3(0.0f, 0.0f, 0.0f);
    invLen = 1.0f / len;
    r.x = v.x * invLen;
    r.y = v.y * invLen;
    r.z = v.z * invLen;
    return r;
}

Vec3 rotate_xy(const Vec3 &v, float angleY, float angleX)
{
    float cy;
    float sy;
    float cx;
    float sx;
    float x1;
    float z1;
    float y1;
    Vec3 r;

    cy = std::cos(angleY);
    sy = std::sin(angleY);
    cx = std::cos(angleX);
    sx = std::sin(angleX);
    x1 = v.x * cy + v.z * sy;
    z1 = -v.x * sy + v.z * cy;
    y1 = v.y;
    r.x = x1;
    r.y = y1 * cx - z1 * sx;
    r.z = y1 * sx + z1 * cx;
    return r;
}

Vec3 translate(const Vec3 &v, const Vec3 &t)
{
    Vec3 r;

    r.x = v.x + t.x;
    r.y = v.y + t.y;
    r.z = v.z + t.z;
    return r;
}

Vec2 project_perspective(const Vec3 &v, float fovScale,
                         float width, float height)
{
    float invz;
    float sx;
    float sy;
    Vec2 p;

    invz = 1.0f / v.z;
    sx = v.x * invz * fovScale;
    sy = v.y * invz * fovScale;
    p.x = width * 0.5f + sx * width * 0.5f;
    p.y = height * 0.5f - sy * height * 0.5f;
    return p;
}
