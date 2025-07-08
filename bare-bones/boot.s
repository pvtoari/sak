/* Declare constants for the multiboot2 header. */
#.set FLAGS,    ALIGN | MEMINFO | VIDEO_MODE
.set MAGIC,    0xE85250D6       /* magic number */
.set ARCHITECTURE,	0       /* 32 bit protected mode */
.set HEADER_LENGTH, header_end - header_start
.set CHECKSUM, -(MAGIC + HEADER_LENGTH)  /* checksum */

# multiboot2 magic weeeeeee now we using tags hell yeah
.section .multiboot2
.align 8
.long MAGIC
.long ARCHITECTURE
.long HEADER_LENGTH
.long CHECKSUM

header_start:

.word 5 # type = 5
.word 0 # no flags
.word 20 # size = 20
.word 1024 # width
.word 768 # height
.word 32 # depth

# end tag
.word 0 # type = 0
.word 0 # no flags
.word 8 # size = 8

header_end:

/*
The multiboot standard does not define the value of the stack pointer register
(esp) and it is up to the kernel to provide a stack. This allocates room for a
small stack by creating a symbol at the bottom of it, then allocating 16384
bytes for it, and finally creating a symbol at the top. The stack grows
downwards on x86. The stack is in its own section so it can be marked nobits,
which means the kernel file is smaller because it does not contain an
uninitialized stack. The stack on x86 must be 16-byte aligned according to the
System V ABI standard and de-facto extensions. The compiler will assume the
stack is properly aligned and failure to align the stack will result in
undefined behavior.
*/

.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.global _start
.type _start, @function
_start:
	# this runs in protected mode, 32-bit, no interrupts, no paging, multiboot is well-defined

	# initialize stack for C
	mov $stack_top, %esp

	/*
	a GDT will be born here
	*/		
	
	# Bootloader leaves magic and multiboot headers at eax and eab, push them so we can grab them at C

	push %eax
	push %ebx
	call kernel_main
	
	# disable ints
	cli
	# since no ints, halt just halts forever lol
1:	hlt
	# if halt stops halting due to a non-maskable interrupt, halt again
	jmp 1b

/*
Set the size of the _start symbol to the current location '.' minus its start.
This is useful when debugging or when you implement call tracing.
*/
.size _start, . - _start
