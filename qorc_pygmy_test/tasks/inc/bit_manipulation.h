#ifndef __BIT_MANIPULATION_H__
#define __BIT_MANIPULATION_H__

#include <stdint.h>

int16_t read_bitfield_from_byte(uint8_t byte_val, uint8_t low_bit, uint8_t high_bit);
int16_t read_bitfield_from_byte_m(uint8_t byte_val, uint8_t mask, uint8_t shift);

int16_t write_bitfield_to_byte(uint8_t byte_val, uint8_t low_bit, uint8_t high_bit, uint8_t bitfield_val);
int16_t write_bitfield_to_byte_m(uint8_t byte_val, uint8_t mask, uint8_t shift, uint8_t bitfield_val);

int32_t twosComplement(int32_t val, uint8_t bits);


#endif // #ifndef __BIT_MANIPULATION_H__
