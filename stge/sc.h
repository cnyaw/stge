
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/03/28 Waync created.
//

#pragma once

namespace stge {

class Script
{
public:

  class Cursor
  {
  public:

    typedef std::list<Script>::const_iterator iter;

    const Script* sc;
    iter cursor;
    int count, repeat;
    Context const* ctx;

    struct Frame
    {
      iter cursor;
      int count, repeat;
      Frame(iter i, int c, int r) : cursor(i), count(c), repeat(r)
      {
      }
    };

    std::list<Frame> stack;

    const Script& curr() const
    {
      return *cursor;
    }

    bool end() const
    {
      if (0 == sc) {
        return true;
      } else {
        if (stack.empty()) {
          return sc->sc.end() == cursor;
        } else {
          return stack.back().cursor->sc.end() == cursor;
        }
      }
    }

    void next()
    {
      assert(!end());

      switch (curr().type)
      {
      case Script::ROOT:
        {
          stack.push_back(Frame(cursor, count, repeat));
          count = repeat = 0;
          cursor = curr().sc.begin();
        }
        return;

      case Script::REPEAT:
        {
          stack.push_back(Frame(cursor, count, repeat));
          count = (int)curr().param[0](*ctx);
          repeat = 0;
          cursor = curr().sc.begin();
        }
        return;

      case Script::OPTION:
        {
          int sz = (int)curr().sc.size();
          int szOpt = sz;
          if (2 <= sz) {
            iter c = curr().sc.end();
            c--;                        // Else command.
            c--;                        // Else tag.
            if (Script::ELSE == c->type) {
              szOpt -= 2;
            }
          }
          int option = (int)curr().param[0](*ctx);
          if (0 <= option && szOpt > option) {
            stack.push_back(Frame(cursor, count, repeat));
            count = repeat = 0;
            cursor = curr().sc.begin();
            for (int i = 0; i < option; i++) {
              ++cursor;
            }
          } else if (szOpt != sz) {     // Has else condition.
            stack.push_back(Frame(cursor, count, repeat));
            count = repeat = 0;
            cursor = curr().sc.end();
            cursor--;
          } else {
            break;
          }
        }
        return;
      }

      ++cursor;

      while (!stack.empty()) {
        const iter stkCursor = stack.back().cursor;
        if (stkCursor->sc.end() != cursor && Script::OPTION != stkCursor->type) {
          break;
        }
        switch (stkCursor->type)
        {
        case Script::ROOT:
        case Script::REPEAT:
        case Script::OPTION:
          if (Script::REPEAT == stkCursor->type &&
              (-1 == count || 0 < --count)) {
            ++repeat;
            cursor = stkCursor->sc.begin();
          } else {
            Frame f = stack.back();
            stack.pop_back();
            cursor = f.cursor;
            count = f.count;
            repeat = f.repeat;
            ++cursor;
          }
          break;
        }
      }
    }

    void set(const Script* sc_, Context const* c)
    {
      sc = sc_;
      ctx = c;
      if (sc) {
        cursor = sc->sc.begin();
      }
      count = repeat = 0;
      stack.clear();
    }
  };

  enum CONSTANT
  {
    NUM_PARAM = 6
  };

  enum NODE
  {
    NODE_NOP = 0,
    ROOT,
    REPEAT,
    OPTION,
    ELSE,
    FORK,                               // Run in new action.
    CALL,                               // Call action.
    FIRE,
    SLEEP,
    DIRECTION,
    SPEED,
    CHANGEDIRECTION,
    CHANGESPEED,
    CHANGEX,
    CHANGEY,
    CLEAR,
    USERDATA
  };

  enum PARAM
  {
    AIM = 1217,
    ADD,
    OBJ
  };

  int type;
  Param param[1 + NUM_PARAM];           // Param plus 1 name.
  std::list<Script> sc;

  Script() : type(ROOT)
  {
  }

  Script(int type) : type(type)
  {
  }

  Script(int type, Param const& param0) : type(type)
  {
    param[0] = param0;
  }

  Script(int type, Param const& param0, Param const& param1) : type(type)
  {
    param[0] = param0;
    param[1] = param1;
  }

  Script(int type, Param const& param0, Param const& param1, Param const& param2) : type(type)
  {
    param[0] = param0;
    param[1] = param1;
    param[2] = param2;
  }

  Script(int type, Param const& param0, Param const& param1, Param const& param2, Param const& param3) : type(type)
  {
    param[0] = param0;
    param[1] = param1;
    param[2] = param2;
    param[3] = param3;
  }

  Script(int type, Param const& param0, Param const& param1, Param const& param2, Param const& param3, Param const& param4) : type(type)
  {
    param[0] = param0;
    param[1] = param1;
    param[2] = param2;
    param[3] = param3;
    param[4] = param4;
  }

  Script(int type, Param const& param0, Param const& param1, Param const& param2, Param const& param3, Param const& param4, Param const& param5) : type(type)
  {
    param[0] = param0;
    param[1] = param1;
    param[2] = param2;
    param[3] = param3;
    param[4] = param4;
    param[5] = param5;
  }

  Script(int type, Param const& param0, Param const& param1, Param const& param2, Param const& param3, Param const& param4, Param const& param5, Param const& param6) : type(type)
  {
    param[0] = param0;
    param[1] = param1;
    param[2] = param2;
    param[3] = param3;
    param[4] = param4;
    param[5] = param5;
    param[6] = param6;
  }
};

typedef std::map<std::string, Script> ScriptManager;

} // namespace stge

// end of file sc.h
