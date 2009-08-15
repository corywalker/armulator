#include "util.h"

uint32_t get_bits(uint32_t number, uint8_t start, uint8_t length)
{
	return (number >> start) % (1 << length);
}

uint32_t rol(uint32_t value, uint32_t shift) {
    shift &= 31;
    return (value << shift) | (value >> (32 - shift));
}

uint32_t ror(uint32_t value, uint32_t shift) {
    shift &= 31;
    return (value >> shift) | (value << (32 - shift));
}

uint32_t asr(int32_t value, uint32_t shift) //this might screw up on some compilers
{
	return value >> shift; //todo: define custom function so not implementation specific
}

uint8_t is_overflow(uint32_t num1, uint32_t num2)
{
	return num1 + num2 < num1;
}

uint8_t is_underflow(uint32_t num1, uint32_t num2)
{
	return num1 - num2 > num1;
}

uint8_t is_signed_overflow(int32_t num1, uint8_t operation, int32_t num2)
{
	int32_t ans32;
	int64_t ans64;
	if(operation) {
		ans32 = num1 - num2;
		ans64 = num1 - num2;
	}
	else {
		ans32 = num1 + num2;
		ans64 = num1 + num2;
	}
	return !(ans32 == ans64);
}
