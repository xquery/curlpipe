// This file was generated on Thu Nov 15, 2018 18:49 (UTC+01) by REx v5.47 which is Copyright (c) 1979-2017 by Gunther Rademacher <grd@gmx.net>
// REx command line: csparser.ebnf -name csparser -tree -cpp -faster

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
      //swprintf(buffer, L"%d", size);
      message += buffer;
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
    //swprintf(buffer, L"%d", line);
    message += buffer;
    message += L", column ";
    //swprintf(buffer, L"%d", column);
    message += buffer;
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
      lookahead1W(7);               // S^WS | EOF | '"' | '//' | '['
      if (l1 == 9)                  // EOF
      {
        break;
      }
      whitespace();
      parse_Expr();
    }
    consume(9);                     // EOF
    eventHandler->endNonterminal(L"CS", e0);
  }

private:

  void parse_Expr()
  {
    eventHandler->startNonterminal(L"Expr", e0);
    parse_items();
    if (l1 == 3)                    // Operator
    {
      whitespace();
      parse_statement();
      for (;;)
      {
        if (l1 != 3)                // Operator
        {
          break;
        }
        whitespace();
        parse_statement();
      }
    }
    if (l1 == 15)                   // ';'
    {
      whitespace();
      parse_Separator();
    }
    eventHandler->endNonterminal(L"Expr", e0);
  }

  void parse_statement()
  {
    eventHandler->startNonterminal(L"statement", e0);
    consume(3);                     // Operator
    lookahead1W(5);                 // S^WS | '"' | '//' | '['
    whitespace();
    parse_items();
    eventHandler->endNonterminal(L"statement", e0);
  }

  void parse_items()
  {
    eventHandler->startNonterminal(L"items", e0);
    parse_item();
    for (;;)
    {
      lookahead1W(10);              // Operator | S^WS | EOF | '"' | ',' | '//' | ';' | '['
      if (l1 != 12)                 // ','
      {
        break;
      }
      consume(12);                  // ','
      lookahead1W(5);               // S^WS | '"' | '//' | '['
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
    case 17:                        // '['
      parse_URI();
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
    consume(15);                    // ';'
    eventHandler->endNonterminal(L"Separator", e0);
  }

  void parse_literal()
  {
    eventHandler->startNonterminal(L"literal", e0);
    consume(10);                    // '"'
    lookahead1W(0);                 // astring | S^WS | '//'
    consume(5);                     // astring
    lookahead1W(2);                 // S^WS | '"' | '//'
    consume(10);                    // '"'
    eventHandler->endNonterminal(L"literal", e0);
  }

  void parse_URI()
  {
    eventHandler->startNonterminal(L"URI", e0);
    consume(17);                    // '['
    lookahead1W(15);                // scheme | nstring | path_delim | S^WS | '"' | '#' | '//' | '?' | ']'
    if (l1 == 2)                    // scheme
    {
      consume(2);                   // scheme
    }
    lookahead1W(11);                // nstring | path_delim | S^WS | '"' | '#' | '//' | '?' | ']'
    if (l1 == 4)                    // nstring
    {
      whitespace();
      parse_hostport();
    }
    for (;;)
    {
      lookahead1W(9);               // path_delim | S^WS | '"' | '#' | '//' | '?' | ']'
      if (l1 != 6)                  // path_delim
      {
        break;
      }
      whitespace();
      parse_segment();
    }
    if (l1 == 11)                   // '#'
    {
      whitespace();
      parse_fragment();
    }
    lookahead1W(8);                 // S^WS | '"' | '//' | '?' | ']'
    if (l1 == 16)                   // '?'
    {
      whitespace();
      parse_query();
    }
    lookahead1W(6);                 // S^WS | '"' | '//' | ']'
    whitespace();
    parse_headers();
    consume(18);                    // ']'
    eventHandler->endNonterminal(L"URI", e0);
  }

  void parse_header()
  {
    eventHandler->startNonterminal(L"header", e0);
    parse_literal();
    lookahead1W(3);                 // S^WS | '//' | ':'
    consume(14);                    // ':'
    lookahead1W(2);                 // S^WS | '"' | '//'
    whitespace();
    parse_literal();
    eventHandler->endNonterminal(L"header", e0);
  }

  void parse_headers()
  {
    eventHandler->startNonterminal(L"headers", e0);
    for (;;)
    {
      lookahead1W(6);               // S^WS | '"' | '//' | ']'
      if (l1 != 10)                 // '"'
      {
        break;
      }
      whitespace();
      parse_header();
    }
    eventHandler->endNonterminal(L"headers", e0);
  }

  void parse_host()
  {
    eventHandler->startNonterminal(L"host", e0);
    consume(4);                     // nstring
    eventHandler->endNonterminal(L"host", e0);
  }

  void parse_port()
  {
    eventHandler->startNonterminal(L"port", e0);
    consume(7);                     // digit
    for (;;)
    {
      lookahead1W(13);              // path_delim | digit | S^WS | '"' | '#' | '//' | '?' | ']'
      if (l1 != 7)                  // digit
      {
        break;
      }
      consume(7);                   // digit
    }
    eventHandler->endNonterminal(L"port", e0);
  }

  void parse_hostport()
  {
    eventHandler->startNonterminal(L"hostport", e0);
    parse_host();
    lookahead1W(14);                // path_delim | S^WS | '"' | '#' | '//' | ':' | '?' | ']'
    if (l1 == 14)                   // ':'
    {
      consume(14);                  // ':'
      lookahead1W(1);               // digit | S^WS | '//'
      whitespace();
      parse_port();
    }
    eventHandler->endNonterminal(L"hostport", e0);
  }

  void parse_segment()
  {
    eventHandler->startNonterminal(L"segment", e0);
    consume(6);                     // path_delim
    lookahead1W(0);                 // astring | S^WS | '//'
    consume(5);                     // astring
    lookahead1W(9);                 // path_delim | S^WS | '"' | '#' | '//' | '?' | ']'
    switch (l1)
    {
    case 6:                         // path_delim
      lookahead2W(12);              // astring | path_delim | S^WS | '"' | '#' | '//' | '?' | ']'
      break;
    default:
      lk = l1;
      break;
    }
    if (lk == 198                   // path_delim path_delim
     || lk == 326                   // path_delim '"'
     || lk == 358                   // path_delim '#'
     || lk == 518                   // path_delim '?'
     || lk == 582)                  // path_delim ']'
    {
      consume(6);                   // path_delim
    }
    eventHandler->endNonterminal(L"segment", e0);
  }

  void parse_query()
  {
    eventHandler->startNonterminal(L"query", e0);
    consume(16);                    // '?'
    lookahead1W(0);                 // astring | S^WS | '//'
    consume(5);                     // astring
    eventHandler->endNonterminal(L"query", e0);
  }

  void parse_fragment()
  {
    eventHandler->startNonterminal(L"fragment", e0);
    consume(11);                    // '#'
    lookahead1W(0);                 // astring | S^WS | '//'
    consume(5);                     // astring
    eventHandler->endNonterminal(L"fragment", e0);
  }

  void try_comments()
  {
    consumeT(13);                   // '//'
    lookahead1W(0);                 // astring | S^WS | '//'
    consumeT(5);                    // astring
    for (;;)
    {
      lookahead1W(4);               // END | astring | S^WS | '//'
      if (l1 != 5)                  // astring
      {
        break;
      }
      consumeT(5);                  // astring
    }
  }

  void try_whitespace()
  {
    switch (l1)
    {
    case 8:                         // S^WS
      consumeT(8);                  // S^WS
      break;
    default:
      try_comments();
      break;
    }
  }

  void consume(int t)
  {
    if (l1 == t)
    {
      whitespace();
      eventHandler->terminal(TOKEN[l1], b1, e1);
      b0 = b1; e0 = e1; l1 = l2; if (l1 != 0) {
      b1 = b2; e1 = e2; l2 = 0; }
    }
    else
    {
      error(b1, e1, 0, l1, t);
    }
  }

  void consumeT(int t)
  {
    if (l1 == t)
    {
      b0 = b1; e0 = e1; l1 = l2; if (l1 != 0) {
      b1 = b2; e1 = e2; l2 = 0; }
    }
    else
    {
      error(b1, e1, 0, l1, t);
    }
  }

  void skip(int code)
  {
    int b0W = b0; int e0W = e0; int l1W = l1;
    int b1W = b1; int e1W = e1;

    l1 = code; b1 = begin; e1 = end;
    l2 = 0;

    try_whitespace();

    b0 = b0W; e0 = e0W; l1 = l1W; if (l1 != 0) {
    b1 = b1W; e1 = e1W; }
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
      if (code != 8)                // S^WS
      {
        if (code != 13)             // '//'
        {
          break;
        }
        skip(code);
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

  int error(int b, int e, int s, int l, int t)
  {
    throw ParseException(b, e, s, l, t);
  }

  int lk, b0, e0;
  int l1, b1, e1;
  int l2, b2, e2;
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
    for (int i = 0; i < 19; i += 32)
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
/*   0 */ 28, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4,
/*  36 */ 0, 0, 5, 6, 0, 0, 0, 0, 7, 0, 6, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 11, 12, 13, 14, 15, 0, 16, 16, 16, 16,
/*  69 */ 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 0, 18, 0, 6, 0,
/*  97 */ 16, 16, 16, 16, 19, 20, 16, 21, 22, 16, 16, 23, 16, 16, 16, 24, 16, 16, 25, 26, 16, 16, 16, 16, 16, 16, 6, 27,
/* 125 */ 6, 0, 0
};

const int csparser::MAP1[] =
{
/*   0 */ 54, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
/*  27 */ 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
/*  54 */ 90, 136, 199, 168, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
/*  76 */ 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 28, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
/* 103 */ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4,
/* 140 */ 0, 0, 5, 6, 0, 0, 0, 0, 7, 0, 6, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 11, 12, 13, 14, 15, 0, 16, 16, 16, 16,
/* 173 */ 19, 20, 16, 21, 22, 16, 16, 23, 16, 16, 16, 24, 16, 16, 25, 26, 16, 16, 16, 16, 16, 16, 6, 27, 6, 0, 0, 16,
/* 201 */ 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 0, 18,
/* 229 */ 0, 6
};

const int csparser::INITIAL[] =
{
/*  0 */ 1, 2, 3, 4, 129, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};

const int csparser::TRANSITION[] =
{
/*   0 */ 243, 243, 243, 243, 243, 243, 243, 243, 232, 232, 243, 243, 243, 243, 243, 243, 242, 334, 240, 243, 243, 243,
/*  22 */ 243, 243, 392, 396, 243, 243, 243, 243, 243, 243, 243, 252, 243, 243, 243, 243, 243, 243, 242, 459, 287, 302,
/*  44 */ 243, 243, 243, 243, 242, 459, 311, 302, 243, 243, 243, 243, 242, 454, 311, 302, 243, 243, 243, 243, 319, 327,
/*  66 */ 482, 277, 346, 243, 243, 243, 467, 464, 311, 302, 243, 243, 243, 243, 356, 266, 240, 279, 243, 243, 243, 243,
/*  88 */ 242, 520, 311, 302, 243, 243, 243, 243, 364, 361, 372, 378, 243, 243, 243, 243, 242, 426, 386, 243, 243, 243,
/* 110 */ 243, 243, 242, 419, 404, 302, 243, 243, 243, 243, 244, 412, 243, 243, 243, 243, 243, 243, 242, 272, 311, 302,
/* 132 */ 243, 243, 243, 243, 338, 440, 243, 243, 243, 243, 243, 243, 485, 475, 243, 243, 243, 243, 243, 243, 242, 272,
/* 154 */ 311, 293, 243, 243, 243, 243, 242, 526, 311, 302, 243, 243, 243, 243, 242, 432, 311, 302, 243, 243, 243, 243,
/* 176 */ 242, 272, 493, 302, 243, 243, 243, 243, 242, 272, 311, 299, 243, 243, 243, 243, 242, 272, 311, 505, 243, 243,
/* 198 */ 243, 243, 242, 272, 311, 303, 243, 243, 243, 243, 242, 272, 311, 501, 243, 243, 243, 243, 242, 259, 513, 243,
/* 220 */ 243, 243, 243, 243, 348, 447, 243, 243, 243, 243, 243, 243, 592, 592, 592, 592, 592, 592, 592, 592, 401, 0,
/* 242 */ 401, 0, 0, 0, 0, 0, 0, 0, 0, 1088, 768, 0, 768, 768, 768, 768, 768, 0, 256, 0, 401, 0, 0, 0, 0, 256, 0, 401,
/* 270 */ 0, 960, 0, 0, 343, 401, 0, 0, 343, 0, 0, 0, 0, 0, 0, 33, 33, 401, 0, 401, 0, 256, 0, 343, 343, 343, 343, 351,
/* 298 */ 343, 343, 343, 349, 343, 343, 343, 343, 343, 343, 343, 343, 351, 401, 0, 401, 0, 0, 0, 343, 343, 18, 18, 18,
/* 322 */ 18, 18, 18, 18, 18, 466, 18, 466, 466, 466, 466, 466, 0, 276, 0, 401, 0, 0, 0, 0, 1152, 0, 1152, 0, 34, 192,
/* 348 */ 0, 0, 0, 0, 0, 0, 640, 0, 401, 0, 0, 960, 0, 0, 0, 0, 403, 0, 0, 0, 0, 0, 0, 0, 403, 0, 403, 0, 0, 0, 346,
/* 379 */ 346, 346, 346, 346, 346, 346, 346, 401, 0, 401, 256, 256, 0, 0, 0, 704, 0, 704, 704, 704, 704, 704, 704, 704,
/* 403 */ 0, 401, 0, 401, 0, 0, 256, 343, 343, 1088, 0, 1088, 1088, 1088, 1088, 1088, 0, 278, 0, 401, 0, 0, 0, 0, 277,
/* 428 */ 0, 401, 0, 0, 0, 0, 343, 401, 0, 0, 345, 0, 0, 1152, 0, 0, 0, 0, 0, 0, 640, 0, 0, 0, 0, 0, 0, 832, 0, 401, 0,
/* 459 */ 0, 0, 0, 401, 0, 0, 0, 0, 401, 512, 0, 0, 0, 0, 0, 0, 1216, 0, 1216, 1216, 1216, 1216, 1216, 0, 896, 401, 0,
/* 486 */ 0, 0, 0, 0, 1216, 0, 1216, 401, 0, 401, 0, 0, 0, 343, 347, 348, 343, 343, 350, 343, 343, 343, 343, 343, 352,
/* 511 */ 343, 343, 401, 0, 401, 0, 256, 0, 0, 0, 1024, 0, 401, 0, 0, 0, 0, 343, 401, 0, 0, 344, 0
};

const int csparser::EXPECTED[] =
{
/*  0 */ 8480, 8576, 9472, 24832, 140544, 271616, 141056, 337152, 339264, 177928, 339280, 339296, 339392, 355648,
/* 14 */ 339284, 256, 32, 8192, 32, 8, 8, 8, 16, 20, 20, 16, 20, 20, 20, 20, 20, 20, 4, 4
};

const wchar_t *csparser::TOKEN[] =
{
  L"(0)",
  L"END",
  L"scheme",
  L"Operator",
  L"nstring",
  L"astring",
  L"'/'",
  L"digit",
  L"S",
  L"EOF",
  L"'\"'",
  L"'#'",
  L"','",
  L"'//'",
  L"':'",
  L"';'",
  L"'?'",
  L"'['",
  L"']'"
};

#endif

// End
