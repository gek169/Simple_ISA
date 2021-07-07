
static char* expr_buf = NULL;

typedef struct asm_expr_stack_elem{
	UU data_u32;
	float data_float;
	char* ident_name;
	void (*codegen_load)(struct asm_expr_stack_elem); 
	/*
		the code that is generated for loading, when the top of the stack holds an element of the native type.
		load this as our type.
		The stack (both) always looks like this:
		<value>
		<whatever this thing pushed on the stack>
		<crap...>
	*/
	void (*codegen_store)(struct asm_expr_stack_elem);
	/*
		the code that is generated for assignment, when the top of the stack holds an element of the native type.
		The stack (both) always looks like this:
		<value>
		<whatever this thing pushed on the stack>
		<crap...>
	*/
	unsigned char flags; /*
		1 = is_const. Can constant propagation be performed?
		2 = is_variable. Does it require codegen for loading, etc?
	*/
	unsigned char type; 
	/*
		Possible valid interpretations of this object.
		1 = u8
		2 = u16
		3 = u32
		4 = i32
		5 = float
		needed because, 
		for instance, this is invalid:
		..eu16: uint(22+27*2) + 5
		because math cannot be performed in a 16 bit unsigned expression between a u32 and the 16 bit 5.
		however this is valid:
		..eu16: uint_to_ushort(22+27*2) + 5
	*/
	unsigned long ptr_level;
	/*
		how many levels of indirection are there?
	*/
} asm_expr_stack_elem;
