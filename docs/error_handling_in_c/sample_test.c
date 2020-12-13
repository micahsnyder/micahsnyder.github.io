/*
 * Copyright (c) 2020 Micah Snyder
 *
 * Application to test the sample code for the "Error Handling in C" article.
 */

/* Standard headers */
#include <stdbool.h>    /* For bool */
#include <stdlib.h>     /* For malloc/free */
#include <string.h>     /* For strdup */
#include <stdio.h>      /* For printf */

/* 3rd-party headers */
#include <pthread.h>

/* Our headers */
#include "sample_test.h"

pthread_mutex_t data_array_lock = PTHREAD_MUTEX_INITIALIZER;
named_data_t ** g_data_array    = NULL;
size_t g_data_array_size        = 0;
size_t g_num_data_elements      = 0;

/**
 * @brief Clean up the global data array.
 */
void free_data_array() {
    size_t i = 0;

    /* Lock the array so we can safely add our new element. */
    pthread_mutex_lock(&data_array_lock);

    if (NULL == g_data_array) {
        /* Nothing to clean up */
        goto unlock;
    }
    for (i = 0; i < g_num_data_elements; i++) {
        /*
         * Free each data element.
         * NULL checks not required because the num elements
         * integer indicates that the element was allocated.
         */
        free(g_data_array[i]->name);
        free(g_data_array[i]);
    }
    free(g_data_array);
    g_data_array = NULL;

unlock:
    /* Unlock the array so other threads can access it once more. */
    pthread_mutex_unlock(&data_array_lock);

    return;
}

int main(void) {
    int status = 1;
    if (false == append_data_element("Hello", (void *)"World")) {
        printf("Failed to add element to array\n");
        goto done;
    }
    status = 0;
    printf("Added element to array\n");

done:
    free_data_array();
    return status;
}
