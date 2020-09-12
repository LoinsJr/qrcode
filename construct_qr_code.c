#include "construct_qr_code.h"

#include <stdio.h>
#include <stdlib.h>

extern int get_bit_sequence_v1M(int message_length, const char * message, int buffer_size, bool *buffer);

static void add_finder_patterns(int qr_code_size, bool **qr_code);

static void add_timing_patterns(int qr_code_size, bool **qr_code);

static void place_data_bits(int qr_code_size, bool **qr_code, const bool *bit_sequence);

bool **construct_qr_code_v1M(int message_length, const char *message)
{
    enum { BIT_SEQUENCE_SIZE = 208 };
    bool *bit_sequence = calloc(BIT_SEQUENCE_SIZE, sizeof(*bit_sequence));
    int error_code = get_bit_sequence_v1M(message_length, message, BIT_SEQUENCE_SIZE, bit_sequence);
    bool **qr_code = calloc(V1M_QR_CODE_SIZE, sizeof(*qr_code));
    for (int i = 0; i < V1M_QR_CODE_SIZE; ++i) {
        qr_code[i] = calloc(V1M_QR_CODE_SIZE, sizeof(**qr_code));
    }

    add_finder_patterns(V1M_QR_CODE_SIZE, qr_code);
    add_timing_patterns(V1M_QR_CODE_SIZE, qr_code);
    qr_code[13][8] = 1; /* dark module */
    place_data_bits(V1M_QR_CODE_SIZE, qr_code, bit_sequence);

    free(bit_sequence);
    return qr_code;
}

void print_qr_code(int qr_code_size, bool **qr_code)
{
    for (int i = 0; i < qr_code_size; ++i) {
        for (int j = 0; j < qr_code_size; ++j) {
            printf("%d", qr_code[i][j]);
        }
        puts("");
    }
}

static void add_finder_patterns(int qr_code_size, bool **qr_code)
{
    enum { FINDER_PATTERNS_AMOUNT = 3 };
    struct position { int pos_i; int pos_j; };
    const struct position CENTER_POSITIONS[FINDER_PATTERNS_AMOUNT] = {
        { 3, 3 },
        { qr_code_size - 4, 3 },
        { 3, qr_code_size - 4 }
    };
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            for (int k = 0; k < FINDER_PATTERNS_AMOUNT; ++k) {
                int center_i = CENTER_POSITIONS[k].pos_i;
                int center_j = CENTER_POSITIONS[k].pos_j;
                qr_code[center_i + i][center_j + j] = 1;
            }
        }
    }
    for (int i = -3; i <= 3; ++i) {
        for (int k = 0; k < FINDER_PATTERNS_AMOUNT; ++k) {
            int center_i = CENTER_POSITIONS[k].pos_i;
            int center_j = CENTER_POSITIONS[k].pos_j;
            qr_code[center_i - 3][center_j + i] = 1;
            qr_code[center_i + 3][center_j + i] = 1;
            qr_code[center_i + i][center_j - 3] = 1;
            qr_code[center_i + i][center_j + 3] = 1;
        }
    }
}

static void add_timing_patterns(int qr_code_size, bool **qr_code)
{
    enum { FINDER_PATTERN_SIZE = 7 };
    int i = FINDER_PATTERN_SIZE + 1;
    while (i < qr_code_size - FINDER_PATTERN_SIZE) {
        qr_code[FINDER_PATTERN_SIZE - 1][i] = 1;
        qr_code[i][FINDER_PATTERN_SIZE - 1] = 1;
        i += 2;
    }
}

static void place_data_bits(int qr_code_size, bool **qr_code, const bool *bit_sequence)
{
    int k = 0;
    int i = qr_code_size - 1;
    int j = i;
    bool upwards = true;
    while (j >= 0) {
        if (j == 6) {
            i = 9;
            j = 5;
            upwards = false;
        }
        if (!(i <= 8 && j <= 8 || i <= 8 && j >= 13 ||
                i == 6 || i >= 13 && j <= 8)) {
            qr_code[i][j] = bit_sequence[k++];
            qr_code[i][j - 1] = bit_sequence[k++];
        }
        i += upwards ? -1 : 1;
        if (i >= qr_code_size || i < 0) {
            j -= 2;
            i -= upwards ? -1 : 1;
            upwards = !upwards;
        }
    }
}