#ifndef VECTOR_H_kei9s93k
#define VECTOR_H_kei9s93k

#include <stdlib.h>
#include <string.h>

#ifndef vector_clib_malloc
#define vector_clib_malloc malloc
#endif
#ifndef vector_clib_calloc
#define vector_clib_calloc calloc
#endif
#ifndef vector_clib_realloc
#define vector_clib_realloc realloc
#endif
#ifndef vector_clib_free
#define vector_clib_free free
#endif
#ifndef vector_clib_assert
#include <assert.h>
#define vector_clib_assert assert
#endif

typedef void(*vector_elem_destructor_t)(void *elem_ptr);

typedef struct vector_metadata_t {
	size_t size;
	size_t capacity;
	vector_elem_destructor_t elem_destructor;
} vector_metadata_t;

#define vector_type(T) T *
#define vector(T) vector_type(T)
#define vector_iterator(T) vector_type(T)

#ifdef VECTOR_LINEAR_GROW
#define vector_compute_next_grow(size) \
	((size) + 1)
#else
#define vector_compute_next_grow(size) \
	((size) ? ((size)<<1) : 1)
#endif

#define vec_to_base(vec) \
	(&( ((vector_metadata_t *)(vec))[-1] ))

#define base_to_vec(ptr) \
	(&( ((vector_metadata_t *)(ptr))[1] ))

#define vector_begin(vec) \
	(vec)
#define vector_end(vec) \
	( (vec) ? ((vec) + vector_size(vec)) : NULL )

#define vector_front(vec) \
	(*(vec))
#define vector_back(vec) \
	( vector_end(vec)[-1] )

#define sizeof_vector(vec) \
	( (vec) ? ( sizeof(vector_metadata_t) + vector_capacity(vec) * sizeof(*(vec)) ) : 0 )

#define vector_size(vec) \
	( (vec) ? ( vec_to_base(vec)->size ) : (size_t)0 )
#define vector_capacity(vec) \
	( (vec) ? ( vec_to_base(vec)->capacity ) : (size_t)0 )
#define vector_elem_destructor(vec) \
	( (vec) ? ( vec_to_base(vec)->elem_destructor ) : NULL )

#define vector_empty(vec) \
	( (vector_size(vec) == 0) ? 1 : 0 )

#define vector_set_size(vec, size_) \
	{ if(vec) vec_to_base(vec)->size = (size_); }

#define vector_set_capacity(vec, cap) \
	{ if(vec) vec_to_base(vec)->capacity = (cap); }

#define vector_set_elem_destructor(vec, elem_destructor_fn) {							\
	if(vec) {												\
		vec_to_base(vec)->elem_destructor = (elem_destructor_fn);					\
	}													\
}

#define vector_grow(vec, size) {										\
	size_t _vect_size = (size) * sizeof(*vec) + sizeof(vector_metadata_t);					\
	if(vec) {												\
		void *_p1__ = vec_to_base(vec);									\
		void *_p2__ = vector_clib_realloc(_p1__, _vect_size);						\
		vector_clib_assert(_p2__);									\
		(vec) = base_to_vec(_p2__);									\
	}													\
	else {													\
		void *_ptr__ = vector_clib_malloc(_vect_size);							\
		vector_clib_assert(_ptr__);									\
		(vec) = base_to_vec(_ptr__);									\
		vector_set_size(vec, 0);									\
		vector_set_elem_destructor(vec, NULL);								\
	}													\
	vector_set_capacity(vec, size);										\
}

#define vector_reserve(vec, size) {										\
	size_t _vect_cap = vector_capacity(vec);								\
	if(_vect_cap < (size)) {										\
		vector_grow(vec, size);										\
	}													\
}

#define vector_resize(vec, size) {										\
	vector_reserve(vec, size);										\
	vector_set_size(vec, size);										\
}

#define vector_clear(vec) \
	{ vector_set_size(vec, 0); }

#define vector_free(vec) {											\
	if(vec) {												\
		vector_elem_destructor_t _elem_dest_func = vector_elem_destructor(vec);				\
		if(_elem_dest_func) {										\
			for(size_t i = 0; i < vector_size(vec); ++i) {						\
				_elem_dest_func(&( (vec)[i] ));							\
			}											\
		}												\
		vector_clib_free(vec_to_base(vec));								\
	}													\
}

#define vector_init(vec, size, elem_destructor_fn) {								\
	if(!(vec)) {												\
		vector_grow(vec, size);										\
		vector_set_elem_destructor(vec, elem_destructor_fn);						\
	}													\
}

#define vector_init_memset(vec, size, value, elem_destructor_fn) {						\
	if(!(vec)) {												\
		vector_grow(vec, size);										\
		vector_set_size(vec, size);									\
		memset(vec, value, (size) * sizeof(*vec));							\
		vector_set_elem_destructor(vec, elem_destructor_fn);						\
	}													\
}

#define vector_init_fill(vec, size, value, elem_destructor_fn) {						\
	if(!(vec)) {												\
		vector_grow(vec, size);										\
		vector_set_size(vec, size);									\
		for(size_t i = 0; i < size; ++i) {								\
			(vec)[i] = (value);									\
		}												\
		vector_set_elem_destructor(vec, elem_destructor_fn);						\
	}													\
}

#define vector_push_back(vec, value) {										\
	size_t _vect_cap = vector_capacity(vec);								\
	if(_vect_cap <= vector_size(vec)) {									\
		vector_grow(vec, vector_compute_next_grow(_vect_cap));						\
	}													\
	size_t _vect_size = vector_size(vec);									\
	(vec)[_vect_size] = (value);										\
	vector_set_size(vec, _vect_size + 1);									\
}
#define vector_pop_back(vec) {											\
	if(vec) {												\
		size_t _vect_size = vector_size(vec);								\
		vector_clib_assert(_vect_size);									\
		vector_set_size(vec, _vect_size - 1);								\
	}													\
}

void *__vector_lib_copy(void *ptr, size_t size) {
	void *cop = malloc(size);
	memcpy(cop, ptr, size);
	return cop;
}

#define vector_copy(vec) \
	( base_to_vec(__vector_lib_copy(vec_to_base(vec), sizeof_vector(vec))) )

#endif
