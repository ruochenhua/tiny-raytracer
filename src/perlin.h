#pragma once
#ifndef PERLIN_H
#define PERLIN_H

#include "rt_common.h"
//perlin噪声生成类
class perlin
{
public:
    perlin()
    {
        randfloat = new double[point_count];
        for(int i = 0; i < point_count; ++i)
        {
            randfloat[i] = random_double();
        }

        perm_x = perlin_generate_perm();
        perm_y = perlin_generate_perm();
        perm_z = perlin_generate_perm();
    }

    ~perlin()
    {
        delete[] randfloat;
        delete[] perm_x;
        delete[] perm_y;
        delete[] perm_z;
    }

    double noise(const point3& p) const
    {
        auto i = int(4*p.x()) & 255;
        auto j = int(4*p.y()) & 255;
        auto k = int(4*p.z()) & 255;

        return randfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
    }
private:
    static const int point_count = 256;
    double *randfloat;
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
            // int tmp = p[i];
            // p[i] = p[target];
            // p[target] = tmp;
            std::swap(p[i], p[target]);
        }
    }
};

#endif