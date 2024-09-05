#ifndef FUN_H
#define FUN_H

size_t least_power_of_two(size_t _n) {
	size_t res = 1;
	while(res < _n)
		res <<= 1;
	return res;
}

#endif
