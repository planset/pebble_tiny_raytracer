#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "src/mylib.h"
#include "src/raytracer.h"


void test_mylib(){
    assert(_floor(1.0) == 1);
    assert(_floor(1.1) == 1);
    assert(_floor(-1.0) == -1);
    assert(_floor(-1.1) == -2);

    assert(_pow(2, 0) == 1);
    assert(_pow(-3, 3) == -27);
    assert(_pow(1.234, 3) == 1.879080904);
    assert(_pow(-2, -3) == -0.125000);

    assert(_max(2, 4) == 4);
    assert(_max(3, 1) == 3);

    assert(_sqrt(16) == 4);
    assert(_sqrt(25) == 5);
    assert(_sqrt(0.0001) == 0.01);
    assert(_sqrt(0.000001) == 0.001);
    //assert(_sqrt(0.00000001) == 0.0001); //むり
}

void test_raytracer() {
    printf("start raytracer\n");

    byte raw_data[W * H] = {};
    byte result[W * H] = {};

    create_image(raw_data, result);

//    printf("raw_data\n");
//    for (int y=0; y<H; y++) {
//        for(int x=0; x<W; x++){
//            printf("%d ", raw_data[y*W+x]);
//        }
//        printf("\n");
//    }
//    printf("\n");

    printf("result\n");
    for (int y=0; y<H; y++) {
        for(int x=0; x<W; x++){
            printf("%d", result[y*W+x]);
        }
        printf("\n");
    }

    printf("end raytracer\n");
}

int main() {

    test_mylib();

    test_raytracer();

    return 0;
}
