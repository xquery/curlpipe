// This file was generated on Sat Nov 17, 2018 09:13 (UTC+01) by REx v5.47 which is Copyright (c) 1979-2017 by Gunther Rademacher <grd@gmx.net>
// REx command line: csparser.ebnf -name csparser -cpp -tree -faster

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
      lookahead1W(9);               // S^WS | EOF | '"' | '//' | '['
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
    if (l1 == 13)                   // '//'
    {
      parse_comments();
    }
    whitespace();
    parse_items();
    if (l1 == 2)                    // Operator
    {
      whitespace();
      parse_statement();
      for (;;)
      {
        if (l1 != 2)                // Operator
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
    consume(2);                     // Operator
    lookahead1W(5);                 // S^WS | '"' | '['
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
      lookahead1W(16);              // Operator | S^WS | EOF | '"' | ',' | '//' | ';' | '['
      if (l1 != 12)                 // ','
      {
        break;
      }
      consume(12);                  // ','
      lookahead1W(5);               // S^WS | '"' | '['
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
    lookahead1W(1);                 // astring | S^WS
    consume(5);                     // astring
    lookahead1W(3);                 // S^WS | '"'
    consume(10);                    // '"'
    eventHandler->endNonterminal(L"literal", e0);
  }

  void parse_URI()
  {
    eventHandler->startNonterminal(L"URI", e0);
    consume(17);                    // '['
    lookahead1W(15);                // scheme | nstring | path_delim | S^WS | '"' | '#' | '?' | ']'
    if (l1 == 1)                    // scheme
    {
      consume(1);                   // scheme
    }
    lookahead1W(11);                // nstring | path_delim | S^WS | '"' | '#' | '?' | ']'
    if (l1 == 3)                    // nstring
    {
      whitespace();
      parse_hostport();
    }
    for (;;)
    {
      lookahead1W(10);              // path_delim | S^WS | '"' | '#' | '?' | ']'
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
    lookahead1W(8);                 // S^WS | '"' | '?' | ']'
    if (l1 == 16)                   // '?'
    {
      whitespace();
      parse_query();
    }
    lookahead1W(6);                 // S^WS | '"' | ']'
    whitespace();
    parse_headers();
    consume(18);                    // ']'
    eventHandler->endNonterminal(L"URI", e0);
  }

  void parse_header()
  {
    eventHandler->startNonterminal(L"header", e0);
    parse_literal();
    lookahead1W(4);                 // S^WS | ':'
    consume(14);                    // ':'
    lookahead1W(3);                 // S^WS | '"'
    whitespace();
    parse_literal();
    eventHandler->endNonterminal(L"header", e0);
  }

  void parse_headers()
  {
    eventHandler->startNonterminal(L"headers", e0);
    for (;;)
    {
      lookahead1W(6);               // S^WS | '"' | ']'
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
    consume(3);                     // nstring
    eventHandler->endNonterminal(L"host", e0);
  }

  void parse_port()
  {
    eventHandler->startNonterminal(L"port", e0);
    consume(7);                     // digit
    for (;;)
    {
      lookahead1W(13);              // path_delim | digit | S^WS | '"' | '#' | '?' | ']'
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
    lookahead1W(14);                // path_delim | S^WS | '"' | '#' | ':' | '?' | ']'
    if (l1 == 14)                   // ':'
    {
      consume(14);                  // ':'
      lookahead1W(2);               // digit | S^WS
      whitespace();
      parse_port();
    }
    eventHandler->endNonterminal(L"hostport", e0);
  }

  void parse_segment()
  {
    eventHandler->startNonterminal(L"segment", e0);
    consume(6);                     // path_delim
    lookahead1W(0);                 // pstring | S^WS
    consume(4);                     // pstring
    lookahead1W(10);                // path_delim | S^WS | '"' | '#' | '?' | ']'
    switch (l1)
    {
    case 6:                         // path_delim
      lookahead2W(12);              // pstring | path_delim | S^WS | '"' | '#' | '?' | ']'
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
    lookahead1W(1);                 // astring | S^WS
    consume(5);                     // astring
    eventHandler->endNonterminal(L"query", e0);
  }

  void parse_fragment()
  {
    eventHandler->startNonterminal(L"fragment", e0);
    consume(11);                    // '#'
    lookahead1W(1);                 // astring | S^WS
    consume(5);                     // astring
    eventHandler->endNonterminal(L"fragment", e0);
  }

  void parse_comments()
  {
    eventHandler->startNonterminal(L"comments", e0);
    consume(13);                    // '//'
    lookahead1W(1);                 // astring | S^WS
    consume(5);                     // astring
    for (;;)
    {
      lookahead1W(7);               // astring | S^WS | '"' | '['
      if (l1 != 5)                  // astring
      {
        break;
      }
      consume(5);                   // astring
    }
    eventHandler->endNonterminal(L"comments", e0);
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
    int i0 = t * 37 + s - 1;
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
/*  36 */ 0, 0, 5, 6, 0, 0, 0, 0, 7, 6, 6, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 11, 12, 13, 14, 15, 0, 16, 16, 16, 16,
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
/* 140 */ 0, 0, 5, 6, 0, 0, 0, 0, 7, 6, 6, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 11, 12, 13, 14, 15, 0, 16, 16, 16, 16,
/* 173 */ 19, 20, 16, 21, 22, 16, 16, 23, 16, 16, 16, 24, 16, 16, 25, 26, 16, 16, 16, 16, 16, 16, 6, 27, 6, 0, 0, 16,
/* 201 */ 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 0, 18,
/* 229 */ 0, 6
};

const int csparser::INITIAL[] =
{
/*  0 */ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17
};

const int csparser::TRANSITION[] =
{
/*   0 */ 274, 274, 274, 274, 274, 274, 274, 274, 232, 232, 238, 274, 274, 274, 274, 274, 252, 479, 249, 274, 274, 274,
/*  22 */ 274, 274, 260, 265, 273, 274, 274, 274, 274, 274, 274, 282, 274, 274, 274, 274, 274, 274, 252, 479, 290, 335,
/*  44 */ 340, 274, 274, 274, 252, 479, 290, 454, 340, 274, 274, 274, 252, 479, 314, 454, 340, 274, 274, 274, 241, 351,
/*  66 */ 359, 450, 394, 274, 274, 274, 373, 306, 290, 454, 340, 274, 274, 274, 381, 532, 389, 274, 407, 274, 274, 274,
/*  88 */ 252, 479, 418, 454, 340, 274, 274, 274, 426, 552, 434, 441, 446, 274, 274, 274, 252, 479, 462, 475, 274, 274,
/* 110 */ 274, 274, 252, 479, 487, 328, 340, 274, 274, 274, 274, 495, 274, 274, 274, 274, 274, 274, 252, 399, 290, 454,
/* 132 */ 340, 274, 274, 274, 410, 503, 504, 274, 274, 274, 274, 274, 343, 512, 274, 274, 274, 274, 274, 274, 252, 399,
/* 154 */ 290, 297, 340, 274, 274, 274, 252, 467, 290, 454, 340, 274, 274, 274, 252, 365, 290, 454, 340, 274, 274, 274,
/* 176 */ 252, 399, 520, 454, 340, 274, 274, 274, 252, 399, 290, 321, 340, 274, 274, 274, 252, 399, 290, 454, 528, 274,
/* 198 */ 274, 274, 252, 399, 290, 454, 302, 274, 274, 274, 252, 399, 290, 540, 340, 274, 274, 274, 252, 479, 389, 548,
/* 220 */ 274, 274, 274, 274, 274, 560, 561, 274, 274, 274, 274, 274, 594, 594, 594, 594, 594, 594, 594, 594, 0, 0, 405,
/* 243 */ 0, 0, 0, 0, 0, 405, 218, 0, 339, 339, 405, 0, 0, 0, 0, 0, 405, 0, 0, 0, 704, 0, 704, 704, 704, 704, 704, 704,
/* 271 */ 704, 704, 704, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 768, 768, 768, 768, 768, 768, 0, 0, 339, 339, 405, 0, 279, 279,
/* 298 */ 0, 0, 0, 279, 279, 279, 290, 0, 0, 0, 0, 0, 339, 512, 0, 0, 832, 0, 339, 339, 405, 0, 279, 279, 0, 0, 0, 279,
/* 326 */ 288, 279, 279, 0, 0, 192, 279, 279, 279, 279, 0, 192, 0, 279, 279, 279, 279, 0, 0, 0, 0, 0, 0, 1216, 0, 0, 22,
/* 353 */ 448, 448, 448, 448, 448, 448, 22, 0, 0, 339, 405, 896, 0, 0, 0, 279, 339, 0, 0, 281, 339, 405, 512, 0, 0, 0,
/* 379 */ 0, 405, 339, 405, 0, 0, 960, 0, 0, 405, 192, 0, 339, 339, 405, 0, 0, 0, 37, 128, 0, 0, 0, 279, 339, 0, 0, 279,
/* 407 */ 0, 36, 36, 0, 0, 0, 0, 0, 1152, 0, 1152, 1024, 0, 339, 339, 405, 0, 279, 279, 340, 405, 0, 0, 0, 0, 0, 405, 0,
/* 435 */ 0, 340, 340, 405, 0, 285, 285, 0, 0, 0, 285, 285, 285, 285, 0, 0, 0, 0, 0, 279, 0, 0, 0, 279, 279, 279, 279,
/* 462 */ 219, 0, 339, 339, 405, 0, 0, 0, 279, 339, 0, 0, 280, 0, 192, 192, 0, 0, 0, 0, 0, 339, 0, 0, 0, 220, 0, 339,
/* 490 */ 339, 405, 0, 279, 279, 1088, 0, 1088, 1088, 1088, 1088, 1088, 1088, 0, 1152, 0, 0, 0, 0, 0, 0, 0, 1216, 0,
/* 514 */ 1216, 1216, 1216, 1216, 1216, 1216, 0, 0, 339, 339, 405, 0, 279, 286, 291, 279, 279, 0, 0, 0, 0, 0, 339, 0,
/* 538 */ 960, 0, 287, 0, 0, 0, 279, 279, 289, 279, 0, 0, 192, 0, 0, 0, 0, 0, 340, 0, 0, 0, 0, 640, 0, 0, 0, 0, 0, 0, 0
};

const int csparser::EXPECTED[] =
{
/*  0 */ 272, 288, 384, 1280, 16640, 132352, 263424, 132384, 328960, 141056, 331072, 331080, 331088, 331200, 347456,
/* 15 */ 331082, 177924, 256, 16, 16, 32, 8192, 8, 10, 10, 4, 4, 4, 8, 10, 10, 10, 10, 10, 10, 2, 2
};

const wchar_t *csparser::TOKEN[] =
{
  L"(0)",
  L"scheme",
  L"Operator",
  L"nstring",
  L"pstring",
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
