#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

int toint(uint8_t hex[4]);
long tolong(uint8_t hex[4]);

// each row of pixels has to be a multiple of 4 bytes. Leftovers set to 0x00
// 11 biWidth = 11 pixels means each row is 36 bytes for example

// create array with 54 bytes to store header (unsigned integer of value = 8 bits)
// the header may be in fact longer than 54 bytes (up to 138) but we just need the 3 values
uint8_t header[54];
uint8_t filesize[4];
uint8_t offset[4];
uint8_t width[4];
uint8_t height[4];
long filesize_dec;
int offset_dec;
int width_dec;
int height_dec;
int pixels_dec;
int row_padding;

struct RGB
{
  int red;
  int green;
  int blue;
  int brightness;
};

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Missing input file, usage: ./img2ascii filename output\n");
    return 1;
  }
  
  FILE *input = fopen(argv[1], "rb");
  if (input == NULL)
  {
    printf("Failed to open file.\n");
    return 2;
  }
  
  // load the header into the header array
  fread(header, 1, 54, input);
  
  memcpy(filesize, &header[2], 4);  // Copy 4 bytes starting from header[2]
  memcpy(offset, &header[10], 4);  // Copy 4 bytes starting from header[10]
  memcpy(width, &header[18], 4);   // Copy 4 bytes starting from header[18]
  memcpy(height, &header[22], 4);  // Copy 4 bytes starting from header[22]
  
  filesize_dec = tolong(filesize);
  offset_dec = toint(offset);
  width_dec = toint(width);
  height_dec = toint(height);

  pixels_dec = filesize_dec - offset_dec;
  row_padding = (4 - ((width_dec * 3) % 4) % 4); 

  /*printf("Filesize: %ld\n", filesize_dec);*/
  /*printf("Offset: %i\n", offset_dec);*/
  /*printf("Width: %i\n", width_dec);*/
  /*printf("Height: %i\n", height_dec);*/

  // array to hold data for the pixels of pixels_dec size bytes (uint8*) is a type cast
  uint8_t *pixels = (uint8_t *)malloc(pixels_dec);
  if (pixels == NULL)
  {
    printf("Error allocating pixel memory");
    fclose(input);
    return 3;
  }
 
  // seek to the point in the bmp where the pixel data starts
  fseek(input, offset_dec, SEEK_SET);

  // read the pixel data into the array
  fread(pixels, 1, pixels_dec, input);
  
  fclose(input);
  // FROM THIS POINT WE DONT NEED HEADER!
  

  // allocate memory for array of pointers
  struct RGB **rows = malloc(height_dec * sizeof(struct RGB *));
  if (rows == NULL)
  {
    printf("Error allocating memory for rows array");
    return 4;
  }
  for (int h = 0; h < height_dec; h++)
  {
    struct RGB *pixel_data = malloc(width_dec * sizeof(struct RGB));
    if (pixel_data == NULL)
    {
      printf("Error allocating memory for pixel data array");
      return 5;
    }
    rows[h] = pixel_data;
  }

  for (int h = 0; h < height_dec; h++)
  {
    for (int w = 0, b = 0, g = 1, r = 2; r < (width_dec*3); w++, b += 3, g += 3, r += 3)
    { 
      int row_height = h * (width_dec*3 + row_padding -4);
      int blue = (int)(pixels[b + row_height]);
      int green = (int)(pixels[g + row_height]);
      int red = (int)(pixels[r + row_height]);
      rows[h][w].blue = blue;
      rows[h][w].green = green;
      rows[h][w].red = red;
      rows[h][w].brightness = (red + green + blue) / 3;
    }

    // Add row to array of rows
  }
  free(pixels);

 
  // make an array of pointers (arrays) outsdide of any loop so we can free memory later
  struct RGB **rows_blur = malloc(height_dec * sizeof(struct RGB *));
  if (rows_blur == NULL)
  {
    printf("Error allocating memory to rows_new");
    return 6;
  }


  for (int h = 0; h < height_dec; h++)
  {
    struct RGB *row_blur = malloc(width_dec * sizeof(struct RGB));
    if (row_blur == NULL)
    {
      printf("Error allocating memory to row_new");
      return 7;
    }

    rows_blur[h] = row_blur;

  }
  
  for (int h = 0; h < height_dec; h++)
  {

    for (int w = 0; w < width_dec; w++)
    {

      int red_sum = 0;
      int green_sum = 0;
      int blue_sum = 0;
      int brightness_sum = 0;
      int count = 0;
      
      for (int dh = -4; dh <= 4; dh++)
      {
        for (int dw = -4; dw <= 4; dw++)
        {
          int new_h = h + dh;
          int new_w = w + dw;
          
          if ((new_h >= 0) && (new_h < height_dec) && (new_w >=0) && (new_w < width_dec))
          {
            red_sum += rows[new_h][new_w].red;
            green_sum += rows[new_h][new_w].green;
            blue_sum += rows[new_h][new_w].blue;
            brightness_sum += rows[new_h][new_w].brightness;
            count++;
          }
        }
      }
      
      rows_blur[h][w].red = round((float)red_sum / count);
      rows_blur[h][w].green = round((float)green_sum / count);
      rows_blur[h][w].blue = round((float)blue_sum / count);
      rows_blur[h][w].brightness = round((float)brightness_sum / count);
    }
  }
 
  // at this point we have a 2d array with blurred pixels, ie all pixels in 3x3 grid have same values

  int new_width = width_dec / 9;
  int new_height = height_dec / 9;
  
  struct RGB **small_rows = malloc(new_height * (sizeof(struct RGB *)));
  if (small_rows == NULL)
  {
    printf("Error allocating memory to small_rows");
    return 8;
  }
  
  for (int h = 0; h < new_height; h++)
  {
    struct RGB *small_row = malloc(new_width * (sizeof(struct RGB)));
    if (small_row == NULL)
    {
      printf("Error allocating memory to small_row");
      return 9;
    }
    small_rows[h] = small_row;
  }


  for (int h = 0; h < new_height; h++)
  {
    for (int w = 0; w < new_width; w++)
    {
      // Assign the pixel in the small_rows from the averaged 3x3 block in rows_blur
      if (h*9 < height_dec && w*9 < width_dec)
      {
        small_rows[h][w] = rows_blur[h*9][w*9];
      }
    }
  }

  
  FILE *output = fopen(argv[2], "w");
  if (output == NULL)
  {
    printf("Error saving file\n");
    return 10;
  }


  for (int h = new_height - 1; h > -1; h--)
  {
    for (int w = 0; w < new_width; w++)
    {
      if (small_rows[h][w].brightness > 200)
      {
        fprintf(output, "@@");
      }
      else if (small_rows[h][w].brightness > 150 && small_rows[h][w].brightness < 200)
      {
        fprintf(output, "##");
      }
      else if (small_rows[h][w].brightness > 100 && small_rows[h][w].brightness < 150)
      {
        fprintf(output, "&&");
      }
      else if (small_rows[h][w].brightness > 50 && small_rows[h][w].brightness < 100)
      {
        fprintf(output, "..");
      }
      else
      {
        fprintf(output, "  ");
      }
    }
    fprintf(output, "\n");
  }


  fclose(output);


      
    // free memory in rows;
  for (int h = 0; h < height_dec; h++)
  {
    free(rows[h]);
  }
  free(rows);
   
  for (int h = 0; h < height_dec; h++)
  {
    free(rows_blur[h]);
  }
  free(rows_blur);

  for (int h = 0; h < new_height; h++)
  {
    free(small_rows[h]);
  }
  free(small_rows);

  return 0; 
}


// << shifts the bits to the left by the ammount of bits after the <<
// 00FF << 8 will become FF00
int toint(uint8_t hex[4])
{
  int i0 = (int)hex[0];
  int i1 = (int)hex[1] << 8;
  int i2 = (int)hex[2] << 16;
  int i3 = (int)hex[3] << 24;
  return i0 + i1 + i2 + i3;
}


long tolong(uint8_t hex[4])
{
  long i0 = (long)hex[0];
  long i1 = (long)hex[1] << 8;
  long i2 = (long)hex[2] << 16;
  long i3 = (long)hex[3] << 24;
  return i0 + i1 + i2 + i3;
}
