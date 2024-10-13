#include "../source/malloc.h"

int main()
{
    void *temp[100] = {0};
    for (size_t i = 0; i < 100; i++)
    {
        void *p = malloc(1);
        if (i % 2 == 0)
            temp[i] = p;
    }

    for (size_t i = 0; i < 100; i++)
    {
        free(temp[i]);
    }
    void *p = malloc(42);
    for (size_t i = 0; i < 100; i++)
    {
        void *p = malloc(1);
        if (i % 2 == 0)
            temp[i] = p;
    }
    p = malloc(42);

    show_alloc_mem();

    return (0);
}
