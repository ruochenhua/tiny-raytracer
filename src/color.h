#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "interval.h"
using color = vec3;
static const interval intensity(0.0, 0.999);
// util function to write color
void WriteColorToFile(FILE* imageFile, const color& pixelColor)
{
    auto r = pixelColor.x();
    auto g = pixelColor.y();
    auto b = pixelColor.z();

    int ir = int(256 * intensity.clamp(r));
    int ig = int(256 * intensity.clamp(g));
    int ib = int(256 * intensity.clamp(b));

    fprintf(imageFile, "%d %d %d\n", ir, ig, ib);   // dimensions
}

#endif