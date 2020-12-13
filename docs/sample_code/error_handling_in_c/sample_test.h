/*
 * Copyright (c) 2020 Micah Snyder
 */

/* Standard libs */
#include <stdbool.h>    /* For bool */
#include <stdlib.h>     /* For malloc/free */
#include <string.h>     /* For strdup */
#include <stdio.h>      /* For printf */

/* 3rd-party Libs */
#include <pthread.h>

typedef struct {
    char * name;
    void * data;
} named_data_t;

/* We'll allocate the array of data pointers in increments of 100 */
#define ARRAY_BLK_SZ 100

extern pthread_mutex_t data_array_lock;
extern named_data_t ** g_data_array;
extern size_t g_data_array_size;     /* Size of array (in element) */
extern size_t g_num_data_elements;   /* Number of element in array */

bool append_data_element(const char * name, void * data);
