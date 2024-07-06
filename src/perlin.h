#pragma once
#ifndef PERLIN_H
#define PERLIN_H

#include "rt_common.h"
// perlin噪声生成类
// 这里贴一篇perlin算法的实现例子https://blog.csdn.net/qq_38680728/article/details/122110828
class perlin
{
public:
    perlin()
    {
        randvec = new vec3[point_count];
        for(int i = 0; i < point_count; ++i)
        {
            randvec[i] = unit_vector(vec3::random(-1, 1));
        }

        perm_x = perlin_generate_perm();
        perm_y = perlin_generate_perm();
        perm_z = perlin_generate_perm();
    }

    ~perlin()
    {
        delete[] randvec;
        delete[] perm_x;
        delete[] perm_y;
        delete[] perm_z;
    }

    double noise(const point3& p) const
    {
        // 获取小数点后面的值
        auto u = p.x() - floor(p.x());
        auto v = p.y() - floor(p.y());
        auto w = p.z() - floor(p.z());
        
        auto i = int(floor(p.x()));
        auto j = int(floor(p.y()));
        auto k = int(floor(p.z()));
        
        vec3 c[2][2][2];
        // 获取p点所在区域（索引floor和ceiling之间）两个点的perlin噪音值
        for (int di=0; di < 2; di++)
            for (int dj=0; dj < 2; dj++)
                for (int dk=0; dk < 2; dk++)
                    c[di][dj][dk] = randvec[
                        perm_x[(i+di) & 255] ^
                        perm_y[(j+dj) & 255] ^
                        perm_z[(k+dk) & 255]
                    ];

        // 根据所在区间两个点的数值和在这个区间的小数点坐标，做三线性插值得到p点的perlin噪音值
        // return trilinear_interp(c, u, v, w);
        return perlin_interp(c, u, v, w);
        //return randfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
    }
private:
    static const int point_count = 256;
    vec3* randvec = nullptr;
    int *perm_x, *perm_y, *perm_z;

    int* perlin_generate_perm()
    {
        auto p = new int[point_count];
        for(int i = 0; i < point_count; ++i)
        {
            p[i] = i;
        }

        permute(p, point_count);

        return p;
    }

    void permute(int* p, int n)
    {
        for(int i = n-1; i > 0; i--)
        {
            // 选择一个随机交换
            int target = rand() % (i+1);
            std::swap(p[i], p[target]);
        }
    }

    // 三线性插值
    // 已知c[0][0][0]和c[1][1][1]的值，获取坐标uvw（c之间）的值
    double trilinear_interp(double c[2][2][2], double u, double v, double w) const
    {
        auto accum = 0.0;
        for(int i = 0; i < 2; ++i)
        {
            for(int j = 0; j < 2; ++j)
            {
                for(int k = 0; k < 2; ++k)
                {
                    accum += (i*u + (1-i)*(1-u)) * (j*v + (1-j)*(1-v)) * (k*w + (1-k)*(1-w))
                            * c[i][j][k];
                }
            }
        }
        
        return accum;
    }

    double perlin_interp(const vec3 c[2][2][2], double u, double v, double w) const
    {
        // 利用Hermitian Smoothing（埃尔米特平滑）将线性插值的一些artifact处理掉
        u = u*u*(3-2*u);
        v = v*v*(3-2*v);
        w = w*w*(3-2*w);

        auto accum = 0.0;
        for(int i = 0; i < 2; ++i)
        {
            for(int j = 0; j < 2; ++j)
            {
                for(int k = 0; k < 2; ++k)
                {
                    vec3 weight_v(u-i, v-j, w-k);
                    accum += (i*u + (1-i)*(1-u)) * (j*v + (1-j)*(1-v)) * (k*w + (1-k)*(1-w))
                            * dot(c[i][j][k], weight_v);
                }
            }
        }
        
        return accum;
    }
    
};

#endif