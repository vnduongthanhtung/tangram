#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <stdlib.h>

#define array(type)						struct {type* data; int size;}
#define array2(type)					struct {type* data; int rows; int cols;}


#define EMPTY_ARRAY						{NULL, 0}
#define EMPTY_ARRAY2					{NULL, 0, 0}

#define gen_var(a) 						a ## __gen
#define _array(type)					array_ ## type
#define _array2(type)					array2_ ## type

#define declare_array(type, arr, sz) 	_array(type) arr = EMPTY_ARRAY;\
										type gen_var(arr)[sz];\
										arr.data = gen_var(arr);\
										arr.size = sz;									
										
#define new_array(type, size)			{(type*)calloc(size, sizeof(type)), size}

#define free_array(arr)					{\
											free((arr).data);\
											(arr).data = NULL;\
											(arr).size = 0;\
										}

#define declare_array2(type,arr,sz1,sz2) \
										_array2(type) arr = EMPTY_ARRAY2;\
										type gen_var(arr)[sz1*sz2];\
										arr.data = gen_var(arr);\
										arr.rows = sz1;\
										arr.cols = sz2;\
										
										
#define new_array2(type,sz1,sz2)		{(type*)calloc(sz1*sz2, sizeof(type)), sz1, sz2}

#define free_array2(arr) 				{\
											free((arr).data);\
											(arr).data = NULL;\
											(arr).rows = 0;\
											(arr).cols = 0;\
										}										

#define at_array_q(arr,i) 				((arr).data[i])
#define at_array2_q(arr,i,j) 			((arr).data[(i)*(arr).cols+(j)])

#define at_array_s(arr,i) 				(*(((unsigned)(i)<(arr).size)? ((arr).data+(i)) : NULL))
#define at_array2_s(arr,i,j) 			(*(((unsigned)(i)<(arr).rows&&(unsigned)(j)<(arr).cols)? ((arr).data+(i)*(arr).cols+(j)) : NULL))

#ifdef __DEBUG__
#define at_array(arr,i) 				at_array_s(arr,i)
#define at_array2(arr,i,j) 				at_array2_s(arr,i,j)
#else
#define at_array(arr,i)					at_array_q(arr,i)
#define at_array2(arr,i,j)				at_array2_q(arr,i,j)				
#endif

#define at_parray(parr, i)				at_array(*(parr),i)
#define at_parray2(parr,i,j)			at_array2(*(parr),i,j)

typedef array(int) 						array_int;
typedef array(char) 					array_char;
typedef array(long) 					array_long;
typedef array(short) 					array_short;
typedef array(double) 					array_double;
typedef array(float) 					array_float;
typedef array(unsigned char) 			array_byte;
typedef array(void*) 					array_ptr;

typedef array2(int) 					array2_int;
typedef array2(char) 					array2_char;
typedef array2(long) 					array2_long;
typedef array2(short) 					array2_short;
typedef array2(double) 					array2_double;
typedef array2(float) 					array2_float;
typedef array2(unsigned char) 			array2_byte;
#endif
