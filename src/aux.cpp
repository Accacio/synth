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

double square(double time, double freq){
    double timbre = std::sin(time * 1 * freq * 2 * M_PI )>0? 1: -1;
    return timbre*0.8;
}

double saw(double time, double freq){

    double timbre =
       (2.0/M_PI)*(freq*M_PI* fmod(time, 1/freq))- M_PI/2;
    return timbre;
}

double sine(double time, double freq){

    double timbre =
        std::sin(time * 1 * freq * 2 * M_PI );
    return timbre;
}

double noise(double time, double freq){

    double timbre =
        2*((double) rand()/ (double) RAND_MAX)-1;
    return timbre;
}
