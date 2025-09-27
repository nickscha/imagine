/* imagine.h - v0.1 - public domain data structures - nickscha 2025

A C89 standard compliant, single header, nostdlib (no C Standard Library) Image Library (IMAGINE).

LICENSE

  Placed in the public domain and also MIT licensed.
  See end of file for detailed license information.

*/
#ifndef IMAGINE_H
#define IMAGINE_H

/* #############################################################################
 * # COMPILER SETTINGS
 * #############################################################################
 */
/* Check if using C99 or later (inline is supported) */
#if __STDC_VERSION__ >= 199901L
#define IMAGINE_INLINE inline
#elif defined(__GNUC__) || defined(__clang__)
#define IMAGINE_INLINE __inline__
#elif defined(_MSC_VER)
#define IMAGINE_INLINE __inline
#else
#define IMAGINE_INLINE
#endif

#define IMAGINE_API static

typedef struct imagine_buffer
{
  unsigned int width;
  unsigned int height;
  unsigned int stride;      /* bytes per pixel: 1=grayscale/monochrome, 3=RGB, 4=RGBA */
  unsigned char monochrome; /* 1 if grayscale/1-bit, 0 if color */
  unsigned char *pixels;    /* user-provided buffer */
  unsigned int capacity;    /* bytes allocated in pixels */
  unsigned int size;        /* bytes actually used for image data */
} imagine_buffer;

IMAGINE_API IMAGINE_INLINE int imagine_is_space(unsigned char c)
{
  return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

IMAGINE_API IMAGINE_INLINE int imagine_is_digit(unsigned char c)
{
  return c >= '0' && c <= '9';
}

IMAGINE_API IMAGINE_INLINE int imagine_parse_int(unsigned char **ptr, unsigned char *end, int *out)
{
  int value = 0;
  unsigned char *p = *ptr;

  while (p < end && imagine_is_space(*p))
  {
    p++;
  }

  if (p >= end)
  {
    return -1;
  }

  while (p < end && imagine_is_digit(*p))
  {
    value = value * 10 + (*p - '0');
    p++;
  }

  *out = value;
  *ptr = p;
  return 0;
}

/* --- PNM Loader (PPM/PGM) --- */
IMAGINE_API IMAGINE_INLINE int imagine_load_pnm(unsigned char *buf, unsigned int buf_size, imagine_buffer *out)
{
  unsigned char type;
  int is_ascii, monochrome;
  unsigned char *p, *end;
  int width, height, maxval;
  unsigned int stride, needed;
  unsigned int i, count;

  if (buf_size < 2 || buf[0] != 'P')
  {
    return -1;
  }

  type = buf[1];
  is_ascii = (type == '2' || type == '3');
  monochrome = (type == '2' || type == '5');

  if (type != '2' && type != '3' && type != '5' && type != '6')
  {
    return -1;
  }

  p = buf + 2;
  end = buf + buf_size;

  while (p < end && imagine_is_space(*p))
  {
    p++;
  }

  while (p < end && *p == '#')
  {
    while (p < end && *p != '\n')
    {
      p++;
    }

    while (p < end && imagine_is_space(*p))
    {
      p++;
    }
  }

  width = 0;
  height = 0;
  maxval = 0;

  if (imagine_parse_int(&p, end, &width))
  {
    return -1;
  }

  if (imagine_parse_int(&p, end, &height))
  {
    return -1;
  }

  if (imagine_parse_int(&p, end, &maxval))
  {
    return -1;
  }

  if (width <= 0 || height <= 0 || maxval <= 0 || maxval > 255)
  {
    return -1;
  }

  if (p < end && imagine_is_space(*p))
  {
    p++;
  }

  stride = monochrome ? 1 : 3;
  needed = (unsigned int)width * (unsigned int)height * stride;

  if (out->capacity < needed)
  {
    return -1;
  }

  if (!is_ascii)
  {
    if ((unsigned int)(end - p) < needed)
    {
      return -1;
    }

    for (i = 0; i < needed; i++)
    {
      out->pixels[i] = p[i];
    }
  }
  else
  {
    count = 0;
    while (p < end && count < needed)
    {
      int val;

      if (imagine_parse_int(&p, end, &val))
      {
        return -1;
      }

      if (val < 0)
      {
        val = 0;
      }

      if (val > 255)
      {
        val = 255;
      }

      out->pixels[count++] = (unsigned char)val;
    }

    if (count != needed)
    {
      return -1;
    }
  }

  out->width = (unsigned int)width;
  out->height = (unsigned int)height;
  out->stride = stride;
  out->monochrome = (unsigned char)monochrome;
  out->size = needed;
  return 0;
}

/* --- BMP Loader (1/8/24/32 bpp) --- */
IMAGINE_API IMAGINE_INLINE int imagine_load_bmp(unsigned char *buf, unsigned int buf_size, imagine_buffer *out)
{
  unsigned int pixel_offset, width, height, row_stride, needed;
  unsigned short bpp;
  unsigned int stride;
  unsigned int x, y;

  if (buf_size < 54)
  {
    return -1;
  }

  if (buf[0] != 'B' || buf[1] != 'M')
  {
    return -1;
  }

  pixel_offset = buf[10] | (buf[11] << 8) | (buf[12] << 16) | (buf[13] << 24);
  width = buf[18] | (buf[19] << 8) | (buf[20] << 16) | (buf[21] << 24);
  height = buf[22] | (buf[23] << 8) | (buf[24] << 16) | (buf[25] << 24);
  bpp = (unsigned short)(buf[28] | (buf[29] << 8));
  row_stride = ((width * bpp + 31) / 32) * 4;

  if (bpp != 1 && bpp != 8 && bpp != 24 && bpp != 32)
  {
    return -1;
  }

  stride = (bpp == 1 || bpp == 8) ? 1 : 3;
  needed = width * height * stride;

  if (out->capacity < needed)
  {
    return -1;
  }

  for (y = 0; y < height; y++)
  {
    unsigned char *row = buf + pixel_offset + (height - 1 - y) * row_stride;

    for (x = 0; x < width; x++)
    {
      unsigned char *dst = out->pixels + (y * width + x) * stride;

      if (bpp == 1)
      {
        unsigned char byte = row[x / 8];
        int bit = 7 - (x % 8);
        dst[0] = (byte & (1 << bit)) ? 255 : 0;
      }
      else if (bpp == 8)
      {
        dst[0] = row[x];
      }
      else
      {
        dst[0] = row[2];
        dst[1] = row[1];
        dst[2] = row[0];
      }
    }
  }

  out->width = width;
  out->height = height;
  out->stride = stride;
  out->monochrome = (unsigned char)((bpp == 1 || bpp == 8) ? 1 : 0);
  out->size = needed;
  return 0;
}

/* --- Public Load API --- */
IMAGINE_API IMAGINE_INLINE int imagine_load_from_memory(unsigned char *binary_buffer, unsigned int binary_buffer_size, imagine_buffer *out)
{
  if (binary_buffer_size < 4)
  {
    return -1;
  }

  if (binary_buffer[0] == 'P')
  {
    return imagine_load_pnm(binary_buffer, binary_buffer_size, out);
  }

  if (binary_buffer[0] == 'B' && binary_buffer[1] == 'M')
  {
    return imagine_load_bmp(binary_buffer, binary_buffer_size, out);
  }

  return -1;
}

#endif /* IMAGINE_H */

/*
   ------------------------------------------------------------------------------
   This software is available under 2 licenses -- choose whichever you prefer.
   ------------------------------------------------------------------------------
   ALTERNATIVE A - MIT License
   Copyright (c) 2025 nickscha
   Permission is hereby granted, free of charge, to any person obtaining a copy of
   this software and associated documentation files (the "Software"), to deal in
   the Software without restriction, including without limitation the rights to
   use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is furnished to do
   so, subject to the following conditions:
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
   ------------------------------------------------------------------------------
   ALTERNATIVE B - Public Domain (www.unlicense.org)
   This is free and unencumbered software released into the public domain.
   Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
   software, either in source code form or as a compiled binary, for any purpose,
   commercial or non-commercial, and by any means.
   In jurisdictions that recognize copyright laws, the author or authors of this
   software dedicate any and all copyright interest in the software to the public
   domain. We make this dedication for the benefit of the public at large and to
   the detriment of our heirs and successors. We intend this dedication to be an
   overt act of relinquishment in perpetuity of all present and future rights to
   this software under copyright law.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
   WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
   ------------------------------------------------------------------------------
*/
