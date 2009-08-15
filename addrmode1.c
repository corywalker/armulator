#include "addrmode1.h"

shifter_result addrmode1(arm940t * cpu, uint32_t word)
{
	shifter_result out;
	if(get_bits(word, 25, 3) == 1) //32-bit immediate
		out = adr1_imm(cpu, word);
	else if((get_bits(word, 25, 3) == 0) && (get_bits(word, 4, 1) == 0)) //immediate shifts
	{
		if((get_bits(word, 5, 2) == 0) && (get_bits(word, 7, 5) == 0))
			out = adr1_reg(cpu, word);
		else if(get_bits(word, 5, 2) == 0)
			out = adr1_lsl_imm(cpu, word);
		else if(get_bits(word, 5, 2) == 1)
			out = adr1_lsr_imm(cpu, word);
		else if(get_bits(word, 5, 2) == 2)
			out = adr1_asr_imm(cpu, word);
		else if(get_bits(word, 5, 2) == 3)
			out = adr1_ror_imm(cpu, word);
	}
	else if((get_bits(word, 25, 3) == 0) && (get_bits(word, 7, 1) == 0) && (get_bits(word, 4, 1) == 1)) //register shifts
	{
		if(get_bits(word, 5, 2) == 0)
			out = adr1_lsl_reg(cpu, word);
		else if(get_bits(word, 5, 2) == 1)
			out = adr1_lsr_reg(cpu, word);
		else if(get_bits(word, 5, 2) == 2)
			out = adr1_asr_reg(cpu, word);
		else if((get_bits(word, 5, 2) == 3) && get_bits(word, 8, 4))
			out = adr1_ror_reg(cpu, word);
		else if((get_bits(word, 5, 2) == 3) && !get_bits(word, 8, 4))
			out = adr1_rrx_reg(cpu, word);
	}
	return out;
}

shifter_result adr1_imm(arm940t * cpu, uint32_t word)
{
	shifter_result out;
	uint32_t operand = get_bits(word, 0, 12);
	uint8_t immed8 = get_bits(operand, 0, 8);
	uint8_t rotate_imm = get_bits(operand, 8, 4) << 1; // multiply by 2
	uint32_t rotated = ror(immed8, rotate_imm);
	out.shifter_operand = rotated;
	if(rotate_imm == 0)
		out.shifter_carry_out = get_c(&cpu->CPSR);
	else
		out.shifter_carry_out = rotated & (1<<31) ? 1 : 0;
	return out;
}

shifter_result adr1_reg(arm940t * cpu, uint32_t word)
{
	shifter_result out;
	out.shifter_operand = cpu->R[get_bits(word, 0, 4)];
	out.shifter_carry_out = get_c(&cpu->CPSR);
	return out;
}

shifter_result adr1_lsl_imm(arm940t * cpu, uint32_t word)
{
	shifter_result out;
	uint8_t rm = get_bits(word, 0, 4);
	uint8_t shift_imm = get_bits(word, 7, 11);
	if(shift_imm == 0) {
		out.shifter_operand = cpu->R[rm];
		out.shifter_carry_out = get_c(&cpu->CPSR);
	}
	else {
		out.shifter_operand = cpu->R[rm] << shift_imm;
		out.shifter_carry_out = get_bits(cpu->R[rm], 32-shift_imm, 1);
	}
	return out;
}

shifter_result adr1_lsl_reg(arm940t * cpu, uint32_t word)
{
	shifter_result out;
	uint8_t rm = get_bits(word, 0, 4);
	uint8_t rs = get_bits(word, 8, 4);
	uint8_t rsval = get_bits(cpu->R[rs], 0, 8);
	if(rsval == 0) {
	    out.shifter_operand = cpu->R[rm];
	    out.shifter_carry_out = get_c(&cpu->CPSR);
	}
	else if(rsval < 32) {
		out.shifter_operand = cpu->R[rm] << rsval;
	    out.shifter_carry_out = get_bits(cpu->R[rm], 32 - rsval, 1);
	}
	else if(rsval == 32){
		out.shifter_operand = 0;
	    out.shifter_carry_out = get_bits(cpu->R[rm], 0, 1);
	}
	else { /* Rs[7:0] > 32 */
		out.shifter_operand = 0;
	    out.shifter_carry_out = 0;
	}
	return out;
}

shifter_result adr1_lsr_imm(arm940t * cpu, uint32_t word)
{
	shifter_result out;
	uint8_t rm = get_bits(word, 0, 4);
	uint8_t shift_imm = get_bits(word, 7, 11);
	if(shift_imm == 0) {
		out.shifter_operand = 0;
		out.shifter_carry_out = get_bits(cpu->R[rm], 31, 1);
	}
	else {
		out.shifter_operand = cpu->R[rm] >> shift_imm;
		out.shifter_carry_out = get_bits(cpu->R[rm], shift_imm - 1, 1);
	}
	return out;
}

shifter_result adr1_lsr_reg(arm940t * cpu, uint32_t word)
{
	shifter_result out;
	uint8_t rm = get_bits(word, 0, 4);
	uint8_t rs = get_bits(word, 8, 4);
	uint8_t rsval = get_bits(cpu->R[rs], 0, 8);
	ASSERT((rm != 15) && (rs != 15), "Using R15 with this is UNPREDICTABLE");
	if(rsval == 0) {
	    out.shifter_operand = cpu->R[rm];
	    out.shifter_carry_out = get_c(&cpu->CPSR);
	}
	else if(rsval < 32) {
		out.shifter_operand = cpu->R[rm] >> rsval;
	    out.shifter_carry_out = get_bits(cpu->R[rm], rsval - 1, 1);
	}
	else if(rsval == 32){
		out.shifter_operand = 0;
	    out.shifter_carry_out = get_bits(cpu->R[rm], 31, 1);
	}
	else { /* Rs[7:0] > 32 */
		out.shifter_operand = 0;
	    out.shifter_carry_out = 0;
	}
	return out;
}

shifter_result adr1_asr_imm(arm940t * cpu, uint32_t word)
{
	shifter_result out;
	uint8_t rm = get_bits(word, 0, 4);
	uint8_t shift_imm = get_bits(word, 7, 5);
	if(shift_imm == 0) {
	    if(get_bits(cpu->R[rm], 31, 1) == 0) {
	        out.shifter_operand = 0;
	        out.shifter_carry_out = get_bits(cpu->R[rm], 31, 1);
	    }
	    else { /* Rm[31] == 1 */
	        out.shifter_operand = 0xFFFFFFFF;
	        out.shifter_carry_out = get_bits(cpu->R[rm], 31, 1);
	    }
	}
	else { /* shift_imm > 0 */
	    out.shifter_operand = asr(cpu->R[rm], shift_imm);
	    out.shifter_carry_out = get_bits(cpu->R[rm], shift_imm-1, 1);
	}
	return out;
}

shifter_result adr1_asr_reg(arm940t * cpu, uint32_t word)
{
	shifter_result out;
	uint8_t rm = get_bits(word, 0, 4);
	uint8_t rs = get_bits(word, 8, 4);
	uint8_t rsval = get_bits(cpu->R[rs], 0, 8);
	ASSERT((rm != 15) && (rs != 15), "Using R15 with this is UNPREDICTABLE");
	if(rsval == 0) {
	    out.shifter_operand = cpu->R[rm];
	    out.shifter_carry_out = get_c(&cpu->CPSR);
	}
	else if(rsval < 32) {
		out.shifter_operand = asr(cpu->R[rm], rsval);
	    out.shifter_carry_out = get_bits(cpu->R[rm], rsval - 1, 1);
	}
	else if(rsval == 32){
		out.shifter_operand = 0;
	    out.shifter_carry_out = get_bits(cpu->R[rm], 31, 1);
	}
	else { /* Rs[7:0] > 32 */
		out.shifter_operand = 0xFFFFFFFF;
	    out.shifter_carry_out = get_bits(cpu->R[rm], 31, 1);
	}
	return out;
}

shifter_result adr1_ror_imm(arm940t * cpu, uint32_t word)
{
	shifter_result out;
	uint8_t rm = get_bits(word, 0, 4);
	uint8_t shift_imm = get_bits(word, 7, 5);
	if(shift_imm == 0)
		printf("call adr1_rrx_reg() instead!\n");
	else { /* shift_imm > 0 */
		out.shifter_operand = ror(cpu->R[rm], shift_imm);
		out.shifter_carry_out = get_bits(cpu->R[rm], shift_imm-1, 1);
	}
	return out;
}

shifter_result adr1_ror_reg(arm940t * cpu, uint32_t word)
{
	shifter_result out;
	uint8_t rm = get_bits(word, 0, 4);
	uint8_t rs = get_bits(word, 8, 4);
	uint8_t rsval = get_bits(cpu->R[rs], 0, 5);
	ASSERT((rm != 15) && (rs != 15), "Using R15 with this is UNPREDICTABLE");
	if(get_bits(cpu->R[rs], 0, 8) == 0) {
	    out.shifter_operand = cpu->R[rm];
	    out.shifter_carry_out = get_c(&cpu->CPSR);
	}
	else if(rsval == 0) {
	    out.shifter_operand = cpu->R[rm];
	    out.shifter_carry_out = get_bits(cpu->R[rm], 31, 1);
	}
	else { /* Rs[4:0] > 0 */
	    out.shifter_operand = ror(cpu->R[rm], rsval);
	    out.shifter_carry_out = get_bits(cpu->R[rm], rsval-1, 1);
	}
	return out;
}

shifter_result adr1_rrx_reg(arm940t * cpu, uint32_t word)
{
	shifter_result out;
	uint8_t rm = get_bits(word, 0, 4);
	out.shifter_operand = (get_c(&cpu->CPSR) >> 31) | (cpu->R[rm] >> 1);
	out.shifter_carry_out = get_bits(cpu->R[rm], 0, 1);
	return out;
}
