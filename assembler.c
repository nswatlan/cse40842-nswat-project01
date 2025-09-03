//norah swatland project 1 
//albaCore assembler 
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_LINE 100 //idk why 100 i may increase or decrease

//function prototypes
void remove_whitespace(char *s); 
int assemble(char *input_file, char *output_file); 

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
		printf("intr: %s\n", tokens[0]);
		printf("rw: %s\n", tokens[1]);
		printf("ra: %s\n", tokens[2]); 
		printf("rb: %s\n", tokens[3]);		
	} 
	

	return 0;
}


