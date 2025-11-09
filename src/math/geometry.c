#include <math/geometry.h>

void rotate_x(vec3 *v, double angle) {
    float s = sin(angle), c = cos(angle);
    double y = v->y, z = v->z;

    v->y = c * y - s * z;
    v->z = s * y + c * z;
}

void rotate_y(vec3 *v, double angle) {
    float s = sin(angle), c = cos(angle);
    double x = v->x, z = v->z;

    v->x = c * x + s * z;
    v->z = -s * x + c * z;
}

void rotate_z(vec3 *v, double angle) {
    float s = sin(angle), c = cos(angle);
    double x = v->x, y = v->y;

    v->x = c * x - s * y;
    v->y = s * x + c * y;
}

vec2 project(vec3 *v, int width, int height, int scale, double z_offset) {
    vec2 res;
    double x = v->x, y = v->y, z = v->z;
    z += z_offset;

    res.x = width / 2 + scale * x / z;
    res.y = height / 2 + scale * y / z;

    return res;
}
