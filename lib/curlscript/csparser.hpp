// This file was generated on Wed Aug 1, 2018 20:32 (UTC+02) by REx v5.47 which is Copyright (c) 1979-2017 by Gunther Rademacher <grd@gmx.net>
// REx command line: csparser.ebnf -name csparser -tree -cpp

#ifndef CSPARSER_HPP
#define CSPARSER_HPP

#include <vector>
#include <stdio.h>
#include <string>

class csparser
{
public:
  class EventHandler;

  csparser(const wchar_t *string, EventHandler *t)
  {
    initialize(string, t);
  }

  virtual ~csparser()
  {
  }

  class EventHandler
  {
  public:
    virtual ~EventHandler() {}

    virtual void reset(const wchar_t *string) = 0;
    virtual void startNonterminal(const wchar_t *name, int begin) = 0;
    virtual void endNonterminal(const wchar_t *name, int end) = 0;
    virtual void terminal(const wchar_t *name, int begin, int end) = 0;
    virtual void whitespace(int begin, int end) = 0;
  };

  class Symbol
  {
  public:
    virtual ~Symbol() {}

    const wchar_t *name;
    int begin;
    int end;

    virtual void send(EventHandler *e) = 0;

  protected:
    Symbol(const wchar_t *name, int begin, int end)
    {
      this->name = name;
      this->begin = begin;
      this->end = end;
    }
  };

  class Terminal : public Symbol
  {
  public:
    Terminal(const wchar_t *name, int begin, int end)
    : Symbol(name, begin, end)
    {}

    void send(EventHandler *e)
    {
      e->terminal(name, begin, end);
    }
  };

  class Nonterminal : public Symbol
  {
  public:
    std::vector<Symbol *> *children;

    Nonterminal(const wchar_t *name, int begin, int end, std::vector<Symbol *> *children)
    : Symbol(name, begin, end)
    {
      this->children = children;
    }

    ~Nonterminal()
    {
      for (std::vector<Symbol *>::iterator child = children->begin(); child != children->end(); ++child)
        delete *child;
      delete children;
    }

    void send(EventHandler *e)
    {
      e->startNonterminal(name, begin);
      int pos = begin;
      for (std::vector<Symbol *>::iterator i = children->begin(); i != children->end(); ++i)
      {
        Symbol *c = *i;
        if (pos < c->begin) e->whitespace(pos, c->begin);
        c->send(e);
        pos = c->end;
      }
      if (pos < end) e->whitespace(pos, end);
      e->endNonterminal(name, end);
    }
  };

  class TopDownTreeBuilder : public EventHandler
  {
  public:
    TopDownTreeBuilder()
    {
      input = 0;
      stack.clear();
      top = -1;
    }

    void reset(const wchar_t *input)
    {
      this->input = input;
      top = -1;
    }

    void startNonterminal(const wchar_t *name, int begin)
    {
      Nonterminal *nonterminal = new Nonterminal(name, begin, begin, new std::vector<Symbol *>());
      if (top++ >= 0) addChild(nonterminal);
      if ((size_t) top >= stack.size())
        stack.resize(stack.size() == 0 ? 64 : stack.size() << 1);
      stack[top] = nonterminal;
    }

    void endNonterminal(const wchar_t *name, int end)
    {
      stack[top]->end = end;
      if (top > 0) --top;
    }

    void terminal(const wchar_t *name, int begin, int end)
    {
      addChild(new Terminal(name, begin, end));
    }

    void whitespace(int begin, int end)
    {
    }

    void serialize(EventHandler *e)
    {
      e->reset(input);
      stack[0]->send(e);
    }

  private:
    void addChild(Symbol *s)
    {
      Nonterminal *current = stack[top];
      current->children->push_back(s);
    }

    const wchar_t *input;
    std::vector<Nonterminal *> stack;
    int top;
  };

  class ParseException
  {
  private:
    int begin, end, offending, expected, state;
    friend class csparser;

  protected:
    ParseException(int b, int e, int s, int o, int x)
    : begin(b), end(e), offending(o), expected(x), state(s)
    {
    }

  public:
    const wchar_t *getMessage() const
    {
      return offending < 0
           ? L"lexical analysis failed"
           : L"syntax error";
    }

    int getBegin() const {return begin;}
    int getEnd() const {return end;}
    int getState() const {return state;}
    int getOffending() const {return offending;}
    int getExpected() const {return expected;}
  };

  void initialize(const wchar_t *source, EventHandler *parsingEventHandler)
  {
    eventHandler = parsingEventHandler;
    input = source;
    reset(0, 0, 0);
  }

  const wchar_t *getInput() const
  {
    return input;
  }

  int getTokenOffset() const
  {
    return b0;
  }

  int getTokenEnd() const
  {
    return e0;
  }

  void reset(int l, int b, int e)
  {
            b0 = b; e0 = b;
    l1 = l; b1 = b; e1 = e;
    end = e;
    eventHandler->reset(input);
  }

  void reset()
  {
    reset(0, 0, 0);
  }

  static const wchar_t *getOffendingToken(ParseException e)
  {
    return e.getOffending() < 0 ? 0 : TOKEN[e.getOffending()];
  }

  static void getExpectedTokenSet(const ParseException &e, const wchar_t **set, int size)
  {
    if (e.expected < 0)
    {
      getTokenSet(- e.state, set, size);
    }
    else if (size == 1)
    {
      set[0] = 0;
    }
    else if (size > 1)
    {
      set[0] = TOKEN[e.expected];
      set[1] = 0;
    }
  }

  std::wstring getErrorMessage(const ParseException &e)
  {
    std::wstring message(e.getMessage());
    wchar_t buffer[11];
    const wchar_t *found = getOffendingToken(e);
    if (found != 0)
    {
      message += L", found ";
      message += found;
    }
    const wchar_t *expected[64];
    getExpectedTokenSet(e, expected, sizeof expected / sizeof *expected);
    message += L"\nwhile expecting ";
    const wchar_t *delimiter(expected[1] ? L"[" : L"");
    for (const wchar_t **x = expected; *x; ++x)
    {
      message += delimiter;
      message += *x;
      delimiter = L", ";
    }
    message += expected[1] ? L"]\n" : L"\n";
    int size = e.getEnd() - e.getBegin();
    if (size != 0 && found == 0)
    {
      message += L"after successfully scanning ";
//      swprintf(buffer, L"%d", size);
//      message += buffer;
      message += L" characters beginning ";
    }
    int line = 1;
    int column = 1;
    for (int i = 0; i < e.getBegin(); ++i)
    {
      if (input[i] == L'\n')
      {
        ++line;
        column = 1;
      }
      else
      {
        ++column;
      }
    }
    message += L"at line ";
//    swprintf(buffer, L"%d", line);
//    message += buffer;
    message += L", column ";
//    swprintf(buffer, L"%d", column);
//    message += buffer;
    message += L":\n...";
    const wchar_t *w = input + e.getBegin();
    for (int i = 0; i < 64 && *w; ++i)
    {
      message += *w++;
    }
    message += L"...";
    return message;
  }

  void parse_CS()
  {
    eventHandler->startNonterminal(L"CS", e0);
    for (;;)
    {
      lookahead1W(12);              // S^WS | EOF | '"' | '$' | '/' | '[' | 'set'
      if (l1 == 8)                  // EOF
      {
        break;
      }
      whitespace();
      parse_Expr();
    }
    consume(8);                     // EOF
    eventHandler->endNonterminal(L"CS", e0);
  }

private:

  void parse_Expr()
  {
    eventHandler->startNonterminal(L"Expr", e0);
    switch (l1)
    {
    case 17:                        // 'set'
      parse_opt();
      lookahead1W(0);               // Operator | S^WS
      consume(2);                   // Operator
      lookahead1W(5);               // S^WS | '"' | '['
      switch (l1)
      {
      case 9:                       // '"'
        whitespace();
        parse_literal();
        break;
      default:
        whitespace();
        parse_URI();
        break;
      }
      break;
    default:
      parse_items();
      lookahead1W(14);              // Operator | S^WS | EOF | '"' | '$' | '/' | ';' | '[' | 'set'
      if (l1 == 2)                  // Operator
      {
        consume(2);                 // Operator
        lookahead1W(11);            // S^WS | '"' | '$' | '/' | '['
        whitespace();
        parse_items();
        for (;;)
        {
          lookahead1W(14);          // Operator | S^WS | EOF | '"' | '$' | '/' | ';' | '[' | 'set'
          if (l1 != 2)              // Operator
          {
            break;
          }
          consume(2);               // Operator
          lookahead1W(11);          // S^WS | '"' | '$' | '/' | '['
          whitespace();
          parse_items();
        }
      }
      break;
    }
    lookahead1W(13);                // S^WS | EOF | '"' | '$' | '/' | ';' | '[' | 'set'
    if (l1 == 14)                   // ';'
    {
      whitespace();
      parse_Separator();
    }
    eventHandler->endNonterminal(L"Expr", e0);
  }

  void parse_opt()
  {
    eventHandler->startNonterminal(L"opt", e0);
    consume(17);                    // 'set'
    lookahead1W(1);                 // nstring | S^WS
    consume(3);                     // nstring
    eventHandler->endNonterminal(L"opt", e0);
  }

  void parse_items()
  {
    eventHandler->startNonterminal(L"items", e0);
    parse_item();
    lookahead1W(15);                // Operator | S^WS | EOF | '"' | '$' | ',' | '/' | ';' | '[' | 'set'
    if (l1 == 11)                   // ','
    {
      consume(11);                  // ','
      lookahead1W(11);              // S^WS | '"' | '$' | '/' | '['
      whitespace();
      parse_item();
    }
    eventHandler->endNonterminal(L"items", e0);
  }

  void parse_item()
  {
    eventHandler->startNonterminal(L"item", e0);
    switch (l1)
    {
    case 15:                        // '['
      parse_URI();
      break;
    case 10:                        // '$'
      parse_var();
      break;
    case 12:                        // '/'
      parse_segment();
      break;
    default:
      parse_literal();
      break;
    }
    eventHandler->endNonterminal(L"item", e0);
  }

  void parse_Separator()
  {
    eventHandler->startNonterminal(L"Separator", e0);
    consume(14);                    // ';'
    eventHandler->endNonterminal(L"Separator", e0);
  }

  void parse_literal()
  {
    eventHandler->startNonterminal(L"literal", e0);
    consume(9);                     // '"'
    lookahead1W(3);                 // astring | S^WS
    consume(5);                     // astring
    lookahead1W(4);                 // S^WS | '"'
    consume(9);                     // '"'
    eventHandler->endNonterminal(L"literal", e0);
  }

  void parse_var()
  {
    eventHandler->startNonterminal(L"var", e0);
    consume(10);                    // '$'
    lookahead1W(1);                 // nstring | S^WS
    consume(3);                     // nstring
    eventHandler->endNonterminal(L"var", e0);
  }

  void parse_URI()
  {
    eventHandler->startNonterminal(L"URI", e0);
    consume(15);                    // '['
    lookahead1W(7);                 // scheme | S^WS | '/' | ']'
    if (l1 == 1)                    // scheme
    {
      consume(1);                   // scheme
      lookahead1W(8);               // nstring | S^WS | '/' | ']'
      if (l1 == 3)                  // nstring
      {
        whitespace();
        parse_hostport();
      }
    }
    whitespace();
    parse_path();
    consume(16);                    // ']'
    eventHandler->endNonterminal(L"URI", e0);
  }

  void parse_hostport()
  {
    eventHandler->startNonterminal(L"hostport", e0);
    parse_host();
    lookahead1W(10);                // S^WS | '/' | ':' | ']'
    if (l1 == 13)                   // ':'
    {
      consume(13);                  // ':'
      lookahead1W(9);               // digit | S^WS | '/' | ']'
      whitespace();
      parse_port();
    }
    eventHandler->endNonterminal(L"hostport", e0);
  }

  void parse_host()
  {
    eventHandler->startNonterminal(L"host", e0);
    consume(3);                     // nstring
    eventHandler->endNonterminal(L"host", e0);
  }

  void parse_port()
  {
    eventHandler->startNonterminal(L"port", e0);
    for (;;)
    {
      lookahead1W(9);               // digit | S^WS | '/' | ']'
      if (l1 != 6)                  // digit
      {
        break;
      }
      consume(6);                   // digit
    }
    eventHandler->endNonterminal(L"port", e0);
  }

  void parse_path()
  {
    eventHandler->startNonterminal(L"path", e0);
    for (;;)
    {
      lookahead1W(6);               // S^WS | '/' | ']'
      if (l1 != 12)                 // '/'
      {
        break;
      }
      whitespace();
      parse_segment();
    }
    eventHandler->endNonterminal(L"path", e0);
  }

  void parse_segment()
  {
    eventHandler->startNonterminal(L"segment", e0);
    consume(12);                    // '/'
    lookahead1W(2);                 // string | S^WS
    consume(4);                     // string
    eventHandler->endNonterminal(L"segment", e0);
  }

  void consume(int t)
  {
    if (l1 == t)
    {
      whitespace();
      eventHandler->terminal(TOKEN[l1], b1, e1);
      b0 = b1; e0 = e1; l1 = 0;
    }
    else
    {
      error(b1, e1, 0, l1, t);
    }
  }

  void whitespace()
  {
    if (e0 != b1)
    {
      eventHandler->whitespace(e0, b1);
      e0 = b1;
    }
  }

  int matchW(int set)
  {
    int code;
    for (;;)
    {
      code = match(set);
      if (code != 7)                // S^WS
      {
        break;
      }
    }
    return code;
  }

  void lookahead1W(int set)
  {
    if (l1 == 0)
    {
      l1 = matchW(set);
      b1 = begin;
      e1 = end;
    }
  }

  int error(int b, int e, int s, int l, int t)
  {
    throw ParseException(b, e, s, l, t);
  }

  int     b0, e0;
  int l1, b1, e1;
  EventHandler *eventHandler;

  const wchar_t *input;
  int begin;
  int end;

  int match(int tokenSetId)
  {
    begin = end;
    int current = end;
    int result = INITIAL[tokenSetId];
    int state = 0;

    for (int code = result & 63; code != 0; )
    {
      int charclass;
      int c0 = input[current];
      ++current;
      if (c0 < 0x80)
      {
        charclass = MAP0[c0];
      }
      else if (c0 < 0xd800)
      {
        int c1 = c0 >> 5;
        charclass = MAP1[(c0 & 31) + MAP1[(c1 & 31) + MAP1[c1 >> 5]]];
      }
      else
      {
        charclass = 0;
      }

      state = code;
      int i0 = (charclass << 6) + code - 1;
      code = TRANSITION[(i0 & 7) + TRANSITION[i0 >> 3]];
      if (code > 63)
      {
        result = code;
        code &= 63;
        end = current;
      }
    }

    result >>= 6;
    if (result == 0)
    {
      end = current - 1;
      int c1 = input[end];
      if (c1 >= 0xdc00 && c1 < 0xe000) --end;
      return error(begin, end, state, -1, -1);
    }

    if (input[begin] == 0) end = begin;
    return (result & 31) - 1;
  }

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

  static void getTokenSet(int tokenSetId, const wchar_t **set, int size)
  {
    int s = tokenSetId < 0 ? - tokenSetId : INITIAL[tokenSetId] & 63;
    for (int i = 0; i < 18; i += 32)
    {
      int j = i;
      for (unsigned int f = ec(i >> 5, s); f != 0; f >>= 1, ++j)
      {
        if ((f & 1) != 0)
        {
          if (size > 1)
          {
            set[0] = TOKEN[j];
            ++set;
            --size;
          }
        }
      }
    }
    if (size > 0)
    {
      set[0] = 0;
    }
  }

  static int ec(int t, int s)
  {
    int i0 = t * 34 + s - 1;
    return EXPECTED[i0];
  }

  static const int MAP0[];
  static const int MAP1[];
  static const int INITIAL[];
  static const int TRANSITION[];
  static const int EXPECTED[];
  static const wchar_t *TOKEN[];
};

const int csparser::MAP0[] =
{
/*   0 */ 25, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 0,
/*  36 */ 4, 0, 0, 0, 0, 0, 0, 0, 5, 0, 6, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 10, 0, 11, 12, 13, 0, 14, 14, 14, 14, 14,
/*  70 */ 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 0, 16, 0, 6, 0, 14,
/*  98 */ 14, 14, 14, 17, 18, 14, 19, 20, 14, 14, 21, 14, 14, 14, 22, 14, 14, 23, 24, 14, 14, 14, 14, 14, 14, 0, 13, 0,
/* 126 */ 0, 0
};

const int csparser::MAP1[] =
{
/*   0 */ 54, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
/*  27 */ 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
/*  54 */ 90, 136, 199, 168, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
/*  76 */ 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 25, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
/* 103 */ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 0,
/* 140 */ 4, 0, 0, 0, 0, 0, 0, 0, 5, 0, 6, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 10, 0, 11, 12, 13, 0, 14, 14, 14, 14, 17,
/* 174 */ 18, 14, 19, 20, 14, 14, 21, 14, 14, 14, 22, 14, 14, 23, 24, 14, 14, 14, 14, 14, 14, 0, 13, 0, 0, 0, 14, 14,
/* 202 */ 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 0, 16, 0,
/* 230 */ 6
};

const int csparser::INITIAL[] =
{
/*  0 */ 1, 2, 323, 388, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
};

const int csparser::TRANSITION[] =
{
/*   0 */ 216, 216, 216, 216, 216, 216, 216, 216, 208, 208, 215, 216, 216, 216, 216, 216, 225, 422, 328, 216, 216, 216,
/*  22 */ 216, 216, 310, 237, 216, 216, 216, 216, 216, 216, 216, 245, 216, 216, 216, 216, 216, 216, 216, 347, 216, 216,
/*  44 */ 216, 216, 216, 216, 253, 216, 410, 216, 216, 216, 216, 216, 366, 265, 216, 216, 273, 216, 216, 216, 253, 283,
/*  66 */ 410, 216, 216, 216, 216, 216, 216, 293, 216, 275, 216, 216, 216, 216, 216, 450, 216, 216, 216, 216, 216, 216,
/*  88 */ 225, 422, 304, 216, 216, 216, 216, 216, 324, 296, 336, 216, 216, 216, 216, 216, 338, 285, 328, 216, 216, 216,
/* 110 */ 216, 216, 412, 346, 410, 216, 216, 216, 216, 216, 379, 355, 216, 216, 216, 216, 216, 216, 229, 363, 216, 216,
/* 132 */ 216, 216, 216, 216, 412, 346, 410, 374, 216, 216, 216, 216, 387, 346, 410, 216, 216, 216, 216, 216, 395, 346,
/* 154 */ 410, 216, 216, 216, 216, 216, 412, 346, 403, 216, 216, 216, 216, 216, 412, 346, 410, 420, 216, 216, 216, 216,
/* 176 */ 412, 346, 410, 257, 216, 216, 216, 216, 412, 430, 410, 217, 216, 216, 216, 216, 412, 346, 438, 446, 216, 216,
/* 198 */ 216, 216, 216, 316, 216, 216, 216, 216, 216, 216, 529, 529, 529, 529, 529, 529, 529, 529, 0, 0, 0, 0, 0, 0, 0,
/* 223 */ 0, 31, 210, 0, 0, 406, 0, 0, 0, 0, 0, 0, 1088, 1088, 0, 0, 0, 640, 640, 640, 640, 640, 0, 0, 0, 704, 704, 704,
/* 251 */ 704, 704, 0, 0, 341, 406, 0, 0, 0, 0, 0, 32, 0, 0, 832, 832, 832, 832, 832, 832, 832, 832, 34, 128, 0, 0, 0,
/* 278 */ 0, 0, 0, 33, 33, 0, 448, 0, 0, 0, 0, 0, 0, 192, 192, 0, 0, 896, 0, 0, 0, 0, 0, 0, 211, 211, 0, 192, 0, 0, 0,
/* 309 */ 406, 0, 0, 0, 0, 640, 640, 0, 0, 0, 0, 576, 576, 576, 576, 211, 0, 0, 406, 0, 0, 0, 0, 0, 406, 0, 0, 0, 0,
/* 338 */ 192, 0, 0, 406, 0, 0, 0, 0, 276, 0, 0, 0, 0, 0, 0, 0, 768, 0, 0, 0, 1024, 1024, 1024, 1024, 1024, 1088, 1088,
/* 365 */ 1088, 0, 0, 0, 0, 0, 0, 832, 832, 28, 0, 0, 0, 31, 0, 0, 0, 0, 0, 1024, 0, 0, 0, 276, 341, 406, 0, 0, 0, 23,
/* 395 */ 0, 276, 341, 406, 0, 0, 0, 24, 0, 0, 0, 276, 341, 406, 26, 0, 0, 0, 276, 341, 406, 0, 0, 0, 0, 0, 29, 0, 0, 0,
/* 425 */ 0, 0, 0, 210, 210, 276, 0, 0, 0, 25, 25, 25, 25, 0, 0, 0, 276, 341, 406, 0, 27, 0, 0, 30, 1152, 0, 0, 0, 0, 0,
/* 455 */ 960, 960, 960
};

const int csparser::EXPECTED[] =
{
/*  0 */ 132, 136, 144, 160, 640, 33408, 69760, 69762, 69768, 69824, 77952, 38528, 169856, 186240, 186244, 188292, 128,
/* 17 */ 4, 4, 8, 16, 32, 2, 2, 131072, 2, 2, 131072, 2, 2, 2, 2, 2, 2
};

const wchar_t *csparser::TOKEN[] =
{
  L"(0)",
  L"scheme",
  L"Operator",
  L"nstring",
  L"string",
  L"astring",
  L"digit",
  L"S",
  L"EOF",
  L"'\"'",
  L"'$'",
  L"','",
  L"'/'",
  L"':'",
  L"';'",
  L"'['",
  L"']'",
  L"'set'"
};

#endif

// End
