#ifndef CALC_H
#define CALC_H

#include <stdint.h>

#define PI 3.141592653589793
/* Math operations */
double add(double a, double b);
double sub(double a, double b);
double mul(double a, double b);
double divi(double a, double b);

/* Calculator logic */
double calculate(double a, double b, char op);

/* Convert double → string */
void double_to_string(double value, char* buf);

/* Main calculator */
int minimal_calc(uint32_t* fb, uint32_t pitch);

#endif

