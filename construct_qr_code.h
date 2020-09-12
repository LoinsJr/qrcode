#ifndef CONSTRUCT_QR_CODE_H
#define CONSTRUCT_QR_CODE_H

#include <stdbool.h>

enum { V1M_QR_CODE_SIZE = 21 };

bool **construct_qr_code_v1M(int message_length, const char *message);

void print_qr_code(int qr_code_size, bool **qr_code);

#endif