
//
// Copyright (c) 2010 Waync Cheng.
// All Rights Reserved.
//
// 2010/02/28 Waync created.
//

#pragma once

#ifdef CONST                            // Fix build error.
#undef CONST
#endif

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable:4267)
#endif
#include "include/yard.hpp"
#include "grammars/yard_c_grammar.hpp"
#ifdef _MSC_VER
# pragma warning(pop)
#endif

using namespace yard;
using namespace yard::text_grammar;

namespace stge {

//
// Build VM OP.
//

namespace parser {

#define STR_STGE_PARSER_MAGIC_TAG "____temp_script___121_180_13____"

class Context
{
  Context()
  {
  }

public:
  ScriptManager* mScm;
  std::vector<Script*> mCursor;
  std::vector<std::vector<std::string> > mParam;
  std::string mLastError;

  static Context& inst()
  {
    static Context i;
    return i;
  }

  static void trim(std::string& str)
  {
    str.erase(std::remove_if(str.begin(), str.end(), isspace), str.end());
  }
};

template<class Rule_T> struct OpNewScript
{
  template<typename ParserState_T>
  static bool Match(ParserState_T& p)
  {
    bool b = false;
    try {
      b = Rule_T::template Match(p);
    } catch(...) {
      throw;
    }

    if (b) {
      Context& ctx = Context::inst();
      ctx.mScm->operator[](STR_STGE_PARSER_MAGIC_TAG) = Script();
      ctx.mCursor.push_back(&(ctx.mScm->operator[](STR_STGE_PARSER_MAGIC_TAG)));
      ctx.mParam.push_back(std::vector<std::string>());
    }

    return b;
  }
};

template<class Rule_T> struct OpEndScript
{
  template<typename ParserState_T>
  static bool Match(ParserState_T& p)
  {
    bool b = false;
    try {
      b = Rule_T::template Match(p);
    } catch(...) {
      throw;
    }

    if (b) {
      Context& ctx = Context::inst();
      ctx.mScm->operator[](ctx.mParam.back().back()) = ctx.mScm->operator[](STR_STGE_PARSER_MAGIC_TAG);
      ctx.mScm->erase(STR_STGE_PARSER_MAGIC_TAG);
      ctx.mCursor.pop_back();
      ctx.mParam.pop_back();
    }

    return b;
  }
};

template<int Cmd, class Rule_T> struct OpBlock
{
  template<typename ParserState_T>
  static bool Match(ParserState_T& p)
  {
    bool b = false;
    try {
      b = Rule_T::template Match(p);
    } catch(...) {
      throw;
    }

    if (b) {
      Context& ctx = Context::inst();
      ctx.mCursor.back()->sc.push_back(Script(Cmd));
      ctx.mCursor.push_back(&ctx.mCursor.back()->sc.back());
      ctx.mParam.push_back(std::vector<std::string>());
    }
    return b;
  }
};

template<class Rule_T> struct OpEnd
{
  template<typename ParserState_T>
  static bool Match(ParserState_T& p)
  {
    bool b = false;
    try {
      b = Rule_T::template Match(p);
    } catch(...) {
      throw;
    }

    if (b) {
      Context& ctx = Context::inst();
      ctx.mCursor.pop_back();
      ctx.mParam.pop_back();
    }

    return b;
  }
};

template<int Cmd, class Rule_T> struct OpCmd
{
  template<typename ParserState_T>
  static bool Match(ParserState_T& p)
  {
    bool b = false;
    try {
      b = Rule_T::template Match(p);
    } catch(...) {
      throw;
    }

    if (b) {
      Context::inst().mCursor.back()->sc.push_back(Script(Cmd));
    }

    return b;
  }
};

template<class Rule_T> struct OpNewParam
{
  template<typename ParserState_T>
  static bool Match(ParserState_T& p)
  {
    bool b = false;
    try {
      b = Rule_T::template Match(p);
    } catch(...) {
      throw;
    }

    if (b) {
      Context::inst().mParam.push_back(std::vector<std::string>());
    }

    return b;
  }
};

template<class Rule_T> struct OpEndParam
{
  template<typename ParserState_T>
  static bool Match(ParserState_T& p)
  {
    bool b = false;
    try {
      b = Rule_T::template Match(p);
    } catch(...) {
      throw;
    }

    if (b) {
      Context::inst().mParam.pop_back();
    }

    return b;
  }
};

template<class Rule_T> struct OpParam
{
  template<typename ParserState_T>
  static bool Match(ParserState_T& p)
  {
    ParserState_T pp(p);
    pp.SetPos(p.GetPos());

    bool b = false;
    try {
      b = Rule_T::template Match(p);
    } catch(...) {
      throw;
    }

    if (b) {

      std::string s(pp.GetPos(), p.GetPos());

      s.resize(std::remove(s.begin(), s.end(), ' ') - s.begin());

      std::string::size_type pos = s.find(';');
      if (std::string::npos != pos) {
        s = s.substr(0, pos);
      }

      Context::trim(s);

      Context::inst().mParam.back().push_back(s);
    }

    return b;
  }
};

template<class Rule_T> struct OpParam2
{
  template<typename ParserState_T>
  static bool Match(ParserState_T& p)
  {
    ParserState_T pp(p);
    pp.SetPos(p.GetPos());

    bool b = false;
    try {
      b = Rule_T::template Match(p);
    } catch(...) {
      throw;
    }

    if (b) {

      std::string s(pp.GetPos(), p.GetPos());

      s.resize(std::remove(s.begin(), s.end(), ' ') - s.begin());
      Context::trim(s);

      Context& ctx = Context::inst();

      if ("aim" == s) {
        ctx.mParam.back().push_back("1217");
      } else if ("add" == s) {
        ctx.mParam.back().push_back("1218");
      } else if ("obj" == s) {
        ctx.mParam.back().push_back("1219");
      }
    }

    return b;
  }
};

template<class Rule_T> struct OpSetParam
{
  template<typename ParserState_T>
  static bool Match(ParserState_T& p)
  {
    bool b = false;
    try {
      b = Rule_T::template Match(p);
    } catch(...) {
      throw;
    }

    if (b) {

      Context& ctx = Context::inst();

      for (size_t i = 0; i < ctx.mParam.back().size(); ++i) {
        ctx.mCursor.back()->sc.back().param[i] = ctx.mParam.back()[i];
      }

      ctx.mParam.back().clear();
    }
    return b;
  }
};

template<class Rule_T> struct OpSetParam2
{
  template<typename ParserState_T>
  static bool Match(ParserState_T& p)
  {
    bool b = false;
    try {
      b = Rule_T::template Match(p);
    } catch(...) {
      throw;
    }

    if (b) {

      Context& ctx = Context::inst();

      for (size_t i = 0; i < ctx.mParam.back().size(); ++i) {
        ctx.mCursor.back()->param[i] = ctx.mParam.back()[i];
      }

      ctx.mParam.back().clear();
    }

    return b;
  }
};

//
// Grammer.
//

struct NewLine : Or<CharSeq<'\r', '\n'>, CharSeq<'\n'> >  {};
struct LineComment : Seq<CharSeq<';'>, UntilPast<NewLine> > {};
struct WS : Star<Or<Char<' '>, Char<'\t'>, NewLine, LineComment, Char<'\r'>, Char<'\v'>, Char<'\f'> > > {};

template<typename R>
struct Tok : Seq<R, WS> {};

struct Id : Tok<Ident>
{
};

template<char C>
struct CharTok : Seq<Char<C>, WS> {};

template<typename T>
struct Keyword : Tok<NoFailSeq<T, NotAlphaNum> > {};

template<typename R>
struct Paranthesized : NoFailSeq<CharTok<'('>, R, CharTok<')'> > {};

struct RAND : Keyword<CharSeq<'r', 'a', 'n', 'd'> > {};
struct SIN : Keyword<CharSeq<'s', 'i', 'n'> > {};
struct COS : Keyword<CharSeq<'c', 'o', 's'> > {};
struct SCRIPT : Keyword<CharSeq<'s','c','r','i', 'p', 't'> > {};
struct END : Keyword<CharSeq<'e', 'n', 'd'> > {};
struct REPEAT : Keyword<CharSeq<'r', 'e', 'p', 'e', 'a', 't'> > {};
struct OPTION : Keyword<CharSeq<'o', 'p', 't', 'i', 'o', 'n'> > {};
struct FORK : Keyword<CharSeq<'f', 'o', 'r', 'k'> > {};
struct CALL : Keyword<CharSeq<'c', 'a', 'l', 'l'> > {};
struct FIRE : Keyword<CharSeq<'f', 'i', 'r', 'e'> > {};
struct SLEEP : Keyword<CharSeq<'s', 'l', 'e', 'e', 'p'> > {};
struct DIRECTION : Keyword<CharSeq<'d', 'i', 'r', 'e', 'c', 't', 'i', 'o', 'n'> > {};
struct AIM : Keyword<CharSeq<'a', 'i', 'm'> > {};
struct ADD : Keyword<CharSeq<'a', 'd', 'd'> > {};
struct OBJ : Keyword<CharSeq<'o', 'b', 'j'> > {};
struct SPEED : Keyword<CharSeq<'s', 'p', 'e', 'e', 'd'> > {};
struct CHANGEDIRECTION : Keyword<CharSeq<'c', 'h', 'a', 'n', 'g', 'e', 'd', 'i', 'r', 'e', 'c', 't', 'i', 'o', 'n'> > {};
struct CHANGESPEED : Keyword<CharSeq<'c', 'h', 'a', 'n', 'g', 'e', 's', 'p', 'e', 'e', 'd'> > {};
struct CHANGEX : Keyword<CharSeq<'c', 'h', 'a', 'n', 'g', 'e', 'x'> > {};
struct CHANGEY : Keyword<CharSeq<'c', 'h', 'a', 'n', 'g', 'e', 'y'> > {};
struct CLEAR : Keyword<CharSeq<'c', 'l', 'e', 'a', 'r'> > {};
struct USERDATA : Keyword<CharSeq<'u', 's', 'e', 'r', 'd', 'a', 't', 'a'> > {};

struct ARGREP : Keyword<CharSeq<'$', 'r', 'e', 'p'> > {};
struct ARG1 : Keyword<CharSeq<'$', '1'> > {};
struct ARG2 : Keyword<CharSeq<'$', '2'> > {};
struct ARG3 : Keyword<CharSeq<'$', '3'> > {};
struct ARG4 : Keyword<CharSeq<'$', '4'> > {};
struct ARG5 : Keyword<CharSeq<'$', '5'> > {};
struct ARG6 : Keyword<CharSeq<'$', '6'> > {};
struct ARGX : Keyword<CharSeq<'$', 'x'> > {};
struct ARGY : Keyword<CharSeq<'$', 'y'> > {};
struct ARGW : Keyword<CharSeq<'$', 'w'> > {};
struct ARGH : Keyword<CharSeq<'$', 'h'> > {};
struct ARGDIR : Keyword<CharSeq<'$', 'd', 'i', 'r'> > {};
struct ARGSPD : Keyword<CharSeq<'$', 's', 'p', 'e', 'e', 'd'> > {};

template<typename R>
struct CommaArg : Seq<CharTok<','>, R> {};

template<typename R>
struct OptArg : Opt<CommaArg<R> > {};

struct Expr;

struct DecNumber  : Seq<Opt<Char<'-'> >, Plus<Digit>, Opt<Seq<Char<'.'>, Star<Digit> > >, NotAlphaNum, WS>  {};
struct RandExpr : NoFailSeq<RAND, Paranthesized<Opt<Seq<Expr, OptArg<Expr> > > > > {};
struct SinExpr : NoFailSeq<SIN, Paranthesized<Expr> > {};
struct CosExpr : NoFailSeq<COS, Paranthesized<Expr> > {};
struct ArgExpr : Or<ARGREP, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, Or<ARGX, ARGY, ARGW, ARGH, ARGDIR, ARGSPD> > {};
struct Factor : Or<DecNumber, RandExpr, Paranthesized<Expr>, SinExpr, CosExpr, ArgExpr, Seq<CharTok<'-'>, Factor> > {};
struct Term : Seq<Factor, Star<Or<Seq<CharTok<'*'>, Factor>, Seq<CharTok<'/'>, Factor> > > > {};
struct Expr : Seq<Term, Star<Or<Seq<CharTok<'+'>, Term>, Seq<CharTok<'-'>, Term> > > > {};

struct Command;

template<typename R>
struct BlockSeq : NoFailSeq<R, END> {};

struct Repeat : OpEnd<OpSetParam2<OpEndParam<BlockSeq<NoFailSeq<OpBlock<Script::REPEAT, REPEAT>, Paranthesized<OpNewParam<OpParam<Expr> > >, Star<Command> > > > > >
{
};

struct Option : OpEnd<OpSetParam2<OpEndParam<BlockSeq<NoFailSeq<OpBlock<Script::OPTION, OPTION>, Paranthesized<OpNewParam<OpParam<Expr> > >, Star<Command> > > > > >
{
};

struct ParamList : Seq<OpParam<Expr>, OptArg<Seq<OpParam<Expr>, OptArg<Seq<OpParam<Expr>, OptArg<Seq<OpParam<Expr>, OptArg<Seq<OpParam<Expr>, OptArg<OpParam<Expr> > > > > > > > > > >
{
};

struct Fork : OpSetParam<NoFailSeq<OpCmd<Script::FORK, FORK>, Paranthesized<Seq<OpParam<Id>, OptArg<ParamList> > > > >
{
};

struct Call : OpSetParam<NoFailSeq<OpCmd<Script::CALL, CALL>, Paranthesized<Seq<OpParam<Id>, OptArg<ParamList> > > > >
{
};

struct Fire : OpSetParam<NoFailSeq<OpCmd<Script::FIRE, FIRE>, Paranthesized<Opt<Seq<OpParam<Id>, OptArg<ParamList> > > > > >
{
};

struct Sleep : OpSetParam<NoFailSeq<OpCmd<Script::SLEEP, SLEEP>, Paranthesized<OpParam<Expr> > > >
{
};

struct DirParam : OptArg<OpParam2<Or<AIM, ADD, OBJ> > >
{
};

struct SpdParam : OptArg<OpParam2<Or<ADD, OBJ> > >
{
};

struct Direction : OpSetParam<NoFailSeq<OpCmd<Script::DIRECTION, DIRECTION>, Paranthesized<Seq<OpParam<Expr>, DirParam> > > >
{
};

struct Speed : OpSetParam<NoFailSeq<OpCmd<Script::SPEED, SPEED>, Paranthesized<Seq<OpParam<Expr>, SpdParam> > > >
{
};

struct ChangeDirection : OpSetParam<NoFailSeq<OpCmd<Script::CHANGEDIRECTION, CHANGEDIRECTION>, Paranthesized<Seq<OpParam<Expr>, CommaArg<OpParam<Expr> >, DirParam> > > >
{
};

struct ChangeSpeed : OpSetParam<NoFailSeq<OpCmd<Script::CHANGESPEED, CHANGESPEED>, Paranthesized<Seq<OpParam<Expr>, CommaArg<OpParam<Expr> >, SpdParam> > > >
{
};

struct ChangeX : OpSetParam<NoFailSeq<OpCmd<Script::CHANGEX, CHANGEX>, Paranthesized<Seq<OpParam<Expr>, CommaArg<OpParam<Expr> >, SpdParam> > > >
{
};

struct ChangeY : OpSetParam<NoFailSeq<OpCmd<Script::CHANGEY, CHANGEY>, Paranthesized<Seq<OpParam<Expr>, CommaArg<OpParam<Expr> >, SpdParam> > > >
{
};

struct Clear : NoFailSeq<OpCmd<Script::CLEAR, CLEAR>, Paranthesized<WS> >
{
};

struct Userdata : OpSetParam<NoFailSeq<OpCmd<Script::USERDATA, USERDATA>, Paranthesized<Seq<OpParam<Expr>, OptArg<Seq<OpParam<Expr>, OptArg<Seq<OpParam<Expr>, OptArg<OpParam<Expr> > > > > > > > > >
{
};

struct Command : Or<Repeat, Option, Fork, Call, Fire, Sleep, Direction, Speed, Or<ChangeDirection, ChangeSpeed, ChangeX, ChangeY, Clear, Userdata> >
{
};

struct StgeScript : OpEndScript<OpEndParam<BlockSeq<NoFailSeq<OpNewScript<SCRIPT>, OpNewParam<OpParam<Id> >, Star<Command> > > > >
{
};

struct ScriptFile : Seq<WS, Star<StgeScript> >
{
};

struct BasicParser : yard::BasicParser<char>
{
  typedef const char* Iterator;

  BasicParser(Iterator first, Iterator last) : yard::BasicParser<char>(first, last) {}

  //
  // Parse function.
  //

  template<typename StartRule_T>
  bool Parse() {
    try {
      return StartRule_T::Match(*this);
    } catch (...) {
      return false;
    }
  }

  template<typename T>
  void OutputLocation()
  {
    Iterator pFirst = GetPos();
    while (pFirst > mBegin && *pFirst != '\n') {
      pFirst--;
    }

    if (*pFirst == '\n') {
      ++pFirst;
    }

    //
    // Count Column.
    //

    int n = (int)(GetPos() - pFirst + 1);

    //
    // Count lines.
    //

    int nline = 1;
    for (Iterator i = mBegin; i < pFirst; ++i) {
      if (*i == '\n') {
        ++nline;
      }
    }

    std::stringstream ss;
    ss << "Line" << nline << ":" << n << ": Ill formed input, failed to parse!";

    Context::inst().mLastError = ss.str();
  }
};

} // parser

struct Parser
{
  static bool parse(std::istream& istream, ScriptManager& scm)
  {
    istream.unsetf(std::ios::skipws);

    std::string str;
    std::copy(
           std::istream_iterator<char>(istream),
           std::istream_iterator<char>(),
           std::back_inserter(str));

    parser::Context::inst().mScm = &scm;
    parser::Context::inst().mCursor.clear();
    parser::Context::inst().mParam.clear();
    parser::Context::inst().mLastError = "";

    bool ret = false;

    try {
      char const* out = str.c_str() + str.length();
      parser::BasicParser parser(str.c_str(), out);
      ret = parser.Parse<Seq<parser::ScriptFile, EndOfInput> >();
    } catch (...) {
      return false;
    }

    return ret;
  }

  static bool parse(std::string const& filename, ScriptManager& scm)
  {
    std::ifstream f(filename.c_str());
    if (!f) {
      return false;
    } else {
      return parse(f, scm);
    }
  }

  static std::string getLastError()
  {
    return parser::Context::inst().mLastError;
  }
};

} // namespace stge

// end of file par_yard.h
