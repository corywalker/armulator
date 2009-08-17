#include "arm940t.h"

instruction parse_instruction(arm940t * cpu, uint32_t word)
{
	instruction inst;
	inst.word = word;
	inst.cond = word >> 28;

	uint32_t word_adj = word;
    if(inst.cond!=0xF) word_adj&=0x0FFFFFFF;         //Condition is already accounted for
    //printf("%8.8X\n",instruction&0xFE000000);
         if(word_adj==0x0AFFFFFE) printf("HALT");
    else if(word_adj==0x01A00000) printf("NOP");
    //else if((word_adj&0xFE000090)==0x00000090) //arm_mels(word_adj&~(0xFE000090));           //Multiplies, extra loads/stores
    //else if((word_adj&0xFF900000)==0x01000000) //  arm_mi(word_adj&~(0xFF900000));           //Miscellaneous instructions
    else if(((word_adj&0xFE000010)==0x00000000)||((word_adj&0xFE000090)==0x00000010)||((word_adj&0xFE000000)==0x02000000)) { //arm_dpis(word_adj&~(0xFE000010));           //Data Processing immediate shift
		inst.group = 0;
		inst.char3 = get_bits(word, 21, 4); //opcode
		inst.char0 = get_bits(word, 20, 1); //S
		inst.char1 = get_bits(word, 16, 4); //SBZ
		inst.char2 = get_bits(word, 12, 4); //Rd
		inst.short0 = get_bits(word, 0, 12); //shifter_operand
		print_instruction(inst);
    }
    //else if((word_adj&0xFF980000)==0x03200000) //arm_misr(word_adj&~(0xFF980000));           //Move immediate to status register
    //else if((word_adj&0xFE000000)==0x04000000) //arm_lsio(word_adj&~(0xFE000000));           //Load/store immediate offset
    //else if((word_adj&0xFE000010)==0x06000000) //arm_lsro(word_adj&~(0xFE000010));           //Load/store register offset
    //else if((word_adj&0xFE000000)==0x08000000) // arm_lsm(word_adj&~(0xFE000000));           //Load/store multiple
    else if((word_adj&0xFE000000)==0x0A000000) { //   arm_b(word_adj&~(0xFE000000));           //Branch and branch with link
		inst.group = 1;
		inst.char0 = get_bits(word, 24, 1); //L
		inst.int0 = get_bits(word, 0, 24); //target_address
		print_instruction(inst);
    }
    //else if((word_adj&0xFE000000)==0xFA000000) // arm_b_t(word_adj&~(0xFE000000));           //Branch and branch with link and change to Thumb(only one that uses 0xF)
    //else if((word_adj&0xFF000000)==0x0F000000) // arm_swi(word_adj&~(0xFF000000));           //Software interrupt
    //else if((word_adj&0xFE000000)==0x0C000000) //arm_cpls(word_adj&~(0xFE000000));           //Coprocessor load/store and double register transfers
    //else if((word_adj&0xFF000000)==0x0E000000) // arm_cdp(word_adj&~(0xFF000000));           //Coprocessor data processing
    //else if((word_adj&0xFF000000)==0x0E000000) // arm_crt(word_adj&~(0xFF000000));           //Coprocessor register transfers
    else printf("Undefined instruction\n"); //instruction didn't match any previous profiles

	return inst;
}

void cycle(arm940t * cpu)
{
	if(cpu->have_decoded)
	{
		printf("   "); // tab for debug messages
		if(condition(cpu, cpu->decoded.cond)) // check if condition is met
		{
			uint32_t oldpc = cpu->R[15];
			switch (cpu->decoded.group)
			{
				case 0:
					process_opcode(cpu, cpu->decoded.char3, cpu->decoded.char2,
						cpu->decoded.char1, addrmode1(cpu, cpu->decoded.word), cpu->decoded.char0);
					break;
				case 1:
					if(get_bits(cpu->decoded.int0, 23, 1))
						cpu->decoded.int0 += 1056964608;
					cpu->decoded.int0 <<= 2;
					cpu->R[15] += cpu->decoded.int0;
					printf("jumping to %u\n", cpu->R[15]);
					break;
				default:
					printf("Behavior not defined\n");
					break;
			}
			if(oldpc != cpu->R[15])
			{
				cpu->have_decoded = 0;
				cpu->have_fetched = 0;
			}
		}
		else
			printf("Condition not met\n");
	}
	if(cpu->have_fetched)
	{
		cpu->decoded = parse_instruction(cpu, cpu->fetched);
		cpu->have_decoded = 1;
	}
	if(cpu->R[15] < cpu->binarysz)
	{
		cpu->fetched = get_be_word(cpu, cpu->R[15]);
		cpu->have_fetched = 1;
	}
	else
		cpu->have_fetched = 0;
	cpu->R[15] += 4;
}

void print_operation(char line[], uint8_t cond, uint8_t sbit)
{
	printf("%s", line);
	const char arm_cond[16][3]={"eq","ne","hs","lo","mi","pl","vs","vc","hi",
			"ls","ge","lt","gt","le","",""};
	printf("%s", arm_cond[cond]);
	if(sbit)
		printf("s");
}
uint8_t condition(arm940t * cpu, uint8_t cond)
{
	switch(cond)
	{
	case 0:
		return get_z(&cpu->CPSR);
	case 1:
		return !get_z(&cpu->CPSR);
	case 2:
		return get_c(&cpu->CPSR);
	case 3:
		return !get_c(&cpu->CPSR);
	case 4:
		return get_n(&cpu->CPSR);
	case 5:
		return !get_n(&cpu->CPSR);
	case 6:
		return get_v(&cpu->CPSR);
	case 7:
		return !get_v(&cpu->CPSR);
	case 8:
		return get_c(&cpu->CPSR) && !get_v(&cpu->CPSR);
	case 9:
		return !get_c(&cpu->CPSR) || get_v(&cpu->CPSR);
	case 10:
		return get_n(&cpu->CPSR) == get_v(&cpu->CPSR);
	case 11:
		return get_n(&cpu->CPSR) != get_v(&cpu->CPSR);
	case 12:
		return (!get_z(&cpu->CPSR)) && (get_n(&cpu->CPSR) == get_v(&cpu->CPSR));
	case 13:
		return (get_z(&cpu->CPSR)) && (get_n(&cpu->CPSR) != get_v(&cpu->CPSR));
	case 14:
		return 1;
	case 15:// unconditional expression
		return 1;
	default:
		return 0;
	}
}

uint32_t get_be_word(arm940t * cpu, uint32_t address)
{
	uint32_t word = 0;
	if(address <= cpu->binarysz-4)
	{
		word += cpu->binary[address];
		word += cpu->binary[address+1] << 8;
		word += cpu->binary[address+2] << 16;
		word += cpu->binary[address+3] << 24;
	}
	else
		printf("Address out of range");
	return word;
}

arm940t new_arm940t()
{
	arm940t ret;
	ret.NUM_REGS = 16;
	// clear regs;
	int i;
	for(i = 0; i < ret.NUM_REGS; i++)
		ret.R[i] = 0;
	ret.have_fetched = 0;
	ret.have_decoded = 0;
	return ret;
}

void delete_arm940t(arm940t * cpu)
{
	free(cpu->binary);
}

void process_opcode(arm940t * cpu, uint8_t opcode, uint8_t rd, uint8_t rn, shifter_result shifted, uint8_t s)
{
	uint8_t td;

	//printf("opcode: %u - rd: %u - rn: %u - s: %u", opcode, rd, rn, s);
	//this sh needs to handle flags
	switch (opcode) {
	case 0: //AND
		cpu->R[rd] = cpu->R[rn] & shifted.shifter_operand;
		break;
	case 1: //EOR
		cpu->R[rd] = cpu->R[rn] ^ shifted.shifter_operand;
		break;
	case 2: //SUB
		if (s)
			set_c(&cpu->CPSR, cpu->R[rn] >= shifted.shifter_operand); //rd can equal rn dipsh
		cpu->R[rd] = cpu->R[rn] - shifted.shifter_operand;
		break;
	case 3: //RSB
		cpu->R[rd] = shifted.shifter_operand - cpu->R[rn];
		break;
	case 4: //ADD
		cpu->R[rd] = cpu->R[rn] + shifted.shifter_operand;
		printf("R%u incremented to %u\n", rn, cpu->R[rn]);
		break;
	case 5: //ADC
		cpu->R[rd] = cpu->R[rn] + shifted.shifter_operand + get_c(&cpu->CPSR);
		break;
	case 6: //SBC
		cpu->R[rd] = cpu->R[rn] - shifted.shifter_operand - (!get_c(&cpu->CPSR));
		break;
	case 7: //RSC
		cpu->R[rd] = shifted.shifter_operand - cpu->R[rn] - (!get_c(&cpu->CPSR));
		break;
	case 8: //TST
		td = cpu->R[rn] & shifted.shifter_operand;
		break;
	case 9: //TEQ
		td = cpu->R[rn] ^ shifted.shifter_operand;
		break;
	case 10: //CMP
		td = cpu->R[rn] - shifted.shifter_operand;
		break;
	case 11: //CMN
		td = cpu->R[rn] + shifted.shifter_operand;
		break;
	case 12: //ORR
		cpu->R[rd] = shifted.shifter_operand | cpu->R[rn];
		break;
	case 13: //MOV
		cpu->R[rd] = shifted.shifter_operand;
		set_c(&cpu->CPSR, shifted.shifter_carry_out);
		printf("R%u = %u\n", rd, cpu->R[rd]);
		break;
	case 14: //BIC
		cpu->R[rd] = cpu->R[rn] & (~shifted.shifter_operand);
		break;
	case 15: //MVN
		cpu->R[rd] = ~shifted.shifter_operand;
		break;
	}
	if (opcode >= 8 && opcode <= 11) { //the test instructions
		//opcode agnostic functions
		set_n(&cpu->CPSR, td >> 31 == 1);
		set_z(&cpu->CPSR, td != 0);
		//opcode specific functions
		if(opcode == 8 || opcode == 9)
			set_c(&cpu->CPSR, shifted.shifter_carry_out);
		if(opcode == 10)
		{
			set_c(&cpu->CPSR, !is_underflow(cpu->R[rn], shifted.shifter_operand));
			set_v(&cpu->CPSR, is_signed_overflow(cpu->R[rn], 1, shifted.shifter_operand));
		}
		if(opcode == 11)
		{
			set_c(&cpu->CPSR, !is_overflow(cpu->R[rn], shifted.shifter_operand));
			set_v(&cpu->CPSR, is_signed_overflow(cpu->R[rn], 0, shifted.shifter_operand));
		}
		printf("n=%u z=%u c=%u v=%u\n", get_n(&cpu->CPSR), get_z(&cpu->CPSR), get_c(&cpu->CPSR), get_v(&cpu->CPSR));
		return; // do not execute the s updates
	}
	if (s) {
		set_n(&cpu->CPSR, cpu->R[rd] >> 31 == 1); // todo: look up how s updates differ between opcodes
		set_z(&cpu->CPSR, cpu->R[rd] == 0); // todo: add s update debug printf
	}
	//sprintf(cr, "%s=0x%X", arm_reg[rd], cpu->R[rd]);
	//if(rd==15) PC+=4;    //da ARM
}

void print_instruction(instruction inst)
{
	switch (inst.group) {
	case 0: { //data processing
		switch (inst.char3) {//opcode
		case 13: //AND
			print_operation("mov", inst.cond, inst.char0);//todo rename
			printf(" R%i, %u\n", inst.char2, inst.short0);
			break;
		case 4:
			print_operation("add", inst.cond, inst.char0);
			printf(" R%i, R%i, %u\n", inst.char2, inst.char1, inst.short0);
			break;
		case 10:
			print_operation("cmp", inst.cond, 0);
			printf(" R%i, %u\n", inst.char0, inst.short0);
			break;
		default:
			printf("Printing not defined for this opcode");
		}
		break;
	}
	case 1:
		print_operation("b", inst.cond, inst.char0);
		printf(" %u\n", inst.int0);
		break;
	default:
		printf("Printing not defined for this group");
	}
}
