#pragma once
#ifndef VEC3_H
#define VEC3_H

#include "rt_common.h"

// vector 3 class
class vec3
{
public:
    double e[3];

    vec3() : e{0, 0, 0} {}
    vec3(double x, double y, double z) : e{x, y, z} {}

    double x() const {return e[0];}
    double y() const {return e[1];}
    double z() const {return e[2];}

    vec3 operator-() const {return vec3(-e[0], -e[1], -e[2]);}
    double operator[](int i) const {return e[i];}
    double& operator[](int i) {return e[i];}

    vec3& operator+=(const vec3& v)
    {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];

        return *this;
    }
    
    vec3& operator*=(double t)
    {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;

        return *this;
    }

    vec3& operator/=(double t)
    {
        return *this *= 1/t;
    }

    double length_squared() const
    {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }
    
    double length() const 
    {
        return sqrt(length_squared());
    }

    bool near_zero() const
    {
        auto MIN_VALUE = 1e-8;
        return (fabs(e[0]) < MIN_VALUE) && (fabs(e[1]) < MIN_VALUE) && (fabs(e[2]) < MIN_VALUE);
    }

    static vec3 random()
    {
        return vec3(random_double(), random_double(), random_double());
    }

    static vec3 random(double min, double max)
    {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }
};

// point3 is an alias for vec3
using point3 = vec3;

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3& v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

inline vec3 operator/(const vec3& v, double t) {
    return (1/t) * v;
}

inline double dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(const vec3& v) {
    return v / v.length();
}

inline vec3 random_in_unit_sphere()
{
    // 这里是教程的简单随机向量算法，要保证向量在一个半径为1的球内
    // 一直执行随机算法直到找到合适的值(why?为什么不随机一个值然后直接normalize?)
    // todo：肯定有优化方法
    while(true)
    {
        auto p = vec3::random(-1, 1);
        if(p.length_squared() < 1)
        {
            return p;
        }
    }
}

inline vec3 random_unit_vector()
{
    return unit_vector(random_in_unit_sphere());
}

// 获取法线同一面的，在球面上的随机向量
inline vec3 random_on_hemisphere(const vec3& normal)
{
    vec3 on_unit_sphere = random_unit_vector();
    if(dot(on_unit_sphere, normal) > 0.0)
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

// 计算反射
inline vec3 reflect(const vec3& in_ray, const vec3& normal)
{
    return in_ray - 2*dot(in_ray, normal) * normal;
}

// 根据斯涅尔定律（折射定律）计算光的折射行为
// refraction_ratio是光穿越两个介质的折射率的比值，具体是初始介质除以进入的介质
inline vec3 refract(const vec3& in_ray, const vec3& normal, double refraction_ratio)
{
    auto cos_theta = fmin(dot(-in_ray, normal), 1.0);
    vec3 r_out_perp = refraction_ratio * (in_ray + cos_theta*normal);   //折射向量垂直于法线的分量
    vec3 r_out_parall = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * normal;    //折射向量平行于法线的分量
    return r_out_parall + r_out_perp;
}

#endif