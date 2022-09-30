/*  Edit line 22 to select your secret keys
 *	Then just compile and run (note: this was tested using Apple clang version 11.0.3)
 *
 *	
 *
 */

#include "crypto.h"
uint16_t oracle(uint16_t p);
uint16_t max(int * frequency_array);





//implements a chosen plaintext oracle
//INPUT: plaintext
//OUTPUT: ciphertext
uint16_t oracle(uint16_t p)
{
	//The 'secret' keys.
	const uint16_t keys[5] = {1,2,3,3,0x1234};		// CHOOSE YOUR 'secret' KEYS $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


	return encrypt(p, keys); 
}










//This is the differential attack. It is broken into two parts.
//Let the last round key k5 = xxxx xxxx xxxx xxxx
//The first part of the differential attack recovers the question marks (xxxx ???? xxxx ????) potion of the key
//The second part of the differential attack recovers the question marks (???? xxxx ???? xxxx) potion of the key
//
//Puttting these two together we are able to recover the whole of the last round key.

int main(void)
{
	uint16_t p1,p2,c1,c2;
	uint16_t partial_subkey,y1,y2,i,j;
	uint16_t max1,max2,recovered_key,recovered_subkey_part1,recovered_subkey_part2;


	//PART 1 ###################################################################################################
	//this is the first differential, used to recover the question marks in the last subkey (xxxx ???? xxxx ????) 
	static int subkey_frequency_1[256];
	const uint16_t delta_p1 = 0x0B00;
	const uint16_t delta_c1 = 0x0606;
	for(i=0;i<5000;i++)
	{
		//create plaintext pairs that satisfy the differential delta_p
		p1 = (uint16_t) rand();
		p2 = p1 ^ delta_p1;
	
		//get ciphertext pair from the oracle
		c1 = oracle(p1);
		c2 = oracle(p2);


		for(j=0;j<256;j++)
		{
			//creates the partial subkey to test
			partial_subkey = (0x000F&j) ^ ((0x00F0&j)<<4);

			//partially decrypt ciphertext c1 
			y1 = mix_key(c1, partial_subkey);
			y1 = inverse_s_box(y1);
			//partially decrypt ciphertext c2
			y2 = mix_key(c2, partial_subkey);
			y2 = inverse_s_box(y2);

			//if the differential of c1 and c2 matches then, the likelihood of this partial_subkey gets incremented by 1
			if((y1^y2) == delta_c1)
			{
				subkey_frequency_1[j]++;
			}
		}
	}


	//PART 2 ###################################################################################################
	//this is the second differential, used to recover the question marks in the last subkey (???? xxxx ???? xxxx) 
    static int subkey_frequency_2[256];
    const uint16_t delta_p2 = 0x0500;
    const uint16_t delta_c2 = 0x6060;
    for(i=0;i<10000;i++)
    {   
        //create plaintext pairs that satisfy the differential delta_p
        p1 = (uint16_t) rand();
        p2 = p1 ^ delta_p2;
    
        //get ciphertext pair from the oracle
        c1 = oracle(p1);
        c2 = oracle(p2);


        for(j=0;j<256;j++)
        {   
            //creates the partial subkey to test
            partial_subkey = ((j&0x000F)<<4) ^ ((j&0x00F0)<<8);

            //partially decrypt ciphertext c1 
            y1 = mix_key(c1, partial_subkey);
            y1 = inverse_s_box(y1);
            //partially decrypt ciphertext c2
            y2 = mix_key(c2, partial_subkey);
            y2 = inverse_s_box(y2);

            //if the differential of c1 and c2 matches then, the likelihood of this partial_subkey gets incremented by 1
            if((y1^y2) == delta_c2)
            {   
                subkey_frequency_2[j]++;
            }   
        }   
    }   	

	//recover the last round key from the subkey_frequency array and format it for output
	max1 = max(subkey_frequency_1);
	max2 = max(subkey_frequency_2);
	recovered_subkey_part1 = ((max1 & 0xF0) << 4) ^ (max1 & 0x0F);
	recovered_subkey_part2 = ((max2 & 0xF0) << 8) ^ ((max2 & 0x0F) << 4);
	recovered_key = recovered_subkey_part1 ^ recovered_subkey_part2;

	printf("After applying differential analysis to the cipher with the supplied keys\n");
	printf("The last round key is: %d or %x in hex\n", recovered_key, recovered_key);

	return 0;
}


//returns the index of the most frequent subkey
//note: the index of the array represents the subkey
uint16_t max(int * frequency_array) 
{
	uint16_t i,index_of_max=0;
	int max;

	max = 0;
	for(i=0;i<256;i++)
	{
		if(frequency_array[i]>max)
		{
			max = frequency_array[i];
			index_of_max = i;
		}
	}

	return index_of_max;
}
