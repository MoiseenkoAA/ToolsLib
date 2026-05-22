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
;section .data
;section .bss
section .text
;
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
;
;



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
