.386
.model flat, C
.const
ENCODED_DATA_SIZE equ 128
MAX_MESSAGE_LENGTH           equ 16
PAD_BYTE_236                 equ 236
PAD_BYTE_17                  equ 17
ENCODING_FACTOR              equ 45
.code
extern convert_to_binary : proto
extern get_symbol_code : proto
; return 1 if fail, 0 if success
; take string length,
; pointer to string ending with 0,
; buffer size, pointer to buffer as arguments
encode_data_v1M proc
    push   ebp
    mov    ebp,    esp
    push   edi
    push   esi
    push   ebx
    
    mov    eax,    [ebp + 8]
    cmp    eax,    MAX_MESSAGE_LENGTH
    ja     error
    ; check if buffer size is too small
    mov    eax,    [ebp + 16]
    cmp    eax,    ENCODED_DATA_SIZE
    jb     error
    ;encode mode and message length
    mov    esi,    [ebp + 12]
    ; 0010 - alphanumeric mode
    mov    edi,    [ebp + 20]
    xor    eax,     eax
    mov    [edi],   al
    mov    [edi + 1], al
    mov    eax,     1
    mov    [edi + 2], al
    xor    eax,    eax
    mov    [edi + 3], al
    add    edi,    4
    ; convert message length to 9-bit binary
    push   edi
    push   9
    push   [ebp + 8]
    call   convert_to_binary
    add    esp,    12
    add    edi,    9
    ; encode message
    mov    ecx,    [ebp + 8]
    xor    ebx,    ebx
encode_loop:
    cmp    ecx,    1
    jbe    end_encode_loop
    ; encode pair of symbols
    mov    bl,     [esi]
    push   ebx
    call   get_symbol_code
    add    esp,    4
    ; check for unacceptable symbol
    cmp    eax,    -1
    je     error

    mov    ebx,    ENCODING_FACTOR
    mul    ebx
    push   eax
    
    mov    bl,     [esi + 1]
    push   ebx
    call   get_symbol_code
    add    esp,    4

    pop    edx
    ; check for unacceptable symbol
    cmp    eax,    0
    jl     error

    add    eax,    edx    
    ; convert to binary and put in the buffer
    mov    ebx,    ecx ; save register ecx
    push   edi
    push   11
    push   eax
    call   convert_to_binary
    add    esp,    12
    mov    ecx,    ebx

    add    edi,    11
    add    esi,    2
    sub    ecx,    2
    jmp    encode_loop
end_encode_loop:
    test   ecx,    ecx
    jz     determine_terminator
    ; encode symbol
    mov    bl,     [esi]
    push   ebx
    call   get_symbol_code
    add    esp,    4
    ; check for unaccepatable symbol
    cmp    eax,    0
    jl     error

    ; encode last symbol to 6-bit binary
    push   edi
    push   6
    push   eax
    call   convert_to_binary
    add    esp,    12

    add    edi,    6
determine_terminator:
    mov    eax,    edi
    sub    eax,    [ebp + 20]
    mov    ecx,    ENCODED_DATA_SIZE
    sub    ecx,    eax
    xor    eax,    eax
    cmp    ecx,    4
    jbe    add_terminator_loop
    mov    ecx,    4
add_terminator_loop:
    mov    [edi],  al
    inc    edi
    loop   add_terminator_loop
    ; make multiple of 8
    mov    edx,    edi
    sub    edx,    [ebp + 20]
    and    edx,    111b
    mov    ecx,    8
    sub    ecx,    edx
make_multiple_of_8_loop:
    mov    [edi],  al
    inc    edi
    loop   make_multiple_of_8_loop
    ; add pad bytes
    mov    ecx,    edi
    sub    ecx,    [ebp + 20]
    mov    eax,    ENCODED_DATA_SIZE
    sub    eax,    ecx
    mov    ebx,    8
    cdq
    div    ebx
    mov    ecx,    eax
add_pad_bytes_loop:
    push   edi
    push   8
    push   PAD_BYTE_236
    mov    ebx,    ecx ; save register ecx
    and    ecx,    1
    jz     call_convert_to_binary
    mov    edx,    PAD_BYTE_17
    mov    [esp],  edx
call_convert_to_binary:
    call convert_to_binary
    add    esp,    12
    
    mov    ecx,    ebx
    add    edi,    8
    dec    ecx
    jnz    add_pad_bytes_loop
    jmp success
error:
    mov    eax,    1
    jmp    return
success:
    xor    eax,    eax
return:
    pop    ebx
    pop    esi
    pop    edi
    mov    esp,    ebp
    pop    ebp
    ret
encode_data_v1M endp
end