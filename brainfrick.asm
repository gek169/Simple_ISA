#!/usr/bin/sisa16_asm -run

//..include"libc.hasm"
..include"libc_pre.hasm"
..(2):
..dinclude"libc_pre.bin"
..(0x40):
:brainfrick_prompt:
bytes '\r', '\n';
..ascii:Enter BF:
bytes '\r', '\n', 0;

..(0x10): 
	:brainfrick_program_memory:

..(0x60): //You have quite a bit of freedom with the cells.
	:brainfrick_cells:

..(0x30):
:brainfrick_bracket_parsing_depth:
bytes %/0%;
..decl_farproc:interpret_bf
	bf_interpreter_looptop:
		//fetch a character from the array.
		rx0_2; cbrx0; farilda; 
				apush;lb '>'; cmp; bpop; sc %bf_ptr_right%; jmpifeq;
			ab;	apush;lb '<'; cmp; bpop; sc %bf_ptr_left%; jmpifeq;
			ab;	apush;lb '+'; cmp; bpop; sc %bf_incr%; jmpifeq;
			ab;	apush;lb '-'; cmp; bpop; sc %bf_decr%; jmpifeq;
			ab;	apush;lb '.'; cmp; bpop; sc %bf_output%; jmpifeq;
			ab;	apush;lb ','; cmp; bpop; sc %bf_input%; jmpifeq;
			ab;	apush;lb '['; cmp; bpop; sc %bf_leftbracket%; jmpifeq;
			ab;	apush;lb ']'; cmp; bpop; sc %bf_rightbracket%; jmpifeq;
			farret;
		bf_ptr_right:
			rx0_3; rxincr; rx3_0;
			rx0_2; rxincr; rx2_0;

			sc %bf_interpreter_looptop%; jmp;
		bf_ptr_left:
			rx0_3; rxdecr; rx3_0;
			rx0_2; rxincr; rx2_0;
			sc %bf_interpreter_looptop%; jmp;
		bf_output:
			rx0_3; cbrx0; farilda; putchar; la '\n'; interrupt;
			rx0_2; rxincr; rx2_0;
			sc %bf_interpreter_looptop%; jmp;
		bf_input:
			rx0_3; cbrx0; getchar; farista;
			rx0_2; rxincr; rx2_0;
			sc %bf_interpreter_looptop%; jmp;
		bf_incr:
			rx0_3; cbrx0; farilda; aincr; farista;
			rx0_2; rxincr; rx2_0;
			sc %bf_interpreter_looptop%; jmp;
		bf_decr:
			rx0_3; cbrx0; farilda; adecr; farista;
			rx0_2; rxincr; rx2_0;
			sc %bf_interpreter_looptop%; jmp;
		bf_leftbracket:
			rx0_3; cbrx0; farilda; 
			nota; 
			sc %bf_leftbracket_do_jump_past%; jmpifeq;
			//No jumping!
			rx0_2; rxincr; rx2_0;
			sc %bf_interpreter_looptop%; jmp;
			bf_leftbracket_do_jump_past:
				lrx0 %/1%; 
				farstrx0 %&brainfrick_bracket_parsing_depth%;
				rx0_2; rxincr; rx2_0;
				bf_leftbracket_looptop:
					rx0_2; cbrx0; farilda;
					apush;
					lb '['; cmp; bpop; sc %bf_leftbracket_found_leftbracket%; jmpifeq;
					ab;apush;
					lb ']'; cmp; bpop; sc %bf_leftbracket_found_rightbracket%; jmpifeq;
					//it was neither!
					rxincr; rx2_0;
					sc %bf_leftbracket_looptop%; jmp;
					
					bf_leftbracket_found_leftbracket:
						farldrx0 %&brainfrick_bracket_parsing_depth%; 
						rxincr;
						farstrx0 %&brainfrick_bracket_parsing_depth%;
						rx0_2; rxincr; rx2_0;
						sc %bf_leftbracket_looptop%; jmp;	
					bf_leftbracket_found_rightbracket:
						farldrx0 %&brainfrick_bracket_parsing_depth%; 
						rxdecr;
						farstrx0 %&brainfrick_bracket_parsing_depth%;
						lb 0; rx1b; rxcmp;
						sc %bf_leftbracket_loopend%; jmpifeq; //the break condition.
						rx0_2; rxincr; rx2_0;
						sc %bf_leftbracket_looptop%; jmp;
			bf_leftbracket_loopend:
			rx0_2; rxincr; rx2_0; //skip that right bracket.
			sc %bf_interpreter_looptop%; jmp;
		bf_rightbracket:
			rx0_3; cbrx0; farilda; 
			boolify; 
			sc %bf_rightbracket_do_jump_past%; jmpifeq;
			//No jumping!
			rx0_2; rxincr; rx2_0;
			sc %bf_interpreter_looptop%; jmp;

			bf_rightbracket_do_jump_past:
				lrx0 %/1%; 
				farstrx0 %&brainfrick_bracket_parsing_depth%;
				rx0_2; rxdecr; rx2_0;
				bf_rightbracket_looptop:
					rx0_2; cbrx0; farilda;
					apush;
					lb '['; cmp; bpop; sc %bf_rightbracket_found_leftbracket%; jmpifeq;
					ab;apush;
					lb ']'; cmp; bpop; sc %bf_rightbracket_found_rightbracket%; jmpifeq;
					//it was neither! Remember, the right bracket moves the pointer BACKWARDS.
					rxdecr; rx2_0;
					sc %bf_rightbracket_looptop%; jmp;
					bf_rightbracket_found_leftbracket:
						farldrx0 %&brainfrick_bracket_parsing_depth%; 
						rxdecr;
						farstrx0 %&brainfrick_bracket_parsing_depth%;
						lb 0; rx1b; rxcmp;
						sc %bf_rightbracket_loopend%; jmpifeq; //the break condition.
						rx0_2; rxdecr; rx2_0;
						sc %bf_rightbracket_looptop%; jmp;	
					bf_rightbracket_found_rightbracket:
						farldrx0 %&brainfrick_bracket_parsing_depth%; 
						rxincr;
						farstrx0 %&brainfrick_bracket_parsing_depth%;
						rx0_2; rxdecr; rx2_0;
						sc %bf_rightbracket_looptop%; jmp;
			bf_rightbracket_loopend:
			rx0_2; rxincr; rx2_0; //The one after the left bracket.
			sc %bf_interpreter_looptop%; jmp;
:bf_lexer_lbracket_count:
bytes %/0%;
:bf_lexer_rbracket_count:
bytes %/0%;
..decl_farproc:bf_lex
	lrx0 %/brainfrick_program_memory%;
	
	lrx0 %/0%; //success.	
	farret;

..main(3): 
	lrx0 %/krenel_boot%;
	proc_krenel;
	halt;
	krenel_boot:
	lrx0 %/brainfrick_prompt%;
	proc_prints; la '\n'; interrupt;
	lrx0 %/brainfrick_program_memory%;
	proc_gets;
	//interpret the code entered.
	lrx2 %/brainfrick_program_memory%;
	lrx3 %/brainfrick_cells%;
	//TODO: lex the brainfrick
	interpret_bf;
	sc %krenel_boot%; jmp;
	halt;
