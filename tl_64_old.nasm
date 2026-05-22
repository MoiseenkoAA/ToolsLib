; Args
; Unix:  RDI, RSI, RDX, RCX, R8, R9
; Win64: RCX, RDX, R8, R9, [rsp+0x28], [rsp+0x30]
;
; Unix can be used:  RDI, RSI, RDX, RCX, R8, R9, RAX, R10, R11
; Win64 can be used:           RCX, RDX, R8, R9, RAX, R10, R11
;
; Universal
; Необходимо сохранять RBX, RSI, RDI, RBP, R12, R13, R14, R15, XMM6..XMM15.
;
;
; linux2win64 stub 4:
; mov r8, rdx
; mov r9, rcx
; mov rcx, rdi
; mov rdx, rsi
; ...
;
; linux2win64 stub 5:
; mov r10, r8
; mov r8, rdx
; mov r9, rcx
; mov rcx, rdi
; mov rdx, rsi
; jmp LongInt2_mul_000
;LongInt2_mul:
;  mov r10, [rsp + 28h] ; Mul2
;LongInt2_mul_000:
;
;extern "C"
;{
;    int __fastcall my_htonl(_IP x);
;}
;
section .data
section .bss
section .text
;
;;	global m11	
;	global ?m11@@YAHXZ
;?m11@@YAHXZ:
;;m11:
;	mov eax, 12345
;	ret
;
;	global my_htonl
;my_htonl:
;mov eax, 0F0000000h
;or eax, eax
;add eax, 020000000h
;test eax, eax
;inc eax
;    mov eax, ecx
;    bswap eax
;    ret
;
;
;Соглашения о связях
;; System V AMD64
;Необходимо сохранять RBP, RBX, R12, R13, R14, R15.
;При использовании инструкций MMX требуется выполнить FEMMS.
;Флаг DF должен быть сброшен.
;Управляющие биты MXCSR и FPU сохраняет вызываемый.
;Статусные биты MXCSR и FPU сохраняет вызывающий.
;; Microsoft:
;Необходимо сохранять RBP, RBX, RSI, RDI, R12, R13, R14, R15, XMM6..XMM15.
;Флаг DF должен быть сброшен.
;Управляющие биты MXCSR и FPU сохраняет вызываемый.
;Статусные биты MXCSR и FPU сохраняет вызывающий.
;
;
               global CMaaTestMemForWrite_unix
CMaaTestMemForWrite_unix:
; (RDI, RSI)
;extern "C"
;{
;   void CMaaTestMemForWrite_unix(void * p, _uqword len) noexcept;
;}
        xor eax, eax
        mov rcx, rsi
        shr rcx, 3
        jz CMaaTestMemForWrite_unix_2
CMaaTestMemForWrite_unix_1:
        lock add [rdi], rax
        add rdi, 8
        ;loop CMaaTestMemForWrite_unix_1
        dec rcx
        jne CMaaTestMemForWrite_unix_1
CMaaTestMemForWrite_unix_2:
        mov ecx, esi
        and ecx, 7
        jnz CMaaTestMemForWrite_unix_3
        ret
CMaaTestMemForWrite_unix_3:
        lock add [rdi], al
        inc rdi
        ;loop CMaaTestMemForWrite_unix_3
        dec ecx
        jne CMaaTestMemForWrite_unix_3
        ret
;
;
; 64 bits hash functions
;
;-----------------------------------------------------------------------------------------------------------------------------------
; Visual Studio 2022 is optimizing more effectively then CMaaStandardHashFunction32_win() by expanding main loop for small fixed len
;-----------------------------------------------------------------------------------------------------------------------------------
               global CMaaStandardHashFunction32_unix
CMaaStandardHashFunction32_unix:
; (RDI, RSI)
;extern "C"
;{
;   _dword CMaaStandardHashFunction32_unix(const void * p, _dword len) noexcept;
;}
        ;mov rcx, rdi
        ;mov rdx, rsi
        ;; jmp CMaaStandardHashFunction32_win

        mov ecx, esi
        xor eax, eax
;align 8
CMaaStandardHashFunction32_unix_1:
        rol eax, 7
        xor al, [rdi]
        inc rdi
        ;loop CMaaStandardHashFunction32_unix_1
        dec ecx
        jne CMaaStandardHashFunction32_unix_1
        ret
;
               global CMaaStandardHashFunction32_win
CMaaStandardHashFunction32_win:
; (RCX, RDX)
; Необходимо сохранять RBX, RSI, RDI, RBP, R12, R13, R14, R15, XMM6..XMM15.
; Win64 can be used: RCX, RDX, R8, R9, RAX, R10, R11
;extern "C"
;{
;   _dword CMaaStandardHashFunction32_win(const void * p, _dword len) noexcept;
;}
;   rcx - p
;   rdx - len
;
; v2:
;        xchg rcx, rdx
;        ; rdx - p, rcx - len
;        xor eax, eax
;        mov ecx, ecx
;CMaaStandardHashFunction32_win_1:
;        rol eax, 7
;        xor al, [rdx]
;        inc rdx
;        ;loop CMaaStandardHashFunction32_win_1
;        dec ecx
;        jne CMaaStandardHashFunction32_win_1
;        ret
;;
; v3:
        ; rcx - p, rdx - len
        xor eax, eax
;align 8
CMaaStandardHashFunction32_win_1:
        rol eax, 7
        xor al, [rcx]
        inc rcx
        dec edx
        jne CMaaStandardHashFunction32_win_1
        ret
;
;
;
;-----------------------------------------------------------------------------------------------------------------------------------
; Visual Studio 2022 is optimizing more effectively then CMaaStandardHashFunction64_win() by expanding main loop for small fixed len
;-----------------------------------------------------------------------------------------------------------------------------------
               global CMaaStandardHashFunction64_unix
CMaaStandardHashFunction64_unix:
; (RDI, RSI)
;extern "C"
;{
;   _uqword CMaaStandardHashFunction32_unix(const void * p, _dword len) noexcept;
;}
        ;mov rcx, rdi
        ;mov rdx, rsi
        ;; jmp CMaaStandardHashFunction64_win

        xor rax, rax
        xor rcx, rcx
;align 8
CMaaStandardHashFunction64_unix_1:
        rol rax, 7
        mov cl, [rdi]
        xor rax, rcx
        inc rdi
        ;loop CMaaStandardHashFunction64_unix_1
        dec esi
        jne CMaaStandardHashFunction64_unix_1
        ret
;
               global CMaaStandardHashFunction64_win
CMaaStandardHashFunction64_win:
; (RCX, RDX)
; Необходимо сохранять RBX, RSI, RDI, RBP, R12, R13, R14, R15, XMM6..XMM15.
; Win64 can be used: RCX, RDX, R8, R9, RAX, R10, R11
;extern "C"
;{
;   _uqword CMaaStandardHashFunction32_win(const void * p, _dword len) noexcept;
;}
;   rcx - p
;   rdx - len
        xor rax, rax
        xor r8, r8
;align 8
CMaaStandardHashFunction64_win_1:
        rol rax, 7
        mov r8b, [rcx]
        xor rax, r8
        inc rcx
        dec edx
        jne CMaaStandardHashFunction64_win_1
        ret
;
;__int64 MaaMulDiv64(__int64 x, __int64 y, __int64 z, __int64 * pRest = nullptr, __int64 * pHighPart = nullptr) noexcept;
;#ifdef _WIN64
;extern "C"
;{
;    _uqword MaaAsmMulDiv64_win(_uqword x, _uqword y, _uqword z, _uqword * pRest = nullptr, _uqword * pHighPart = nullptr) noexcept;
;}
;#define MaaMulDiv64 MaaAsmMulDiv64_win
;#endif
;#ifdef __linux__
;#define MaaMulDiv64 MaaAsmMulDiv64_unix
;#endif

    global MaaAsmMulDiv64_unix
MaaAsmMulDiv64_unix:
; Unix:  RDI, RSI, RDX, RCX, R8, R9
    mov r10, r8
    mov r8, rdx
    mov r9, rcx
    mov rcx, rdi
    mov rdx, rsi
    jmp MaaAsmMulDiv64_000
    global MaaAsmMulDiv64_win
MaaAsmMulDiv64_win:
; Win64: RCX, RDX, R8, R9, [rsp+0x28]
    mov r10, [rsp + 28h]
MaaAsmMulDiv64_000:
; Args: RCX, RDX, R8, R9, R10
;
; Unix can be used:  RDI, RSI, RDX, RCX, R8, R9, RAX, R10, R11
; Win64 can be used:           RCX, RDX, R8, R9, RAX, R10, R11
;
    mov rax, rdx
    mul rcx
    mov rcx, rax
    mov rax, rdx
    xor rdx, rdx
    div r8
    or r10, r10
    jz MaaAsmMulDiv64_pHighPart_null
    mov [r10], rax
MaaAsmMulDiv64_pHighPart_null:
    mov rax, rcx
    div r8
    or r9, r9
    jz MaaAsmMulDiv64_pRest_null
    mov [r9], rdx
MaaAsmMulDiv64_pRest_null:
    ret
;
;

; Args
; Unix:  RDI, RSI, RDX, RCX, R8, R9
; Win64: RCX, RDX, R8, R9, [rsp+0x28], [rsp+0x30]
;
; Unix can be used:  RDI, RSI, RDX, RCX, R8, R9, RAX, R10, R11
; Win64 can be used:           RCX, RDX, R8, R9, RAX, R10, R11
;
; Universal
; Необходимо сохранять RBX, RSI, RDI, RBP, R12, R13, R14, R15, XMM6..XMM15.
;
;
;#ifdef _WIN64
;extern "C"
;{
;    bool MaaMemCmpQ_win(const void * ptr1, const void * ptr2, _uqword len) noexcept; // compares len bytes, len >= 0
;}
;#define MaaMemCmpQ MaaMemCmpQ_win
;#endif
;#ifdef __linux__
;extern "C"
;{
;    bool MaaMemCmpQ_unix(const void * ptr1, const void * ptr2, _uqword len) noexcept; // compares len bytes, len >= 0
;}
;#define MaaMemCmpQ MaaMemCmpQ_unix
;#endif

    global MaaMemCmpQ_unix
MaaMemCmpQ_unix:
; Unix:  RDI, RSI, RDX, RCX, R8, R9
; Unix can be used:  RDI, RSI, RDX, RCX, R8, R9, RAX, R10, R11
    mov rcx, rdx
    shr rcx, 3
    jz    MaaMemCmpQ_unix_skip8
align 8
MaaMemCmpQ_unix_loop:
; 0
;
;    mov rcx, rdx
;    shr rcx, 4
;    jz    MaaMemCmpQ_unix_skip8
;
;    movdqu xmm0, [rsi]
;    por xmm0, [rdi]
;;    pxor xmm0, [rdi]
;    jnz  MaaMemCmpQ_unix_ret
;    lea rsi, [rsi + 10h]
;    lea rdi, [rdi + 10h]
;    dec rcx
;    jnz  MaaMemCmpQ_unix_loop
;
;    mov  eax, edx
;    and  eax, 8
;    jz   MaaMemCmpQ_unix_skip8
;    mov rax, [rsi]
;    cmp rax, [rdi]
;    jnz  MaaMemCmpQ_unix_ret
;    lea rsi, [rsi + 8]
;    lea rdi, [rdi + 8]
;    jmp MaaMemCmpQ_unix_skip8


    mov rax, [rsi]
    cmp rax, [rdi]
    jnz  MaaMemCmpQ_unix_ret
    lea rsi, [rsi + 8]
    lea rdi, [rdi + 8]
    dec rcx
    jz  MaaMemCmpQ_unix_skip8
; 1
;    mov rax, [rsi]
;    cmp rax, [rdi]
;    jnz  MaaMemCmpQ_unix_ret
;    lea rsi, [rsi + 8]
;    lea rdi, [rdi + 8]
;    dec rcx
;    jz  MaaMemCmpQ_unix_skip8
; 2
;    mov rax, [rsi]
;    cmp rax, [rdi]
;    jnz  MaaMemCmpQ_unix_ret
;    lea rsi, [rsi + 8]
;    lea rdi, [rdi + 8]
;    dec rcx
;    jz  MaaMemCmpQ_unix_skip8
; 3
    mov rax, [rsi]
    cmp rax, [rdi]
    jnz  MaaMemCmpQ_unix_ret
    lea rsi, [rsi + 8]
    lea rdi, [rdi + 8]
    dec rcx
    jnz  MaaMemCmpQ_unix_loop

;    repe cmpsq
;    jnz  MaaMemCmpQ_unix_ret

;    jnz  MaaMemCmpQ_unix_ret_

MaaMemCmpQ_unix_skip8:

;    mov ecx, edx
;    and ecx, 7
;    jz   MaaMemCmpQ_unix_ret_
;    repe cmpsb
;MaaMemCmpQ_unix_ret_:
;    setz al
;    ret


    mov  eax, edx
    and  eax, 7
    lea  rcx, [rel MaaMemCmpQ_unix_jump_table]
    jmp [rcx + 8 * rax]
;MaaMemCmpQ_unix_ref:

;MaaMemCmpQ_unix_ret0:
;MaaMemCmpQ_unix_ret:
;    setz al
;    ret

MaaMemCmpQ_unix4:
    mov eax, [rsi]
    cmp eax, [rdi]
;MaaMemCmpQ_unix_ret4:
MaaMemCmpQ_unix_ret0:
MaaMemCmpQ_unix_ret:
    setz al
    ret

MaaMemCmpQ_unix2:
    mov ax, [rsi]
    cmp ax, [rdi]
;MaaMemCmpQ_unix_ret2:
    setz al
    ret

MaaMemCmpQ_unix6:
    mov eax, [rsi]
    cmp eax, [rdi]
    jnz MaaMemCmpQ_unix_ret6
    mov ax, [rsi + 4]
    cmp ax, [rdi + 4]
MaaMemCmpQ_unix_ret6:
    setz al
    ret

MaaMemCmpQ_unix5:
    mov eax, [rsi]
    cmp eax, [rdi]
    jnz MaaMemCmpQ_unix_ret5
    mov al, [rsi + 4]
    cmp al, [rdi + 4]
MaaMemCmpQ_unix_ret5:
    setz al
    ret

MaaMemCmpQ_unix7:
    mov eax, [rsi]
    cmp eax, [rdi]
    jnz MaaMemCmpQ_unix_ret7
    mov ax, [rsi + 4]
    cmp ax, [rdi + 4]
    jnz MaaMemCmpQ_unix_ret7
    mov al, [rsi + 6]
    cmp al, [rdi + 6]
MaaMemCmpQ_unix_ret7:
    setz al
    ret

MaaMemCmpQ_unix1:
    mov al, [rsi]
    cmp al, [rdi]
    setz al
    ret

MaaMemCmpQ_unix3:
    mov ax, [rsi]
    cmp ax, [rdi]
    jnz MaaMemCmpQ_unix_ret3
    mov al, [rsi + 2]
    cmp al, [rdi + 2]
MaaMemCmpQ_unix_ret3:
    setz al
    ret








;
    global MaaMemCmpQ_win
MaaMemCmpQ_win:
; Win64: RCX, RDX, R8, R9, [rsp+0x28], [rsp+0x30]
; Win64 can be used: RCX, RDX, R8, R9, RAX, R10, R11
;
    mov  r10, rdi
    mov  r11, rsi
    mov  rdi, rcx
    mov  rsi, rdx
    mov  rcx, r8
    shr  rcx, 3
    jz    MaaMemCmpQ_win_skip8
align 8
MaaMemCmpQ_win_loop:
; 0
;
;    mov  rcx, r8
;    shr  rcx, 4
;    jz    MaaMemCmpQ_win_skip8
;
;    movdqu xmm0, [rsi]
;    por xmm0, [rdi]
;;    pxor xmm0, [rdi]
;    jnz  MaaMemCmpQ_win_ret
;    lea rsi, [rsi + 10h]
;    lea rdi, [rdi + 10h]
;    dec rcx
;    jnz  MaaMemCmpQ_win_loop
;
;    mov  eax, r8d
;    and  eax, 8
;    jz   MaaMemCmpQ_win_skip8
;    mov rax, [rsi]
;    cmp rax, [rdi]
;    jnz  MaaMemCmpQ_win_ret
;    lea rsi, [rsi + 8]
;    lea rdi, [rdi + 8]
;    jmp MaaMemCmpQ_win_skip8


    mov rax, [rsi]
    cmp rax, [rdi]
    jnz  MaaMemCmpQ_win_ret
    lea rsi, [rsi + 8]
    lea rdi, [rdi + 8]
    dec rcx
    jz  MaaMemCmpQ_win_skip8
; 1
;    mov rax, [rsi]
;    cmp rax, [rdi]
;    jnz  MaaMemCmpQ_win_ret
;    lea rsi, [rsi + 8]
;    lea rdi, [rdi + 8]
;    dec rcx
;    jz  MaaMemCmpQ_win_skip8
; 2
;    mov rax, [rsi]
;    cmp rax, [rdi]
;    jnz  MaaMemCmpQ_win_ret
;    lea rsi, [rsi + 8]
;    lea rdi, [rdi + 8]
;    dec rcx
;    jz  MaaMemCmpQ_win_skip8
; 3
    mov rax, [rsi]
    cmp rax, [rdi]
    jnz  MaaMemCmpQ_win_ret
    lea rsi, [rsi + 8]
    lea rdi, [rdi + 8]
    dec rcx
    jnz  MaaMemCmpQ_win_loop

;    repe cmpsq
;    jnz  MaaMemCmpQ_win_ret

;    jnz  MaaMemCmpQ_win_ret_

MaaMemCmpQ_win_skip8:

;    mov ecx, r8d
;    and ecx, 7
;    jz   MaaMemCmpQ_win_ret_
;    repe cmpsb
;MaaMemCmpQ_win_ret_:
;    setz al
;    mov rsi, r11
;    mov rdi, r10
;    ret


    mov  eax, r8d
    and  eax, 7
    lea  rcx, [rel MaaMemCmpQ_win_jump_table]
    jmp [rcx + 8 * rax]
;MaaMemCmpQ_win_ref:

;MaaMemCmpQ_win_ret0:
;MaaMemCmpQ_win_ret:
;    setz al
;    mov rsi, r11
;    mov rdi, r10
;    ret

MaaMemCmpQ_win4:
    mov eax, [rsi]
    cmp eax, [rdi]
;MaaMemCmpQ_win_ret4:
MaaMemCmpQ_win_ret0:
MaaMemCmpQ_win_ret:
    setz al
    mov rsi, r11
    mov rdi, r10
    ret

MaaMemCmpQ_win2:
    mov ax, [rsi]
    cmp ax, [rdi]
;MaaMemCmpQ_win_ret2:
    setz al
    mov rsi, r11
    mov rdi, r10
    ret

MaaMemCmpQ_win6:
    mov eax, [rsi]
    cmp eax, [rdi]
    jnz MaaMemCmpQ_win_ret6
    mov ax, [rsi + 4]
    cmp ax, [rdi + 4]
MaaMemCmpQ_win_ret6:
    setz al
    mov rsi, r11
    mov rdi, r10
    ret

MaaMemCmpQ_win5:
    mov eax, [rsi]
    cmp eax, [rdi]
    jnz MaaMemCmpQ_win_ret5
    mov al, [rsi + 4]
    cmp al, [rdi + 4]
MaaMemCmpQ_win_ret5:
    setz al
    mov rsi, r11
    mov rdi, r10
    ret

MaaMemCmpQ_win7:
    mov eax, [rsi]
    cmp eax, [rdi]
    jnz MaaMemCmpQ_win_ret7
    mov ax, [rsi + 4]
    cmp ax, [rdi + 4]
    jnz MaaMemCmpQ_win_ret7
    mov al, [rsi + 6]
    cmp al, [rdi + 6]
MaaMemCmpQ_win_ret7:
    setz al
    mov rsi, r11
    mov rdi, r10
    ret

MaaMemCmpQ_win1:
    mov al, [rsi]
    cmp al, [rdi]
    setz al
    mov rsi, r11
    mov rdi, r10
    ret

MaaMemCmpQ_win3:
    mov ax, [rsi]
    cmp ax, [rdi]
    jnz MaaMemCmpQ_win_ret3
    mov al, [rsi + 2]
    cmp al, [rdi + 2]
MaaMemCmpQ_win_ret3:
    setz al
    mov rsi, r11
    mov rdi, r10
    ret
;
;
;#ifdef _WIN64
;extern "C"
;{
;    void MaaXLat_win(char * b, char * e, const char * t) noexcept;
;}
;#define MaaXLat MaaXLat_win
;#endif
;#ifdef __linux__
;extern "C"
;{
;    void MaaXLat_unix(char * b, char * e, const char * t) noexcept;
;}
;#define MaaXLat MaaXLat_unix
;#endif
    global MaaXLat_win
MaaXLat_win:
; Win64: RCX, RDX, R8, R9, [rsp+0x28], [rsp+0x30]
; Win64 can be used: RCX, RDX, R8, R9, RAX, R10, R11
    cmp rcx, rdx
    jge MaaXLat_win1
;align 8
MaaXLat_win_loop:
    movzx eax, byte [rcx]
    movzx eax, byte [rax+r8]
    mov [rcx], al
    inc rcx
    cmp rcx, rdx
    jc MaaXLat_win_loop
MaaXLat_win1:
    ret
;
    global MaaXLat_unix
MaaXLat_unix:
; Unix:  RDI, RSI, RDX, RCX, R8, R9
; Unix can be used:  RDI, RSI, RDX, RCX, R8, R9, RAX, R10, R11
    cmp rdi, rsi
    jge MaaXLat_unix1
;align 8
MaaXLat_unix_loop:
    movzx eax, byte [rdi]
    movzx eax, byte [rax+rdx]
    mov [rdi], al
    inc rdi
    cmp rdi, rsi
    jc MaaXLat_unix_loop
MaaXLat_unix1:
    ret
;
;
;
section .data
    align 10h
    MaaMemCmpQ_unix_jump_table: dq MaaMemCmpQ_unix_ret0, MaaMemCmpQ_unix1, MaaMemCmpQ_unix2, MaaMemCmpQ_unix3, MaaMemCmpQ_unix4, MaaMemCmpQ_unix5, MaaMemCmpQ_unix6, MaaMemCmpQ_unix7
    align 10h
    MaaMemCmpQ_win_jump_table:  dq MaaMemCmpQ_win_ret0,  MaaMemCmpQ_win1,  MaaMemCmpQ_win2,  MaaMemCmpQ_win3,  MaaMemCmpQ_win4,  MaaMemCmpQ_win5,  MaaMemCmpQ_win6,  MaaMemCmpQ_win7
;
