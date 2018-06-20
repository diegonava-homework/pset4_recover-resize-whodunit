/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Resizes bmp by a scale of n from infile to outfile
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{

// ensure the  proper usage
    if (argc != 4)
    {
        printf("Usage: ./copy infile outfile\n");
        return 1;
    }


// remember  the filenames and scale size
    int scale_size = atoi(argv[1]);
    
    if (scale_size < 1 || scale_size > 100)
    {
        printf("Scale must be between 1 and 100\n");
        return 1;
    }
    char* infile = argv[2];
    char* outfile = argv[3];


// open the  input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }


// open  the output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }


// read  the infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);


// read the  infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    

// ensure  the infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    

// create the outfile's BITMAPFILEHEADER and BITMAPINFOHEADER
    BITMAPFILEHEADER out_bf = bf;
    BITMAPINFOHEADER out_bi = bi;
    

// rescale new dimension for the outfile
    out_bi.biWidth *= scale_size;
    out_bi.biHeight *= scale_size;


// determine  the padding for scanlines of both infile and outfile
    int in_padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int out_padding = (4 - (out_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    

// determine outfile biSizeImage and bfSize (biSizeImage + 54 bytes header files)
    out_bi.biSizeImage = abs(out_bi.biHeight) * (out_bi.biWidth * sizeof(RGBTRIPLE) + out_padding);
    out_bf.bfSize = (out_bi.biSizeImage + 54);
    

// write  the outfile's BITMAPFILEHEADER
    fwrite(&out_bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    
// write the outfile's BITMAPINFOHEADER
    fwrite(&out_bi, sizeof(BITMAPINFOHEADER), 1, outptr);


// iterate over the infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        for (int j = 0; j < scale_size; j++)
        {


// set pointer to the  beginning of line
            fseek(inptr, 54 + (bi.biWidth * 3 + in_padding) * i, SEEK_SET);
            


// iterate over  thepixels in scanline
            for (int k = 0; k < bi.biWidth; k++)
            {


// the temporary storage
                RGBTRIPLE triple;
        


// read the  RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                

// iterate  in each pixel n times
                for (int l = 0; l < scale_size; l++)
                {
                    // write RGB triple to outfile
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }
            


// then add it back to the outfile
            for (int y = 0; y < out_padding; y++)
            {
                fputc(0x00, outptr);
            }
        }
    }


// close  the infile
    fclose(inptr);


// close the outfile
    fclose(outptr);


// good!
    return 0;
}