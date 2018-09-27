#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

#define MIN(x,y) (((x)<(y))?(x):(y))
#define MAX(x,y) (((x)>(y))?(x):(y))
#define ABS(x) (((x) > 0) ? (x) : -(x))

namespace std{
	void __throw_bad_alloc();
	void __throw_length_error(char const*e);
	void __throw_out_of_range(char const*e);
	void __throw_out_of_range_fmt(char const*e, ...);
}

#ifdef __cplusplus

extern "C" {
#endif

int modulo(int nombre, int modulo);

#ifdef __cplusplus
}
#endif

#endif
