#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

extern int encode_data_v1M(unsigned, char *, unsigned, bool *);

extern int get_symbol_code(char);

int main(int argc, char **argv)
{
    bool *array = calloc(208, sizeof(*array));
    printf("%d\n", encode_data_v1M(11, "HELLO WORLD", 208, array));
    for (int i = 0; i < 208; ++i)
    {
        printf("%d", array[i]);
    }
    return 0;
}
