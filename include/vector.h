#ifndef VECTOR_H
#define VECTOR_H

#include "debug.h"
#include "fun.h"

#include <stdlib.h>

#define declare_vector(T)\
struct vector_##T {\
	size_t _size, _max_size;\
	T *_buf;\
};\
void _double_size_vector_##T(struct vector_##T *_vect) {\
	_vect->_max_size <<= 1;\
	_vect->_buf = (T*)ptr_handler(realloc((void*)_vect->_buf, _vect->_max_size * sizeof(T)), "Realloc() return NULL pointer in _double_size() function.\n");\
}\
void free_vector_##T(struct vector_##T *_vect) {\
	if(_vect->_buf) {\
		free(_vect->_buf);\
		_vect->_buf = NULL;\
	}\
}\
void init_size_vector_##T(struct vector_##T *_vect, size_t init_size) {\
	_vect->_size = init_size;\
	_vect->_max_size = least_power_of_two(init_size);\
	_vect->_buf = (T*)ptr_handler(calloc(_vect->_max_size, sizeof(T)), "Calloc() return NULL pointer in vector init(size_t) function.\n");\
}\
void init_empty_vector_##T(struct vector_##T *_vect) {\
	init_size_vector_##T(_vect, 2);\
	_vect->_size = 0;\
}\
void init_value_vector_##T(struct vector_##T *_vect, size_t init_size, T init_val) {\
	_vect->_size = init_size;\
	_vect->_max_size = least_power_of_two(init_size);\
	_vect->_buf = (T*)ptr_handler(malloc(_vect->_max_size * sizeof(T)), "Malloc() return NULL pointer in vector init(size_t, T) function.\n");\
	for(size_t i = 0; i < _vect->_size; ++i)\
		_vect->_buf[i] = init_val;\
}\
int empty_vector_##T(struct vector_##T *_vect) {\
	return (_vect->_size == 0) ? 1 : 0;\
}\
size_t size_vector_##T(struct vector_##T *_vect) {\
	return _vect->_size;\
}\
size_t max_size_vector_##T(struct vector_##T *_vect) {\
	return _vect->_max_size;\
}\
void push_back_vector_##T(struct vector_##T *_vect, T val) {\
	if(_vect->_size == _vect->_max_size)\
		_double_size_vector_##T(_vect);\
	_vect->_buf[_vect->_size++] = val;\
}\
void pop_back_vector_##T(struct vector_##T *_vect) {\
	if(_vect->_size == 0) {\
		perror("pop_back() function used by an empty vector.\n");\
		exit(1);\
	}\
	--_vect->_size;\
}

#endif
