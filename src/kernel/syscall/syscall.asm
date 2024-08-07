global syscall_enable:function
section .etext exec nowrite



[bits 64]
syscall_enable:
	mov ecx, 0xc0000080
	rdmsr
	or eax, 1
	wrmsr
	xor eax, eax
	add ecx, 1
	mov edx, 0x00130008
	wrmsr
	lea rdx, _syscall_handler
	mov rax, rdx
	add ecx, 1
	shr rdx, 32
	wrmsr
	mov eax, 0xfffffffd
	add ecx, 2
	xor edx, edx
	wrmsr
	ret



extern _random_entropy_pool
extern _random_entropy_pool_length
extern _signal_return_from_syscall
extern _syscall_invalid
extern _syscall_table_list
extern _syscall_table_list_length
extern scheduler_set_timer
global _exception_user_return:function
section .text exec nowrite



[bits 64]
_syscall_handler:
	swapgs
	mov qword [gs:16], rsp
	mov rsp, qword [gs:8]
	push r15
	push r14
	push r13
	push r12
	push r11
	push rcx
	push rbp
	push rbx
	push r9
	push r8
	push r10
	push rdx
	push rsi
	push rdi
	mov bx, 0x10
	mov ds, bx
	mov es, bx
	mov r15, -0x0001 ; ERROR_INVALID_SYSCALL
	xor r14, r14
	xor r13, r13
	xor r12, r12
	xor r11, r11
	xor r10, r10
	xor rbp, rbp
	mov rbx, rax
	mov edi, 1 ; SCHEDULER_TIMER_KERNEL
	call scheduler_set_timer
	mov rdi, qword [rsp]
	mov rsi, qword [rsp+8]
	mov rdx, qword [rsp+16]
	mov rcx, qword [rsp+24]
	mov r8, qword [rsp+32]
	mov r9, qword [rsp+40]
	mov eax, ebx
	shr rbx, 32
	cmp ebx, dword [_syscall_table_list_length]
	jge ._syscall_return
	mov r10, qword [_syscall_table_list]
	mov r10, qword [r10+rbx*8]
	test r10, r10
	jz ._syscall_return
	mov r11, qword [r10+8]
	cmp eax, dword [r10+16]
	jge ._syscall_return
	mov rax, qword [r11+rax*8]
	test rax, rax
	jz ._syscall_return
	sti
	call rax
._exception_return:
	cli
	mov rdi, rax
	call _signal_return_from_syscall
	mov r15, rax
._syscall_return:
	xor edi, edi ; SCHEDULER_TIMER_USER
	call scheduler_set_timer
	rdtsc
	mov edx, dword [_random_entropy_pool_length]
	and edx, 0x3c
	lock xor dword [_random_entropy_pool+rdx], eax
	mov bx, 0x1b
	mov ds, bx
	mov es, bx
	mov rax, r15
	pop rdi
	pop rsi
	pop rdx
	pop r10
	pop r8
	pop r9
	pop rbx
	pop rbp
	pop rcx
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
	mov rsp, qword [gs:16]
	swapgs
	o64 sysret



_exception_user_return:
	mov rsp, qword [gs:8]
	sub rsp, 112
	mov rax, rdi
	jmp _syscall_handler._exception_return
