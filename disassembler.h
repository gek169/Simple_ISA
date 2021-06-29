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
	if(!f){
		puts("//ERROR: Could not open file to disassemble.");
		exit(1);
	}
#endif
	unsigned long stepper = i;
#ifndef SISA_DEBUGGER
	fseek(f, location, SEEK_SET);
#endif
	location &= 0xffFFff;
	printf("\nsection 0x%lx;\n", location);
	for(i = location; i < end_location;){
		unsigned char opcode;
		if(i >= 0x1000000){
			puts("//Disassembly reached end of usable memory.");
			goto end;
		}else if((i & 0xffFF) == 0){
			printf("//<Region Boundary 0x%06lx >\n", i);
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
		opcode = M[stepper++];i++;
#endif
		if(opcode == 0) 		n_halts++; 		else if(opcode < n_insns) 	n_halts = 0;
		if(opcode >= n_insns) 	n_illegals++; 	else if(opcode != 0) 		n_illegals = 0;
		if(
			opcode < n_insns &&  /*Valid opcode*/
			(insns_numargs[opcode] > 0) && /*With arguments*/
			(((i-1)+insns_numargs[opcode])>>16) != ((i-1)>>16)
		){
			printf("\n//(E_BAD_BINARY)\n//op is '%s' which has %u args but crosses the region boundary.\n//This is most likely the result of interpreting embedded data as code.\n",insns[opcode],insns_numargs[opcode]);
			if( ((((i-1)+insns_numargs[opcode])>>16)&0xff) == 0){
				puts("//This opcode crosses over the memory image boundary. Some of its data is junk:");
			}
		}
		
		if(opcode >= n_insns){
			if(enable_dis_comments)
				printf("%-12s 0x%x                ;//0x%06lx  : Illegal Opcode (E_ILLEGAL_OPCODE) Probably Data.\n","bytes", (unsigned int)opcode, i-1);
			else
				printf("%-12s 0x%x                ;\n","bytes", (unsigned int)opcode);
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
				thechar = M[stepper++];
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
			if(!enable_dis_comments) printf(";\n");
			if(enable_dis_comments)
			{
				printf(";//0x%06lx  :", opcode_i);
				if(bad_flag) printf("(E_WONT_RUN)");
				if(unfinished_flag) printf("(E_UNFINISHED_EOF AT ARGUMENT %u)", unfinished_flag-1);
				if(streq(insns[opcode], "farret")){
					puts(" End of Procedure");
				}else if(
					streq(insns[opcode], "lla") 
					|| streq(insns[opcode], "llb")
				)
				{
					if( (short_interpretation & 0xFF00) == 0)
						puts(" <TIP> replace with single-byte assignment, high byte is zero.");
					else
						puts("");
				}else if(streq(insns[opcode], "ret")){
					puts(" End of Local Procedure");
				} else if(opcode == 0 && n_halts == 1){
					puts(" End of Control Flow");
				} else if(streq(insns[opcode], "jmp")){
					puts(" Jump");
				}else if(streq(insns[opcode], "jmpifeq")){
					puts(" Conditional Jump");
				}else if(streq(insns[opcode], "cbrx0")
						||streq(insns[opcode], "carx0")){
					puts(" ?: Far memory array access through RX0. Check array alignment!");
				}else if(streq(insns[opcode], "jmpifneq")){
					puts(" Conditional Jump");
				}else if(streq(insns[opcode], "clock")){
					puts(" Interact w/ sys time: A=ms, B=s, C=clock()");
				}else if(streq(insns[opcode], "emulate_seg")){
					puts(" Sandboxing Insn w/ Shared Segment. Jumps to 0x000000 and catches exceptions");
				}else if(streq(insns[opcode], "emulate")){
					puts(" Sandboxing Insn. Jump to 0x000000 and catches exceptions.");
				}else if(streq(insns[opcode], "interrupt")){
					puts(" <DEVICE> interrupt returning value to A.");
				}else if(streq(insns[opcode], "putchar")){
					puts(" <DEVICE> write A to device.");
				}else if(streq(insns[opcode], "getchar")){
					puts(" <DEVICE> read A from device.");
				}else if(streq(insns[opcode], "sc")){
					if(short_interpretation == (opcode_i & 0xffFF)){
						puts(" \?!: Loop top. <TIP>: replace with cpc.");
					}else{
						puts(" ?: arg is jump target");
					}
				}else if(streq(insns[opcode], "crx0")
						||streq(insns[opcode], "crx1")
						||streq(insns[opcode], "crx2")
						||streq(insns[opcode], "crx3")){
					puts(" ?: Computed Jump through RX register.");
				}else if(streq(insns[opcode], "ca")){
					puts(" ?\?: Computed Jump through register A");
				}else if(streq(insns[opcode], "cb")){
					puts(" ?\?: Computed Jump through register B");
				}else if(streq(insns[opcode], "astp")){
					puts(" Stack manip, maybe retrieving function arguments?");
				}else if(streq(insns[opcode], "call")){
					puts(" Local Procedure Call");
				}else if(streq(insns[opcode], "farcall")){
					puts(" Procedure Call");
				}else if(streq(insns[opcode], "lfarpc")){
					printf(" Region Jump to %u\n", (unsigned)byte_interpretation);
				}else if(streq(insns[opcode], "farjmprx0")){
					puts(" Far Jump Through RX0");
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
					puts(" ?: Loading from fixed position variable.");
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
					puts(" ?: Storing to fixed position variable.");
				}else if(streq(insns[opcode], "cpc")){
					puts(" ?: Loop top is next insn");
				}else {printf("\n");}
				if(unfinished_flag){
					puts("\n//End of File, Last Opcode is inaccurately disassembled (E_UNFINISHED_EOF)");
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
