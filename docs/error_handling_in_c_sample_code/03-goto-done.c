/*
 * Copyright (c) 2020 Micah Snyder
 *
 * Example using "goto done" pattern.
 */

#include "sample_test.h"

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
    new_element = malloc(sizeof(named_data_t));
    if (NULL == new_element) {
        /* Out of memory! */
        goto done;
    }

    /* We don't own the name, so let's duplicate it. */
    new_element->name = strdup(name);
    if (NULL == new_element->name) {
        /* Out of memory! */
        goto done;
    }

    /* We're given ownership of the data, so we'll assign the pointer. */
    new_element->data = data;

    /* Lock the array so we can safely add our new element. */
    pthread_mutex_lock(&data_array_lock);

    /*
     * Append our data element to the end of the array.
     */
    if (NULL == g_data_array) {
        /* Array doesn't exist, let's allocate it */

        g_data_array = malloc(ARRAY_BLK_SZ * sizeof(named_data_t*));
        if (NULL == g_data_array) {
            /* Failed to allocate memory for data array! */
            goto unlock;
        }

        g_data_array_size = ARRAY_BLK_SZ;

    } else if (g_num_data_elements == g_data_array_size) {
        /* Array is full, allocate more memory */
        named_data_t ** temp;

        temp = realloc(
            g_data_array,
            (g_data_array_size + ARRAY_BLK_SZ) * sizeof(named_data_t*));
        if (NULL == temp) {
            /* Failed to increase size of data array! */
            goto unlock;
        }
        g_data_array = temp;
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
