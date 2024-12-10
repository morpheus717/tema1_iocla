#include "functional.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void for_each(void (*func)(void *), array_t list)
{
	for (int i = 0; i < list.len; i++)
		func((list.data + (i * list.elem_size)));
}

array_t map(void (*func)(void *, void *),
			int new_list_elem_size,
			void (*new_list_destructor)(void *),
			array_t list)
{
	array_t new;
	new.len = list.len;
	new.destructor = new_list_destructor;
	new.elem_size = new_list_elem_size;
	new.data = malloc(new.elem_size * new.len);
	for (int i = 0; i < list.len; i++) {
		func(new.data + i * new.elem_size, list.data + i * list.elem_size);
		if (list.destructor)
			list.destructor(list.data + i * list.elem_size);
	}
	free(list.data);
	return new;
}

array_t filter(boolean(*func)(void *), array_t list)
{
	array_t new;
	new.elem_size = list.elem_size;
	new.destructor = list.destructor;
	int new_len = 0;
	for (int i = 0; i < list.len; i++)
		new_len += !!func(list.data + i * list.elem_size);
	new.len = new_len;
	new.data = malloc(new.len * new.elem_size);
	int k = 0;
	for (int i = 0; i < list.len; i++) {
		if (!!func(list.data + i * list.elem_size)) {
			memcpy(new.data + k * new.elem_size, list.data +
				   i * list.elem_size, new.elem_size);
			k++;
		} else if (list.destructor) {
			list.destructor(list.data + i * list.elem_size);
		}
	}
	free(list.data);
	return new;
}

void *reduce(void (*func)(void *, void *), void *acc, array_t list)
{
	for (int i = 0; i < list.len; i++)
		func(acc, list.data + i * list.elem_size);
	return acc;
}

void for_each_multiple(void(*func)(void **), int varg_c, ...)
{
	va_list ptr;
	va_start(ptr, varg_c);
	int min_dim = __INT_MAX__;
	array_t *vector = malloc(sizeof(array_t) * varg_c);
	for (int i = 0; i < varg_c; i++) {
		vector[i] = va_arg(ptr, array_t);
		if (vector[i].len < min_dim)
			min_dim = vector[i].len;
	}
	va_end(ptr);
	void **column = malloc(sizeof(void *) * varg_c);
	for (int i = 0; i < min_dim; i++) {
		for (int j = 0; j < varg_c; j++)
			column[j] = vector[j].data + i * vector[j].elem_size;
		func(column);
	}
	free(column);
	free(vector);
}

array_t map_multiple(void (*func)(void *, void **),
					 int new_list_elem_size,
					 void (*new_list_destructor)(void *),
					 int varg_c, ...)
{
	array_t new;
	new.destructor = new_list_destructor;
	new.elem_size = new_list_elem_size;
	va_list ptr;
	va_start(ptr, varg_c);
	int min_dim = __INT_MAX__;
	array_t *vector = malloc(sizeof(array_t) * varg_c);
	for (int i = 0; i < varg_c; i++) {
		vector[i] = va_arg(ptr, array_t);
		if (vector[i].len < min_dim)
			min_dim = vector[i].len;
	}
	va_end(ptr);
	new.len = min_dim;
	new.data = malloc(new.elem_size * new.len);
	void **column = malloc(sizeof(void *) * varg_c);
	for (int i = 0; i < min_dim; i++) {
		for (int j = 0; j < varg_c; j++)
			column[j] = vector[j].data + i * vector[j].elem_size;
		func(new.data + i * new.elem_size, column);
	}
	for (int i = 0; i < varg_c; i++) {
		if (vector[i].destructor) {
			for (int j = 0; j < vector[i].len; j++)
				vector[i].destructor(vector[i].data + j * vector[i].elem_size);
		}
		free(vector[i].data);
	}
	free(vector);
	free(column);
	return new;
}

void *reduce_multiple(void(*func)(void *, void **), void *acc, int varg_c, ...)
{
	va_list ptr;
	va_start(ptr, varg_c);
	array_t *vector = malloc(sizeof(array_t) * varg_c);
	int min_dim = __INT_MAX__;
	for (int i = 0; i < varg_c; i++) {
		vector[i] = va_arg(ptr, array_t);
		if (vector[i].len < min_dim)
			min_dim = vector[i].len;
	}
	va_end(ptr);
	void **column = malloc(sizeof(void *) * varg_c);
	for (int i = 0; i < min_dim; i++) {
		for (int j = 0; j < varg_c; j++)
			column[j] = vector[j].data + i * vector[j].elem_size;
		func(acc, column);
	}
	free(vector);
	free(column);
	return NULL;
}
