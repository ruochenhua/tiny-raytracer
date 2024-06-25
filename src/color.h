#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "interval.h"
using color = vec3;
static const interval intensity(0.0, 0.999);

inline double linear_to_gamma(double linear_component)
{
    if(linear_component > 0)
    {
        return sqrt(linear_component);
    }

    return 0;
}
// util function to write color
void WriteColorToFile(FILE* imageFile, const color& pixelColor)
{
    auto r = linear_to_gamma(pixelColor.x());
    auto g = linear_to_gamma(pixelColor.y());
    auto b = linear_to_gamma(pixelColor.z());

    int ir = int(256 * intensity.clamp(r));
    int ig = int(256 * intensity.clamp(g));
    int ib = int(256 * intensity.clamp(b));

    fprintf(imageFile, "%d %d %d\n", ir, ig, ib);   // dimensions
}

#endif