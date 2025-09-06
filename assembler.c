//norah swatland project 1 
//albaCore assembler 
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#define MAX_LINE 100 //idk why 100 i may increase or decrease

//structs and enum
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


int main(int argc, char *argv[]) {	
	if (argc != 3) {
		printf("Usage: %s <input.s> <output.bin>\n", argv[0]); 
		return -1; 
	}
	assemble(argv[1],argv[2]); 

	return 0; 
}

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
	
int assemble(char *input_file, char *output_file) {
	FILE *fp_input = fopen(input_file, "r"); //file pointers for i/o
	FILE *fp_output = fopen(output_file, "wb"); 
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
		int rw = get_reg_num(tokens[1]);
		printf("%d\n", rw); 	
	} 
	

	return 0;
}

int get_reg_num(char *reg) { //func to return int version of reg 
	int reg_num = atoi(reg + 1);
	if (reg_num >=0 && reg_num <= 15) {
		return reg_num; 
	}
	return -1; //invalid reg 
}

