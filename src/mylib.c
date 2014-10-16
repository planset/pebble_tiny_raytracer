#include "mylib.h"

int _floor(float val){
    int t = ((int)(val*10))%10;
    if (val < 0 && t){
        val -= 1;
    }
    return (int)val;
}

double _pow(double val, int mul){
    if (mul == 0){
        val = 1;
    } else if (mul > 0) {
        double _val = val;
        for (int i=mul-1; i--;){
            val *= _val;
        }
    } else {
        double _val = 1.0/val;
        val = _val;
        for (int i=-1*mul-1; i--;){
            val *= _val;
        }
    }
    return val;
}

double _max(double a, double b) {
    return a > b ? a : b;
}

double _sqrt( const double val )
{
    if (val <=0) 
        return 0;

    double x = val / 2.0 ;

    for (int i=16; i--;){
        x = (x + val / x) / 2.0 ;
    }

    return x;
}

