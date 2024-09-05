#ifndef BIGINT_H_kejlw92l
#define BIGINT_H_kejlw92l

#include "vector.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef bigint_lib_assert
#include <assert.h>
#define bigint_lib_assert assert
#endif

#ifndef __BIGINT_READ_BUFFER_SIZE
#define __BIGINT_READ_BUFFER_SIZE 100000
#endif

#ifndef base
#define base 1000000000
#endif

vector(int) __bigint_lib_vector_format(vector(int) a) {
	while(vector_size(a) > 1 && vector_back(a) == 0)
		vector_pop_back(a);
	if(vector_empty(a))
		vector_push_back(a, 0);
	return a;
}

int __bigint_lib_vector_cmp(vector(int) a, vector(int) b) {
	if(!a)
		return b ? -1 : 0;
	if(!b)
		return a ? 1 : 0;
	bigint_lib_assert(a && b);
	a = __bigint_lib_vector_format(a);
	b = __bigint_lib_vector_format(b);
	if(vector_size(a) < vector_size(b))
		return -1;
	if(vector_size(a) > vector_size(b))
		return 1;
	bigint_lib_assert(vector_size(a) == vector_size(b));
	for(int i = (int)vector_size(a) - 1; i >= 0; --i) {
		if(a[i] < b[i])
			return -1;
		if(a[i] > b[i])
			return 1;
	}
	return 0;
}

vector(int) __bigint_lib_string_to_vector(char *str) {
	int len = strlen(str);
	vector(int) a = NULL;
	for(int i = len; i > 0; i -= 9) {
		str[i] = 0;
		vector_push_back(a, atoi( (i < 9) ? str : str + i - 9 ));
	}
	a = __bigint_lib_vector_format(a);
	return a;
}

void __bigint_lib_vector_file_print(FILE *f, vector(int) a) {
	a = __bigint_lib_vector_format(a);
	fprintf(f, "%d", vector_back(a));
	for(int i = (int)vector_size(a) - 2; i >= 0; --i)
		fprintf(f, "%09d", a[i]);
}
void __bigint_lib_vector_file_println(FILE *f, vector(int) a) {
	a = __bigint_lib_vector_format(a);
	fprintf(f, "%d", vector_back(a));
	for(int i = (int)vector_size(a) - 2; i >= 0; --i)
		fprintf(f, "%09d", a[i]);
	fputc('\n', f);
}
void __bigint_lib_vector_print(vector(int) a) {
	__bigint_lib_vector_file_print(stdout, a);
}
void __bigint_lib_vector_println(vector(int) a) {
	__bigint_lib_vector_file_println(stdout, a);
}

vector(int) __bigint_lib_vector_add(vector(int) a, vector(int) b) {
	int carry = 0;
	for(size_t i = 0; i < vector_size(a) || i < vector_size(b) || carry; ++i) {
		if(i == vector_size(a))
			vector_push_back(a, 0);
		a[i] += carry + (i < vector_size(b) ? b[i] : 0);
		carry = a[i] >= base;
		if(carry) a[i] -= base;
	}
	return a;
}

/*
 * It must hold a >= b here
 */
vector(int) __bigint_lib_vector_sub(vector(int) a, vector(int) b) {
	int carry = 0;
	for(size_t i = 0; i < vector_size(b) || carry; ++i) {
		bigint_lib_assert(i < vector_size(a));
		a[i] -= carry + (i < vector_size(b) ? b[i] : 0);
		carry = a[i] < 0;
		if(carry) a[i] += base;
	}
	a = __bigint_lib_vector_format(a);
	return a;
}

vector(int) __bigint_lib_vector_mul_int(vector(int) a, int b) {
	int carry = 0;
	for(size_t i = 0; i < vector_size(a) || carry; ++i) {
		if(i == vector_size(a))
			vector_push_back(a, 0);
		long long val = carry + (long long)a[i] * b;
		a[i] = val % base;
		carry = val / base;
	}
	a = __bigint_lib_vector_format(a);
	return a;
}

vector(int) __bigint_lib_vector_mul(vector(int) a, vector(int) b) {
	vector(int) c = NULL;
	vector_init_memset(c, (vector_size(a) + vector_size(b)), 0, NULL);
	for(size_t i = 0; i < vector_size(a); ++i) {
		for(int j = 0, carry = 0; j < (int)vector_size(b) || carry; ++j) {
			long long val = c[i + j] + carry + (long long)a[i] * (j < (int)vector_size(b) ? b[j] : 0);
			c[i + j] = val % base;
			carry = val / base;
		}
	}
	c = __bigint_lib_vector_format(c);
	return c;
}

typedef struct bigint {
	int sign;
	vector(int) ch;
} bigint;

void bigint_swap(bigint *a, bigint *b) {
	bigint tmp = *a;
	*a = *b;
	*b = tmp;
}

bigint bigint_copy(bigint a) {
	bigint b = a;
	b.ch = vector_copy(a.ch);
	bigint_lib_assert(a.ch != b.ch);
	return b;
}
void bigint_free(bigint a) {
	vector_free(a.ch);
}

bigint bigint_read_file(FILE *f) {
	static char buf[__BIGINT_READ_BUFFER_SIZE];
	char ch;
	int sgn = 0;
	for(ch = fgetc(f); !isdigit(ch); ch = fgetc(f))
		if(ch == '-')
			sgn ^= 1;

	size_t len = 0;
	for(; isdigit(ch); ch = fgetc(f))
		buf[len++] = ch;
	buf[len] = 0;

	bigint res;
	res.sign = sgn;
	res.ch = __bigint_lib_string_to_vector(buf);
	return res;
}
bigint bigint_read() {
	return bigint_read_file(stdin);
}

void bigint_print_file(FILE *f, bigint a) {
	if(a.sign)
		fputc('-', f);
	__bigint_lib_vector_print(a.ch);
}
void bigint_println_file(FILE *f, bigint a) {
	if(a.sign)
		fputc('-', f);
	__bigint_lib_vector_print(a.ch);
	putchar('\n');
}
void bigint_print(bigint a) {
	bigint_print_file(stdout, a);
}
void bigint_println(bigint a) {
	bigint_println_file(stdout, a);
}

void bigint_add_equal(bigint *a, bigint *b) {
	if(a->sign == b->sign) {
		a->ch = __bigint_lib_vector_add(a->ch, b->ch);
	}
	else {
		if(__bigint_lib_vector_cmp(a->ch, b->ch) < 0) {
			vector(int) bch_copy = vector_copy((b->ch));
			bch_copy = __bigint_lib_vector_sub(bch_copy, a->ch);
			a->sign ^= 1;
			vector_free(a->ch);
			a->ch = bch_copy;
		}
		else {
			a->ch = __bigint_lib_vector_sub(a->ch, b->ch);
		}
	}
}
void bigint_sub_equal(bigint *a, bigint *b) {
	b->sign ^= 1;
	bigint_add_equal(a, b);
	b->sign ^= 1;
}

bigint bigint_add(bigint a, bigint b) {
	bigint c = bigint_copy(a);
	bigint_add_equal(&c, &b);
	return c;
}

bigint bigint_sub(bigint a, bigint b) {
	bigint c = bigint_copy(a);
	bigint_sub_equal(&c, &b);
	return c;
}

bigint bigint_mul(bigint a, bigint b) {
	bigint c = {
		a.sign ^ b.sign,
		__bigint_lib_vector_mul(a.ch, b.ch)
	};
	return c;
}

#undef __BIGINT_READ_BUFFER_SIZE
#undef base

#endif
