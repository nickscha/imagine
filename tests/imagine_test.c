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
#include "stdio.h"

void imagine_test_load(void)
{
  unsigned char pixels[BUF_SIZE];
  unsigned char binary_buffer[BUF_SIZE];
  unsigned int binary_buffer_size;

  imagine img = {0};
  img.pixels = pixels;
  img.pixels_capacity = BUF_SIZE;

  if (!pio_read("grayscale.ppm", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size))
  {
    assert(pio_read("tests/grayscale.ppm", binary_buffer, (unsigned long)BUF_SIZE, (unsigned long *)&binary_buffer_size));
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

int main(void)
{
  imagine_test_load();

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
