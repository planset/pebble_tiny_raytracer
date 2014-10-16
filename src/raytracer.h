/*
 * =====================================================================================
 *
 *       Filename:  raytracer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014/10/14 06時18分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#define W 60
#define H 60


typedef unsigned char byte;

typedef struct Point {
    double x;
    double y;
    double z;
} Point;

typedef struct Color {
    byte r;
    byte g;
    byte b;
} Color;

typedef struct Sphere {
    byte radius;
    Point pos;
    Color color;
    byte specular_exponent;
    byte reflectiveness;
} Sphere;

typedef struct Light {
    byte intensity;
    Point pos;
} Light;


void create_image(byte [], byte []);

