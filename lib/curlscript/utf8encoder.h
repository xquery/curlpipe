/******************************************************************************
 * curlscript - https://github.com/xquery/curlscript
 ******************************************************************************
 * Copyright (c) 2017-2018 James Fuller <jim.fuller@webcomposite.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
******************************************************************************/

#ifndef CURLSCRIPT_UTF8ENCODER_H
#define CURLSCRIPT_UTF8ENCODER_H

#include <cstddef>
#include <cstdio>
#include <string>

class MalformedInputException
{
public:
    MalformedInputException(size_t offset) : offset(offset) {}
    size_t getOffset() const {return offset;}

private:
    size_t offset;
};

class Utf8Encoder
{
public:
    static std::string encode(const wchar_t *unencoded)
    {
        return encode(unencoded, wcslen(unencoded));
    }

    static std::string encode(const wchar_t *unencoded, size_t size)
    {
        std::string encoded;
        encoded.reserve(size + 3);

        for (size_t i = 0; i < size; ++i)
        {
            if (encoded.size() + 4 >= encoded.capacity()) encoded.reserve(encoded.capacity() * 2);

            int w = unencoded[i];
            if (w < 0x80)
            {
                encoded += w;
            }
            else if (w < 0x800)
            {
                encoded += 0xc0 | (w >> 6);
                encoded += 0x80 | (w & 0x3f);
            }
            else if (w < 0xd800)
            {
                encoded += 0xe0 | ( w          >> 12);
                encoded += 0x80 | ((w & 0xfff) >>  6);
                encoded += 0x80 | ( w &  0x3f       );
            }
            else if (w < 0xe000)
            {
                if (++i >= size)
                {
                    throw MalformedInputException(i - 1);
                }
                int w2 = unencoded[i];
                if (w2 < 0xdc00 || w2 > 0xdfff)
                {
                    throw MalformedInputException(i - 1);
                }
                w = (((w  & 0x3ff) << 10) | (w2 & 0x3ff)) + 0x10000;
                encoded += 0xf0 | ( w            >> 18);
                encoded += 0x80 | ((w & 0x3ffff) >> 12);
                encoded += 0x80 | ((w &   0xfff) >>  6);
                encoded += 0x80 | ( w &    0x3f       );
            }
            else if (w < 0x10000)
            {
                encoded += 0xe0 | ( w          >> 12);
                encoded += 0x80 | ((w & 0xfff) >>  6);
                encoded += 0x80 | ( w &  0x3f       );
            }
            else if (w < 0x110000)
            {
                encoded += 0xf0 | ( w            >> 18);
                encoded += 0x80 | ((w & 0x3ffff) >> 12);
                encoded += 0x80 | ((w &   0xfff) >>  6);
                encoded += 0x80 | ( w &    0x3f       );
            }
            else
            {
                throw MalformedInputException(i);
            }
        }
        return encoded;
    }
};

#endif
