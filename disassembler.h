static int disassembler(char* fname, 
		unsigned long location, 
		unsigned long SISA16_DISASSEMBLER_MAX_HALTS,
		unsigned long end_location){
	unsigned long n_halts = 0;
	unsigned long n_illegals = 0;
	unsigned long i = location & 0xffFFff;
#ifndef SISA_DEBUGGER
	FILE* f; 
	f = fopen(fname, "rb");
	if(!f){puts("//ERROR: Could not open file to disassemble.");	exit(1);}
#endif
	unsigned long stepper = i;
#ifndef SISA_DEBUGGER
	fseek(f, location, SEEK_SET);
#endif
	location &= 0xffFFff;
#ifndef SISA_DEBUGGER
	printf("\nsection 0x%lx;\n", location);
#else
	printf("\n\rsection 0x%lx;\n\r", location);
#endif
	for(i = location; i < end_location && i < 0x1000000;){
		unsigned char opcode;
		if(i >= 0x1000000){
#ifdef SISA_DEBUGGER
			printf("//Disassembly reached end of usable memory.\r\n");
#else
			puts("//Disassembly reached end of usable memory.");
#endif
			goto end;
		}else if((i & 0xffFF) == 0){
#ifdef SISA_DEBUGGER
		printf("//<Region Boundary 0x%06lx >\n\r", i);
#else
		printf("//<Region Boundary 0x%06lx >\n", i);
#endif
		}
#ifndef SISA_DEBUGGER
		if((unsigned long)ftell(f) != i){
			puts("//<Disassembly could not proceed, reason: end of file>");
			goto end;
		}
#endif

#ifndef SISA_DEBUGGER
		opcode = fgetc(f);i++;
#else
		opcode = M[stepper++ & 0xffFFff];i++;
#endif
		if(opcode == 0) 		n_halts++; 		else if(opcode < n_insns) 	n_halts = 0;
		if(opcode >= n_insns) 	n_illegals++; 	else if(opcode != 0) 		n_illegals = 0;
		if(
			opcode < n_insns &&  /*Valid opcode*/
			(insns_numargs[opcode] > 0) && /*With arguments*/
			(((i-1)+insns_numargs[opcode])>>16) != ((i-1)>>16)
		){
#ifndef SISA_DEBUGGER
			printf("\n//(E_BAD_BINARY)\n//op is '%s' which has %u args but crosses the region boundary.\n//This is most likely the result of interpreting embedded data as code.\n",insns[opcode],insns_numargs[opcode]);
#else
			printf("\n\r//(E_BAD_BINARY)\n\r//op is '%s' which has %u args but crosses the region boundary.\n\r//This is most likely the result of interpreting embedded data as code.\n\r",insns[opcode],insns_numargs[opcode]);
#endif
			if( ((((i-1)+insns_numargs[opcode])>>16)&0xff) == 0){
#ifndef SISA_DEBUGGER
				puts("//This opcode crosses over the memory image boundary. Some of its data is junk:");
#else
				printf("//This opcode crosses over the memory image boundary. Some of its data is junk:\r\n");
#endif
			}
		}
		
		if(opcode >= n_insns){
			if(enable_dis_comments)
#ifndef SISA_DEBUGGER
				printf("%-12s 0x%x                ;//0x%06lx  : Illegal Opcode (E_ILLEGAL_OPCODE) Probably Data.\n","bytes", (unsigned int)opcode, i-1);
#else
				printf("%-12s 0x%x                ;//0x%06lx  : Illegal Opcode (E_ILLEGAL_OPCODE) Probably Data.\n\r","bytes", (unsigned int)opcode, i-1);
#endif
			else
#ifndef SISA_DEBUGGER
				printf("%-12s 0x%x                ;\n","bytes", (unsigned int)opcode);
#else
				printf("%-12s 0x%x                ;\n\r","bytes", (unsigned int)opcode);
#endif
			continue;
		}else{
			unsigned long arg_i = 0;
			unsigned char bad_flag = 0;
			unsigned int unfinished_flag = 0;
			unsigned long required_spaces = 20;
			unsigned long opcode_i = i-1;
			U short_interpretation = 0;
			u byte_interpretation = 0;
			printf("%-13s",insns[opcode]);
			for(arg_i = 0; arg_i < insns_numargs[opcode]; arg_i++){
				unsigned char thechar;
				if(  ((i & 0xffFF) == 0) || (i >= 0x1000000)){
					bad_flag = 1;
				}
				if(arg_i > 0) {
					printf(",");
					required_spaces -= 5;
				}else{
					required_spaces -= 4;
				}


#ifndef SISA_DEBUGGER
				if(feof(f) || (unsigned long)ftell(f) != i){
					unfinished_flag = arg_i + 1;
				}
				thechar = fgetc(f);
#else
				thechar = M[stepper++ & 0xffFFff];
#endif
				short_interpretation += thechar;
				byte_interpretation = thechar;
				if(arg_i == 0)short_interpretation *= 256;
				printf("0x%02x",(unsigned int)thechar);i++;
			}
			
			if(required_spaces < 50)
			{unsigned long li; for(li=0;li<required_spaces;li++){
				printf(" ");
			}}
			if(!enable_dis_comments) 
#ifndef SISA_DEBUGGER
				printf(";\n");
#else
				printf(";\n\r");
#endif
			if(enable_dis_comments)
			{
				printf(";//0x%06lx  :", opcode_i);
				if(bad_flag) printf("(E_WONT_RUN)");
				if(unfinished_flag) printf("(E_UNFINISHED_EOF AT ARGUMENT %u)", unfinished_flag-1);
				if(streq(insns[opcode], "farret")){
#ifndef SISA_DEBUGGER
					puts(" End of Procedure");
#else
					printf(" End of Procedure\r\n");
#endif
				}else if(
					streq(insns[opcode], "lla") 
					|| streq(insns[opcode], "llb")
				)
				{
					if( (short_interpretation & 0xFF00) == 0)
#ifndef SISA_DEBUGGER
						puts(" <TIP> replace with single-byte assignment, high byte is zero.");
#else
						printf(" <TIP> replace with single-byte assignment, high byte is zero.\r\n");
#endif
					else
#ifndef SISA_DEBUGGER
						printf("\n");
#else
						printf("\n\r");
#endif
				}else if(streq(insns[opcode], "ret")){
#ifndef SISA_DEBUGGER
					puts(" End of Local Procedure");
#else
					printf(" End of Local Procedure\r\n");					
#endif
				} else if(opcode == 0 && n_halts == 1){
#ifndef SISA_DEBUGGER
					puts(" End of Control Flow");
#else
					printf(" End of Control Flow\r\n");
#endif
				} else if(streq(insns[opcode], "jmp")){
#ifndef SISA_DEBUGGER				
					puts(" Jump");
#else
					printf(" Jump\r\n");
#endif
				}else if(streq(insns[opcode], "jmpifeq")){
#ifndef SISA_DEBUGGER
					puts(" Conditional Jump");
#else
					printf(" Conditional Jump\r\n");
#endif
				}else if(streq(insns[opcode], "cbrx0")
						||streq(insns[opcode], "carx0")){
#ifndef SISA_DEBUGGER
					puts(" ?: Far memory array access through RX0. Check array alignment!");
#else
					printf(" ?: Far memory array access through RX0. Check array alignment!\r\n");
#endif
				}else if(streq(insns[opcode], "jmpifneq")){
#ifndef SISA_DEBUGGER
					puts(" Conditional Jump");
#else
					printf(" Conditional Jump\r\n");
#endif
				}else if(streq(insns[opcode], "clock")){
#ifndef SISA_DEBUGGER
					puts(" Interact w/ sys time: A=ms, B=s, C=clock()");
#else
					printf(" Interact w/ sys time: A=ms, B=s, C=clock()\r\n");
#endif
				}else if(streq(insns[opcode], "emulate_seg")){
#ifndef SISA_DEBUGGER
					puts(" Sandboxing Insn w/ Shared Segment. Jumps to 0x000000 and catches exceptions");
#else
					printf(" Sandboxing Insn w/ Shared Segment. Jumps to 0x000000 and catches exceptions\r\n");
#endif
				}else if(streq(insns[opcode], "emulate")){
#ifndef SISA_DEBUGGER
					puts(" Sandboxing Insn. Jump to 0x000000 and catches exceptions.");
#else
					printf(" Sandboxing Insn. Jump to 0x000000 and catches exceptions.\r\n");
#endif
				}else if(streq(insns[opcode], "interrupt")){
#ifndef SISA_DEBUGGER
					puts(" <DEVICE> interrupt returning value to A.");
#else
					printf(" <DEVICE> interrupt returning value to A.\r\n");
#endif
				}else if(streq(insns[opcode], "putchar")){
#ifndef SISA_DEBUGGER
					puts(" <DEVICE> write A to device.");
#else
					printf(" <DEVICE> write A to device.\r\n");
#endif
				}else if(streq(insns[opcode], "getchar")){
#ifndef SISA_DEBUGGER
					puts(" <DEVICE> read A from device.");
#else
					printf(" <DEVICE> read A from device.\r\n");
#endif
				}else if(streq(insns[opcode], "sc")){
					if(short_interpretation == (opcode_i & 0xffFF)){
#ifndef SISA_DEBUGGER
						puts(" \?!: Loop top. <TIP>: replace with cpc.");
#else
						printf(" \?!: Loop top. <TIP>: replace with cpc.\r\n");
#endif
					}else{
#ifndef SISA_DEBUGGER
						puts(" ?: arg is jump target");
#else
						printf(" ?: arg is jump target\r\n");
#endif
					}
				}else if(streq(insns[opcode], "crx0")
						||streq(insns[opcode], "crx1")
						||streq(insns[opcode], "crx2")
						||streq(insns[opcode], "crx3")){
#ifndef SISA_DEBUGGER
					puts(" ?: Computed Jump through RX register.");
#else
					printf(" ?: Computed Jump through RX register.\r\n");
#endif
				}else if(streq(insns[opcode], "ca")){
#ifndef SISA_DEBUGGER			
					puts(" ?\?: Computed Jump through register A");
#else
					printf(" ?\?: Computed Jump through register A\r\n");
#endif
				}else if(streq(insns[opcode], "cb")){
#ifndef SISA_DEBUGGER
					puts(" ?\?: Computed Jump through register B");
#else
					printf(" ?\?: Computed Jump through register B\r\n");
#endif
				}else if(streq(insns[opcode], "astp")){
#ifndef SISA_DEBUGGER
					puts(" Stack manip, maybe retrieving function arguments?");
#else
					printf(" Stack manip, maybe retrieving function arguments?\r\n");
#endif
				}else if(streq(insns[opcode], "call")){
#ifndef SISA_DEBUGGER
					puts(" Local Procedure Call");
#else
					printf(" Local Procedure Call\r\n");
#endif
				}else if(streq(insns[opcode], "farcall")){
#ifndef SISA_DEBUGGER
					puts(" Procedure Call");
#else
					printf(" Procedure Call\r\n");
#endif
				}else if(streq(insns[opcode], "lfarpc")){
#ifndef SISA_DEBUGGER
					printf(" Region Jump to %u\n", (unsigned)byte_interpretation);
#else
					printf(" Region Jump to %u\r\n", (unsigned)byte_interpretation);
#endif
				}else if(streq(insns[opcode], "farjmprx0")){
#ifndef SISA_DEBUGGER
					puts(" Far Jump Through RX0");
#else
					printf(" Far Jump Through RX0\r\n");
#endif
				}else if( streq(insns[opcode], "farllda")
						||streq(insns[opcode], "farlldb")
						||streq(insns[opcode], "farldc")
						||streq(insns[opcode], "farldrx0")
						||streq(insns[opcode], "farldrx1")
						||streq(insns[opcode], "farldrx2")
						||streq(insns[opcode], "farldrx3")
						||streq(insns[opcode], "lda")
						||streq(insns[opcode], "ldb")
						||streq(insns[opcode], "ldc")
						||streq(insns[opcode], "llda")
						||streq(insns[opcode], "lldb")
				){
#ifndef SISA_DEBUGGER
					puts(" ?: Loading from fixed position variable.");
#else
					printf(" ?: Loading from fixed position variable.\r\n");
#endif
				}else if( streq(insns[opcode], "farstla")
						||streq(insns[opcode], "farstlb")
						||streq(insns[opcode], "farstc")
						||streq(insns[opcode], "farstrx0")
						||streq(insns[opcode], "farstrx1")
						||streq(insns[opcode], "farstrx2")
						||streq(insns[opcode], "farstrx3")
						||streq(insns[opcode], "sta")
						||streq(insns[opcode], "stb")
						||streq(insns[opcode], "stc")
						||streq(insns[opcode], "stla")
						||streq(insns[opcode], "stlb")
				){
#ifndef SISA_DEBUGGER
					puts(" ?: Storing to fixed position variable.");
#else
					printf(" ?: Storing to fixed position variable.\r\n");
#endif
				}else if(streq(insns[opcode], "cpc")){
#ifndef SISA_DEBUGGER
					puts(" ?: Loop top is next insn");
#else
					printf(" ?: Loop top is next insn\r\n");
#endif
				}else {
#ifndef SISA_DEBUGGER
					printf("\n");
#else
					printf("\r\n");
#endif
					}
				if(unfinished_flag){
#ifndef SISA_DEBUGGER
					puts("\n//End of File, Last Opcode is inaccurately disassembled (E_UNFINISHED_EOF)");
#else
					printf("\n\r//End of File, Last Opcode is inaccurately disassembled (E_UNFINISHED_EOF)\r\n");
#endif
					goto end;
				}
			}
		}
		if((n_halts + n_illegals) > SISA16_DISASSEMBLER_MAX_HALTS){
			puts("\n//Reached Halt/Invalid Opcode Limit. Disassembly finished.");
			goto end;
		}
	}
	puts("\n//Reached End.\n");
	end:
#ifndef SISA_DEBUGGER
	fclose(f);
#endif
	return 0;
}
