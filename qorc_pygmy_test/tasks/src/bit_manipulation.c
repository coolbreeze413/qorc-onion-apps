#include <stdint.h>
#include <stdio.h>

#include "bit_manipulation.h"


// cast to (uint8_t) if > 0
int16_t read_bitfield_from_byte(uint8_t byte_val, uint8_t low_bit, uint8_t high_bit)
{  
    uint8_t shift = low_bit;
    uint8_t mask = ((1 << (high_bit - low_bit + 1)) - 1) << shift;
    uint8_t bitfield_val = 0x00;
  
    bitfield_val = read_bitfield_from_byte_m(byte_val, mask, shift);  
  
    return bitfield_val;
}


int16_t read_bitfield_from_byte_m(uint8_t byte_val, uint8_t mask, uint8_t shift)
{  
    uint8_t bitfield_val = 0x00;
  
    bitfield_val = ((byte_val & mask) >> shift);
  
    return bitfield_val;    
}


// cast to (uint8_t) if > 0
int16_t write_bitfield_to_byte(uint8_t byte_val, uint8_t low_bit, uint8_t high_bit, uint8_t bitfield_val)
{
    uint8_t shift = low_bit;
    uint8_t mask = ((1 << (high_bit - low_bit + 1)) - 1) << shift;    
    
    byte_val = write_bitfield_to_byte_m(byte_val, mask, shift, bitfield_val);
    
    return byte_val;  
}


int16_t write_bitfield_to_byte_m(uint8_t byte_val, uint8_t mask, uint8_t shift, uint8_t bitfield_val)
{  
    byte_val = (byte_val & ~mask) | ((bitfield_val << shift) & mask);
    return byte_val;
}


int32_t twosComplement(int32_t val, uint8_t bits) 
{
    if (val & ((uint32_t)1 << (bits - 1))) 
    {
        val -= (uint32_t)1 << bits;
    }
    return val;
}