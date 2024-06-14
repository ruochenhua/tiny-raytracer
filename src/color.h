#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
using color = vec3;

// util function to write color
void WriteColor(std::ostream& out, const color& pixelColor)
{
    auto r = pixelColor.x();
    auto g = pixelColor.y();
    auto b = pixelColor.z();

    int rbyte = int(255.999 * r);
    int gbyte = int(255.999 * g);
    int bbyte = int(255.999 * b);

    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

void WriteColorToFile(FILE* imageFile, const color& pixelColor)
{
    auto r = pixelColor.x();
    auto g = pixelColor.y();
    auto b = pixelColor.z();

    int ir = int(255.999 * r);
    int ig = int(255.999 * g);
    int ib = int(255.999 * b);

    fprintf(imageFile, "%d %d %d\n", ir, ig, ib);   // dimensions
}

#endif