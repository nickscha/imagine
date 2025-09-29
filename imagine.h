/* imagine.h - v0.1 - public domain data structures - nickscha 2025

A C89 standard compliant, single header, nostdlib (no C Standard Library) Image Library (IMAGINE).

LICENSE

  Placed in the public domain and also MIT licensed.
  See end of file for detailed license information.

*/
#ifndef IMAGINE_H
#define IMAGINE_H

/* ########################################################################## */
/* COMPILER SETTINGS */
/* ########################################################################## */
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

typedef struct imagine
{
  unsigned int width;
  unsigned int height;
  unsigned int stride;      /* bytes per pixel: 1=gray, 3=RGB */
  unsigned char monochrome; /* 1 if grayscale, 0 if color */
  unsigned char *pixels;    /* user-provided buffer */
  unsigned int pixels_capacity;
  unsigned int pixels_size;

} imagine;

/* ########################################################################## */
/* HELPERS */
/* ########################################################################## */
IMAGINE_API IMAGINE_INLINE unsigned short imagine_read16(const unsigned char *p)
{
  return (unsigned short)(p[0] | (p[1] << 8));
}

IMAGINE_API IMAGINE_INLINE unsigned int imagine_read32(const unsigned char *p)
{
  return (unsigned int)(p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24));
}

/* ########################################################################## */
/* NETPBM (P1–P7) */
/* ########################################################################## */
IMAGINE_API IMAGINE_INLINE unsigned char *imagine_ppm_skip(unsigned char *p, unsigned char *end)
{
  while (p < end)
  {
    if (*p == '#')
    {
      while (p < end && *p != '\n' && *p != '\r')
        p++;
    }
    else if (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
    {
      p++;
    }
    else
    {
      break;
    }
  }
  return p;
}

IMAGINE_API IMAGINE_INLINE unsigned char *imagine_ppm_parse_uint(unsigned char *p, unsigned char *end, unsigned int *out)
{
  unsigned int v = 0;
  p = imagine_ppm_skip(p, end);

  while (p < end && *p >= '0' && *p <= '9')
  {
    v = v * 10 + (unsigned int)(*p - '0');
    p++;
  }

  *out = v;
  return p;
}

IMAGINE_API int imagine_load_netpbm(imagine *img, unsigned char *buffer, unsigned int size)
{
  unsigned char *p, *end, fmt;
  unsigned int w, h, maxval;
  unsigned int i, n;

  if (size < 2 || buffer[0] != 'P')
  {
    return 0;
  }

  fmt = buffer[1];
  p = buffer + 2;
  end = buffer + size;

  p = imagine_ppm_parse_uint(p, end, &w);
  p = imagine_ppm_parse_uint(p, end, &h);

  if (w == 0 || h == 0)
  {
    return 0;
  }

  maxval = 1;
  if (fmt != '1' && fmt != '4')
  {
    p = imagine_ppm_parse_uint(p, end, &maxval);
    if (maxval == 0)
      return 0;
  }

  img->width = w;
  img->height = h;
  img->monochrome = (fmt == '1' || fmt == '4' || fmt == '2' || fmt == '5');
  img->stride = img->monochrome ? 1 : 3;
  img->pixels_size = w * h * img->stride;

  if (img->pixels_capacity < img->pixels_size)
  {
    return 0;
  }

  n = w * h;

  /* ASCII P1 (bitmap 0/1) */
  if (fmt == '1')
  {
    for (i = 0; i < n; ++i)
    {
      unsigned int bit;
      p = imagine_ppm_parse_uint(p, end, &bit);
      img->pixels[i] = (unsigned char)(bit ? 0 : 255);
    }
  }
  /* Binary P4 (bitmap packed bits) */
  else if (fmt == '4')
  {
    unsigned int x, y;
    unsigned int rowbytes = (w + 7) / 8;
    p = imagine_ppm_skip(p, end); /* move to pixel data */

    if ((unsigned int)(end - p) < rowbytes * h)
      return 0; /* not enough data */

    for (y = 0; y < h; ++y)
    {
      unsigned char *row = p + y * rowbytes;
      for (x = 0; x < w; ++x)
      {
        unsigned int byte = row[x >> 3];
        unsigned int bit = (byte >> (7 - (x & 7))) & 1;
        img->pixels[y * w + x] = (unsigned char)(bit ? 0 : 255);
      }
    }
  }
  /* ASCII grayscale P2 */
  else if (fmt == '2')
  {
    for (i = 0; i < n; ++i)
    {
      unsigned int v;
      p = imagine_ppm_parse_uint(p, end, &v);
      img->pixels[i] = (unsigned char)((255U * v) / maxval);
    }
  }
  /* Binary grayscale P5 */
  else if (fmt == '5')
  {
    unsigned int v;
    p = imagine_ppm_skip(p, end);
    for (i = 0; i < n; ++i)
    {
      if (p >= end)
        return 0;
      v = *p++;
      img->pixels[i] = (unsigned char)((255U * v) / maxval);
    }
  }
  /* ASCII RGB P3 */
  else if (fmt == '3')
  {
    for (i = 0; i < n; ++i)
    {
      unsigned int r, g, b;
      p = imagine_ppm_parse_uint(p, end, &r);
      p = imagine_ppm_parse_uint(p, end, &g);
      p = imagine_ppm_parse_uint(p, end, &b);
      img->pixels[i * 3 + 0] = (unsigned char)((255U * r) / maxval);
      img->pixels[i * 3 + 1] = (unsigned char)((255U * g) / maxval);
      img->pixels[i * 3 + 2] = (unsigned char)((255U * b) / maxval);
    }
  }
  /* Binary RGB P6 */
  else if (fmt == '6')
  {
    p = imagine_ppm_skip(p, end);
    for (i = 0; i < n; ++i)
    {
      if (p + 3 > end)
        return 0;
      img->pixels[i * 3 + 0] = (unsigned char)((255U * p[0]) / maxval);
      img->pixels[i * 3 + 1] = (unsigned char)((255U * p[1]) / maxval);
      img->pixels[i * 3 + 2] = (unsigned char)((255U * p[2]) / maxval);
      p += 3;
    }
  }
  /* PAM P7 (partial: only RGB/GRAYSCALE with DEPTH 1/3/4) */
  else if (fmt == '7')
  {
    /* Not fully implemented: header parsing required */
    return 0;
  }
  else
  {
    return 0;
  }

  return 1;
}

/* ########################################################################## */
/* BMP LOADER (1,4,8,16,24,32-bit, BI_RGB only)                               */
/* ########################################################################## */
IMAGINE_API int imagine_load_bmp(imagine *img, unsigned char *buffer, unsigned int size)
{
  unsigned int bfOffBits, width, height, planes, bitCount, compression;
  unsigned int clrUsed;
  unsigned char *p, *end, *dst;
  unsigned int x, y, rowSize;
  unsigned int paletteEntries, b, g, r, a;
  unsigned char *palette;

  if (size < 54 || buffer[0] != 'B' || buffer[1] != 'M')
  {
    return 0;
  }

  bfOffBits = imagine_read32(buffer + 10);
  width = imagine_read32(buffer + 18);
  height = imagine_read32(buffer + 22);
  planes = imagine_read16(buffer + 26);
  bitCount = imagine_read16(buffer + 28);
  compression = imagine_read32(buffer + 30);
  clrUsed = imagine_read32(buffer + 46);

  if (planes != 1 || compression != 0 || width == 0 || height == 0)
  {
    return 0;
  }

  p = buffer + bfOffBits;
  end = buffer + size;

  /* Determine palette size */
  paletteEntries = 0;

  if (bitCount <= 8)
  {
    paletteEntries = clrUsed;

    if (paletteEntries == 0)
    {
      paletteEntries = 1U << bitCount;
    }

    if (size < 54 + paletteEntries * 4)
    {
      return 0;
    }

    palette = buffer + 54;
  }
  else
  {
    palette = 0;
  }

  /* Choose stride and format */
  if (bitCount == 1 || bitCount == 4 || bitCount == 8)
  {
    img->stride = 3;
    img->monochrome = 0;
  }
  else if (bitCount == 16)
  {
    img->stride = 3;
    img->monochrome = 0;
  }
  else if (bitCount == 24)
  {
    img->stride = 3;
    img->monochrome = 0;
  }
  else if (bitCount == 32)
  {
    img->stride = 4;
    img->monochrome = 0;
  }
  else
  {
    return 0;
  }

  img->width = width;
  img->height = height;
  img->pixels_size = width * height * img->stride;

  if (img->pixels_capacity < img->pixels_size)
  {
    return 0;
  }

  dst = img->pixels;

  /* Row size in file (padded to 4 bytes) */
  rowSize = ((width * bitCount + 31) / 32) * 4;

  /* BMP stores bottom-up */
  for (y = 0; y < height; y++)
  {
    unsigned char *row = p + (height - 1 - y) * rowSize;

    if (row + rowSize > end)
    {
      return 0;
    }

    if (bitCount == 1)
    {
      for (x = 0; x < width; x++)
      {
        unsigned int byteIndex = x >> 3;
        unsigned int bitIndex = 7 - (x & 7);
        unsigned char idx = (row[byteIndex] >> bitIndex) & 1;

        b = palette[idx * 4 + 0];
        g = palette[idx * 4 + 1];
        r = palette[idx * 4 + 2];

        *dst++ = (unsigned char)r;
        *dst++ = (unsigned char)g;
        *dst++ = (unsigned char)b;
      }
    }
    else if (bitCount == 4)
    {
      for (x = 0; x < width; x++)
      {
        unsigned int byteIndex = x >> 1;
        unsigned char idx;

        if ((x & 1) == 0)
        {
          idx = (row[byteIndex] >> 4) & 0xF;
        }
        else
        {
          idx = row[byteIndex] & 0xF;
        }

        b = palette[idx * 4 + 0];
        g = palette[idx * 4 + 1];
        r = palette[idx * 4 + 2];

        *dst++ = (unsigned char)r;
        *dst++ = (unsigned char)g;
        *dst++ = (unsigned char)b;
      }
    }
    else if (bitCount == 8)
    {
      for (x = 0; x < width; x++)
      {
        unsigned char idx = row[x];

        if (idx >= paletteEntries)
        {
          idx = 0;
        }

        b = palette[idx * 4 + 0];
        g = palette[idx * 4 + 1];
        r = palette[idx * 4 + 2];

        *dst++ = (unsigned char)r;
        *dst++ = (unsigned char)g;
        *dst++ = (unsigned char)b;
      }
    }
    else if (bitCount == 16)
    {
      for (x = 0; x < width; x++)
      {
        unsigned short px = row[x * 2] | (row[x * 2 + 1] << 8);

        r = (px >> 10) & 0x1F;
        g = (px >> 5) & 0x1F;
        b = (px >> 0) & 0x1F;
        r = (r * 255) / 31;
        g = (g * 255) / 31;
        b = (b * 255) / 31;

        *dst++ = (unsigned char)r;
        *dst++ = (unsigned char)g;
        *dst++ = (unsigned char)b;
      }
    }
    else if (bitCount == 24)
    {
      for (x = 0; x < width; x++)
      {
        b = row[x * 3 + 0];
        g = row[x * 3 + 1];
        r = row[x * 3 + 2];

        *dst++ = (unsigned char)r;
        *dst++ = (unsigned char)g;
        *dst++ = (unsigned char)b;
      }
    }
    else if (bitCount == 32)
    {
      for (x = 0; x < width; x++)
      {
        b = row[x * 4 + 0];
        g = row[x * 4 + 1];
        r = row[x * 4 + 2];
        a = row[x * 4 + 3];

        *dst++ = (unsigned char)r;
        *dst++ = (unsigned char)g;
        *dst++ = (unsigned char)b;
        *dst++ = (unsigned char)a; /* keep alpha */
      }
    }
  }

  return 1;
}

/* ########################################################################## */
/* TGA LOADER (uncompressed RGB/gray) */
/* ########################################################################## */
IMAGINE_API int imagine_load_tga(imagine *img, unsigned char *buffer, unsigned int size)
{
  unsigned char idlen, type, bpp;
  unsigned int w, h;
  unsigned char *src;
  unsigned char *dst;
  unsigned int x, y;

  if (size < 18)
  {
    return 0;
  }

  idlen = buffer[0];
  type = buffer[2];

  w = imagine_read16(buffer + 12);
  h = imagine_read16(buffer + 14);

  bpp = buffer[16];

  if (w == 0 || h == 0)
  {
    return 0;
  }

  if (!(type == 2 || type == 3))
  {
    return 0;
  }

  if (bpp == 8)
  {
    img->stride = 1;
    img->monochrome = 1;
  }
  else if (bpp == 24)
  {
    img->stride = 3;
    img->monochrome = 0;
  }
  else if (bpp == 32)
  {
    img->stride = 3;
    img->monochrome = 0;
  }
  else
  {
    return 0;
  }

  img->width = w;
  img->height = h;
  img->pixels_size = w * h * img->stride;

  if (img->pixels_capacity < img->pixels_size)
  {
    return 0;
  }

  src = buffer + 18 + idlen;
  dst = img->pixels;

  for (y = 0; y < h; ++y)
  {
    for (x = 0; x < w; ++x)
    {
      if (bpp == 8)
      {
        *dst++ = *src++;
      }
      else if (bpp == 24)
      {
        unsigned char b = *src++;
        unsigned char g = *src++;
        unsigned char r = *src++;

        *dst++ = r;
        *dst++ = g;
        *dst++ = b;
      }
      else if (bpp == 32)
      {
        unsigned char b = *src++;
        unsigned char g = *src++;
        unsigned char r = *src++;

        src++; /* skip alpha */
        *dst++ = r;
        *dst++ = g;
        *dst++ = b;
      }
    }
  }

  return 1;
}

/* ########################################################################## */
/* PCX LOADER (RLE, 8-bit or 24-bit) */
/* ########################################################################## */
IMAGINE_API int imagine_load_pcx(imagine *img, unsigned char *buffer, unsigned int size)
{
  unsigned char bpp, planes;
  unsigned short xmin, ymin, xmax, ymax, w, h, bytes_per_line;
  unsigned char *src, *end, *dst;
  unsigned int y, p;

  if (size < 128 || buffer[0] != 0x0A)
  {
    return 0;
  }

  bpp = buffer[3];
  xmin = imagine_read16(buffer + 4);
  ymin = imagine_read16(buffer + 6);
  xmax = imagine_read16(buffer + 8);
  ymax = imagine_read16(buffer + 10);
  w = (unsigned short)(xmax - xmin + 1);
  h = (unsigned short)(ymax - ymin + 1);
  planes = buffer[65];
  bytes_per_line = imagine_read16(buffer + 66);

  if (w == 0 || h == 0)
  {
    return 0;
  }

  if (planes == 1 && bpp == 8)
  {
    img->stride = 1;
    img->monochrome = 1;
  }
  else if (planes == 3 && bpp == 8)
  {
    img->stride = 3;
    img->monochrome = 0;
  }
  else
  {
    return 0;
  }

  img->width = w;
  img->height = h;
  img->pixels_size = w * h * img->stride;

  if (img->pixels_capacity < img->pixels_size)
  {
    return 0;
  }

  src = buffer + 128;
  end = buffer + size;
  dst = img->pixels;

  for (y = 0; y < h; ++y)
  {
    for (p = 0; p < planes; ++p)
    {
      unsigned int filled = 0;

      while (filled < bytes_per_line && src < end)
      {
        unsigned char c = *src++;

        if ((c & 0xC0) == 0xC0)
        {
          unsigned int run = c & 0x3F;
          unsigned char val;

          if (src >= end)
          {
            return 0;
          }

          val = *src++;
          while (run-- && filled < bytes_per_line)
          {
            if (p < img->stride && filled < w)
            {
              dst[(y * w + filled) * img->stride + p] = val;
            }

            filled++;
          }
        }
        else
        {
          if (p < img->stride && filled < w)
          {
            dst[(y * w + filled) * img->stride + p] = c;
          }

          filled++;
        }
      }
    }
  }

  if (planes == 1 && bpp == 8)
  {
    unsigned char *pal;
    unsigned char lut[256];
    unsigned int i, n;

    if (size < 769)
    {
      return 0;
    }

    pal = buffer + size - 769;

    if (*pal != 0x0C)
    {
      return 0;
    }

    pal++;

    for (i = 0; i < 256; ++i)
    {
      unsigned char r = pal[i * 3 + 0];
      unsigned char g = pal[i * 3 + 1];
      unsigned char b = pal[i * 3 + 2];

      lut[i] = (unsigned char)((r + g + b) / 3);
    }

    n = w * h;

    for (i = 0; i < n; ++i)
    {
      img->pixels[i] = lut[img->pixels[i]];
    }
  }

  return 1;
}

/* ########################################################################## */
/* ICO LOADER (BMP only) */
/* ########################################################################## */
IMAGINE_API int imagine_load_ico(imagine *img, unsigned char *buffer, unsigned int size)
{
  unsigned short reserved, type, count;
  unsigned int offset;

  if (size < 6)
  {
    return 0;
  }

  reserved = imagine_read16(buffer);
  type = imagine_read16(buffer + 2);
  count = imagine_read16(buffer + 4);

  if (reserved != 0 || type != 1 || count < 1 || size < 22)
  {
    return 0;
  }

  offset = imagine_read32(buffer + 18);

  if (offset >= size)
  {
    return 0;
  }

  if (buffer[offset] == 0x89 && buffer[offset + 1] == 'P')
  {
    return 0;
  }

  return imagine_load_bmp(img, buffer + offset, size - offset);
}

/* ########################################################################## */
/* DDS LOADER (raw RGB/gray, no compression) */
/* ########################################################################## */
IMAGINE_API int imagine_load_dds(imagine *img, unsigned char *buffer, unsigned int size)
{
  unsigned int h, w, pf_size, fourcc, bpp;
  unsigned char *src, *end, *dst;
  unsigned int x, y;

  if (size < 128 || !(buffer[0] == 'D' && buffer[1] == 'D' && buffer[2] == 'S' && buffer[3] == ' '))
  {
    return 0;
  }

  h = imagine_read32(buffer + 12);
  w = imagine_read32(buffer + 16);
  pf_size = imagine_read32(buffer + 76);
  fourcc = imagine_read32(buffer + 84);
  bpp = imagine_read32(buffer + 88);

  if (w == 0 || h == 0 || pf_size != 32 || fourcc != 0)
  {
    return 0;
  }

  if (bpp == 24)
  {
    img->stride = 3;
    img->monochrome = 0;
  }
  else if (bpp == 32)
  {
    img->stride = 3;
    img->monochrome = 0;
  }
  else if (bpp == 8)
  {
    img->stride = 1;
    img->monochrome = 1;
  }
  else
  {
    return 0;
  }

  img->width = w;
  img->height = h;
  img->pixels_size = w * h * img->stride;

  if (img->pixels_capacity < img->pixels_size)
  {
    return 0;
  }

  src = buffer + 128;
  end = buffer + size;

  if ((unsigned int)(end - src) < img->pixels_size)
  {
    return 0;
  }

  dst = img->pixels;

  for (y = 0; y < h; ++y)
  {
    for (x = 0; x < w; ++x)
    {
      if (bpp == 24)
      {
        unsigned char b = *src++, g = *src++, r = *src++;

        *dst++ = r;
        *dst++ = g;
        *dst++ = b;
      }
      else if (bpp == 32)
      {
        unsigned char b = *src++, g = *src++, r = *src++;

        src++;
        *dst++ = r;
        *dst++ = g;
        *dst++ = b;
      }
      else if (bpp == 8)
      {
        *dst++ = *src++;
      }
    }
  }

  return 1;
}

/* ########################################################################## */
/* DISPATCHER */
/* ########################################################################## */
IMAGINE_API IMAGINE_INLINE int imagine_load(imagine *img, unsigned char *buf, unsigned int size)
{
  if (size >= 2 && buf[0] == 'P')
  {
    if (buf[1] >= '1' && buf[1] <= '6')
    {
      return imagine_load_netpbm(img, buf, size);
    }

    if (buf[1] == '7')
    {
      return imagine_load_netpbm(img, buf, size); /* PAM partial */
    }
  }

  if (size >= 2 && buf[0] == 'B' && buf[1] == 'M')
  {
    return imagine_load_bmp(img, buf, size);
  }

  if (size >= 18 && (buf[2] == 2 || buf[2] == 3))
  {
    return imagine_load_tga(img, buf, size);
  }

  if (size >= 4 && buf[0] == 0x0A)
  {
    return imagine_load_pcx(img, buf, size);
  }

  if (size >= 4 && buf[0] == 'D' && buf[1] == 'D' && buf[2] == 'S')
  {
    return imagine_load_dds(img, buf, size);
  }

  if (size >= 6 && imagine_read16(buf + 2) == 1)
  {
    return imagine_load_ico(img, buf, size);
  }

  return 0;
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
