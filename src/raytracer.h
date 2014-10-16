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

