/* imagine.h - v0.1 - public domain data structures - nickscha 2025

A C89 standard compliant, single header, nostdlib (no C Standard Library) Image Library (IMAGINE).

This Test class defines cases to verify that we don't break the excepted behaviours in the future upon changes.

LICENSE

  Placed in the public domain and also MIT licensed.
  See end of file for detailed license information.

*/
#include "../imagine.h"   /* Image Library               */
#include "../deps/test.h" /* Simple Testing framework    */
#include "../deps/perf.h" /* Simple Performance profiler */
#include "../deps/pio.h"  /* Read/Write Files            */

#define BUF_SIZE 128 * 128

static void imagine_test_load(void)
{
  unsigned char pixels[BUF_SIZE];
  unsigned char binary_buffer[BUF_SIZE];
  unsigned int binary_buffer_size;

  imagine img = {0};
  img.pixels = pixels;
  img.pixels_capacity = BUF_SIZE;

  if (!pio_read("images/grayscale.ppm", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size))
  {
    assert(pio_read("tests/images/grayscale.ppm", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size));
  }

  assert(binary_buffer_size > 0);
  assert(imagine_load(&img, binary_buffer, binary_buffer_size));
  assert(img.width == 2);
  assert(img.height == 2);
  assert(img.stride == 1);
  assert(img.monochrome == 1);
  assert(img.pixels_size == 2 * 2);
  assert(img.pixels[0] == 0);
  assert(img.pixels[1] == 128);
  assert(img.pixels[2] == 200);
  assert(img.pixels[3] == 255);
}

static void imagine_test_all_netpbm(void)
{
  unsigned char pixels[BUF_SIZE];
  unsigned char binary_buffer[BUF_SIZE];
  unsigned int binary_buffer_size;

  imagine img = {0};
  img.pixels = pixels;
  img.pixels_capacity = BUF_SIZE;

  if (!pio_read("images/test-p1.pbm", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size))
  {
    assert(pio_read("tests/images/test-p1.pbm", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size));
  }

  assert(binary_buffer_size > 0);
  assert(imagine_load(&img, binary_buffer, binary_buffer_size));
  assert(img.width == 2);
  assert(img.height == 2);
  assert(img.stride == 1);
  assert(img.monochrome == 1);
  assert(img.pixels_size == 2 * 2);
  assert(img.pixels[0] == 255);
  assert(img.pixels[1] == 0);
  assert(img.pixels[2] == 0);
  assert(img.pixels[3] == 255);

  if (!pio_read("images/test-p2.pgm", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size))
  {
    assert(pio_read("tests/images/test-p2.pgm", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size));
  }

  assert(binary_buffer_size > 0);
  assert(imagine_load(&img, binary_buffer, binary_buffer_size));
  assert(img.width == 2);
  assert(img.height == 2);
  assert(img.stride == 1);
  assert(img.monochrome == 1);
  assert(img.pixels_size == 2 * 2);
  assert(img.pixels[0] == 0);
  assert(img.pixels[1] == 128);
  assert(img.pixels[2] == 200);
  assert(img.pixels[3] == 255);

  if (!pio_read("images/test-p3.ppm", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size))
  {
    assert(pio_read("tests/images/test-p3.ppm", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size));
  }

  assert(binary_buffer_size > 0);
  assert(imagine_load(&img, binary_buffer, binary_buffer_size));
  assert(img.width == 2);
  assert(img.height == 2);
  assert(img.stride == 3);
  assert(img.monochrome == 0);
  assert(img.pixels_size == 2 * 2 * 3);
  assert(img.pixels[0] == 255);
  assert(img.pixels[1] == 0);
  assert(img.pixels[2] == 0);
  assert(img.pixels[3] == 0);
  assert(img.pixels[4] == 255);
  assert(img.pixels[5] == 0);
  assert(img.pixels[6] == 0);
  assert(img.pixels[7] == 0);
  assert(img.pixels[8] == 255);
  assert(img.pixels[9] == 255);
  assert(img.pixels[10] == 255);
  assert(img.pixels[11] == 255);

  if (!pio_read("images/test-p4.pbm", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size))
  {
    assert(pio_read("tests/images/test-p4.pbm", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size));
  }

  assert(binary_buffer_size > 0);
  assert(imagine_load(&img, binary_buffer, binary_buffer_size));
  assert(img.width == 2);
  assert(img.height == 2);
  assert(img.stride == 1);
  assert(img.monochrome == 1);
  assert(img.pixels_size == 2 * 2);
  assert(img.pixels[0] == 255);
  assert(img.pixels[1] == 0);
  assert(img.pixels[2] == 0);
  assert(img.pixels[3] == 255);

  if (!pio_read("images/test-p5.pgm", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size))
  {
    assert(pio_read("tests/images/test-p5.pgm", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size));
  }

  assert(binary_buffer_size > 0);
  assert(imagine_load(&img, binary_buffer, binary_buffer_size));
  assert(img.width == 2);
  assert(img.height == 2);
  assert(img.stride == 1);
  assert(img.monochrome == 1);
  assert(img.pixels_size == 2 * 2);
  assert(img.pixels[0] == 0);
  assert(img.pixels[1] == 128);
  assert(img.pixels[2] == 200);
  assert(img.pixels[3] == 255);

  if (!pio_read("images/test-p6.ppm", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size))
  {
    assert(pio_read("tests/images/test-p6.ppm", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size));
  }

  assert(binary_buffer_size > 0);
  assert(imagine_load(&img, binary_buffer, binary_buffer_size));
  assert(img.width == 2);
  assert(img.height == 2);
  assert(img.stride == 3);
  assert(img.monochrome == 0);
  assert(img.pixels_size == 2 * 2 * 3);
  assert(img.pixels[0] == 255);
  assert(img.pixels[1] == 0);
  assert(img.pixels[2] == 0);
  assert(img.pixels[3] == 0);
  assert(img.pixels[4] == 255);
  assert(img.pixels[5] == 0);
  assert(img.pixels[6] == 0);
  assert(img.pixels[7] == 0);
  assert(img.pixels[8] == 255);
  assert(img.pixels[9] == 255);
  assert(img.pixels[10] == 255);
  assert(img.pixels[11] == 255);

  if (!pio_read("images/test-p7.pam", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size))
  {
    assert(pio_read("tests/images/test-p7.pam", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size));
  }

  /* TODO: implement P7 */
}

static void imagine_test_all_bmp(void)
{
  unsigned char pixels[BUF_SIZE];
  unsigned char binary_buffer[BUF_SIZE];
  unsigned int binary_buffer_size;

  imagine img = {0};
  img.pixels = pixels;
  img.pixels_capacity = BUF_SIZE;

  if (!pio_read("images/test-bmp-1bit.bmp", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size))
  {
    assert(pio_read("tests/images/test-bmp-1bit.bmp", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size));
  }

  assert(binary_buffer_size > 0);
  assert(imagine_load(&img, binary_buffer, binary_buffer_size));
  assert(img.width == 2);
  assert(img.height == 2);
  assert(img.stride == 3);
  assert(img.monochrome == 0);
  assert(img.pixels_size == 2 * 2 * 3);
  assert(img.pixels[0] == 0);
  assert(img.pixels[1] == 0);
  assert(img.pixels[2] == 0);
  assert(img.pixels[3] == 255);
  assert(img.pixels[4] == 255);
  assert(img.pixels[5] == 255);
  assert(img.pixels[6] == 255);
  assert(img.pixels[7] == 255);
  assert(img.pixels[8] == 255);
  assert(img.pixels[9] == 0);
  assert(img.pixels[10] == 0);
  assert(img.pixels[11] == 0);

  if (!pio_read("images/test-bmp-4bit.bmp", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size))
  {
    assert(pio_read("tests/images/test-bmp-4bit.bmp", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size));
  }

  assert(binary_buffer_size > 0);
  assert(imagine_load(&img, binary_buffer, binary_buffer_size));
  assert(img.width == 2);
  assert(img.height == 2);
  assert(img.stride == 3);
  assert(img.monochrome == 0);
  assert(img.pixels_size == 2 * 2 * 3);
  assert(img.pixels[0] == 0);
  assert(img.pixels[1] == 0);
  assert(img.pixels[2] == 0);

  if (!pio_read("images/test-bmp-8bit.bmp", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size))
  {
    assert(pio_read("tests/images/test-bmp-8bit.bmp", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size));
  }

  assert(binary_buffer_size > 0);
  assert(imagine_load(&img, binary_buffer, binary_buffer_size));
  assert(img.width == 2);
  assert(img.height == 2);
  assert(img.stride == 3);
  assert(img.monochrome == 0);
  assert(img.pixels_size == 2 * 2 * 3);
  assert(img.pixels[0] == 0);
  assert(img.pixels[1] == 0);
  assert(img.pixels[2] == 0);
  assert(img.pixels[3] == 1);
  assert(img.pixels[4] == 1);
  assert(img.pixels[5] == 1);
  assert(img.pixels[6] == 1);
  assert(img.pixels[7] == 1);
  assert(img.pixels[8] == 1);
  assert(img.pixels[9] == 2);
  assert(img.pixels[10] == 2);
  assert(img.pixels[11] == 2);

  if (!pio_read("images/test-bmp-16bit.bmp", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size))
  {
    assert(pio_read("tests/images/test-bmp-16bit.bmp", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size));
  }

  assert(binary_buffer_size > 0);
  assert(imagine_load(&img, binary_buffer, binary_buffer_size));
  assert(img.width == 2);
  assert(img.height == 2);
  assert(img.stride == 3);
  assert(img.monochrome == 0);
  assert(img.pixels_size == 2 * 2 * 3);
  assert(img.pixels[0] == 255);
  assert(img.pixels[1] == 0);
  assert(img.pixels[2] == 0);
  assert(img.pixels[3] == 0);
  assert(img.pixels[4] == 255);
  assert(img.pixels[5] == 0);
  assert(img.pixels[6] == 0);
  assert(img.pixels[7] == 0);
  assert(img.pixels[8] == 255);
  assert(img.pixels[9] == 255);
  assert(img.pixels[10] == 255);
  assert(img.pixels[11] == 255);

  if (!pio_read("images/test-bmp-24bit.bmp", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size))
  {
    assert(pio_read("tests/images/test-bmp-24bit.bmp", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size));
  }

  assert(binary_buffer_size > 0);
  assert(imagine_load(&img, binary_buffer, binary_buffer_size));
  assert(img.width == 2);
  assert(img.height == 2);
  assert(img.stride == 3);
  assert(img.monochrome == 0);
  assert(img.pixels_size == 2 * 2 * 3);
  assert(img.pixels[0] == 255);
  assert(img.pixels[1] == 0);
  assert(img.pixels[2] == 0);
  assert(img.pixels[3] == 0);
  assert(img.pixels[4] == 255);
  assert(img.pixels[5] == 0);
  assert(img.pixels[6] == 0);
  assert(img.pixels[7] == 0);
  assert(img.pixels[8] == 255);
  assert(img.pixels[9] == 255);
  assert(img.pixels[10] == 255);
  assert(img.pixels[11] == 255);

  if (!pio_read("images/test-bmp-32bit.bmp", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size))
  {
    assert(pio_read("tests/images/test-bmp-32bit.bmp", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size));
  }

  assert(binary_buffer_size > 0);
  assert(imagine_load(&img, binary_buffer, binary_buffer_size));
  assert(img.width == 2);
  assert(img.height == 2);
  assert(img.stride == 4);
  assert(img.monochrome == 0);
  assert(img.pixels_size == 2 * 2 * 4);
  assert(img.pixels[0] == 255);
  assert(img.pixels[1] == 0);
  assert(img.pixels[2] == 0);
  assert(img.pixels[3] == 255);
  assert(img.pixels[4] == 0);
  assert(img.pixels[5] == 255);
  assert(img.pixels[6] == 0);
  assert(img.pixels[7] == 128);
  assert(img.pixels[8] == 0);
  assert(img.pixels[9] == 0);
  assert(img.pixels[10] == 255);
  assert(img.pixels[11] == 64);
  assert(img.pixels[12] == 255);
  assert(img.pixels[13] == 255);
  assert(img.pixels[14] == 255);
  assert(img.pixels[15] == 0);
}

int main(void)
{
  imagine_test_load();
  imagine_test_all_netpbm();
  imagine_test_all_bmp();

  return 0;
}

/*
   -----------------------------------------------------------------------------
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
