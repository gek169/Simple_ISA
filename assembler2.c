#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "isa_pre.h"
unsigned char STRBUF[0x10000]; /*string buffer. Would be located in main memory.*/
UU CUR_TOKEN_LEN = 0; /*Length of the current token.*/
UU N_TOKENS = 0; /*Number of tokens received.*/
UU TOKEN_IDENT = 0; /*current token identification. 0 means we need to determine what this is part of.*/
/*
	1- text (alphanum, period, and underscore)
	2- number, optionally signed, optionally containing 0x.
	3- unusual character
*/
const UU TOKEN_TEXT = 1;
const UU TOKEN_NUM = 2;
const UU TOKEN_UNU = 3; /*Preserved in case we decide to ever have any multi-character things.*/
const UU TOKEN_FORWARDSLASH = 4;
const UU TOKEN_STRING = 5; /*A string can be at most 64k characters. Is that a huge limitation?*/

/*
	standin for SISA16's SEG_REALLOC.
*/

char istext(unsigned char c){
	return isalpha(c) || (c == '_') || (c == '.');
}

void SEG_REALLOC(UU RX0){
	u* SEGMENT_OLD = SEGMENT;
	UU SEGMENT_PAGES_OLD = SEGMENT_PAGES;
	SEGMENT_PAGES = RX0;
	if(RX0 == 0){
		if(SEGMENT) free(SEGMENT);
		SEGMENT = NULL;
		SEGMENT_PAGES = 0;
	} else {
		if(SEGMENT_PAGES_OLD != SEGMENT_PAGES){
			if(!SEGMENT)
				SEGMENT = calloc(1, 0x100 * SEGMENT_PAGES);
			else
				SEGMENT = realloc(SEGMENT, 0x100 * SEGMENT_PAGES);
		}
		if(!SEGMENT){
			SEGMENT_PAGES = SEGMENT_PAGES_OLD;
			SEGMENT = SEGMENT_OLD;
			printf("\r\n<ERROR> failed malloc.\r\n");
			exit(1);
		}
		if(SEGMENT_OLD)
			if(SEGMENT_PAGES_OLD < SEGMENT_PAGES){
				/*Must initialize memory to zero.*/
				size_t end, i;
				i = ((size_t)SEGMENT_PAGES_OLD) * 256;
				end = ((size_t)SEGMENT_PAGES) * 256;
				for(;i<end;i++)SEGMENT[i] = 0;
			}
		
	}
}

void pushToken(){
	UU needed_space = (CUR_TOKEN_LEN + 255)>>8;
	if(needed_space == 0) {
		printf("<ERROR> empty token?\r\n");
		exit(1);
	}
	SEG_REALLOC(SEGMENT_PAGES + needed_space);
	memcpy(
		SEGMENT+((SEGMENT_PAGES-needed_space)<<8),
		STRBUF, 
		needed_space * 256
	);
	N_TOKENS++;
	CUR_TOKEN_LEN = 0;
	printf("<token %u, identification %u> %s\r\n", (unsigned int)N_TOKENS, (unsigned int)TOKEN_IDENT, SEGMENT+(SEGMENT_PAGES-needed_space)*256);
	TOKEN_IDENT = 0;
}


int main(){
	/*
		The program is received via stdin and is tokenized on-the-fly.
	*/
	while(1){
		unsigned char c = fgetc(stdin);
		if(CUR_TOKEN_LEN > 0xffFF){
			printf("<ERROR> Token exceeds size limit.");
			exit(1);
		}
		if(feof(stdin)) break;
		if(c == 0xFF) break;
		if(c == 0) break;
		/*fputc(c, stdout);*/
		/*
			IDENTIFIER CONTINUATIONS.
		*/
		if(c == '/' && CUR_TOKEN_LEN == 1 && TOKEN_IDENT == TOKEN_FORWARDSLASH){ /**/
			while(!feof(stdin) && c != '\n') c = fgetc(stdin);
			if(feof(stdin)) break;
			if(c == 0xFF) break;
			if(c == 0) break;
			CUR_TOKEN_LEN = 0;
			continue;
		} else if(TOKEN_IDENT == TOKEN_FORWARDSLASH){
			STRBUF[CUR_TOKEN_LEN++] = '\0';
			pushToken();
		} else if(TOKEN_IDENT == TOKEN_TEXT){
			if(!istext(c)){
				STRBUF[CUR_TOKEN_LEN++] = '\0';
				pushToken();
			} else {
				STRBUF[CUR_TOKEN_LEN++] = c;
				continue;
			}
		} else if(TOKEN_IDENT == TOKEN_NUM){
			if(c == '0'
				|| c == '1'
				|| c == '2'
				|| c == '3'
				|| c == '4'
				|| c == '5'
				|| c == '6'
				|| c == '7'
				|| c == '8'
				|| c == '9'
				|| c == 'a' || c == 'A'
				|| c == 'b' || c == 'B'
				|| c == 'c' || c == 'C'
				|| c == 'd' || c == 'D'
				|| c == 'e' || c == 'E'
				|| c == 'f' || c == 'F'
				|| c == 'x' /*Hex 0x*/
				|| c == '.' /*Float decimal point*/
			){
				STRBUF[CUR_TOKEN_LEN++] = c;
				continue;
			} else {
				STRBUF[CUR_TOKEN_LEN++] = '\0';
				pushToken();
			}
		}
		/*
			IDENTIFIER STARTERS.
		*/
		if(c == '\r' ||
		   c == ' '  ||
		   c == '\t' ||
		   c == '\f' ||
		   c == '\n' ||
		   c == '\e' ||
		   c == '\v' ||
		   c < 32) 
		{
			if(CUR_TOKEN_LEN) pushToken();
			TOKEN_IDENT = 0;
			CUR_TOKEN_LEN = 0;
			continue;
		} 
		else if(isdigit(c) && CUR_TOKEN_LEN == 0){
			STRBUF[CUR_TOKEN_LEN++] = c;
			TOKEN_IDENT = TOKEN_NUM;
			continue;
		}
		else if(istext(c) && CUR_TOKEN_LEN == 0){
			STRBUF[CUR_TOKEN_LEN++] = c;
			TOKEN_IDENT = TOKEN_TEXT;
			continue;			
		}
		else if(c == '"' && CUR_TOKEN_LEN == 0){
			/*STRBUF[CUR_TOKEN_LEN++] = c; Don't even store the damned thing.*/
			TOKEN_IDENT = TOKEN_STRING;
			while(!feof(stdin)) {
				c = fgetc(stdin);
				if(c == '"') break;
				if(c == 0xFF) break;
				if(c == 0) break;
				STRBUF[CUR_TOKEN_LEN++] = c;
				if(CUR_TOKEN_LEN > 0xffFF){
					printf("<ERROR> String token exceeds size limit.");
					exit(1);
				}
			}
			if(feof(stdin)
			||(c == 0xFF) 
			||(c == 0)){
				printf("<ERROR> Unterminated string literal.");
				exit(1);
			} 
			continue;
		}else if(c == '/' && CUR_TOKEN_LEN == 0){
			STRBUF[CUR_TOKEN_LEN++] = c;
			TOKEN_IDENT = TOKEN_FORWARDSLASH;
			continue;			
		}else if(CUR_TOKEN_LEN == 0){
			STRBUF[0] = c;
			STRBUF[1] = '\0';
			CUR_TOKEN_LEN = 2;
			TOKEN_IDENT = TOKEN_UNU;
			pushToken();
			continue;
		}

		printf("<TOKENIZER ERROR> unhandled case.\r\n");
		exit(1);

	}

	return 0;
}
