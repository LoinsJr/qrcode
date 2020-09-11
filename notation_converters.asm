.386
.model flat, C
.data
f_str db "%d"
.code
; take 32-bit unsigned decimal int,
; number of bits,
; pointer to array for handling binary
convert_to_binary proc
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
convert_to_binary endp
end