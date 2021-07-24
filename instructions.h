static char* insns[218] = {
	"halt", /*0*/
	"lda",
	"la",
	"ldb",
	"lb",
	"sc",
	"sta",
	"stb",
	"add",
	"sub",
	"mul",
	"div",
	"mod",
	"cmp",
	"jmpifeq",
	"jmpifneq",
	"getchar",
	"putchar",
	"and",
	"or",
	"xor",
	"lsh",
	"rsh",
	"ilda",
	"ildb",
	"cab",
	"ab",
	"ba",
	"alc",
	"ahc",
	"nop",
	"cba",
	/*Special insns for 16 bit mode*/
	"lla",
	"illda",
	"llb",
	"illdb",
	/*Self-Indirecting access mode.*/
	"illdaa",
	"illdbb",
	"illdab",
	"illdba",
	"ca",
	"cb",
	"ac",
	"bc",
	"ista",
	"istb",
	"istla",
	"istlb",
	"jmp",
	"stla",
	"stlb",
	"stc",
	"push",
	"pop",
	"pusha",
	"popa",
	"astp",
	"bstp",
	"compl",
	"cpc",
	/*Call structure*/
	"call",
	"ret",
	/*Far Memory Access*/
	"farillda",
	"faristla",	
	"farilldb",
	"faristlb",
	"farpagel",
	"farpagest",
	"lfarpc",
	/*Call structure*/
	"farcall",
	"farret",
	"farilda",
	"farista",
	"farildb",
	"faristb",
	"priv_drop",
	"user_geta",
	"user_getb",
	"user_getc",
	"user_get0",
	"user_get1",
	"user_get2",
	"user_get3",
	"user_getstp",
	"user_getpc",
	"user_getr",
	"user_farilda",
	"user_seta",
	"task_set",
	"task_kill",
	"syscall",
	/*quickly push registers onto the stack*/
	"alpush",
	"blpush",
	"cpush",
	"apush",
	"bpush",
	/*quickly pop registers from the stack*/
	"alpop",
	"blpop",
	"cpop",
	"apop",
	"bpop",
	"interrupt",
	"clock",
	/*Extra register loads and stores.*/
	/*rx0*/
	"arx0",
	"brx0",
	"crx0",
	"rx0a",
	"rx0b",
	"rx0c",
	/*rx1*/
	"arx1",
	"brx1",
	"crx1",
	"rx1a",
	"rx1b",
	"rx1c",
	/*rx2*/
	"arx2",
	"brx2",
	"crx2",
	"rx2a",
	"rx2b",
	"rx2c",
	/*rx3*/
	"arx3",
	"brx3",
	"crx3",
	"rx3a",
	"rx3b",
	"rx3c",
	/*Register-to-register moves for RX registers.*/
	"rx0_1",
	"rx0_2",
	"rx0_3",

	"rx1_0",
	"rx1_2",
	"rx1_3",

	"rx2_0",
	"rx2_1",
	"rx2_3",

	"rx3_0",
	"rx3_1",
	"rx3_2",
	/*lRXX*/
	"lrx0",
	"lrx1",
	"lrx2",
	"lrx3",
	/*farildRXX*/
	"farildrx0",
	"farildrx1",
	"farildrx2",
	"farildrx3",
	/*faristRXX*/
	"faristrx0",
	"faristrx1",
	"faristrx2",
	"faristrx3",
	/*RXmath*/
	"rxadd",
	"rxsub",
	"rxmul",
	"rxdiv",
	"rxmod",
	"rxrsh",
	"rxlsh",
	"rx0push",
	"rx1push",
	"rx2push",
	"rx3push",
	"rx0pop",
	"rx1pop",
	"rx2pop",
	"rx3pop",
	"rxand",
	"rxor",
	"rxxor",
	"rxcompl",
	"rxcmp",
	"seg_ld",
	"seg_st",
	"seg_realloc",
	"fltadd",
	"fltsub",
	"fltmul",
	"fltdiv",
	"fltcmp",
	"seg_pages",
	"ildrx0_1",
	"ildrx0_0",
	"farjmprx0",
	"istrx0_1",
	"istrx1_0",
	"cbrx0",
	"carx0",
	"rxidiv",
	"rximod",
	/*requires 3 byte arguments.*/
	"farldrx0",
	"farldrx1",
	"farldrx2",
	"farldrx3",
	"farllda",
	"farlldb",
	"farldc",
	"farstrx0",
	"farstrx1",
	"farstrx2",
	"farstrx3",
	"farstla",
	"farstlb",
	"farstc",
	"aincr",
	"adecr",
	"rxincr",
	"rxdecr",
	"emulate",
	"rxitof",
	"rxftoi",
	"emulate_seg",
	"rxicmp",
	/*Boolean logic*/
	"logor",
	"logand",
	"boolify",
	"nota",
	"user_farista",
	"task_ric"
};
static unsigned char insns_numargs[218] = {
	0,/*halt*/
	2,1,2,1, /*load and load constant comboes, lda, la, ldb, lb*/
	2, /*load constant into C*/
	2,2, /*store A or B.*/
	0,0,0,0,0, /*the four elementary, + mod*/
	0, /*cmp*/
	0,0, /*jmp insns*/
	0,0, /*getchar,putchar,*/
	0,0,0,0,0, /*bitwise*/
	0,0, /*indirect load.*/
	0,0,0,0,0,0,0, /*Register to register moves and nop.*/
	
	2,0,2,0, /*16 bit constant loads and loads-through-c*/
	0,0,/*16 bit self-indirect loads*/
	0,0,/*16 bit neighbor-indirect loads*/
	0,0,/*move to C*/
	0,0,/*move from C*/
	0,0,0,0,/*Indirect Stores*/
	0, /*Direct jump.*/
	2,2,2, /*store shorts A,B,and C*/
	2,2, /*push, pop*/
	0,0, /*pusha, popa*/
	0,0, /*retrieve the stack pointer and put it in a or b*/
	0, /*Compl*/
	/*call structure.*/
	0,/*cpc*/
	0,/*call*/
	0,/*ret*/
	0,0, /*Far memory access- a*/
	0,0, /*Far memory access- b*/
	0,0, /*Far page load and store*/
	0,0,0, /*lfarpc, farcall, farret,*/
	0,0,0,0, /*Far memory access- a and b, as single bytes*/
	/*priv_drop*/
	0,
	/*FREE SLOTS!*/
	0,0,0,
	0,0,0,0,
	0,0,0,0,
	0,0,0,0,
	/*More stack ops*/
	0,0,0,
	0,0, 
	/*register pops*/
	0,0,0,
	0,0, 
	/*extended interrupt/device interaction*/
	0,
	/*Clock.*/
	0,
	/*extended registers.*/
	/*RX0*/
		0,0,0,
		0,0,0,
	/*RX1*/
		0,0,0,
		0,0,0,
	/*RX2*/
		0,0,0,
		0,0,0,
	/*RX3*/
		0,0,0,
		0,0,0,
	/*Register-to-register moves for RX registers.*/
		0,0,0,
		0,0,0,
		0,0,0,
		0,0,0,
	/*lRXX*/
		4,4,4,4,
	/*farildRXX and st*/
		0,0,0,0,
		0,0,0,0,
	/*RXmath and CMP*/
		0,0,0,0,
		0,/*mod*/
		0,0,/*shifts*/
		/*push/pop*/
		0,0,0,0,
		0,0,0,0,
		/*bitwise and compl*/
		0,0,0, /*rxand,rxor,rxxor*/
		0,
		/*rxcmp*/
		0,
		/*segment*/
		0,0,0,
		/*float*/
		0,0,0,0,0,
		/*seg_pages*/
		0,
		/*ilrx0_1 and 0_0*/
		0,0,
		/*farjmprx0*/
		0,
		/*istrx0_1 and istrx1_0*/
		0,0,
		/*cbrx0 and carx0*/
		0,0,
		/*rxidiv and rximod*/
		0,0,
		/*farld*/
		3,3,3,3, 3,3,3,
		/*farst*/
		3,3,3,3, 3,3,3,
		/*incr decr*/
		0,0,0,0,
		/*emulate*/
		0,
		/*float-int interop*/
		0,0,
		/*emulate_seg*/
		0,
		/*rxicmp*/
		0,
		/*Boolean ops, logor, logand, boolify, nota*/
		0,0,0,0,
		/*user_farista*/
		0,
		/*task_ric*/
		0
};
static char* insn_repl[218] = {
	"bytes0;", 
	/*The direct load-and-store operations have args.*/
	"bytes1,",
	"bytes2,",
	"bytes3,",
	"bytes4,",
	"bytes5,",
	/*stores have arguments*/
	"bytes6,",
	"bytes7,",
	/*have no arguments.*/
	"bytes8;",
	"bytes9;",
	"bytes10;",
	"bytes11;",
	"bytes12;",
	"bytes13;",
	"bytes14;",
	"bytes15;",
	"bytes16;",
	"bytes17;",
	"bytes18;",
	"bytes19;",
	"bytes20;",
	"bytes21;",
	"bytes22;",
	"bytes23;",
	"bytes24;",
	"bytes25;",
	"bytes26;",
	"bytes27;",
	"bytes28;",
	"bytes29;",
	"bytes30;",
	"bytes31;",
	/*16 bit mode insns*/
	"bytes32,",
	"bytes33;",
	"bytes34,",
	"bytes35;",
	/*self, neighbors indirection*/
	"bytes36;",
	"bytes37;",
	"bytes38;",
	"bytes39;",
	/*direct move to c*/
	"bytes40;",
	"bytes41;",
	/*direct move from c*/
	"bytes42;",
	"bytes43;",
	/*indirect stores*/
	"bytes44;",
	"bytes45;",
	"bytes46;",
	"bytes47;",
	"bytes48;",
	/*Store shorts to constant address*/
	"bytes49,",
	"bytes50,",
	"bytes51,",
	/*Stack pointer operations.*/
	/*Push and pop by constant amounts*/
	"bytes52,",
	"bytes53,",
	/*using a*/
	"bytes54;",
	"bytes55;",
	/*stack pointer retrieval into a or b*/
	"bytes56;",
	"bytes57;",
	/*compl of a*/
	"bytes58;",
	"bytes59;",/*cpc*/
	"bytes60;",  /*call*/
	"bytes61;",  /*ret*/
	/*far memory.*/
	"bytes62;", /*farillda*/
	"bytes63;", /*faristla*/
	"bytes64;", /*farilldb*/
	"bytes65;", 
	"bytes66;", 
	"bytes67;", 
	"bytes68;", 
	"bytes69;", 
	"bytes70;", 
	"bytes71;", 
	"bytes72;", 
	"bytes73;", 
	"bytes74;", 
	
	/*priv_drop*/
	"bytes75;",
	/*FREE SLOTS!*/
	"bytes76;","bytes77;","bytes78;",
	"bytes79;","bytes80;","bytes81;","bytes82;",
	"bytes83;","bytes84;","bytes85;","bytes86;",
	"bytes87;","bytes88;","bytes89;","bytes90;",
	/*Stack ops- pushes*/
	"bytes91;",
	"bytes92;",
	"bytes93;",
	/*single byte stack ops*/
	"bytes94;",
	"bytes95;",
	/*Stack ops- pops*/
	"bytes96;",
	"bytes97;",
	"bytes98;",
	"bytes99;",
	"bytes100;",
	/*interrupt*/
	"bytes101;",
	/*clock*/
	"bytes102;",
	/*extended register interactions.*/
	/*RX0*/
		"bytes103;",
		"bytes104;",
		"bytes105;",

		"bytes106;",
		"bytes107;",
		"bytes108;",
	/*RX1*/
		"bytes109;",
		"bytes110;",
		"bytes111;",

		"bytes112;",
		"bytes113;",
		"bytes114;",
	/*RX2*/
		"bytes115;",
		"bytes116;",
		"bytes117;",

		"bytes118;",
		"bytes119;",
		"bytes120;",
	/*RX3*/
		"bytes121;",
		"bytes122;",
		"bytes123;",

		"bytes124;",
		"bytes125;",
		"bytes126;",
	/*Register-to-register moves for RX registers.*/
		"bytes127;",
		"bytes128;",
		"bytes129;",

		"bytes130;",
		"bytes131;",
		"bytes132;",

		"bytes133;",
		"bytes134;",
		"bytes135;",

		"bytes136;",
		"bytes137;",
		"bytes138;",
	/*lRXX*/
		"bytes139,",
		"bytes140,",
		"bytes141,",
		"bytes142,",
	/*farildRXX*/
		"bytes143;",
		"bytes144;",
		"bytes145;",
		"bytes146;",
	/*faristRXX*/
		"bytes147;",
		"bytes148;",
		"bytes149;",
		"bytes150;",
	/*More RX stuff.*/
		"bytes151;",
		"bytes152;",
		"bytes153;",
		"bytes154;",
		"bytes155;",
		"bytes156;",
		"bytes157;",
		"bytes158;",
		"bytes159;",
		"bytes160;",
		"bytes161;",
		"bytes162;",
		"bytes163;",
		"bytes164;",
		"bytes165;",
		"bytes166;",
		"bytes167;",
		"bytes168;",
		"bytes169;",
	/*rxcmp*/
		"bytes170;",
	/*segment*/
		"bytes171;",
		"bytes172;",
		"bytes173;",
	/*float*/
		"bytes174;",
		"bytes175;",
		"bytes176;",
		"bytes177;",
	/*cmp*/
		"bytes178;",
	/*seg_pages*/
		"bytes179;",
	/*ilrx0_1 and ilrx0_0*/
		"bytes180;",
		"bytes181;",
	/*farjmprx0*/
		"bytes182;",
		"bytes183;",
		"bytes184;",
	/*cbrx0 and carx0*/
		"bytes185;",
		"bytes186;",
	/*rxidiv and rximod*/
		"bytes187;",
		"bytes188;",
	/*the 14 instructions that require 3 bytes as argument.*/
		"bytes189,","bytes190,","bytes191,","bytes192,",   "bytes193,","bytes194,","bytes195,",
		"bytes196,","bytes197,","bytes198,","bytes199,",   "bytes200,","bytes201,","bytes202,",
	/*incr decr*/
		"bytes203;","bytes204;","bytes205;","bytes206;",
	/*emulate*/
		"bytes207;",
	/*Float-Int Interop*/
		"bytes208;","bytes209;",
	/*emulate_seg*/
		"bytes210;",
	/*rxicmp*/
		"bytes211;",
	/*logor, logand, boolify, nota*/
		"bytes212;",
		"bytes213;",
		"bytes214;",
		"bytes215;",
		/*user_farista*/
		"bytes216;",
		/*task_ric*/
		"bytes217;"
};
static const unsigned int n_insns = 218;
