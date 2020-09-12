#include "construct_qr_code.h"

#include <assert.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

extern int get_bit_sequence_v1M(int message_length, const char * message, int buffer_size, bool *buffer);

static void add_finder_patterns(int qr_code_size, bool **qr_code);

static void add_timing_patterns(int qr_code_size, bool **qr_code);

static void place_data_bits(int qr_code_size, bool **qr_code, const bool *bit_sequence);

static void apply_mask(int qr_code_size, bool **qr_code);

static int count_1st_penalty(int qr_code_size, bool **qr_code);

static int count_2nd_penalty(int qr_code_size, bool **qr_code);

static int count_3rd_penalty(int qr_code_size, bool **qr_code);

static int count_4th_penalty(int qr_code_size, bool **qr_code);

static int count_mask_penalty(int qr_code_size, bool **qr_code, int mask_pattern_number);

static bool use_mask_pattern(int i, int j, int mask_pattern_number);

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
    //print_qr_code(V1M_QR_CODE_SIZE, qr_code);
    apply_mask(V1M_QR_CODE_SIZE, qr_code);

    free(bit_sequence);
    return qr_code;
}

void print_qr_code(int qr_code_size, bool **qr_code)
{
    for (int i = 0; i < qr_code_size; ++i) {
        for (int j = 0; j < qr_code_size; ++j) {
            printf("%d ", qr_code[i][j]);
        }
        printf("\n");
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
    enum { FINDER_PATTERN_SIZE = 8 };
    int i = FINDER_PATTERN_SIZE;
    while (i < qr_code_size - FINDER_PATTERN_SIZE) {
        qr_code[FINDER_PATTERN_SIZE - 2][i] = 1;
        qr_code[i][FINDER_PATTERN_SIZE - 2] = 1;
        i += 2;
    }
}

static void place_data_bits(int qr_code_size, bool **qr_code, const bool *bit_sequence)
{
    enum { FINDER_PATTERN_SIZE = 8, TIMING_PATTERN_POSITION = 6 };
    int k = 0;
    int i = qr_code_size - 1;
    int j = i;
    bool upwards = true;
    while (j >= 0) {
        if (j == TIMING_PATTERN_POSITION) {
            i = 9;
            j = 5;
            upwards = false;
        }
        if (!(i <= FINDER_PATTERN_SIZE && j <= FINDER_PATTERN_SIZE ||
                i <= FINDER_PATTERN_SIZE && j >= qr_code_size - FINDER_PATTERN_SIZE ||
                i == TIMING_PATTERN_POSITION || j == TIMING_PATTERN_POSITION ||
                i >= qr_code_size - FINDER_PATTERN_SIZE && j <= FINDER_PATTERN_SIZE)) {
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

static void apply_mask(int qr_code_size, bool **qr_code)
{
    enum { MASK_PATTERNS_AMOUNT = 8, FINDER_PATTERN_SIZE = 8, TIMING_PATTERN_POSITION = 6 };
    int best_mask_number = 0;
    int min_penalty = count_mask_penalty(qr_code_size, qr_code, 0);
    for (int i = 1; i < MASK_PATTERNS_AMOUNT; ++i) {
        int penalty = count_mask_penalty(qr_code_size, qr_code, i);
        if (penalty < min_penalty) {
            best_mask_number = i;
            min_penalty = penalty;
        }
    }
    for (int i = 0; i < qr_code_size; ++i) {
        for (int j = 0; j < qr_code_size; ++j) {
            if (!(i <= FINDER_PATTERN_SIZE && j <= FINDER_PATTERN_SIZE ||
                    i <= FINDER_PATTERN_SIZE && j >= qr_code_size - FINDER_PATTERN_SIZE ||
                    i == TIMING_PATTERN_POSITION || j == TIMING_PATTERN_POSITION ||
                    i >= qr_code_size - FINDER_PATTERN_SIZE && j <= FINDER_PATTERN_SIZE) &&
                    use_mask_pattern(i, j, best_mask_number)) {
                qr_code[i][j] = !qr_code[i][j];
            }
        }
    }
}

static int count_1st_penalty(int qr_code_size, bool **qr_code)
{
    int penalty = 0;
    for (int i = 0; i < qr_code_size; ++i) {
        int same_moduls_in_row_amount = -1;
        int same_moduls_in_column_amount = -1;
        bool last_row_modul = qr_code[i][0];
        bool last_column_modul = qr_code[0][i];
        for (int j = 0; j < qr_code_size; ++j) {
            if (last_row_modul == qr_code[i][j]) {
                same_moduls_in_row_amount++;
            }
            else {
                same_moduls_in_row_amount = 0;
            }
            if (last_column_modul == qr_code[j][i]) {
                same_moduls_in_column_amount++;
            }
            else {
                same_moduls_in_column_amount = 0;
            }
            if (same_moduls_in_row_amount == 5) {
                penalty += 3;
            }
            else if (same_moduls_in_row_amount > 5) {
                penalty++;
            }
            if (same_moduls_in_column_amount == 5) {
                penalty += 3;
            }
            else if (same_moduls_in_column_amount > 5) {
                penalty++;
            }
            last_row_modul = qr_code[i][j];
            last_column_modul = qr_code[j][i];
        }
    }
    return penalty;
}

static int count_2nd_penalty(int qr_code_size, bool **qr_code)
{
    int penalty = 0;
    for (int i = 0; i < qr_code_size - 1; ++i) {
        for (int j = 0; j < qr_code_size - 1; ++j) {
            if (qr_code[i][j] == qr_code[i][j + 1] && 
                    qr_code[i][j] == qr_code[i + 1][j] &&
                    qr_code[i][j] == qr_code[i + 1][j + 1]) {
                penalty += 3;
            }
        }
    }
    return penalty;
}

static int count_3rd_penalty(int qr_code_size, bool **qr_code)
{
    int penalty = 0;
    enum { PATTERN_SIZE = 11 };
    bool pattern[] = { 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0 };
    for (int i = 0; i < qr_code_size; ++i) {
        for (int j = 0; j < qr_code_size - PATTERN_SIZE + 1; ++j) {
            bool match_pattern = true;
            bool match_reversed_pattern = true;
            for (int k = 0; k < PATTERN_SIZE; ++k) {
                if (qr_code[i][j + k] != pattern[k]) {
                    match_pattern = false;
                }
                if (qr_code[i][j + k] != pattern[PATTERN_SIZE - k - 1]) {
                    match_reversed_pattern = false;
                }
                penalty += match_pattern * 40 + match_reversed_pattern * 40;
            }
        }
    }
    for (int i = 0; i < qr_code_size - PATTERN_SIZE + 1; ++i) {
        for (int j = 0; j < qr_code_size; ++j) {
            bool match_pattern = true;
            bool match_reversed_pattern = true;
            for (int k = 0; k < PATTERN_SIZE; ++k) {
                if (qr_code[i + k][j] != pattern[k]) {
                    match_pattern = false;
                }
                if (qr_code[i + k][j] != pattern[PATTERN_SIZE - k - 1]) {
                    match_reversed_pattern = false;
                }
                penalty += match_pattern * 40 + match_reversed_pattern * 40;
            }
        }
    }
    return penalty;
}

static int count_4th_penalty(int qr_code_size, bool **qr_code)
{
    const int total_modules_amount = qr_code_size * qr_code_size;
    int dark_modules = 0;
    for (int i = 0; i < qr_code_size; ++i) {
        for (int j = 0; j < qr_code_size; ++j) {
            dark_modules += qr_code[i][j];
        }
    }
    int percentage_of_dark_modules = dark_modules * 100 / total_modules_amount;
    int prev_multiple_of_5 = percentage_of_dark_modules - percentage_of_dark_modules % 5;
    int next_multiple_of_5 = prev_multiple_of_5 + 5;
    int penalty = 0;
    if (abs(prev_multiple_of_5 - 50) >= abs(next_multiple_of_5 - 50)) {
        penalty = abs(prev_multiple_of_5 - 50) * 2;
    }
    else {
        penalty = abs(next_multiple_of_5 - 50) * 2;
    }
    return penalty;
}

static int count_mask_penalty(int qr_code_size, bool **qr_code, int mask_pattern_number)
{
    enum { FINDER_PATTERN_SIZE = 8, TIMING_PATTERN_POSITION = 6 };
    bool **temporary_qr_code = calloc(qr_code_size, sizeof(*temporary_qr_code));
    for (int i = 0; i < qr_code_size; ++i) {
        temporary_qr_code[i] = calloc(qr_code_size, sizeof(**temporary_qr_code));
        memcpy(temporary_qr_code[i], qr_code[i], qr_code_size * sizeof(**qr_code));
        for (int j = 0; j < qr_code_size; ++j) {
            if (!(i <= FINDER_PATTERN_SIZE && j <= FINDER_PATTERN_SIZE ||
                    i <= FINDER_PATTERN_SIZE && j >= qr_code_size - FINDER_PATTERN_SIZE ||
                    i == TIMING_PATTERN_POSITION || j == TIMING_PATTERN_POSITION ||
                    i >= qr_code_size - FINDER_PATTERN_SIZE && j <= FINDER_PATTERN_SIZE) &&
                    use_mask_pattern(i, j, mask_pattern_number)) {
                temporary_qr_code[i][j] = !temporary_qr_code[i][j];
            }
        }
    }
    int penalty = count_1st_penalty(qr_code_size, temporary_qr_code);
    penalty += count_2nd_penalty(qr_code_size, temporary_qr_code);
    penalty += count_3rd_penalty(qr_code_size, temporary_qr_code);
    penalty += count_4th_penalty(qr_code_size, temporary_qr_code);

    for (int i = 0; i < qr_code_size; ++i) {
        free(temporary_qr_code[i]);
    }
    free(temporary_qr_code);
    return penalty;
}

static bool use_mask_pattern(int i, int j, int mask_pattern_number)
{
    switch (mask_pattern_number) {
        case 0: return (i + j) % 2 == 0;
        case 1: return i % 2 == 0;
        case 2: return j % 3 == 0;
        case 3: return (i + j) % 3 == 0;
        case 4: return (i / 2 + j / 3) % 2 == 0;
        case 5: return (i * j) % 2 + (i * j) % 3 == 0;
        case 6: return ((i * j) % 2 + (i * j) % 3) % 2 == 0;
        case 7: return ((i + j) % 2 + (i * j) % 3) % 2 == 0;
    }
    assert(0);
    return false;
}