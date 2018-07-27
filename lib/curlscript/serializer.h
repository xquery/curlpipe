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

#ifndef CURLSCRIPT_SERIALIZER_H
#define CURLSCRIPT_SERIALIZER_H

#include "csparser.hpp"
#include "utf8encoder.h"

class RawSerializer : public csparser::EventHandler
{
public:
    RawSerializer(bool indent)
            : input(0)
            , delayedTag(0)
            , indent(indent)
            , hasChildElement(false)
            , depth(0)
    {
    }

    void reset(const wchar_t *input)
    {
        DLOG_S(INFO) << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

        this->input = input;
        delayedTag = 0;
        hasChildElement = false;
        depth = 0;
    }

    void startNonterminal(const wchar_t *tag, int b)
    {
        if (delayedTag != 0)
        {
            DLOG_S(INFO) << "<" << Utf8Encoder::encode(delayedTag).c_str()  << ">";
        }
        delayedTag = tag;
        if (indent)
        {
            DLOG_S(INFO) << "\n";
            for (int i = 0; i < depth; ++i)
            {
                DLOG_S(INFO) << "  ";
            }
        }
        hasChildElement = false;
        ++depth;
    }

    void endNonterminal(const wchar_t *tag, int e)
    {
        --depth;
        if (delayedTag != 0)
        {
            delayedTag = 0;
            DLOG_S(INFO) << "</" << Utf8Encoder::encode(tag).c_str()  << ">";
        }
        else
        {
            if (indent)
            {
                if (hasChildElement)
                {
                    DLOG_S(INFO) << "\n";
                    for (int i = 0; i < depth; ++i)
                    {
                        DLOG_S(INFO) << "  ";
                    }
                }
            }
            DLOG_S(INFO) << "</" << Utf8Encoder::encode(tag).c_str()  << ">";
        }
        hasChildElement = true;
    }

    void whitespace(int b, int e)
    {
        characters(b, e);
    }

    void characters(int b, int e)
    {
        if (b < e)
        {
            if (delayedTag != 0)
            {
                DLOG_S(INFO) << "<" << Utf8Encoder::encode(delayedTag).c_str()  << ">";

//                fputc('<' , stdout);
//                fputs(csparser::Utf8Encoder::encode(delayedTag).c_str(), stdout);
//                fputc('>' , stdout);
                delayedTag = 0;
            }
            std::string encoded = Utf8Encoder::encode(input + b, e - b);
            int size = encoded.size();
            for (int i = 0; i < size; ++i)
            {
                char c = encoded[i];
                switch (c)
                {
                    case 0: break;
                    case L'&': DLOG_S(INFO) << "&amp;";
//                        fputs("&amp;", stdout);
                        break;
                    case L'<': DLOG_S(INFO) << "&lt;";
//                        fputs("&lt;", stdout);
                        break;
                    case L'>': DLOG_S(INFO) << "&gt;";
//                        fputs("&gt;", stdout);
                        break;
                    default: DLOG_S(INFO) << c;
//                        fputc(c, stdout);
                }
            }
        }
    }

    void terminal(const wchar_t *tag, int b, int e) override {
        if (tag[0] == L'\'') tag = L"TOKEN";
        startNonterminal(tag, b);
        characters(b, e);
        endNonterminal(tag, e);
    }

private:
    const wchar_t *input;
    const wchar_t *delayedTag;
    bool indent;
    bool hasChildElement;
    int depth;
};

class XMLSerializer : public csparser::EventHandler
{
    std::stringstream ss;

public:
    XMLSerializer(bool indent)
            : input(0)
            , delayedTag(0)
            , indent(indent)
            , hasChildElement(false)
            , depth(0)
    {
    }

    void reset(const wchar_t *input)
    {
        DLOG_S(INFO) << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
        ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

        this->input = input;
        delayedTag = 0;
        hasChildElement = false;
        depth = 0;
    }

    void startNonterminal(const wchar_t *tag, int b)
    {
        if (delayedTag != 0)
        {
            DLOG_S(INFO) << "<" << Utf8Encoder::encode(delayedTag).c_str()  << ">";
            ss << "<" << Utf8Encoder::encode(delayedTag).c_str()  << ">";
        }
        delayedTag = tag;
        if (indent)
        {
            DLOG_S(INFO) << "\n";
            ss << "\n";
            for (int i = 0; i < depth; ++i)
            {
                DLOG_S(INFO) << "  ";
                ss << "  ";
            }
        }
        hasChildElement = false;
        ++depth;
    }

    void endNonterminal(const wchar_t *tag, int e)
    {
        --depth;
        if (delayedTag != 0)
        {
            delayedTag = 0;
            DLOG_S(INFO) << "</" << Utf8Encoder::encode(tag).c_str()  << ">";
            ss << "<" << Utf8Encoder::encode(tag).c_str()  << "/>";
        }
        else
        {
            if (indent)
            {
                if (hasChildElement)
                {
                    DLOG_S(INFO) << "\n";
                    ss << "\n";
                    for (int i = 0; i < depth; ++i)
                    {
                        DLOG_S(INFO) << "  ";
                        ss << "  ";
                    }
                }
            }
            DLOG_S(INFO) << "</" << Utf8Encoder::encode(tag).c_str()  << ">";
            ss << "</" << Utf8Encoder::encode(tag).c_str()  << ">";
        }
        hasChildElement = true;
    }

    void whitespace(int b, int e)
    {
        characters(b, e);
    }

    void characters(int b, int e)
    {
        if (b < e)
        {
            if (delayedTag != 0)
            {
                DLOG_S(INFO) << "<" << Utf8Encoder::encode(delayedTag).c_str()  << ">";
                ss << "<" << Utf8Encoder::encode(delayedTag).c_str()  << ">";
                delayedTag = 0;
            }
            std::string encoded = Utf8Encoder::encode(input + b, e - b);
            int size = encoded.size();
            for (int i = 0; i < size; ++i)
            {
                char c = encoded[i];
                switch (c)
                {
                    case 0: break;
                    case L'&': DLOG_S(INFO) << "&amp;";
                        ss << "&amp;";
                        break;
                    case L'<': DLOG_S(INFO) << "&lt;";
                        ss << "&lt;";
                        break;
                    case L'>': DLOG_S(INFO) << "&gt;";
                        ss << "&gt;";
                        break;
                    default: DLOG_S(INFO) << c;
                             ss << c;
                }
            }
        }
    }

    void terminal(const wchar_t *tag, int b, int e) override {
        if (tag[0] == L'\'') tag = L"TOKEN";
        startNonterminal(tag, b);
        characters(b, e);
        endNonterminal(tag, e);
    }

    string getParsed(){
        return ss.str();
    }

private:
    const wchar_t *input;
    const wchar_t *delayedTag;
    bool indent;
    bool hasChildElement;
    int depth;
};

#endif
