#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

extern int get_bit_sequence_v1M(int message_length, const char * message, int buffer_size, bool *buffer);

int main(int argc, char **argv)
{
    bool *array = calloc(208, sizeof(*array));
    get_bit_sequence_v1M(11, "HELLO WORLD", 208, array);

    for (int i = 0; i < 208; ++i)
    {
        printf("%d", array[i]);
    }

    return 0;
}
