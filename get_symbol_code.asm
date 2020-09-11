.386
.model flat, C
.code
; param: 1-byte char as argument
; return: code for symbol in alphanumeric table
;     or -1 if fail
; TODO: add other symbols
get_symbol_code proc
    push   ebp
    mov    ebp,     esp

    mov    eax,     [ebp + 8]
is_digit:
    cmp    eax, '0'
    jb     is_letter
    cmp    eax, '9'
    ja     is_letter
    sub    eax,     '0'
    jmp    return
is_letter:    
    cmp    eax, 'A'
    jb     is_space
    cmp    eax, 'Z'
    ja     is_space
    sub    eax,     'A'
    add    eax,     10
    jmp    return
is_space:
    cmp    eax,     ' '
    jne    is_point
    mov    eax,    36
    jmp    return
is_point:
    cmp    eax,     '.'
    jne    is_slash
    mov    eax,    42
    jmp    return
is_slash:
    cmp    eax,     '/'
    jne    error
    mov    eax,    43
    jmp    return
error:
    mov    eax,    -1
return:
    mov    esp,    ebp
    pop    ebp
    ret
get_symbol_code endp
end