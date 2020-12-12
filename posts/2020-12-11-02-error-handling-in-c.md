---
title: Error Handling in C
published_date: "2020-12-11 07:32:15 +0000"
layout: default.liquid
is_draft: false
---
# {{ page.title }}

{{ page.published_date }}

----

It is far too easy for C code to spring memory leaks due to shoddy error handling. In a larger code base poor error handling habits can result in years of agony tracking down bugs and digging out from small mountain of technical debt.

No thanks to Edsger Dijkstra's infamous 1968 letter "Go To Statement Considered Harmful", just about every C programmer has been told at one time that `goto` is "bad". This still rings true except for one use of `goto`: error handling.

In this article I'll present an overview of a few different error handling methods available to the C programmer. To illustrate, I've crated example multi-threaded application that stores named data-elements in a global growable array protected by a mutex. The function in our example adds one element to the array, and allocates more space for the array as needed.

By the end of this article, I hope you'll agree with me on the best option for error handling in C (_hint_: it's `goto`).

## Multiple `return`'s

Perhaps the most common method for error handling, especially in older C code involves the use of multiple `return` statements. Allocated memory, thread locks, and other resources are cleaned up as-needed at each `return`.

Without further ado, here's our first variant of the example program featuring the "Multiple `return`s" error handling method:

```c
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

/**
 * @brief Add a new named data element to the global array.
 *
 * @param name      Element name
 * @param data      Pointer to the element data
 * @return true     Element successfully added.
 * @return false    Failed to add element.
 */
bool append_data_element(const char * name, void * data) {
    named_data_t *new_element = NULL;

    if (NULL == name || NULL == data) {
        /* Bad args! */
        printf("add_named_rectangle: Invalid arguments!\n");
        return false;
    }

    /* Allocate a new struct to put on our array. */
    new_element = malloc(sizeof(named_data_t));
    if (NULL == new_element) {
        /* Out of memory! */
        return false;
    }

    /* We don't own the name, so let's duplicate it. */
    new_element->name = strdup(name);
    if (NULL == new_element->name) {
        /* Out of memory! */
        free(new_element);
        return false;
    }

    /* We're being given ownership of the data, so we'll just assign the pointer. */
    new_element->name = data;

    /* Lock the array so we can safely add our new element. */
    pthread_mutex_lock(&data_array_lock);

    /*
     * Append our data element to the end of the array.
     */
    if (NULL == g_data_array) {
        /* Array doesn't exist, let's allocate it */

        g_data_array = malloc(DATA_ARRAY_BLOCK_SIZE * sizeof(named_data_t*));
        if (NULL == g_data_array) {
            /* Failed to allocate memory for data array! */
            free(new_element->name);
            free(new_element);
            return false;
        }

        g_data_array_size = DATA_ARRAY_BLOCK_SIZE;

    } else if (g_num_data_elements == g_data_array_size) {
        /* Array is full, allocate more memory */
        named_data_t ** temp;

        temp = realloc(g_data_array, (g_data_array_size + DATA_ARRAY_BLOCK_SIZE) * sizeof(named_data_t*));
        if (NULL == temp) {
            /* Failed to increase size of data array! */
            free(new_element->name);
            free(new_element);
            return false;
        }

        g_data_array = temp;
        g_data_array_size += DATA_ARRAY_BLOCK_SIZE;
    }

    g_data_array[g_num_data_elements] = new_element;
    g_num_data_elements += 1;

    /* Unlock the array so other threads can access it once more. */
    pthread_mutex_unlock(&data_array_lock);

    printf("add_named_rectangle: Added '%s' element to data array!\n", name);

    return true;
}
```

As you review the above example, note how much duplicate code exists for resource cleanup during error handling. Can you see how easy it would be to forget to clean up a malloced pointer or a locked mutex. This is a recipe for disaster!

## The `do { ... } while(0)`

Another surprisingly popular option for error handling in C is to wrap all function code in a big ol' `do { ... } while(0);` "loop".

I'm not a big fan of the `do { ... } while(0);` approach to error handling for a few reasons...

First and foremost, it's an obvious hack to perform a `goto` without using the word `goto`!

Secondly, it's confusing. To the novice programmer, this approach for error handling is non-obvious because it abuses the do-while looping construct to perform a completely different task. An experienced programmer may appreciate such cleverness but the simple fact is that code which is hard to read is hard to maintain and increases the barrier to entry for novice programmers with little if any real benefit.

Additionally, I think the extra indentation of the  `do { ... } while(0);` is harder to read.

Here's our example using the do-while method:

```c
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

    do {
        if (NULL == name || NULL == data) {
            /* Bad args! */
            break;
        }

        /* Allocate a new struct to put on our array. */
        new_element = malloc(sizeof(named_data_t));
        if (NULL == new_element) {
            /* Out of memory! */
            break;
        }

        /* We don't own the name, so let's duplicate it. */
        new_element->name = strdup(name);
        if (NULL == new_element->name) {
            /* Out of memory! */
            break;
        }

        /* We're being given ownership of the data, so we'll just assign the pointer. */
        new_element->name = data;

        /* Lock the array so we can safely add our new element. */
        pthread_mutex_lock(&data_array_lock);

        /*
         * Append our data element to the end of the array.
         */
        if (NULL == g_data_array) {
            /* Array doesn't exist, let's allocate it */

            g_data_array = malloc(DATA_ARRAY_BLOCK_SIZE * sizeof(named_data_t*));
            if (NULL == g_data_array) {
                /* Failed to allocate memory for data array! */
                pthread_mutex_unlock(&data_array_lock);  /* !! We still have to unlock the mutex before we break */
                break;
            }

            g_data_array_size = DATA_ARRAY_BLOCK_SIZE;

        } else if (g_num_data_elements == g_data_array_size) {
            /* Array is full, allocate more memory */
            named_data_t ** temp;

            temp = realloc(g_data_array, (g_data_array_size + DATA_ARRAY_BLOCK_SIZE) * sizeof(named_data_t*));
            if (NULL == temp) {
                /* Failed to increase size of data array! */
                pthread_mutex_unlock(&data_array_lock);  /* !! We still have to unlock the mutex before we break */
                break;
            }
            g_data_array = temp;
            g_data_array_size += DATA_ARRAY_BLOCK_SIZE;
        }

        g_data_array[g_num_data_elements] = new_element;
        g_num_data_elements += 1;

        /* Unlock the array so other threads can access it once more. */
        pthread_mutex_unlock(&data_array_lock);

        /* Success! */
        status = true;
    } while(0);

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
```

But the real reason this method is bad is that it isn't as flexible as using `goto` for error handling. Here are a couple ways in which the do-while approach to error-handling and resource cleanup breaks down:

1. You can't have multiple exit points. With the `goto`, you can have more than one label if you have different ways of cleaning up. In the do-while example above, you'll see that a lack of multiple labels mean we still have to unlock the mutex before our `break` as there's no way to check if we'd locked the mutex without adding an extra variable.

2. You can't easily escape a loop within the scope of your do-while "loop". Instead, you'll end up having to add another variable and check to determine if you need to `break` a second time.

## Exception Handling

Ha-ha just kidding! 😅 While exception handling is actually available to C programmers on Windows (!), it's not accessible for cross-platform C software development. Call me a snob, but I believe that applications should be written to be as portable as possible.

## The `goto done`

Finally we get to the `goto done` method for error handling and resource cleanup. I don't wish to anger the ghost of Edsger Dijkstra so I should warn you that the key to using `goto` "right" are thus:

1. _Always_ go-forwards! **NEVER** go backwards!
2. Keep it simple! Multiple labels are cool and may help from time to time, like in our example. But 9 times out of 10 you only need one label. Also, I should have to say this but keep your functions at a reasonable size.  They shouldn't be 1-liners but if they get much larger than a page or two, consider refactoring!

Here I present to you the example code using the `goto` method:

```c
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

    /* We're being given ownership of the data, so we'll just assign the pointer. */
    new_element->name = data;

    /* Lock the array so we can safely add our new element. */
    pthread_mutex_lock(&data_array_lock);

    /*
     * Append our data element to the end of the array.
     */
    if (NULL == g_data_array) {
        /* Array doesn't exist, let's allocate it */

        g_data_array = malloc(DATA_ARRAY_BLOCK_SIZE * sizeof(named_data_t*));
        if (NULL == g_data_array) {
            /* Failed to allocate memory for data array! */
            goto unlock;
        }

        g_data_array_size = DATA_ARRAY_BLOCK_SIZE;

    } else if (g_num_data_elements == g_data_array_size) {
        /* Array is full, allocate more memory */
        named_data_t ** temp;

        temp = realloc(g_data_array, (g_data_array_size + DATA_ARRAY_BLOCK_SIZE) * sizeof(named_data_t*));
        if (NULL == temp) {
            /* Failed to increase size of data array! */
            goto unlock;
        }
        g_data_array = temp;
        g_data_array_size += DATA_ARRAY_BLOCK_SIZE;
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
```

Another advantage of the `goto` method is that with `goto`'s and a common scheme for the label (like `done`), you can wrap `malloc`, `strdup`, `realloc`, and other functions with macros to try to enforce consistent error handling when allocation functions fail.

Eg:

```c
#define DO_MALLOC(var, size) \
    do {                     \
        var = malloc(size);  \
        if (NULL == var) {   \
            goto done;       \
        }                    \
    } while (0)
```

`*`As a side note, you may have noticed my the use of a `do { ... } while (0);` there. As it turns out, this confusing-looking contsruct does unfortunately have a place in C programming as a mechanism for writing safe and reliable function-like macros. It is once again a bit of a hack, but it's the best option we have. For more information, have a gander at [this explanation on StackOverflow](https://stackoverflow.com/questions/1067226/c-multi-line-macro-do-while0-vs-scope-block).

So to make use of the macro wrappers in our original example, we have this final variant of our original program:

```c
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
#define DATA_ARRAY_BLOCK_SIZE 100

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

    /* We're being given ownership of the data, so we'll just assign the pointer. */
    new_element->name = data;

    /* Lock the array so we can safely add our new element. */
    pthread_mutex_lock(&data_array_lock);

    /*
     * Append our data element to the end of the array.
     */
    if (NULL == g_data_array) {
        /* Array doesn't exist, let's allocate it */

        DO_MALLOC(g_data_array, DATA_ARRAY_BLOCK_SIZE * sizeof(named_data_t*));
        g_data_array_size = DATA_ARRAY_BLOCK_SIZE;

    } else if (g_num_data_elements == g_data_array_size) {
        /* Array is full, allocate more memory */
        DO_REALLOC(g_data_array, (g_data_array_size + DATA_ARRAY_BLOCK_SIZE) * sizeof(named_data_t*));
        g_data_array_size += DATA_ARRAY_BLOCK_SIZE;
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
```

To end this piece, I'll leave you with some supporting links for how to safely use goto for error handling, and why perhaps do/while(0) can cause problems (with loops):
- https://embeddedgurus.com/barr-code/2018/06/cs-goto-keyword-should-we-use-it-or-lose-it/
- https://ayende.com/blog/183521-C/error-handling-via-goto-in-c
- https://www.cprogramming.com/tutorial/goto.html
- https://embeddedgurus.com/stack-overflow/2010/02/goto-heresy/


Until next time!

-Micah

## *12/11/2020* - Postscript; The `else if`

There is one additional error handling pattern that can work quite well in many situations. I call this one the `else if`.

This pattern works when you're able to construct each line of code as a series of `else if` statements that test for failure with one final `else` for the success-condition. Resource cleanup happens _after_ the final `else` block such that each failure condition and the success condition a followed immediately by resource cleanup.

However, when following this pattern you may at some point need to set a variable in the middle or make some call doesn't chain. This breaks up the `else if` flow and can complicate your error handling pattern. It may be an indication that you should break up the function into multiple functions but resolving these `else if` interruptions in an elegant way often requires careful planning.

Here's an attempt to apply the `else if` error handling pattern to our example program:

```c
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
```

The `else if` pattern is works out ok for a few calls in but it breaks down a bit when you need to set variables or make standalone calls like our call to lock & unlock the mutex, or to allocate memory for the global data array if needed.

With some careful planning and well structured functions, this pattern can be quite effective, but it does take a surprising amount of effort.

I used to like the `else if` pattern quite a bit. Nowadays, the `goto done` pattern is my error handling method of choice. The `goto done` pattern is easier to read, write, and maintain.
