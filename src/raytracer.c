#include "raytracer.h"
#include "mylib.h"

#define DEBUG 1

#ifdef DEBUG
#include <stdio.h>
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...) //
#endif

static Sphere spheres[] = {
    {W, {0, -W, 0}, {9, 0, 0}, 100, 4},
    {1, {0,  1, 4}, {9, 9, 9}, 100, 2}
};

static byte ambient_light = 2;

static Light lights[] = {
    {8, {2, 2, 0}}
};

static void error_diffusion_method(
        byte *result, byte *gray_image, 
        int height, int width)
{

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            byte f = gray_image[y * width + x];
            byte e;

            if ( f > 127 ) {
                e = f - 255;
                f = 1;

            } else {
                e = f;
                f = 0;
            }

            result[y * width + x] = f;

            /* 誤差をばら撒く */
            if( x != width - 1 )
                gray_image[y * width + x + 1] += _floor((7.0 / 16) * e);

            if( (x != 0) && (y != height - 1) )
                gray_image[(y + 1) * width + x - 1] += _floor((3.0 / 16) * e);

            if( y != height - 1 )
                gray_image[(y + 1) * width + x] += _floor((5.0 / 16) * e);

            if (x != width - 1 && y != height - 1)
                gray_image[(y + 1) * width + x + 1] += _floor((1.0 / 16) * e);

        }
    }
}

static double dot(Point *A, Point *B) {
    return A->x * B->x + A->y * B->y + A->z * B->z;
}

static Point A_minus_Bk(Point *A, Point *B, double k) {
    return (Point){A->x - B->x * k, A->y - B->y * k, A->z - B->z * k};
}

static int closest_intersection(
        Point *B, Point *D, 
        double t_min, double t_max,
        double *t)
{
    *t = W;
    double a = 2 * dot(D, D);

    int spheres_length = sizeof spheres / sizeof spheres[0];

    int v = -1;

    for (int i = 0; i < spheres_length; i++) {
        byte r = spheres[i].radius;
        Point j = A_minus_Bk(B, &spheres[i].pos, 1);
        double b = -2 * dot(&j, D);

        double d = _sqrt(b*b -2*a*(dot(&j, &j) - r*r));
        if (d > 0){
            for (int e = 2; e--; d = -d) {
                double f = (b - d)/a;
                if (t_min < f && f < t_max && f < *t) { 
                    v = i;
                    *t = f;
                }
            }
        }
    }

    return v;
}

static double trace_ray(Point *B, Point *D, 
                       double t_min, double t_max, int depth,
                       byte color_type)
{
    double t;
    int s = closest_intersection(B, D, t_min, t_max, &t);
    if (s == -1) {
        return 0;
    }

    Point X = A_minus_Bk(B, D, -t);
    Point N = A_minus_Bk(&X, &spheres[s].pos, 1);

    double d = dot(D, D);
    double n = dot(&N, &N);

    double ambient_light_intensity = ambient_light;

    // For each light
    int lights_length = sizeof lights / sizeof lights[0];

    for (int i=0; i<lights_length; i++) { 
        Point L = A_minus_Bk(&lights[i].pos, &X, 1);
        double k = dot(&N, &L);

        // Add to lighting
        Point M = A_minus_Bk(&L, &N, 2*k/n);
        int v = (closest_intersection(&X, &L, 1.0/W, 1, &t)==-1)? 1 : 0;
        ambient_light_intensity += lights[i].intensity * v *
            (_max(0, k / _sqrt(dot(&L, &L)*n))
             + _max(0, _pow(dot(&M, D) / _sqrt(dot(&M, &M)*d),
                           spheres[s].specular_exponent)
                  ));
    }

    double local_color = 0;
    switch(color_type){
        case 0: 
            local_color = spheres[s].color.r*ambient_light_intensity*2.8;
            break;
        case 1: 
            local_color = spheres[s].color.g*ambient_light_intensity*2.8;
            break;
        case 2:
            local_color = spheres[s].color.b*ambient_light_intensity*2.8;
            break;
    }

    double ref = spheres[s].reflectiveness/9.0;
    Point R = A_minus_Bk(D, &N, 2*dot(&N, D)/n);
    return depth-- ? trace_ray(&X, &R, 1.0/W, W, depth, color_type)*ref
        + local_color*(1 - ref)
        : local_color;
}

void create_image(byte raw_data[], byte result[]) {
    int h = (int)W/2;
    int out_idx = 0;

    Point camera = {0, 1, 0};

    for (int y = h; y-- > -h;) {
        for (int x = -h; x++ < h;) { 
            Point p = (Point){((double)x)/W, ((double)y)/W, 1};
            int R = trace_ray(&camera, &p, 1, W, 2, 0);
            int G = trace_ray(&camera, &p, 1, W, 2, 1);
            int B = trace_ray(&camera, &p, 1, W, 2, 2);

            raw_data[out_idx++] = (77 * R + 150 * G + 29 * B) >> 8;
        }
    }

    error_diffusion_method(result, raw_data, H, W);
}


