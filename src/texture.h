#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include "rt_common.h"
#include "rt_stb_image.h"
#include "perlin.h"

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

// 图片类型贴图
class image_texture : public texture
{
public:
    image_texture(const char* filename) : image(filename) {}

    color value(double u, double v, const point3& p) const override
    {
        // 贴图错误，返回一个debug值
        if(image.height() <= 0 ) return color(0, 1, 1);

        // uv需要在[0,1],v的坐标需要反一下（图片是上到下，我们object的v计算是按照下到上）
        u = interval(0, 1).clamp(u);
        v = 1.0 - interval(0, 1).clamp(v);

        auto i = int(u*image.width());
        auto j = int(v*image.height());
        auto pixel = image.pixel_data(i, j);

        auto color_scale = 1.0/255.0;

        return color_scale*color(pixel[0], pixel[1], pixel[2]);
    }

private:
    rtw_image image;
};

// 噪声贴图（perlin）
class noise_texture : public texture
{
public:
    noise_texture() {}
    noise_texture(double in_scale) : scale(in_scale) {}
    color value(double u, double v, const point3& p) const override
    {
        // 噪声的颜色只和位置相关
        // 这里noise.noise的值是[-1,1],需要映射到[0,1]
        // return color(1,1,1) * 0.5 *(1.0 +  noise.noise(p * scale));
        // return color(.5, .5, .5) * (1 + sin(scale * p.z() + 10 * noise.turb(p, 7)));
        return color(1,1,1) * noise.turb(p, 7);
    }
    
private:
    perlin noise;
    double scale = 1.0;
};


#endif