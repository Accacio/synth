#include "aux.hpp"

double lerp(double a, double b, double i){
    double newi= fmax(0,fmin(1,i));
    // TODO verificar
    // printf("%f %f %f",i,newi,-a*(newi-1)+b*newi);
    return -a*(newi-1) + b*newi;
}
double ilerp(double a, double b, double i){

    return (i-a)/(b-a);
}
