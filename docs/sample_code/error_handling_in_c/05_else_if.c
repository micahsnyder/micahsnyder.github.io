/*
 * Copyright (c) 2020 Micah Snyder
 *
 * Example using "else-if" pattern.
 */

#include "sample_test.h"

bool allocate_global_data_array_if_needed() {
    bool status = false;

    if (NULL == g_data_array) {
        /* Array is not yet allocated */
        g_data_array = malloc(ARRAY_BLK_SZ * sizeof(named_data_t*));
        if (NULL == g_data_array) {
            /* Failed to allocate memory for data array! */
            goto done;
        }
        g_data_array_size = ARRAY_BLK_SZ;
    }

    if (g_num_data_elements == g_data_array_size) {
        /* Array is full, attempt to grow the array */
        named_data_t ** temp;
        temp = realloc(
            g_data_array,
            (g_data_array_size + ARRAY_BLK_SZ) * sizeof(named_data_t*));
        if (NULL == temp) {
            /* Array was is full, and we failed to allocate more memory */
            goto done;
        }
        g_data_array = temp;
        g_data_array_size += ARRAY_BLK_SZ;
    }

    status = true;

done:
    return status;
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
        /* We're given ownership of the data, so we'll assign the pointer. */
        new_element->data = data;

        if (false == add_element(new_element)) {
            /* Failed to add element */
        } else {
            /*
             * Successs
             */
            status = true;
            printf("add_named_rectangle: Added '%s' element to array!\n", name);
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
