#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include "rt_common.h"
// 贴图类
class texture
{
public:
    virtual ~texture() = default;
    virtual color value(double u, double v, const point3& p) const = 0;
};

// 固定颜色
class solid_color : public texture
{
public:
    solid_color(const color& a) : albedo(a)
    {
        
    }

    solid_color(double red, double green, double blue)
        : solid_color(color(red, green, blue)) {}

    color value(double u, double v, const point3& p) const override
    {
        // 这种贴图无论uv值如何，都是返回一种颜色
        return albedo;
    }

private:
    color albedo;
};

// 固定贴图，或者叫做空间贴图（贴图颜色和空间位置相关）
// 这里实现一种棋盘式的贴图

class checker_texture : public texture
{
public:
    checker_texture(double scale, shared_ptr<texture> even_tex, shared_ptr<texture> odd_tex)
        : inv_scale(1.0/scale), even(even_tex), odd(odd_tex) {}

    checker_texture(double scale, const color& c1, const color& c2)
        : inv_scale(1.0/scale)
        , even(make_shared<solid_color>(c1))
        , odd(make_shared<solid_color>(c2)) {}

    color value(double u, double v, const point3& p) const override
    {
        auto x_int = static_cast<int>(std::floor(inv_scale * p.x()));
        auto y_int = static_cast<int>(std::floor(inv_scale * p.y()));
        auto z_int = static_cast<int>(std::floor(inv_scale * p.z()));

        bool is_even = (x_int + y_int + z_int) % 2 == 0;

        return is_even ? even->value(u, v, p) : odd->value(u, v, p);
    }
private:
    double inv_scale;
    shared_ptr<texture> even;
    shared_ptr<texture> odd;
};

#endif