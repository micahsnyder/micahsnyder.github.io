/* Standard libs */
#include <stdbool.h>    /* For bool */
#include <stdint.h>     /* For modern integer types */
#include <stdlib.h>     /* For malloc/free */
#include <string.h>     /* For strdup */
#include <stdio.h>      /* For printf */

/* 3rd-party Libs */
#include <pthread.h>

#define DO_MALLOC(var, size)        \
    do {                            \
        var = malloc(size);         \
        if (NULL == var) {          \
            goto done;              \
        }                           \
    } while (0)

#define DO_REALLOC(var, size)       \
    do {                            \
        void * temp;                \
        temp = realloc(var, size);  \
        if (NULL == temp) {         \
            goto done;              \
        }                           \
        var = temp;                 \
    } while (0)

#define DO_STRDUP(var, buf)         \
    do {                            \
        var = strdup(buf);          \
        if (NULL == var) {          \
            goto done;              \
        }                           \
    } while (0)

typedef struct {
    char * name;
    void * data;
} named_data_t;

/* We'll allocate the array of data pointers in increments of 100 */
#define ARRAY_BLK_SZ 100

static pthread_mutex_t data_array_lock = PTHREAD_MUTEX_INITIALIZER;
static named_data_t ** g_data_array = NULL;
static size_t g_data_array_size = 0; /* Size of array (in element) */
static size_t g_num_data_elements = 0;  /* Number of element in array */

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
        goto done;
    }

    /* Allocate a new struct to put on our array. */
    DO_MALLOC(new_element, sizeof(named_data_t));

    /* We don't own the name, so let's duplicate it. */
    DO_STRDUP(new_element->name, name);

    /* We're given ownership of the data, so we'll assign the pointer. */
    new_element->name = data;

    /* Lock the array so we can safely add our new element. */
    pthread_mutex_lock(&data_array_lock);

    /*
     * Append our data element to the end of the array.
     */
    if (NULL == g_data_array) {
        /* Array doesn't exist, let's allocate it */

        DO_MALLOC(g_data_array, ARRAY_BLK_SZ * sizeof(named_data_t*));
        g_data_array_size = ARRAY_BLK_SZ;

    } else if (g_num_data_elements == g_data_array_size) {
        /* Array is full, allocate more memory */
        DO_REALLOC(
            g_data_array,
            (g_data_array_size + ARRAY_BLK_SZ) * sizeof(named_data_t*));
        g_data_array_size += ARRAY_BLK_SZ;
    }

    g_data_array[g_num_data_elements] = new_element;
    g_num_data_elements += 1;

    /* Success! */
    status = true;

unlock:
    /* Unlock the array so other threads can access it once more. */
    pthread_mutex_unlock(&data_array_lock);

done:

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
