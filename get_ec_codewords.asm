.386
.model flat, C

.const
DIVIDEND_POLYNOMIAL_SIZE equ 16
DIVIDER_POLYNOMIAL_SIZE  equ 11
CODEWORDS_AMOUNT         equ 10
FIELD_ORDER              equ 255
.data
dividend_polynomial db 26 dup(0)
divider_polynomial  db 1, 216, 194, 159, 111, 199, 94, 95, 113, 157, 193
.code
extern convert_binary_to_decimal : proto
extern convert_decimal_to_alpha_notation : proto
extern convert_alpha_notation_to_decimal : proto

break proc
    push   ebp
    mov    ebp,    esp

    mov   esp,  ebp
    pop  ebp
    ret
break endp

break2 proc
    push   ebp
    mov    ebp,    esp

    mov   esp,  ebp
    pop  ebp
    ret
break2 endp


; divide dividend_polynomial and divider_polynomial
; result is written in dividend_polynomial
; return: pointer to leading term
divide_polynomials proc
    push   ebp
    mov    ebp,    esp
    push   edi
    push   esi
    push   ebx

    lea    edi,    [dividend_polynomial]
    mov    ecx,    DIVIDEND_POLYNOMIAL_SIZE
divide_loop:
    test   ecx,    ecx
    jz     end_divide_loop
    push   ecx
    push   edi
    
    xor    edx,    edx
    mov    dl,     [edi]
    push   edx
    call   convert_decimal_to_alpha_notation
    add    esp,    4

    lea    esi,    [divider_polynomial]
    mov    ecx,    DIVIDER_POLYNOMIAL_SIZE
substraction_loop:
    test   ecx,    ecx
    jz     end_substraction_loop
    mov    ebx,    ecx ; save register ecx

    push   eax
    xor    edx,    edx
    mov    dl,     [esi]
    push   edx
    call   convert_decimal_to_alpha_notation
    add    esp,    4

    add    eax,    [esp]
    mov    ecx,    FIELD_ORDER
    cdq
    div    ecx

    push   edx
    call   convert_alpha_notation_to_decimal
    add    esp,    4

    xor    [edi],  al
    pop    eax
    mov    ecx,    ebx
    dec    ecx
    call   break2
    inc    edi
    inc    esi
    jmp    substraction_loop
end_substraction_loop:
    pop    edi
    inc    edi
    pop    ecx
    dec    ecx
    cmp    ecx,    10
    ja     divide_loop
    call   break
    mov    eax,    edi
    jmp    divide_loop
end_divide_loop:
    pop    ebx
    pop    esi
    pop    edi
    mov    esp,    ebp
    pop    ebp
    ret
divide_polynomials endp

; params: encoded message size,
;     encoded message,     
;     buffer size,
;     pointer to buffer for placing codewords
; return: 1 if fail, 0 if success
get_ec_codewords_v1M proc
    push   ebp
    mov    ebp,    esp
    push   esi
    push   edi
    push   ebx
    
    ; check if buffer size is enough
    mov    eax,    [ebp + 16]
    cmp    eax,    CODEWORDS_AMOUNT
    jb     error

    ; encoded message size must be equal to DIVIDEND_POLYNOMIAL_SIZE * 8
    mov    eax,    DIVIDEND_POLYNOMIAL_SIZE
    mov    ebx,    8
    mul    ebx
    cmp    eax,    [ebp + 8]
    jne    error
    
    mov    ecx,    [ebp + 8]
    mov    esi,    [ebp + 12]
    lea    edi,    [dividend_polynomial]
fill_dividend_polynomial_loop:
    test   ecx,    ecx
    jz     end_fill_dividend_polynomial_loop
    mov    ebx,    ecx ; save register ecx
    
    push   esi
    push   8
    call   convert_binary_to_decimal
    add    esp,    8
    
    mov    [edi],  al
    mov    ecx,    ebx
    inc    edi
    add    esi,    8
    sub    ecx,    8
    jmp    fill_dividend_polynomial_loop
end_fill_dividend_polynomial_loop:
    call   divide_polynomials
    jmp    return
error:
    mov    eax,    1
    jmp    return
return:
    pop    ebx
    pop    edi
    pop    esi
    mov    esp,    ebp
    pop    ebp
    ret
get_ec_codewords_v1M endp
end