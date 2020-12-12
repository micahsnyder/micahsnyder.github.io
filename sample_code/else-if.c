/* Standard libs */
#include <stdbool.h>    /* For bool */
#include <stdint.h>     /* For modern integer types */
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
#define DATA_ARRAY_BLOCK_SIZE 100

static pthread_mutex_t data_array_lock = PTHREAD_MUTEX_INITIALIZER;
static named_data_t ** g_data_array = NULL;
static size_t g_data_array_size = 0;    /* Size of array (in element) */
static size_t g_num_data_elements = 0;  /* Number of element in array */

bool allocate_global_data_array_if_needed() {
    named_data_t ** temp;

    if ((NULL == g_data_array) &&
        (NULL == (g_data_array = malloc(DATA_ARRAY_BLOCK_SIZE * sizeof(named_data_t*))))) {
        /* Failed to allocate memory for data array! */
        return false;
    } else {
        g_data_array_size = DATA_ARRAY_BLOCK_SIZE;
    }

    if ((g_num_data_elements == g_data_array_size) &&
        (NULL == (temp = realloc(g_data_array, (g_data_array_size + DATA_ARRAY_BLOCK_SIZE) * sizeof(named_data_t*))))) {
        /* Array was is full, and we failed to allocate more memory */
        return false;
    } else {
        g_data_array = temp;
        g_data_array_size += DATA_ARRAY_BLOCK_SIZE;
    }

    return true;
}

bool add_element(named_data_t *new_element) {
    bool status = false;

    /* Lock the array so we can safely add our new element. */
    pthread_mutex_lock(&data_array_lock);

    /*
     * Append our data element to the end of the array.
     */
    if (false == allocate_global_data_array_if_needed()) {
        /* Failed to allocate the global data array */
    } else {
        /*
         * Successs
         */
        g_data_array[g_num_data_elements] = new_element;
        g_num_data_elements += 1;

        status = true;
    }

    /* Unlock the array so other threads can access it once more. */
    pthread_mutex_unlock(&data_array_lock);

    return status;
}

/**
 * @brief Add a new named data element to the global array.
 *
 * @param name      Element name
 * @param data      Pointer to the element data
 * @return true     Element successfully added.
 * @return false    Failed to add element.
 */
bool append_data_element(const char * name, void * data) {
    bool status = false;
    named_data_t *new_element = NULL;

    if (NULL == name || NULL == data) {
        /* Bad args! */
        printf("add_named_rectangle: Invalid arguments!\n");
    } else if (NULL == (new_element = malloc(sizeof(named_data_t)))) {
        /* Out of memory! */
    } else if (NULL == (new_element->name = strdup(name))){
        /* Out of memory! */
    } else {
        /* We're being given ownership of the data, so we'll just assign the pointer. */
        new_element->name = data;

        if (false == add_element(new_element)) {
            /* Failed to add element */
        } else {
            /*
             * Successs
             */
            printf("add_named_rectangle: Added '%s' element to data array!\n", name);
        }
    }

    /* Clean up, as needed */
    if (false == status) {
        if (NULL != new_element) {
            if (NULL != new_element->name) {
                free(new_element->name);
            }
            free(new_element);
        }
    }

    return status;
}

int main(void) {
    if (true == append_data_element("Hello", (void *)"World")) {
        printf("Added element to array\n");
        return 0;
    }
    printf("Failed to add element to array\n");
    return 1;
}
