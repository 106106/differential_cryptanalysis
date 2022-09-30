#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>


uint16_t s_box(uint16_t x); 
uint16_t inverse_s_box(uint16_t x);
uint16_t perm(uint16_t x); 
uint16_t mix_key(uint16_t x, uint16_t key);
uint16_t encrypt(uint16_t x, const uint16_t * keys);
uint16_t decrypt(uint16_t x, const uint16_t * keys);


//implements the sbox stage
//INPUT: 16-bit unsigned int
//OUTPUT: 16-bit unsigned int
uint16_t s_box(uint16_t x)
{
	//4-bit sbox
    uint16_t sbox[] = {0xE,0x4,0xD,0x1,0x2,0xF,0xB,0x8,0x3,0xA,0x6,0xC,0x5,0x9,0x0,0x7};

	//breaks the 16-bit input into nibbles, runs each nibble through the sbox, and returns the concatonated nibbles (i.e. 16-bits) 
    x = sbox[(x & 0x000F)]
        | (sbox[((x & 0x00F0) >> 4)] << 4)
        | (sbox[((x & 0x0F00) >> 8)] << 8)
        | (sbox[((x & 0xF000) >> 12)] << 12);

    return x;
}


//implements the inverse sbox stage
//INPUT: 16-bit unsigned int
//OUTPUT: 16-bit unsigned int
uint16_t inverse_s_box(uint16_t x)
{
	//4-bit inverse sbox
	uint16_t inverse_sbox[] = {0xE,0x3,0x4,0x8,0x1,0xC,0xA,0xF,0x7,0xD,0x9,0x6,0xB,0x2,0x0,0x5};

	//breaks the 16-bit input into nibbles, runs each nibble through the inverse sbox, and returns the concatonated nibbles (i.e. 16-bits)
	x = inverse_sbox[(x & 0x000F)]
		| (inverse_sbox[((x & 0x00F0) >> 4)] << 4)
		| (inverse_sbox[((x & 0x0F00) >> 8)] << 8)
		| (inverse_sbox[((x & 0xF000) >> 12)] << 12);

	return x;
}


//implements the permutation stage
//INPUT: 16-bit unsigned int
//OUTPUT: 16-bit unsigned int
uint16_t perm(uint16_t x)
{
	//output i of sbox j is connected to input j of sbox i.
    x = (x & 0x8421)
        | ((x & 0x0842) << 3)
        | ((x & 0x0084) << 6)
        | ((x & 0x0008) << 9)
        | ((x & 0x1000) >> 9)
        | ((x & 0x2100) >> 6)
        | ((x & 0x4210) >> 3);

    return x;
}


//implements the xor key mixing
//INPUT: 16-bit unsigned int and a 16-bit unsigned key
//OUTPUT: 16-bit unsigned int
uint16_t mix_key(uint16_t x, uint16_t key)
{
    return (x ^ key);
}


//implements the toy cipher encryption
//INPUT: 16-bit unsigned plaintext and a pointer to an array of 5 16-bit unsigned keys
//OUTPUT: 16-bit unsigned ciphertext
uint16_t encrypt(uint16_t x, const uint16_t * keys)
{
    int i;  
 
    for(i=0;i<3;i++)
    {   
        x = mix_key(x, keys[i]);
        x = s_box(x);
        x = perm(x);
    }    
    x = mix_key(x, keys[3]);
    x = s_box(x);
    x = mix_key(x, keys[4]);

    return x;
}


//implements the toy cipher decryption
//INPUT: 16-bit unsigned ciphertext and a pointer to an array of 5 16-bit unsigned keys
//OUTPUT: 16-bit unsigned plaintext
uint16_t decrypt(uint16_t x, const uint16_t * keys)
{
	int i;
	
	x = mix_key(x, keys[4]);
	x = inverse_s_box(x);
	x = mix_key(x, keys[3]);
	for(i=2;i>=0;i--)
	{
		x = perm(x);				//note perm is an involution (i.e. it own inverse)
		x = inverse_s_box(x);
		x = mix_key(x, keys[i]);
	}

	return x;
}
