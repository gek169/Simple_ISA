/*
Unfinished idea for a very simple BASIC-like language.

*/

#include "stringutil.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

char* infilename = NULL;
FILE* infile = NULL;
char* variable_names[0x10000];
unsigned long n_vars = 0;
unsigned char variable_types[0x10000]; /**/
long variable_locations[0x100000];
unsigned short variable_lengths[0x10000];

unsigned char is_variable_array(unsigned long variable_id){
	if(variable_id >= n_vars) return 0; /*Can't be*/
	return ((variable_types[variable_id]&128)>0);
}

unsigned char is_variable_short(unsigned long variable_id){
	if(variable_id >= n_vars) return 0; /*Can't be*/
	return ((variable_types[variable_id]&1)>0);
}

unsigned char is_variable_byte(unsigned long variable_id){
	if(variable_id >= n_vars) return 0; /*Can't be*/
	return ((variable_types[variable_id]&1)==0);
}

unsigned char is_variable_farptr(unsigned long variable_id){
	if(variable_id >= n_vars) return 0; /*Can't be*/
	return ((variable_types[variable_id]&3)==2);
}

int main(int argc, char** argv){	
	if(argc > 2)
	{char* larg = argv[1]; long i;
		for(i=2; i < argc; i++){
			if(streq("-i", larg)) infilename = argv[i];
			larg = argv[i];
		}
	}
	if(!infilename){
		puts("//<FIFTH ERROR> no input files.");
		exit(1);
	}
	infile = fopen(infilename, "rb");
	puts("#!/usr/bin/sisa16_asm -run");
/*Single-pass compilation.*/
	for(;;){ char* line; unsigned long linesize; char* line_copy;
		if(feof(infile)) break;
		line = read_until_terminator_alloced(infile, &linesize, '\n', 1);
		while(strprefix(" ",line) || strprefix("\t",line)){ /*Remove preceding whitespace*/
			char* line_old = line;
			line = strcatalloc(line+1,"");
			free(line_old);
		}
		if(strlen(line) < 2) goto end; /*Ignore lines that are less than two characters long after removing whitespace.*/
		if(strprefix("//", line)){goto end;}
		if(strprefix("RAW_ASM:", line)){ /*ignore the entire line.*/
			printf("\n%s", line + strlen("RAW_ASM:"));
			goto end;
		}
		/*
			handle line escapes.
		*/
		while(!feof(infile) && strlen(line) > 0 && !strprefix("//",line) && line[strlen(line)-1] == '\\'){char* line_temp;
			line[strlen(line)-1] = '\0';
			line_temp = read_until_terminator_alloced(infile, &linesize, '\n', 1);
			line = strcatallocfb(line,line_temp);
			linesize = strlen(line);
		}
		/*
			change all whitespace characters to spaces.
		*/
		{unsigned long i;
			for(i = 0; i < strlen(line); i++){
				if(isspace(line[i])) line[i] = ' ';
			}
		}
		/* replace all instances of a double space with a single space.

		*/
		{char have_changed = 0;
			do{
				unsigned long len_before, len_after;
				have_changed = 0;
				len_before = strlen(line);
				line = str_repl_allocf(line, "  ", " ");
				if(!line) goto error;
				len_after = strlen(line);
				if(len_before != len_after) have_changed = 1;
			}while(have_changed);
		}
		line_copy = strcatalloc(line, "");
		if(strprefix(":", line)){ /*label declaration statement.*/
			/*remove spaces from this line, we dont need any.*/
			while(strfind(line, " ") > -1) line = str_repl_allocf(line, " ", "");
			printf("\nVAR#%s#@",line + 1);
			goto end;
		}
		if(strprefix("goto ", line)){
			/*What follows is a label which was declared using the above syntax.*/
			printf("\nsc %%%s%%",line + strlen("goto "));
			goto end;
		}
		if(strprefix("var ", line)){
			long loc;
			char is_array = 0;
			char raw_type = 0;
			unsigned long location = 0;
			unsigned short length = 0; char* metaproc;
			/*What follows is the typing information*/
			metaproc = line + strlen("var ");
			if(strprefix("byte",metaproc)){
				raw_type = 0;
				metaproc += strlen("byte");
			} else if(strprefix("short",metaproc)){
				raw_type = 1;
				metaproc += strlen("short");
			} else if(strprefix("farptr",metaproc)){
				raw_type = 2;
				metaproc += strlen("farptr");
			} else {
				printf("\n//FIFTH COULD NOT PROCEED AT THIS LINE: %s DUE TO A VARIABLE DECLARATION WITH AN INVALID TYPE.",line_copy);
				goto error;
			}
			if(strprefix(" ",metaproc))metaproc++;
			/*array declaration.*/
			if(strprefix("[",metaproc)){
				is_array = 1;metaproc++;
				length = strtoul(metaproc, NULL, 0);
				if(length == 0){
					printf("\n//FIFTH COULD NOT PROCEED AT THIS LINE: %s DUE TO A ZERO-LENGTH ARRAY DECLARATION.",line_copy);
					goto end;
				}
				loc = strfind(metaproc, "]");
				if(loc == -1){
					printf("\n//FIFTH COULD NOT PROCEED AT THIS LINE: %s DUE TO MISSING ] IN VARIABLE DECLARATION.",line_copy);
					goto end;
				}
				metaproc += loc+1;
			}
			if( strprefix(" ",metaproc)  )metaproc++;
			/*Memory location as unsigned long.*/
			if(!strprefix("@",metaproc)){
				printf("\n//FIFTH COULD NOT PROCEED AT THIS LINE: %s BECAUSE OF A MISSING AT SIGN!.",line_copy);
				goto end;
			}
			metaproc++; /*Skip that at sign.*/
			location = strtoul(metaproc, NULL, 0);
			if(location == 0){
				printf("\n//FIFTH DETECTS A SUSPICIOUS VARIABLE DECLARATION! Did you really mean to make this variable declaration, %s at zero?",line_copy);
			}
			loc = strfind(metaproc," ");
			if(loc == -1){
				printf("\n//FIFTH CANNOT PROCEED BECAUSE THE MEMORY LOCATION DECLARATION LACKS WHITESPACE AFTER IT. Line: %s", line_copy);
				goto error;
			} 
			if(loc == 0){
				printf("\n//FIFTH CANNOT PROCEED BECAUSE THE MEMORY LOCATION DECLARATION LACKS... A MEMORY LOCATION! Line: %s", line_copy);
				goto error;
			}
			metaproc += 1 + loc;
			/*TODO handle the rest of the line.*/
			goto end;
		}

		puts("\n//Unrecognized line option.\n"); goto error;
		end:;
		free(line); free(line_copy); continue;
		error:;
		free(line); free(line_copy); printf("\n//Fifth compilation !!!FAILED!!! here.\n"); continue;
	}
	return 0;
}
