#include "d.h"
#include "isa.h"
/*
	Textmode emulator for SISA16.

	Uses stdin to recieve memory image before execution.

	ROMs must be exactly 16 megabytes to work correctly and use stdin.
*/

int main(){
	UU i=0;
	if(
		(sizeof(U) != 2) ||
		(sizeof(u) != 1) ||
		(sizeof(UU) != 4)
	){
		puts("SISA16 ERROR!!! Compilation Environment conditions unmet.");
		if(sizeof(U) != 2)
			puts("U is not 2 bytes. Try using something other than unsigned short (default).");
		if(sizeof(u) != 1)
			puts("u is not 2 bytes. Try using something other than unsigned char (default).");
		if(sizeof(UU) != 4)
			puts("UU is not 4 bytes. Try toggling -DUSE_UNSIGNED_INT. the default is to use unsigned int as UU.");
		return 1;
	}
	for(;!feof(stdin);){
		M[i++]=fgetc(stdin);i&=0xffffff;if(i==0)break;
	}
	e();
	if(R==1)puts("\n<Errfl, 16 bit div by 0>\n");
	if(R==2)puts("\n<Errfl, 16 bit mod by 0>\n");
	if(R==3)puts("\n<Errfl, 32 bit div by 0>\n");
	if(R==4)puts("\n<Errfl, 32 bit mod by 0>\n");
}
