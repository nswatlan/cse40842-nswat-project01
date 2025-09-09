//norah swatland project 1 
//albaCore assembler 
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#define MAX_LINE 100 //idk why 100 i may increase or decrease

//struct and enum
typedef enum { //instruction formats (i made them up based on the instruction set)
		A_TYPE, B_TYPE, C_TYPE, D_TYPE, E_TYPE, F_TYPE, G_TYPE, H_TYPE, I_TYPE
} format_t; 

typedef struct { //instructions
	char *name; 
	format_t format; 
	uint8_t bits_15_to_12; 
} instr_def_t; 

//instruction definitions
static instr_def_t instructions[] = {
	{"add", A_TYPE, 0x0}, //a types
	{"sub", A_TYPE, 0x1},
	{"and", A_TYPE, 0x2},
	{"or", A_TYPE, 0x3},
	{"not", A_TYPE, 0x4},
	{"shl", A_TYPE, 0x5},
	{"shr", A_TYPE, 0x6},

	{"ldi", B_TYPE, 0x7},//b type

	{"ld", C_TYPE, 0x8}, //c type

	{"st", D_TYPE, 0x9}, //d type

	{"br", E_TYPE, 0xA}, //e type

	{"bz", F_TYPE, 0xB}, //f types
	{"bn", F_TYPE, 0xC}, 

	{"jal", G_TYPE, 0xD}, //g type

	{"jr", H_TYPE, 0xE}, //h type

	{"quit", I_TYPE, 0xF}, // i type

	{NULL, 0, 0}//sentinel
}; 

//function prototypes
void remove_whitespace(char *s); 
int assemble(char *input_file, char *output_file); 
int get_reg_num(char *reg); 
int get_imm(char *imm); 
instr_def_t *find_instr(char *name); 
uint16_t assemble_a(instr_def_t *instr, int rw, int ra, int rb); 
uint16_t assemble_b(instr_def_t *instr, int rw, int imm8); 
uint16_t assemble_c(instr_def_t *instr, int rw, int imm4, int rb);
uint16_t assemble_d(instr_def_t *instr, int imm4, int ra, int rb);
uint16_t assemble_e(instr_def_t *instr, int imm8);
uint16_t assemble_f(instr_def_t *instr, int imm8, int rb); 
uint16_t assemble_g(instr_def_t *instr, int imm12); 
uint16_t assemble_h(instr_def_t *instr, int ra); 
uint16_t assemble_i(instr_def_t *instr); 
char *convert_to_bin(uint16_t decimal); 
int assemble_interactive(char *output_file); 

int main(int argc, char *argv[]) {	
	if (argc == 2) {
		assemble_interactive(argv[1]);
	}
	if (argc == 1 || argc > 3) {
		printf("(for batch mode) Usage: %s <input.s> <output.bin>\n(for interactive mode) Usage: %s <output.bin>\n", argv[0], argv[0]); 
		return -1; 
	}
	if (argc == 3) {
		assemble(argv[1],argv[2]);  
	}
	return 0; 
}



//function definitions
void remove_whitespace(char *s) { //func to remove space from lines when being read in 
	char *start = s; 
	char *end;
	while (isspace(*start)) { //skip any leading whitespace 
		start++; 
	}
	end = start + strlen(start) - 1; //find end 
	while (end > start && isspace(*end)) { //remove trailing whitespace
		end--; 
	}
	*(end + 1) = '\0'; //null terminate s
	//may need to move s to beginning 
}

//main (not main() but the main) function that will do most of the work
int assemble(char *input_file, char *output_file) {
	FILE *fp_input = fopen(input_file, "r"); //file pointers for i/o
	FILE *fp_output = fopen(output_file, "wb"); //wb is binary mode
	//check fps are not null
	if (!fp_input) { 
		printf("cannot open input file: %s\n", input_file); 
		return -1; 
	}
	if (!fp_output) { 
		printf("cannot open output file: %s\n", output_file); 
		return -1; 
	}
	
	char line[MAX_LINE];
	while (fgets(line, sizeof(line), fp_input)) { //read in each line and remove whitespace
		remove_whitespace(line);
		//parse each line
		char *instruction_line = line;  
		char *tokens[4];
		int token_count = 0; 

		char *token = strtok(instruction_line, " ,\t");
		while (token && token_count < 4) {
			tokens[token_count++] = token;
			token = strtok(NULL, " ,\t");
		}
		/*
		int reg = get_reg_num(tokens[1]); 
		int immediate = get_imm(tokens[2]); 
		printf("reg: %d\n", reg); 
		printf("imm: %d\n", immediate);
		*/
		instr_def_t *instr = find_instr(tokens[0]); 
		if (!instr) {
			printf("unknown instruction '%s'\n", tokens[0]);
			fclose(fp_input); 
			fclose(fp_output); 
			return -1; 
		}
		
		uint16_t machine_code = 0; 
		if (instr->format == A_TYPE) {
			//assemble a type
			int rw = get_reg_num(tokens[1]);
			int ra = get_reg_num(tokens[2]);  
			int rb = get_reg_num(tokens[3]);
			machine_code = assemble_a(instr, rw, ra, rb); 
			//printf("%d\n", (int)machine_code); 
			//printf("binary: %s\n", convert_to_bin((int)(machine_code)));
		}
		else if (instr->format == B_TYPE){
			//assemble b type
			int rw = get_reg_num(tokens[1]); 
			int imm8 = get_imm(tokens[2]); 
			machine_code = assemble_b(instr, rw, imm8); 
			//printf("%d\n", (int)machine_code); 
		}
		else if (instr->format == C_TYPE) {
			//assemble c type
			int rw = get_reg_num(tokens[1]);
			int rb = get_reg_num(tokens[2]); 
			int imm4 = get_imm(tokens[3]);
			machine_code = assemble_c(instr, rw, imm4, rb); 
			//printf("%d\n", (int)machine_code);
		}
		else if (instr->format == D_TYPE) {
			//assemble d type
			int ra = get_reg_num(tokens[1]);
			int rb = get_reg_num(tokens[2]);
			int imm4 = get_imm(tokens[3]);
			machine_code = assemble_d(instr, imm4, ra, rb); 
			//printf("%d\n", (int)machine_code);

		}
		else if (instr->format == E_TYPE) {
			//assemble e type
			int imm8 = get_imm(tokens[1]);
			machine_code = assemble_e(instr, imm8);
			//printf("%d\n", (int)machine_code);
		}
		else if (instr->format == F_TYPE) {
			//assemble f type
			int rb = get_reg_num(tokens[1]);
			int imm8 = get_imm(tokens[2]);
			machine_code = assemble_f(instr, imm8, rb);
			//printf("%d\n", (int)machine_code);
		}
		else if (instr->format == G_TYPE) {
			//assemble g type
			int imm12 = get_imm(tokens[1]);
			machine_code = assemble_g(instr, imm12);
			//printf("%d\n", (int)machine_code);
		}
		else if (instr->format == H_TYPE) {
			//assemble h type
			//jr ra
			int ra = get_reg_num(tokens[1]); 
			machine_code = assemble_h(instr, ra);
			//printf("%d\n", (int)machine_code);
		}
		else if (instr->format == I_TYPE) {
			//assemble i type
			machine_code = assemble_i(instr);
			//printf("%d\n", (int)machine_code);
		}
		int integer_machine = (int)machine_code; 
		fwrite(&integer_machine, sizeof(int),1,fp_output); //fwrite to write binary 
	} 
	printf("assembled!\n");
	return 0;
}

int get_reg_num(char *reg) { //func to return int version of reg 
	int reg_num = atoi(reg + 1);
	if (reg_num >=0 && reg_num <= 15) {
		return reg_num; 
	}
	return -1; //invalid reg 
}

int get_imm(char *imm) {
	return atoi(imm);
}

instr_def_t *find_instr(char *name) {
	for (int i = 0; instructions[i].name != NULL; i++) {
		if (strcmp(instructions[i].name, name) == 0) {
			return &instructions[i];
		}
	}
	return NULL; //instr not found
}
//func to assemble a type. shifts the bits and ors them together to generate machine code
uint16_t assemble_a(instr_def_t *instr, int rw, int ra, int rb) {
	return (instr->bits_15_to_12 << 12) | (rw << 8) | (ra << 4) | (rb); 
}
//b type function
uint16_t assemble_b(instr_def_t *instr, int rw, int imm8) {
	return (instr->bits_15_to_12 << 12) | (rw << 8) | (imm8); 
}
//c type function
uint16_t assemble_c(instr_def_t *instr, int rw, int imm4, int rb) {
	return (instr->bits_15_to_12 << 12) | (rw << 8) | (imm4 << 4) | (rb); 
}
//dtype function
uint16_t assemble_d(instr_def_t *instr, int imm4, int ra, int rb) {
	return (instr->bits_15_to_12 << 12) | (imm4 << 8) | (ra << 4) | (rb); 
}
//etype func
uint16_t assemble_e(instr_def_t *instr, int imm8) {
	return (instr->bits_15_to_12 << 12) | (imm8 << 4) | (0); //using 0 for dont care vals
}
//f type
uint16_t assemble_f(instr_def_t *instr, int imm8, int rb) {
	return (instr->bits_15_to_12 << 12) | (imm8 << 4) | (rb); 
}
//gtype
uint16_t assemble_g(instr_def_t *instr, int imm12) {
	return (instr->bits_15_to_12 << 12) | (imm12);  
}
//h type
uint16_t assemble_h(instr_def_t *instr, int ra) {
	return (instr->bits_15_to_12 << 12) | (0 << 8) | (ra << 4) | (0); //using 0 for don't care vals
}
//i type 
uint16_t assemble_i(instr_def_t *instr) {
	return (instr->bits_15_to_12 << 12) | (0); 
}
int assemble_interactive(char *output_file) {
	FILE *fp_output = fopen(output_file, "wb");
	if (!fp_output) { 
		printf("cannot open output file: %s\n", output_file); 
		return -1; 
	}
	char line[MAX_LINE]; 
	while(fgets(line, sizeof(line), stdin)) {
		line[strcspn(line, "\n")] = '\0'; 
		//this code below is taken from assemble() which i already wrote 
		remove_whitespace(line);
		//parse each line
		char *instruction_line = line;  
		char *tokens[4];
		int token_count = 0; 

		char *token = strtok(instruction_line, " ,\t");
		while (token && token_count < 4) {
			tokens[token_count++] = token;
			token = strtok(NULL, " ,\t");
		}
		instr_def_t *instr = find_instr(tokens[0]); 
		if (instr->format == I_TYPE) {
			break; 
		}
		if (!instr) {
			printf("unknown instruction '%s'\n", tokens[0]); 
			fclose(fp_output); 
			return -1; 
		}
		
		uint16_t machine_code = 0; 
		if (instr->format == A_TYPE) {
			//assemble a type
			int rw = get_reg_num(tokens[1]);
			int ra = get_reg_num(tokens[2]);  
			int rb = get_reg_num(tokens[3]);
			machine_code = assemble_a(instr, rw, ra, rb); 
			//printf("%d\n", (int)machine_code); 
			//printf("binary: %s\n", convert_to_bin((int)(machine_code)));
		}
		else if (instr->format == B_TYPE){
			//assemble b type
			int rw = get_reg_num(tokens[1]); 
			int imm8 = get_imm(tokens[2]); 
			machine_code = assemble_b(instr, rw, imm8); 
			//printf("%d\n", (int)machine_code); 
		}
		else if (instr->format == C_TYPE) {
			//assemble c type
			int rw = get_reg_num(tokens[1]);
			int rb = get_reg_num(tokens[2]); 
			int imm4 = get_imm(tokens[3]);
			machine_code = assemble_c(instr, rw, imm4, rb); 
			//printf("%d\n", (int)machine_code);
		}
		else if (instr->format == D_TYPE) {
			//assemble d type
			int ra = get_reg_num(tokens[1]);
			int rb = get_reg_num(tokens[2]);
			int imm4 = get_imm(tokens[3]);
			machine_code = assemble_d(instr, imm4, ra, rb); 
			//printf("%d\n", (int)machine_code);

		}
		else if (instr->format == E_TYPE) {
			//assemble e type
			int imm8 = get_imm(tokens[1]);
			machine_code = assemble_e(instr, imm8);
			//printf("%d\n", (int)machine_code);
		}
		else if (instr->format == F_TYPE) {
			//assemble f type
			int rb = get_reg_num(tokens[1]);
			int imm8 = get_imm(tokens[2]);
			machine_code = assemble_f(instr, imm8, rb);
			//printf("%d\n", (int)machine_code);
		}
		else if (instr->format == G_TYPE) {
			//assemble g type
			int imm12 = get_imm(tokens[1]);
			machine_code = assemble_g(instr, imm12);
			//printf("%d\n", (int)machine_code);
		}
		else if (instr->format == H_TYPE) {
			//assemble h type
			//jr ra
			int ra = get_reg_num(tokens[1]); 
			machine_code = assemble_h(instr, ra);
			//printf("%d\n", (int)machine_code);
		}
		else if (instr->format == I_TYPE) {
			//assemble i type
			machine_code = assemble_i(instr);
			//printf("%d\n", (int)machine_code);
		}
		int integer_machine = (int)machine_code; 
		fwrite(&integer_machine, sizeof(int),1,fp_output); //fwrite to write binary 
	} 
	printf("hello world\n"); 
}
/*
//func to convert decimal machine code to binary
char *convert_to_bin(uint16_t decimal) {
	int bits = 16; 
	char temp[bits];
	int index = 0;
	//divide by 2 until quotient is 0
	while (decimal > 0 && index < 16) {
		int remainder = decimal % 2;
		temp[index++] = remainder + '0'; //will print 0 if r is 0 and 1 if r is 1
		decimal = decimal / 2; 
	}
	while (index < bits) {
		temp[index++] = '0'; //leading 0s
	}
	//reverse into right order
	char *bin = malloc(bits + 1); // + 1 for null
	for (int i = 0; i < bits; i++) {
		bin[i] = temp[15-i]; 
	}
	bin[bits] = '\0'; 

	return bin; 
}
*/
