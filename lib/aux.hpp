#ifndef __AUX_H_
#define __AUX_H_


#include <cmath>

double ilerp(double a, double b, double i);
double lerp(double a, double b, double i);
double square(double time, double freq);
double saw(double time, double freq);
double sine(double time, double freq);
double noise(double time, double freq);


#endif // __AUX_H_
