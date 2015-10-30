.text
.globl foo
foo:
jmp bar
.section .eh_frame,"a",@unwind
bar:
