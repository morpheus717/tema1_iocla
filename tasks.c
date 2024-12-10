#include "functional.h"
#include "tasks.h"
#include "tests.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void reverse_acc_func(void *acc, void *elem) {
	array_t *ptracc = (array_t *)acc;
	ptracc->len--;
	memcpy(ptracc->data + ptracc->len * ptracc->elem_size,
		   elem, ptracc->elem_size);
}

array_t reverse(array_t list) {
	array_t acc;
	acc.len = list.len;
	acc.destructor = list.destructor;
	acc.elem_size = list.elem_size;
	acc.data = malloc(acc.elem_size * acc.len);
	reduce(reverse_acc_func, &acc, list);
	acc.len = list.len;
	return acc;
}

void number_t_destructor(void *elem) {
	number_t *pelem = elem;
	free(pelem->string);
}

void create_array(array_t *arr, int elem_size, int len,
				  void (*destructor)(void *elem)) {
	arr->elem_size = elem_size;
	arr->destructor = destructor;
	arr->len = len;
	arr->data = malloc(arr->elem_size * arr->len);
}

void nr_array_func(void *new_elem, void **column) {
	int integer = *(int *)column[0];
	int frac = *(int *)column[1];
	char *string = malloc(25 * sizeof(char));
	sprintf(string, "%d.%d", integer, frac);
	number_t *new = (number_t *)new_elem;
	new->integer_part = integer;
	new->fractional_part = frac;
	new->string = string;
}

array_t create_number_array(array_t integer_part, array_t fractional_part) {
	return map_multiple(nr_array_func, sizeof(number_t), number_t_destructor,
						2, integer_part, fractional_part);
}

boolean pass_or_no(void *elem) {
	student_t curr_stud = *(student_t *)elem;
	if (curr_stud.grade >= 5.0)
		return 1;
	return 0;
}

array_t get_passing_students_names(array_t list) {
	array_t filtered = filter(pass_or_no, list);
	return filtered;
}

void subtract_sum(void *out, void *in) {
	*(int *)out -= *(int *)in;
}

void check_sum(void *res, void **in) {
	array_t list = *((array_t *)(in[0]));
	int sum = *((int *)(in[1]));
	reduce(subtract_sum, &sum, list);
	*(boolean *)res = (sum <= 0);
}

array_t check_bigger_sum(array_t list_list, array_t int_list) {
	return map_multiple(check_sum, sizeof(boolean), NULL, 2,
						list_list, int_list);
}

void null_odds(void *acc, void *elem) {
	array_t *ptracc = (array_t *)acc;
	char *celem = malloc(strlen(*(char **)elem) + 1);
	if (ptracc->len % 2 == 0)
		strcpy(celem, *(char **)elem);
	else
		strcpy(celem, "");
	memcpy((ptracc->data + ptracc->len * ptracc->elem_size),
		   &celem, ptracc->elem_size);
	ptracc->len++;
}

boolean elim_nulls(void *elem) {
	if (strlen(*((char **)elem)) == 0)
		return 0;
	return 1;
}

array_t get_even_indexed_strings(array_t list) {
	array_t acc;
	create_array(&acc, list.elem_size, list.len, list.destructor);
	acc.len = 0;
	reduce(null_odds, &acc, list);
	for_each(list.destructor, list);
	free(list.data);
	return filter(elim_nulls, acc);
}

void array2_destructor(void *elem) {
	array_t ptre = *(array_t *)elem;
	array_t arr_in_arr = *(array_t *)(ptre.data);
	for_each(free, arr_in_arr);
}

array_t generate_square_matrix(int n) {
	array_t mat;
	create_array(&mat, sizeof(array_t), n, array2_destructor);
	
}
