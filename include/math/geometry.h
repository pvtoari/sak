#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "math/math.h"

typedef struct {
    double x, y;
} vec2;

typedef struct {
    double x, y, z;
} vec3;

void rotate_x(vec3 *v, double angle);

void rotate_y(vec3 *v, double angle);

void rotate_z(vec3 *v, double angle);

vec2 project(vec3 *v, int width, int height, int scale, double z_offset);

#endif // GEOMETRY_H
