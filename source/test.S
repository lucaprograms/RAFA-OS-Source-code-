.section .rodata
msg_str:
    .string "RAFA OS"

.text
.global testinject1
.type testinject1, @function

hk:
movq $4, %rdi
movq $123, %rsi
movq $321, %rdx
call con
testinject1:
    subq $40, %rsp
movq $8, %rdi
call con
    # Set up registers for condition == 1 (RGB Red)
movq $1, %rdi
    leaq msg_str(%rip), %rsi
    movq $40, %rdx
    movq $30, %rcx
    movl %eax, %r8d
    movq $4096, %r9

    call con


// RDI, RSI, RDX, RCX, R8, R9


    # --- Second call to con ---
    # If condition is the only thing changing, update %rdi and call again
   movq $2,        %rdi       # condition = 2
movq $0,        %rsi       # str = NULL
movq $4,        %rdx       # x = 4
movq $0xFFFF00, %rcx       # y = background
movq $0x000000, %r8        # color = textcolor
movq $0xB8860B, %r9        # pitch = yellow
   call con
   movq $5, %rdi
   movq $0, %rdx
   call con
      movq $5, %rdi
   movq $0, %rdx
   call con
      movq $5, %rdi
   movq $0, %rdx
   call con
      movq $5, %rdi
   movq $0, %rdx
   call con
      movq $5, %rdi
   movq $0, %rdx
   call con
      movq $5, %rdi
   movq $0, %rdx
   call con
      movq $5, %rdi
   movq $0, %rdx
   call con
      movq $5, %rdi
   movq $0, %rdx
   call con
      movq $5, %rdi
   movq $0, %rdx
   call con
      movq $5, %rdi
   movq $0, %rdx
   call con
      movq $5, %rdi
   movq $0, %rdx
   call con
      movq $5, %rdi
   movq $0, %rdx
   call con
      movq $5, %rdi
   movq $1, %rdx
   call con


   movq $7, %rdi
  call con
   cmpb $48, %al
   je hk
   movq $3, %rdi
    call con
    addq $40, %rsp
    ret
