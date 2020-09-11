#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

extern int encode_message_v1M(unsigned, char *, unsigned, bool *);

extern int get_symbol_code(char);
extern int convert_binary_to_decimal(bool *, int);
extern int convert_decimal_to_alpha_notation(int);
extern int convert_alpha_notation_to_decimal(int);

extern unsigned char *get_ec_codewords_v1M(int, bool *, int, bool*);
int main(int argc, char **argv)
{
    bool *array = calloc(208, sizeof(*array));
    bool *bin = calloc(5, sizeof(*bin));
    bin[3] = 1;
    bin[4] = 1;
    printf("%d\n", convert_alpha_notation_to_decimal(convert_decimal_to_alpha_notation(205)));
    printf("%d\n", encode_message_v1M(11, "HELLO WORLD", 208, array));
    //printf("%d\n", get_ec_codewords_v1M(128, array, 11, 0));
    unsigned char *ecc = get_ec_codewords_v1M(128, array, 80, 0);
    for (int i = 0; i < 10; ++i)
    {
        printf("%u ", ecc[i]);
    }
    return 0;
}
