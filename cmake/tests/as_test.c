#ifdef PSEUDO_CFI
asm (".cfi_startproc\n\t.cfi_endproc");
#endif

int
main ()
{

#ifdef PSEUDO_ASCII
asm (".ascii \"string\"");
;
#elif defined (PSEUDO_STRING)
asm (".string \"string\"");
#elif defined (PSEUDO_REGISTER)
asm (".register %g2, #scratch");
#elif defined (SPARC_UA_PCREL)
asm (".text; foo: nop; .data; .align 4; .byte 0; .uaword %r_disp32(foo); .text");
#endif

	return 0;
}
