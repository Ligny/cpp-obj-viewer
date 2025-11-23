#ifndef MATH_HPP
#define MATH_HPP

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

Vec3 make_vec3(float x, float y, float z);
Vec2 make_vec2(float x, float y);

Vec3 sub_vec3(const Vec3 &a, const Vec3 &b);
Vec3 mul_vec3(const Vec3 &v, float s);
float dot_vec3(const Vec3 &a, const Vec3 &b);
Vec3 cross_vec3(const Vec3 &a, const Vec3 &b);
Vec3 normalize_vec3(const Vec3 &v);

Vec3 rotate_xy(const Vec3 &v, float angleY, float angleX);
Vec3 translate(const Vec3 &v, const Vec3 &t);

Vec2 project_perspective(const Vec3 &v, float fovScale,
                         float width, float height);

#endif
