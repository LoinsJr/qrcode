#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "construct_qr_code.h"

int main(int argc, char **argv)
{
    bool **q = construct_qr_code_v1M(11, "HELLO WORLD");   
    print_qr_code(V1M_QR_CODE_SIZE, q);
    return 0;
}
