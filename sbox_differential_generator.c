/* Generates the differential statistics for the sbox
 * 
 */

#include "crypto.h" 

int main(void)
{
	uint16_t sbox[] = {0xE,0x4,0xD,0x1,0x2,0xF,0xB,0x8,0x3,0xA,0x6,0xC,0x5,0x9,0x0,0x7};
	static int diff_table[16][16];
	unsigned int i,j;
	
	//generates the s-box differential characterists
	//rows are input differential and columns are output differential
	//a cell (row/column pair) get incremented to keep track of the differentials
	for(i=0;i<16;i++)
		for(j=0;j<16;j++)
			diff_table[i^j][sbox[i]^sbox[j]]++;
	
	//print out the table to display results	
	printf("    ");
	for(i=0;i<16;i++)
		printf("%.2d  ",i);
	puts("\n");

	for(i=0;i<16;i++)
	{
		printf("%.2d  ",i); 
		for(j=0;j<16;j++)
			{
				printf("%.2d  ",diff_table[i][j]);
			}
		puts("\n");
	}
	
	return 0;
}
