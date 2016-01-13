////////////////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>

#include "pgm.h"
#include "papi.h"

#define MAX_SOURCE_SIZE (0x100000)

int main(int argc, char** argv)
{
	long long ptimer1=0;
	long long ptimer2=0;

	int ipgm_img_width = 0;
	int ipgm_img_height = 0;
	register int i = 0;
	register int j = 0;
	register int r = 0;
	register int c = 0;
	pgm_t ipgm;
	pgm_t opgm;


	/* Image file input */
	readPGM(&ipgm, "lena.pgm");

	ipgm_img_width = ipgm.width;
	ipgm_img_height = ipgm.height;
	printf("c:main program:log img_width is %d \n", ipgm_img_width);

	//Allocate host memory for matrices A and B
	unsigned int size_A = ipgm_img_width*ipgm_img_height;
	unsigned int mem_size_A = sizeof(float) * size_A;
	float* h_A = (float*)malloc(mem_size_A);
	for (i = 0; i < ipgm_img_width; i++) {
		for (j = 0; j < ipgm_img_height; j++) {
			((float*)h_A)[(ipgm_img_width*j) + i] = (float)ipgm.buf[ipgm_img_width*j + i];
		}
	}

	unsigned int size_B = 9;
	unsigned int mem_size_B = sizeof(float) * size_B;
	float* h_B = (float*)malloc(mem_size_B);

	/*HPF*/
	/*
	h_B[0] = -1;
	h_B[1] = -1;
	h_B[2] = -1;
	h_B[3] = -1;
	h_B[4] = 0;
	h_B[5] = -1;
	h_B[6] = -1;
	h_B[7] = -1;
	h_B[8] = -1;
	*/

	/*Laplacian*/
	/*
	h_B[0] = -1;
	h_B[1] = 2;
	h_B[2] = -1;
	h_B[3] = 2;
	h_B[4] = -4;
	h_B[5] = 2;
	h_B[6] = -1;
	h_B[7] = 2;
	h_B[8] = -1;
	*/

	/*Gaussian blur*/
	/*
	h_B[0] = 1;
	h_B[1] = 1;
	h_B[2] = 1;
	h_B[3] = 1;
	h_B[4] = 1;
	h_B[5] = 1;
	h_B[6] = 1;
	h_B[7] = 1;
	h_B[8] = 1;
	*/

	/*SOBEL horizontal edge detection*/   //OK - output is grey edge on black
	/*
	h_B[0] = 1;
	h_B[1] = 2;
	h_B[2] = 1;
	h_B[3] = 0;
	h_B[4] = 0;
	h_B[5] = 0;
	h_B[6] = -1;
	h_B[7] = -2;
	h_B[8] = -1;
	*/

	/*SOBEL vertical edge detection*/ //detecting but output is not white edges on black image
	/*
	h_B[0] = -1;
	h_B[1] = 0;
	h_B[2] = 1;
	h_B[3] = -2;
	h_B[4] = 0;
	h_B[5] = 2;
	h_B[6] = -1;
	h_B[7] = 0;
	h_B[8] = 1;
	*/

	/*PREWITT horizontal edge detection*/
	/*
	h_B[0] = 1;
	h_B[1] = 1;
	h_B[2] = 1;
	h_B[3] = 0;
	h_B[4] = 0;
	h_B[5] = 0;
	h_B[6] = -1;
	h_B[7] = -1;
	h_B[8] = -1;
	*/

	/*PREWITT vertical edge detection mask*/
	/*
	h_B[0] = -1;
	h_B[1] = 0;
	h_B[2] = 1;
	h_B[3] = -1;
	h_B[4] = 0;
	h_B[5] = 1;
	h_B[6] = -1;
	h_B[7] = 0;
	h_B[8] = 1;
	*/

	//Allocate host memory for the result C
	unsigned int size_C = ipgm_img_width * ipgm_img_width;
	unsigned int mem_size_C = sizeof(float) * size_C;
	float* h_C = (float*)malloc(mem_size_C);

	printf("c:main program:log Running_gaussblur\n");
	
	int x, y, sum = 0;

	ptimer1 = PAPI_get_virt_usec();

	for (x = 0; x < ipgm_img_width; x++)
	{
		for (y = 0; y < ipgm_img_height; y++)
		{
			sum = 0;
			for (r = 0; r < 3; r++)
			{
				int tmp = (y + r) * ipgm_img_width + x;
				for (c = 0; c < 3; c++)
				{
					sum += (((float*)h_B)[(r * 3) + c]) * (((float*)h_A)[tmp + c]);	//h_b -> mask, h_A -> input image
				}
			}

			(((float*)h_C)[(y * ipgm_img_width) + x]) = sum;
		}
	}
	ptimer2 = PAPI_get_virt_usec();
	printf("c:main timing:PAPI logic %llu us \n", (ptimer2-ptimer1));


	opgm.width = ipgm_img_width;
	opgm.height = ipgm_img_height;
	normalizeF2PGM(&opgm, h_C);

	/* Image file output */
	writePGM(&opgm, "output.pgm");

	printf("c:main program:log done\n");

	destroyPGM(&ipgm);
	destroyPGM(&opgm);

	free(h_A);
	free(h_B);
	free(h_C);

	return 0;
}
	
