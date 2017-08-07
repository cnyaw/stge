
//
// Copyright (c) 2008 Waync Cheng.
// All Rights Reserved.
//
// 2008/01/30 Waync created.
//

#pragma once

//
// Boost Spirit.
//

#include "boost/spirit/core.hpp"
#include "boost/spirit/utility.hpp"
#include "boost/spirit/symbols.hpp"
#include "boost/spirit/iterator/position_iterator.hpp"
#include "boost/spirit/utility/functor_parser.hpp"
using namespace boost::spirit;

namespace stge {

class Parser
{
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
  };

#ifndef STGE_NO_ERROR_MSG
  struct error_report_parser
  {
    std::string msg;

    error_report_parser(std::string const& msg_ = "") : msg(msg_)
    {
    }

    typedef nil_t result_t;

    template<typename ScannerT>
    int operator()(ScannerT const& scan, result_t& /*result*/) const
    {
      file_position fpos = scan.first.get_position();

      std::stringstream ss;
      ss << fpos.line << ":" << fpos.column << ": " << msg;

      Context::inst().mLastError = ss.str();

      return -1;                        // Fail.
    }
  };

  typedef functor_parser<error_report_parser> my_err_p;
#endif

  typedef position_iterator<char const*> pos_iterator_t;

  struct OpNewScript
  {
    void operator()(const pos_iterator_t begin, const pos_iterator_t end) const
    {
      Context& ctx = Context::inst();

      ctx.mScm->operator[]("____temp_script___121_180_13____") = Script();
      ctx.mCursor.push_back(&ctx.mScm->operator[]("____temp_script___121_180_13____"));
      ctx.mParam.push_back(std::vector<std::string>());
    }
  };

  struct OpEndScript
  {
    void operator()(const pos_iterator_t begin, const pos_iterator_t end) const
    {
      Context& ctx = Context::inst();

      ctx.mScm->operator[](ctx.mParam.back().back()) =
        ctx.mScm->operator[]("____temp_script___121_180_13____"]);
      ctx.mScm->erase("____temp_script___121_180_13____");
      ctx.mCursor.pop_back();
      ctx.mParam.pop_back();
    }
  };

  struct OpBlock
  {
    int mCmd;

    OpBlock(int cmd) : mCmd(cmd)
    {
    }

    void operator()(const pos_iterator_t begin, const pos_iterator_t end) const
    {
      Context& ctx = Context::inst();

      ctx.mCursor.back()->sc.push_back(Script(mCmd));
      ctx.mCursor.push_back(&ctx.mCursor.back()->sc.back());
      ctx.mParam.push_back(std::vector<std::string>());
    }
  };

  struct OpEnd
  {
    void operator()(const pos_iterator_t begin, const pos_iterator_t end) const
    {
      Context& ctx = Context::inst();

      ctx.mCursor.pop_back();
      ctx.mParam.pop_back();
    }
  };

  struct OpCmd
  {
    int mCmd;

    OpCmd(int cmd) : mCmd(cmd)
    {
    }

    void operator()(const pos_iterator_t begin, const pos_iterator_t end) const
    {
      Context::inst().mCursor.back()->sc.push_back(Script(mCmd));
    }
  };

  struct OpNewParam
  {
    void operator()(const pos_iterator_t begin, const pos_iterator_t end) const
    {
      Context::inst().mParam.push_back(std::vector<std::string>());
    }
  };

  struct OpEndParam
  {
    void operator()(const pos_iterator_t begin, const pos_iterator_t end) const
    {
      Context::inst().mParam.pop_back();
    }
  };

  struct OpParam
  {
    void operator()(const pos_iterator_t begin, const pos_iterator_t end) const
    {
      Context::inst().mParam.back().push_back(std::string(begin, end));
    }
  };

  struct OpParam2
  {
    void operator()(const pos_iterator_t begin, const pos_iterator_t end) const
    {
      Context& ctx = Context::inst();

      std::string s(begin, end);
      if ("aim" == s) {
        ctx.mParam.back().push_back("1217");
      } else if ("add" == s) {
        ctx.mParam.back().push_back("1218");
      } else if ("obj" == s) {
        ctx.mParam.back().push_back("1219");
      }
    }
  };

  struct OpSetParam
  {
    void operator()(const pos_iterator_t begin, const pos_iterator_t end) const
    {
      Context& ctx = Context::inst();

      for (size_t i = 0; i < ctx.mParam.back().size(); ++i) {
        ctx.mCursor.back()->sc.back().param[i] = ctx.mParam.back()[i];
      }

      ctx.mParam.back().clear();
    }
  };

  struct OpSetParam2
  {
    void operator()(const pos_iterator_t begin, const pos_iterator_t end) const
    {
      Context& ctx = Context::inst();

      for (size_t i = 0; i < ctx.mParam.back().size(); ++i) {
        ctx.mCursor.back()->param[i] = ctx.mParam.back()[i];
      }

      ctx.mParam.back().clear();
    }
  };

  struct Skip : public grammar<Skip>
  {
    template <typename ScannerT>
    struct definition
    {
      rule<ScannerT> skip;

      definition(Skip const& self)
      {
        skip
            = space_p
            | comment_p(';')
            | comment_p('#')
            | comment_p("//")
            ;
      }

      rule<ScannerT> const& start() const
      {
        return skip;
      }
    };
  };

  struct Grammer : public grammar<Grammer>
  {
    template <typename ScannerT>
    struct definition
    {
#ifndef STGE_NO_ERROR_MSG
      my_err_p expect_script, expect_end, expect_comma, expect_lb, expect_rb;
      my_err_p invalid_id;
#endif

      symbols<> keywords;
      rule<ScannerT> COMMA, LB, RB;
      rule<ScannerT> SCRIPT, END, REPEAT, OPTION, FORK, CALL, FIRE, SLEEP, DIRECTION, SPEED, CHANGEDIRECTION, CHANGESPEED, CHANGEX, CHANGEY, CLEAR, USERDATA;
      rule<ScannerT> AIM, ADD, OBJ;
      rule<ScannerT> IDENTIFIER;
      rule<ScannerT> expression, term, factor, RAND;
      rule<ScannerT> paramlist, usrparamlist, dirparamlist, spdparamlist;
      rule<ScannerT> command, repeat, option;
      rule<ScannerT> fork, call, fire, sleep, direction, speed, changedirection, changespeed, changex, changey, clear, userdata;
      rule<ScannerT> script;
      rule<ScannerT> top;

      definition(Grammer const& self)
      {
        //
        // Errors.
        //

#ifndef STGE_NO_ERROR_MSG
        expect_script = error_report_parser("'script' expected!");
        expect_end = error_report_parser("'end' expected!");
        expect_comma = error_report_parser("',' expected!");
        expect_lb = error_report_parser("'(' expected!");
        expect_rb = error_report_parser("')' expected!");
        invalid_id = error_report_parser("invalid identifier!");
#endif

        //
        // Grammer.
        //

#ifndef STGE_NO_ERROR_MSG
        COMMA           = chlit<>(',') | expect_comma;
        LB              = chlit<>('(') | expect_lb;
        RB              = chlit<>(')') | expect_rb;

        SCRIPT          = strlit<>("script") | expect_script;
        END             = strlit<>("end") | expect_end;
#else
        COMMA           = chlit<>(',');
        LB              = chlit<>('(');
        RB              = chlit<>(')');

        SCRIPT          = strlit<>("script");
        END             = strlit<>("end");
#endif

        keywords        = "script", "end", "repeat", "option", "fork", "call", "fire", "sleep", "direction", "speed", "changedirection", "changespeed", "changex", "changey", "clear", "userdata";
        REPEAT          = strlit<>("repeat");
        OPTION          = strlit<>("option");
        FORK            = strlit<>("fork");
        CALL            = strlit<>("call");
        FIRE            = strlit<>("fire");
        SLEEP           = strlit<>("sleep");
        DIRECTION       = strlit<>("direction");
        SPEED           = strlit<>("speed");
        CHANGEDIRECTION = strlit<>("changedirection");
        CHANGESPEED     = strlit<>("changespeed");
        CHANGEX         = strlit<>("changex");
        CHANGEY         = strlit<>("changey");
        CLEAR           = strlit<>("clear");
        USERDATA        = strlit<>("userdata");

        AIM             = strlit<>("aim");
        ADD             = strlit<>("add");
        OBJ             = strlit<>("obj");

        IDENTIFIER
          = lexeme_d[
              ((alpha_p | '_') >> *(alnum_p | '_'))
              - (keywords >> anychar_p - (alnum_p | '_'))
          ]
#ifndef STGE_NO_ERROR_MSG
          | invalid_id
#endif
          ;

        expression
          = term >> *(('+' >> term)
          | ('-' >> term))
          ;

        term
          = factor >> *(('*' >> factor)
          | ('/' >> factor))
          ;

        factor
          = real_p
          | (LB >> expression >> RB)
          | ('-' >> factor)
          | ('+' >> factor)
          | "$rep" | "$1" | "$2" | "$3" | "$4" | "$5" | "$6" | "$x" | "$y" | "$w" | "$h" | "$dir" | "$speed"
          | "sin" >> LB >> expression >> RB
          | "cos" >> LB >> expression >> RB
          | RAND
          ;

        RAND
          = "rand" >> LB >> !(expression >> !(COMMA >> expression)) >> RB
          ;

        paramlist
          = !(COMMA >> expression[OpParam()] >> !(COMMA >> expression[OpParam()] >> !(COMMA >> expression[OpParam()] >> !(COMMA >> expression[OpParam()] >> !(COMMA >> expression[OpParam()] >> !(COMMA >> expression[OpParam()]))))))
          ;

        usrparamlist
          = expression[OpParam()] >> !(COMMA >> expression[OpParam()] >> !(COMMA >> expression[OpParam()] >> !(COMMA >> expression[OpParam()])))
          ;

        dirparamlist
          = !(COMMA >> (AIM | ADD | OBJ)[OpParam2()])
          ;

        spdparamlist
          = !(COMMA >> (ADD | OBJ)[OpParam2()])
          ;

        command
          = repeat | option | fork | call | fire | sleep | direction | speed | changedirection | changespeed | changex | changey | clear | userdata
          ;

        repeat
          = (REPEAT[OpBlock(Script::REPEAT)] >> LB >> expression[OpParam()][OpNewParam()] >> RB >> *command >> END)[OpEndParam()][OpSetParam2()][OpEnd()]
          ;

        option
          = (OPTION[OpBlock(Script::OPTION)] >> LB >> expression[OpParam()][OpNewParam()] >> RB >> *command >> END)[OpEndParam()][OpSetParam2()][OpEnd()]
          ;

        fork
          = (FORK[OpCmd(Script::FORK)] >> LB >> IDENTIFIER[OpParam()] >> paramlist >> RB)[OpSetParam()]
          ;

        call
          = (CALL[OpCmd(Script::CALL)] >> LB >> IDENTIFIER[OpParam()] >> paramlist >> RB)[OpSetParam()]
          ;

        fire
          = (FIRE[OpCmd(Script::FIRE)] >> LB >> !(IDENTIFIER[OpParam()] >> paramlist) >> RB)[OpSetParam()]
          ;

        sleep
          = (SLEEP[OpCmd(Script::SLEEP)] >> LB >> expression[OpParam()] >> RB)[OpSetParam()]
          ;

        direction
          = (DIRECTION[OpCmd(Script::DIRECTION)] >> LB >> expression[OpParam()] >> dirparamlist >> RB)[OpSetParam()]
          ;

        speed
          = (SPEED[OpCmd(Script::SPEED)] >> LB >> expression[OpParam()] >> spdparamlist >> RB)[OpSetParam()]
          ;

        changedirection
          = (CHANGEDIRECTION[OpCmd(Script::CHANGEDIRECTION)] >> LB >> expression[OpParam()] >> COMMA >> expression[OpParam()] >> dirparamlist >> RB)[OpSetParam()]
          ;

        changespeed
          = (CHANGESPEED[OpCmd(Script::CHANGESPEED)] >> LB >> expression[OpParam()] >> COMMA >> expression[OpParam()] >> spdparamlist >> RB)[OpSetParam()]
          ;

        changex
          = (CHANGEX[OpCmd(Script::CHANGEX)] >> LB >> expression[OpParam()] >> COMMA >> expression[OpParam()] >> spdparamlist >> RB)[OpSetParam()]
          ;

        changey
          = (CHANGEY[OpCmd(Script::CHANGEY)] >> LB >> expression[OpParam()] >> COMMA >> expression[OpParam()] >> spdparamlist >> RB)[OpSetParam()]
          ;

        clear
          = CLEAR[OpCmd(Script::CLEAR)] >> LB >> RB
          ;

        userdata
          = (USERDATA[OpCmd(Script::USERDATA)] >> LB >> usrparamlist >> RB)[OpSetParam()]
          ;

        script
          = (SCRIPT[OpNewScript()] >> IDENTIFIER[OpParam()][OpNewParam()] >> *command >> END)[OpEndParam()][OpEndScript()]
          ;

        top
          = *script >> end_p
          ;
      }

      rule<ScannerT> const& start() const
      {
        return top;
      }
    };
  };

public:

  static bool parse(std::string const& filename, ScriptManager& scm)
  {
    std::ifstream f(filename.c_str());
    if (!f) {
      return false;
    } else {
      return parse(f, scm);
    }
  }

  static bool parse(std::istream& istream, ScriptManager& scm)
  {
    istream.unsetf(std::ios::skipws);

    std::string str;
    std::copy(
           std::istream_iterator<char>(istream),
           std::istream_iterator<char>(),
           std::back_inserter(str));

    Context::inst().mScm = &scm;
    Context::inst().mLastError = "";

    Grammer grammer;
    Skip skip;

    pos_iterator_t begin(str.c_str(), str.c_str() + str.size(), ""), end;

    return boost::spirit::parse(begin, end, grammer, skip).full;
  }

  static std::string getLastError()
  {
    return Context::inst().mLastError;
  }
};

} // namespace stge

// end of file par.h
