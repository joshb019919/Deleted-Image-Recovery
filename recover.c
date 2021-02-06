#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
    // Make sure 2 args.
    if (argc != 2)
    {
        printf("Usage: ./recover image\n");
        return 1;
    }

    // If two args, open image.
    FILE *image = fopen(argv[1], "r");

    // Check image exists.
    if (image == NULL)
    {
        fclose(image);
        printf("File not found\n");
        return 1;
    }

    // Space for buffer.
    int block_size = 512;
    char buffer[block_size];

    // Get length of image.
    fseek(image, 0, SEEK_END);
    int imagelen = ftell(image);
    rewind(image);

    // Assign number of blocks in image.
    int blocks = imagelen / block_size;

    // Set up JPEG.
    FILE *JPEG = NULL;
    char *filename = (char *) malloc(8);
    int jpegs = 0;
    bool first = false;

    // Loop over each block in image.
    for (int i = 0; i < blocks; i++)
    {
        // Read blocks into buffer.
        fread(&buffer, sizeof(char), block_size, image);

        // Test for JPEG header.
        if ((int) buffer[0] == 0xffffffff &&
            (int) buffer[1] == 0xffffffd8 &&
            (int) buffer[2] == 0xffffffff &&
            ((int) buffer[3] & 0xfffffff0) == 0xffffffe0)
        {
            // If first has been found, close JPEG for reopening.
            if (first)
            {
                fclose(JPEG);
            }
            else
            {
                // Mark first as found.
                first = true;
            }

            // Create filename and open file for new JPEG.
            sprintf(filename, "%03i.jpg", jpegs++);
            JPEG = fopen(filename, "w");
        }

        // If JPEG already opened, write image from buffer.
        if (JPEG != NULL)
        {
            fwrite(&buffer, sizeof(char), block_size, JPEG);
        }
    }

    // Tidy up.
    fclose(JPEG);
    fclose(image);
    free(filename);
}