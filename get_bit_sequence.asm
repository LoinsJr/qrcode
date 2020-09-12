.386
.model flat, C

.const
ENCODED_MESSAGE_SIZE  equ 128
CODEWORDS_BUFFER_SIZE equ 80
.code
extern encode_message_v1M : proto
extern get_ec_codewords_v1M : proto

; params: message length,
;     pointer to message,
;     size of buffer,
;     pointer to buffer
; return: status code
get_bit_sequence_v1M proc
    push   ebp
    mov    ebp,    esp
    
    push   [ebp + 20]
    push   [ebp + 16]
    push   [ebp + 12]
    push   [ebp + 8]
    call   encode_message_v1M
    add    esp,    16
    
    test   eax,    eax
    jnz    return
    
    mov    eax,    [ebp + 20]
    add    eax,    ENCODED_MESSAGE_SIZE
    push   eax
    push   CODEWORDS_BUFFER_SIZE
    push   [ebp + 20]
    push   ENCODED_MESSAGE_SIZE
    call   get_ec_codewords_v1M
    add    esp,    16
return:
    mov    esp,    ebp
    pop    ebp
    ret
get_bit_sequence_v1M endp
end