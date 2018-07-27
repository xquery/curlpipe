// This file was generated on Mon Jul 23, 2018 14:04 (UTC+02) by REx v5.47 which is Copyright (c) 1979-2017 by Gunther Rademacher <grd@gmx.net>
// REx command line: csparser.ebnf -tree -cpp

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
    l2 = 0;
    l3 = 0;
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
    lookahead1W(5);                 // S^WS | string | EOF
    whitespace();
    parse_Exprs();
    consume(4);                     // EOF
    eventHandler->endNonterminal(L"CS", e0);
  }

private:

  void parse_Exprs()
  {
    eventHandler->startNonterminal(L"Exprs", e0);
    for (;;)
    {
      lookahead1W(5);               // S^WS | string | EOF
      if (l1 != 3)                  // string
      {
        break;
      }
      whitespace();
      parse_Expr();
    }
    eventHandler->endNonterminal(L"Exprs", e0);
  }

  void parse_Expr()
  {
    eventHandler->startNonterminal(L"Expr", e0);
    parse_URI();
    lookahead1W(7);                 // S^WS | '>' | '|'
    whitespace();
    parse_Operator();
    lookahead1W(1);                 // S^WS | string
    whitespace();
    parse_URI();
    lookahead1W(8);                 // S^WS | string | EOF | ';'
    if (l1 == 9)                    // ';'
    {
      whitespace();
      parse_Separator();
    }
    eventHandler->endNonterminal(L"Expr", e0);
  }

  void parse_Operator()
  {
    eventHandler->startNonterminal(L"Operator", e0);
    switch (l1)
    {
    case 13:                        // '|'
      consume(13);                  // '|'
      break;
    default:
      consume(10);                  // '>'
      break;
    }
    eventHandler->endNonterminal(L"Operator", e0);
  }

  void parse_Separator()
  {
    eventHandler->startNonterminal(L"Separator", e0);
    consume(9);                     // ';'
    eventHandler->endNonterminal(L"Separator", e0);
  }

  void parse_URI()
  {
    eventHandler->startNonterminal(L"URI", e0);
    parse_scheme();
    lookahead1W(13);                // S^WS | string | EOF | '#' | '/' | ':' | ';' | '>' | '?' | '|'
    switch (l1)
    {
    case 3:                         // string
      lookahead2W(15);              // S^WS | string | EOF | '#' | '/' | ':' | '://' | ';' | '>' | '?' | '@' | '|'
      switch (lk)
      {
      case 227:                     // string ':'
        lookahead3W(4);             // IntegerLiteral | S^WS | string
        break;
      }
      break;
    default:
      lk = l1;
      break;
    }
    if (lk == 387                   // string '@'
     || lk == 3299)                 // string ':' string
    {
      whitespace();
      parse_authority();
    }
    lookahead1W(13);                // S^WS | string | EOF | '#' | '/' | ':' | ';' | '>' | '?' | '|'
    switch (l1)
    {
    case 3:                         // string
      lookahead2W(14);              // S^WS | string | EOF | '#' | '/' | ':' | '://' | ';' | '>' | '?' | '|'
      break;
    default:
      lk = l1;
      break;
    }
    if (lk == 99                    // string string
     || lk == 131                   // string EOF
     || lk == 163                   // string '#'
     || lk == 195                   // string '/'
     || lk == 227                   // string ':'
     || lk == 291                   // string ';'
     || lk == 323                   // string '>'
     || lk == 355                   // string '?'
     || lk == 419)                  // string '|'
    {
      whitespace();
      parse_host();
    }
    lookahead1W(13);                // S^WS | string | EOF | '#' | '/' | ':' | ';' | '>' | '?' | '|'
    if (l1 == 7)                    // ':'
    {
      whitespace();
      parse_port();
    }
    lookahead1W(12);                // S^WS | string | EOF | '#' | '/' | ';' | '>' | '?' | '|'
    whitespace();
    parse_path();
    if (l1 == 11)                   // '?'
    {
      whitespace();
      parse_query();
    }
    lookahead1W(10);                // S^WS | string | EOF | '#' | ';' | '>' | '|'
    if (l1 == 5)                    // '#'
    {
      whitespace();
      parse_fragment();
    }
    eventHandler->endNonterminal(L"URI", e0);
  }

  void parse_scheme()
  {
    eventHandler->startNonterminal(L"scheme", e0);
    consume(3);                     // string
    lookahead1W(2);                 // S^WS | '://'
    consume(8);                     // '://'
    eventHandler->endNonterminal(L"scheme", e0);
  }

  void parse_authority()
  {
    eventHandler->startNonterminal(L"authority", e0);
    parse_user();
    lookahead1W(6);                 // S^WS | ':' | '@'
    if (l1 == 7)                    // ':'
    {
      consume(7);                   // ':'
      lookahead1W(1);               // S^WS | string
      whitespace();
      parse_pass();
    }
    lookahead1W(3);                 // S^WS | '@'
    consume(12);                    // '@'
    eventHandler->endNonterminal(L"authority", e0);
  }

  void parse_user()
  {
    eventHandler->startNonterminal(L"user", e0);
    consume(3);                     // string
    eventHandler->endNonterminal(L"user", e0);
  }

  void parse_pass()
  {
    eventHandler->startNonterminal(L"pass", e0);
    consume(3);                     // string
    eventHandler->endNonterminal(L"pass", e0);
  }

  void parse_host()
  {
    eventHandler->startNonterminal(L"host", e0);
    consume(3);                     // string
    eventHandler->endNonterminal(L"host", e0);
  }

  void parse_port()
  {
    eventHandler->startNonterminal(L"port", e0);
    consume(7);                     // ':'
    lookahead1W(0);                 // IntegerLiteral | S^WS
    consume(1);                     // IntegerLiteral
    eventHandler->endNonterminal(L"port", e0);
  }

  void parse_path()
  {
    eventHandler->startNonterminal(L"path", e0);
    for (;;)
    {
      lookahead1W(12);              // S^WS | string | EOF | '#' | '/' | ';' | '>' | '?' | '|'
      if (l1 != 6)                  // '/'
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
    consume(6);                     // '/'
    lookahead1W(1);                 // S^WS | string
    consume(3);                     // string
    eventHandler->endNonterminal(L"segment", e0);
  }

  void parse_query()
  {
    eventHandler->startNonterminal(L"query", e0);
    consume(11);                    // '?'
    eventHandler->endNonterminal(L"query", e0);
  }

  void parse_fragment()
  {
    eventHandler->startNonterminal(L"fragment", e0);
    consume(5);                     // '#'
    lookahead1W(9);                 // S^WS | string | EOF | ';' | '>' | '|'
    switch (l1)
    {
    case 3:                         // string
      lookahead2W(11);              // S^WS | string | EOF | '://' | ';' | '>' | '|'
      break;
    default:
      lk = l1;
      break;
    }
    if (lk == 99                    // string string
     || lk == 131                   // string EOF
     || lk == 291                   // string ';'
     || lk == 323                   // string '>'
     || lk == 419)                  // string '|'
    {
      consume(3);                   // string
    }
    eventHandler->endNonterminal(L"fragment", e0);
  }

  void consume(int t)
  {
    if (l1 == t)
    {
      whitespace();
      eventHandler->terminal(TOKEN[l1], b1, e1);
      b0 = b1; e0 = e1; l1 = l2; if (l1 != 0) {
      b1 = b2; e1 = e2; l2 = l3; if (l2 != 0) {
      b2 = b3; e2 = e3; l3 = 0; }}
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
      if (code != 2)                // S^WS
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

  void lookahead2W(int set)
  {
    if (l2 == 0)
    {
      l2 = matchW(set);
      b2 = begin;
      e2 = end;
    }
    lk = (l2 << 5) | l1;
  }

  void lookahead3W(int set)
  {
    if (l3 == 0)
    {
      l3 = matchW(set);
      b3 = begin;
      e3 = end;
    }
    lk |= l3 << 10;
  }

  int error(int b, int e, int s, int l, int t)
  {
    throw ParseException(b, e, s, l, t);
  }

  int lk, b0, e0;
  int l1, b1, e1;
  int l2, b2, e2;
  int l3, b3, e3;
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

    for (int code = result & 31; code != 0; )
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
        int c1 = c0 >> 4;
        charclass = MAP1[(c0 & 15) + MAP1[(c1 & 63) + MAP1[c1 >> 6]]];
      }
      else
      {
        charclass = 0;
      }

      state = code;
      int i0 = (charclass << 5) + code - 1;
      code = TRANSITION[(i0 & 3) + TRANSITION[i0 >> 2]];
      if (code > 31)
      {
        result = code;
        code &= 31;
        end = current;
      }
    }

    result >>= 5;
    if (result == 0)
    {
      end = current - 1;
      int c1 = input[end];
      if (c1 >= 0xdc00 && c1 < 0xe000) --end;
      return error(begin, end, state, -1, -1);
    }

    if (input[begin] == 0) end = begin;
    return (result & 15) - 1;
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
    int s = tokenSetId < 0 ? - tokenSetId : INITIAL[tokenSetId] & 31;
    for (int i = 0; i < 14; i += 32)
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
    int i0 = t * 21 + s - 1;
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
/*   0 */ 12, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2,
/*  36 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 0, 0, 8, 9, 10, 3, 3, 3, 3, 3, 3, 3,
/*  72 */ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
/* 109 */ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 11, 0, 0, 0
};

const int csparser::MAP1[] =
{
/*   0 */ 54, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
/*  27 */ 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
/*  54 */ 126, 155, 139, 191, 207, 212, 170, 175, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
/*  76 */ 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
/*  98 */ 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
/* 120 */ 155, 155, 155, 155, 155, 155, 12, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 152 */ 0, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 11,
/* 188 */ 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 0, 0, 8, 9, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0,
/* 224 */ 0, 0, 0, 0
};

const int csparser::INITIAL[] =
{
/*  0 */ 1, 130, 3, 4, 133, 134, 7, 8, 137, 138, 139, 140, 141, 142, 143, 144
};

const int csparser::TRANSITION[] =
{
/*   0 */ 108, 108, 108, 108, 108, 108, 108, 108, 104, 104, 104, 104, 107, 108, 108, 108, 108, 108, 145, 114, 108, 108,
/*  22 */ 108, 108, 149, 120, 118, 118, 148, 108, 108, 108, 108, 108, 108, 127, 132, 131, 108, 108, 137, 137, 108, 108,
/*  44 */ 136, 108, 108, 108, 110, 142, 109, 153, 108, 108, 108, 108, 108, 108, 157, 157, 108, 108, 108, 108, 108, 138,
/*  66 */ 161, 162, 108, 108, 108, 108, 108, 108, 108, 166, 108, 108, 108, 108, 122, 123, 108, 122, 108, 108, 108, 108,
/*  88 */ 108, 172, 174, 175, 108, 108, 108, 108, 108, 170, 179, 179, 108, 108, 108, 108, 113, 113, 113, 113, 0, 0, 0,
/* 111 */ 0, 20, 0, 192, 192, 192, 192, 147, 147, 147, 147, 0, 0, 0, 416, 0, 224, 224, 224, 224, 288, 0, 0, 0, 21, 0,
/* 137 */ 82, 0, 0, 0, 352, 0, 0, 256, 0, 0, 192, 0, 0, 147, 0, 0, 0, 256, 276, 276, 320, 320, 320, 320, 0, 352, 352,
/* 164 */ 352, 352, 384, 384, 384, 384, 0, 160, 0, 0, 0, 448, 448, 448, 448, 160, 160, 160, 160
};

const int csparser::EXPECTED[] =
{
/*  0 */ 6, 12, 260, 4100, 14, 28, 4228, 9220, 540, 9756, 9788, 10012, 11900, 12028, 12284, 16380, 4, 2, 8, 256, 256
};

const wchar_t *csparser::TOKEN[] =
{
  L"(0)",
  L"IntegerLiteral",
  L"S",
  L"string",
  L"EOF",
  L"'#'",
  L"'/'",
  L"':'",
  L"'://'",
  L"';'",
  L"'>'",
  L"'?'",
  L"'@'",
  L"'|'"
};

#endif

// End
