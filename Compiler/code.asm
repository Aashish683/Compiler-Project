section .text
	global main
	extern scanf
	extern printf

section .data
	inpformat:  db "%hd",0
	outformat:  db "%hd",10,0,
	b3:	dw	1
	b2:	dw	1
	c2:	dw	1
	d2:	dw	1


main:
push rsi
push rdi
push ax
mov rsi, b2
mov rdi, inpformat
mov al, 0
call scanf
pop ax
pop rdi
pop rsi


push ax
mov ax, 20
mov [c2],  ax
pop ax


push rsi
push rdi
push ax
mov rsi, d2
mov rdi, inpformat
mov al, 0
call scanf
pop ax
pop rdi
pop rsi


push ax
push bx
push bx
mov ax, [b2]
push ax
mov ax, [c2]
push ax
pop bx
pop ax
add ax,bx
pop bx
push ax
mov ax, [d2]
push ax
pop bx
pop ax
add ax,bx
pop bx
mov [b3],  ax
pop ax


push rdi
push rsi
push rax
push rcx
push ax
mov rsi, [b3]
mov rdi, outformat
mov al,0
call printf
pop ax
pop rcx
pop rax
pop rsi
pop rdi




ret
