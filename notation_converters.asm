.386
.model flat, C
.const
ALPHA_MODULO equ 285
.code
; params: pow of alpha notation
; return: decimal
convert_alpha_notation_to_decimal proc
    push   ebp
    mov    ebp,    esp
    
    mov    eax,    1
    mov    ecx,    [ebp + 8]
convert_to_decimal_loop:
    test   ecx,    ecx
    jz     end_convert_to_decimal_loop
    dec    ecx
    shl    eax,    1
    cmp    eax,    256
    jb     convert_to_decimal_loop
    xor    eax,    ALPHA_MODULO    
    jmp    convert_to_decimal_loop
end_convert_to_decimal_loop:
    mov    esp,    ebp
    pop    ebp
    ret
convert_alpha_notation_to_decimal endp

; params: 32-bit unsigned
; return: pow of alpha notation
convert_decimal_to_alpha_notation proc
    push   ebp
    mov    ebp,    esp
    
    xor    eax,    eax
    mov    edx,    [ebp + 8]
convert_to_alpha_loop:
    cmp    edx,    1
    je     end_convert_to_alpha_loop
    mov    ecx,    edx
    and    ecx,    1
    jz     divide_decimal
    xor    edx,    ALPHA_MODULO
    jmp    convert_to_alpha_loop
divide_decimal:
    shr    edx,    1
    inc    eax
    jmp    convert_to_alpha_loop
end_convert_to_alpha_loop:
    mov    esp,    ebp
    pop    ebp
    ret
convert_decimal_to_alpha_notation endp

; params: number of bits,
;     binary string,
; return: decimal
convert_binary_to_decimal proc
    push   ebp
    mov    ebp,    esp
    push   esi
    
    xor    eax,    eax
    mov    ecx,    [ebp + 8]
    mov    esi,    [ebp + 12]
convert_to_decimal_loop:
    test   ecx,    ecx
    jz     end_convert_to_decimal_loop
    shl    eax,    1
    or     al,     [esi]
    inc    esi
    dec    ecx
    jmp    convert_to_decimal_loop
end_convert_to_decimal_loop:
    
    pop    esi
    mov    esp,    ebp
    pop    ebp
    ret
convert_binary_to_decimal endp

; param: 32-bit unsigned decimal int,
;     number of bits,
;     pointer to array for handling binary
convert_decimal_to_binary proc
    push   ebp
    mov    ebp,    esp
    push   edi

    mov    eax,    [ebp + 8]
    mov    ecx,    [ebp + 12]
    mov    edi,    [ebp + 16]
    add    edi,    ecx
convert_loop:
    test   ecx,    ecx
    jz     end_loop
    dec    edi
    mov    edx,    1
    and    edx,    eax
    shr    eax,    1
    mov    [edi],  dl
    dec    ecx
    jmp    convert_loop
end_loop:
    pop    edi
    mov    esp,    ebp
    pop    ebp
    ret
convert_decimal_to_binary endp
end