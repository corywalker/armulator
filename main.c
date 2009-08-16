#include "main.h"

int main()
{
	arm940t cpu = new_arm940t();
	read_binary(&cpu, "bin/test.bi");
	debug(&cpu);
	delete_arm940t(&cpu);
	return 0;
}

// Reads a binary file into the processor
void read_binary(arm940t * cpu, char file[])
{
	FILE*fp ;
	fp = fopen ( file,"r" ) ;
	char error[] = "Error opening ";
	ASSERT(fp, strcat(error, file)); //todo: this needs some work
	// Get binary size
	fseek(fp, 0L, SEEK_END);
	cpu->binarysz = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	// Allocate binary space and load it
	cpu->binary = (uint8_t*) malloc (cpu->binarysz+1);
	fgets((char*)cpu->binary, cpu->binarysz+1, fp);
	fclose(fp);
}

// Starts the debug process
void debug(arm940t * cpu)
{
	// Cycles until the end of the program is reached
	while(cpu->R[15] < cpu->binarysz+8)
	{
		cycle(cpu);
		//for step by step:
		//getchar();
	}
	printf("Execution complete\n");
}
