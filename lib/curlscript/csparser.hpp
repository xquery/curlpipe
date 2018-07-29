// This file was generated on Sun Jul 29, 2018 12:49 (UTC+02) by REx v5.47 which is Copyright (c) 1979-2017 by Gunther Rademacher <grd@gmx.net>
// REx command line: csparser.ebnf -name csparser -cpp -tree

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
      lookahead1W(5);               // S^WS | EOF | '"' | '$' | '/' | '['
      if (l1 == 5)                  // EOF
      {
        break;
      }
      whitespace();
      parse_Expr();
    }
    consume(5);                     // EOF
    eventHandler->endNonterminal(L"CS", e0);
  }

private:

  void parse_Expr()
  {
    eventHandler->startNonterminal(L"Expr", e0);
    parse_item();
    lookahead1W(7);                 // S^WS | '!' | '=' | '>' | '>>' | '?' | '|'
    whitespace();
    parse_Operator();
    lookahead1W(4);                 // S^WS | '"' | '$' | '/' | '['
    whitespace();
    parse_item();
    for (;;)
    {
      lookahead1W(9);               // S^WS | EOF | '!' | '"' | '$' | '/' | ';' | '=' | '>' | '>>' | '?' | '[' | '|'
      if (l1 != 6                   // '!'
       && l1 != 13                  // '='
       && l1 != 14                  // '>'
       && l1 != 15                  // '>>'
       && l1 != 16                  // '?'
       && l1 != 22)                 // '|'
      {
        break;
      }
      whitespace();
      parse_Operator();
      lookahead1W(4);               // S^WS | '"' | '$' | '/' | '['
      whitespace();
      parse_item();
    }
    if (l1 == 12)                   // ';'
    {
      whitespace();
      parse_Separator();
    }
    eventHandler->endNonterminal(L"Expr", e0);
  }

  void parse_item()
  {
    eventHandler->startNonterminal(L"item", e0);
    switch (l1)
    {
    case 17:                        // '['
      parse_URI();
      break;
    case 8:                         // '$'
      parse_var();
      break;
    case 9:                         // '/'
      parse_segment();
      break;
    default:
      parse_literal();
      break;
    }
    eventHandler->endNonterminal(L"item", e0);
  }

  void parse_Operator()
  {
    eventHandler->startNonterminal(L"Operator", e0);
    switch (l1)
    {
    case 22:                        // '|'
      consume(22);                  // '|'
      break;
    case 13:                        // '='
      consume(13);                  // '='
      break;
    case 15:                        // '>>'
      consume(15);                  // '>>'
      break;
    case 14:                        // '>'
      consume(14);                  // '>'
      break;
    case 6:                         // '!'
      consume(6);                   // '!'
      break;
    default:
      consume(16);                  // '?'
      break;
    }
    eventHandler->endNonterminal(L"Operator", e0);
  }

  void parse_Separator()
  {
    eventHandler->startNonterminal(L"Separator", e0);
    consume(12);                    // ';'
    eventHandler->endNonterminal(L"Separator", e0);
  }

  void parse_literal()
  {
    eventHandler->startNonterminal(L"literal", e0);
    consume(7);                     // '"'
    for (;;)
    {
      lookahead1W(8);               // schar | digit | alpha | S^WS | '!' | '"' | '=' | '>' | '>>' | '?' | '|'
      if (l1 == 7)                  // '"'
      {
        break;
      }
      whitespace();
      parse_all();
    }
    consume(7);                     // '"'
    eventHandler->endNonterminal(L"literal", e0);
  }

  void parse_var()
  {
    eventHandler->startNonterminal(L"var", e0);
    consume(8);                     // '$'
    lookahead1W(0);                 // alpha | S^WS
    whitespace();
    parse_nstring();
    eventHandler->endNonterminal(L"var", e0);
  }

  void parse_URI()
  {
    eventHandler->startNonterminal(L"URI", e0);
    consume(17);                    // '['
    lookahead1W(6);                 // S^WS | '/' | ']' | 'file' | 'http' | 'https'
    if (l1 != 9                     // '/'
     && l1 != 18)                   // ']'
    {
      whitespace();
      parse_scheme();
      lookahead1W(1);               // S^WS | '//'
      whitespace();
      parse_hostport();
    }
    whitespace();
    parse_path();
    consume(18);                    // ']'
    eventHandler->endNonterminal(L"URI", e0);
  }

  void parse_scheme()
  {
    eventHandler->startNonterminal(L"scheme", e0);
    switch (l1)
    {
    case 21:                        // 'https'
      consume(21);                  // 'https'
      break;
    case 20:                        // 'http'
      consume(20);                  // 'http'
      break;
    default:
      consume(19);                  // 'file'
      break;
    }
    lookahead1W(2);                 // S^WS | ':'
    consume(11);                    // ':'
    eventHandler->endNonterminal(L"scheme", e0);
  }

  void parse_hostport()
  {
    eventHandler->startNonterminal(L"hostport", e0);
    consume(10);                    // '//'
    lookahead1W(0);                 // alpha | S^WS
    whitespace();
    parse_host();
    if (l1 == 11)                   // ':'
    {
      consume(11);                  // ':'
      lookahead1W(3);               // digit | S^WS | '/' | ']'
      whitespace();
      parse_port();
    }
    eventHandler->endNonterminal(L"hostport", e0);
  }

  void parse_host()
  {
    eventHandler->startNonterminal(L"host", e0);
    parse_nstring();
    eventHandler->endNonterminal(L"host", e0);
  }

  void parse_port()
  {
    eventHandler->startNonterminal(L"port", e0);
    for (;;)
    {
      lookahead1W(3);               // digit | S^WS | '/' | ']'
      if (l1 != 2)                  // digit
      {
        break;
      }
      consume(2);                   // digit
    }
    eventHandler->endNonterminal(L"port", e0);
  }

  void parse_path()
  {
    eventHandler->startNonterminal(L"path", e0);
    for (;;)
    {
      if (l1 != 9)                  // '/'
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
    consume(9);                     // '/'
    lookahead1W(10);                // schar | digit | alpha | S^WS | EOF | '!' | '"' | '$' | '/' | ';' | '=' | '>' |
                                    // '>>' | '?' | '[' | ']' | '|'
    whitespace();
    parse_string();
    eventHandler->endNonterminal(L"segment", e0);
  }

  void parse_nstring()
  {
    eventHandler->startNonterminal(L"nstring", e0);
    consume(3);                     // alpha
    for (;;)
    {
      lookahead1W(11);              // schar | digit | alpha | S^WS | EOF | '!' | '"' | '$' | '/' | ':' | ';' | '=' |
                                    // '>' | '>>' | '?' | '[' | ']' | '|'
      if (l1 != 1                   // schar
       && l1 != 2                   // digit
       && l1 != 3)                  // alpha
      {
        break;
      }
      switch (l1)
      {
      case 3:                       // alpha
        consume(3);                 // alpha
        break;
      case 1:                       // schar
        consume(1);                 // schar
        break;
      default:
        consume(2);                 // digit
        break;
      }
    }
    eventHandler->endNonterminal(L"nstring", e0);
  }

  void parse_string()
  {
    eventHandler->startNonterminal(L"string", e0);
    for (;;)
    {
      lookahead1W(10);              // schar | digit | alpha | S^WS | EOF | '!' | '"' | '$' | '/' | ';' | '=' | '>' |
                                    // '>>' | '?' | '[' | ']' | '|'
      if (l1 != 1                   // schar
       && l1 != 2                   // digit
       && l1 != 3)                  // alpha
      {
        break;
      }
      whitespace();
      parse_character();
    }
    eventHandler->endNonterminal(L"string", e0);
  }

  void parse_character()
  {
    eventHandler->startNonterminal(L"character", e0);
    switch (l1)
    {
    case 3:                         // alpha
      consume(3);                   // alpha
      break;
    case 2:                         // digit
      consume(2);                   // digit
      break;
    default:
      consume(1);                   // schar
      break;
    }
    eventHandler->endNonterminal(L"character", e0);
  }

  void parse_all()
  {
    eventHandler->startNonterminal(L"all", e0);
    switch (l1)
    {
    case 3:                         // alpha
      consume(3);                   // alpha
      break;
    case 2:                         // digit
      consume(2);                   // digit
      break;
    case 1:                         // schar
      consume(1);                   // schar
      break;
    default:
      parse_Operator();
      break;
    }
    eventHandler->endNonterminal(L"all", e0);
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
      if (code != 4)                // S^WS
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
        int c1 = c0 >> 5;
        charclass = MAP1[(c0 & 31) + MAP1[(c1 & 31) + MAP1[c1 >> 5]]];
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
    int s = tokenSetId < 0 ? - tokenSetId : INITIAL[tokenSetId] & 31;
    for (int i = 0; i < 23; i += 32)
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
    int i0 = t * 22 + s - 1;
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
/*  36 */ 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 9, 0, 10, 11, 12, 0, 13, 13, 13, 13, 13,
/*  70 */ 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 0, 15, 0, 5, 0, 13,
/*  98 */ 13, 13, 13, 16, 17, 13, 18, 19, 13, 13, 20, 13, 13, 13, 21, 13, 13, 22, 23, 13, 13, 13, 13, 13, 13, 0, 24, 0,
/* 126 */ 0, 0
};

const int csparser::MAP1[] =
{
/*   0 */ 54, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
/*  27 */ 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
/*  54 */ 90, 136, 199, 168, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
/*  76 */ 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104, 25, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
/* 103 */ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 0,
/* 140 */ 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 9, 0, 10, 11, 12, 0, 13, 13, 13, 13, 16,
/* 174 */ 17, 13, 18, 19, 13, 13, 20, 13, 13, 13, 21, 13, 13, 22, 23, 13, 13, 13, 13, 13, 13, 0, 24, 0, 0, 0, 13, 13,
/* 202 */ 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 0, 15, 0,
/* 230 */ 5
};

const int csparser::INITIAL[] =
{
/*  0 */ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
};

const int csparser::TRANSITION[] =
{
/*   0 */ 212, 212, 212, 212, 212, 212, 212, 212, 208, 208, 208, 211, 212, 212, 212, 212, 212, 283, 217, 212, 212, 212,
/*  22 */ 212, 212, 212, 223, 221, 212, 212, 212, 212, 212, 212, 231, 229, 212, 212, 212, 212, 212, 212, 212, 237, 212,
/*  44 */ 212, 212, 212, 212, 241, 244, 243, 248, 212, 212, 212, 212, 271, 212, 254, 212, 212, 212, 212, 212, 317, 212,
/*  66 */ 316, 212, 212, 212, 212, 212, 212, 212, 258, 212, 212, 212, 212, 212, 212, 225, 262, 212, 212, 212, 212, 212,
/*  88 */ 212, 233, 266, 212, 270, 212, 212, 212, 212, 250, 275, 212, 212, 212, 212, 212, 282, 212, 279, 212, 212, 212,
/* 110 */ 212, 212, 212, 289, 287, 212, 212, 212, 212, 212, 291, 292, 305, 212, 212, 212, 212, 212, 282, 212, 279, 212,
/* 132 */ 311, 212, 212, 212, 282, 333, 279, 212, 212, 212, 212, 212, 282, 296, 279, 212, 212, 212, 212, 212, 282, 212,
/* 154 */ 279, 299, 212, 212, 212, 212, 282, 212, 279, 212, 309, 212, 212, 212, 282, 212, 279, 212, 212, 315, 212, 212,
/* 176 */ 282, 212, 279, 212, 212, 321, 212, 212, 282, 212, 279, 213, 302, 212, 212, 212, 212, 323, 327, 212, 212, 212,
/* 198 */ 212, 212, 212, 331, 337, 212, 212, 212, 212, 212, 173, 173, 173, 173, 0, 0, 0, 0, 19, 224, 224, 224, 224, 256,
/* 222 */ 256, 256, 256, 0, 0, 0, 448, 0, 288, 288, 288, 0, 0, 0, 497, 64, 0, 64, 64, 0, 14, 0, 320, 320, 320, 0, 0,
/* 249 */ 352, 0, 0, 0, 544, 96, 0, 96, 96, 0, 416, 416, 416, 448, 448, 448, 448, 497, 497, 497, 497, 512, 0, 0, 0, 96,
/* 275 */ 544, 544, 544, 544, 128, 0, 128, 128, 0, 0, 0, 224, 0, 576, 576, 576, 0, 0, 0, 608, 0, 0, 0, 16, 0, 0, 18, 0,
/* 303 */ 0, 21, 0, 0, 608, 608, 0, 20, 0, 0, 0, 640, 694, 0, 0, 0, 384, 0, 0, 704, 0, 0, 0, 736, 736, 736, 736, 736, 0,
/* 332 */ 192, 0, 0, 15, 0, 0, 192, 192, 192
};

const int csparser::EXPECTED[] =
{
/*  0 */ 24, 1040, 2064, 262676, 131984, 132016, 3932688, 4317264, 4317406, 4453360, 4715518, 4717566, 16, 1024, 524288,
/* 15 */ 3145728, 32768, 524288, 3145728, 524288, 3145728, 2097152
};

const wchar_t *csparser::TOKEN[] =
{
  L"(0)",
  L"schar",
  L"digit",
  L"alpha",
  L"S",
  L"EOF",
  L"'!'",
  L"'\"'",
  L"'$'",
  L"'/'",
  L"'//'",
  L"':'",
  L"';'",
  L"'='",
  L"'>'",
  L"'>>'",
  L"'?'",
  L"'['",
  L"']'",
  L"'file'",
  L"'http'",
  L"'https'",
  L"'|'"
};

#endif

// End
