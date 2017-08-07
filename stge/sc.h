
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
    std::list<iter> curStack;
    int count, repeat;
    std::list<int> repStack;
    Expression::Context const* ctx;

    const Script& curr() const
    {
      return *cursor;
    }

    bool end() const
    {
      if (0 == sc) {
        return true;
      } else {
        if (curStack.empty()) {
          return sc->sc.end() == cursor;
        } else {
          return curStack.back()->sc.end() == cursor;
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
          repStack.push_back(count);
          repStack.push_back(repeat);
          count = repeat = 0;
          curStack.push_back(cursor);
          cursor = curr().sc.begin();
        }
        return;

      case Script::REPEAT:
        {
          repStack.push_back(count);
          repStack.push_back(repeat);
          count = (int)curr().param[0](*ctx);
          repeat = 0;
          curStack.push_back(cursor);
          cursor = curr().sc.begin();
        }
        return;

      case Script::OPTION:
        {
          int option = (int)curr().param[0](*ctx);
          if (0 <= option && (int)curr().sc.size() > option) {

            repStack.push_back(count);
            repStack.push_back(repeat);
            count = repeat = 0;
            curStack.push_back(cursor);
            cursor = curr().sc.begin();

            for (int i = 0; i < option; i++) {
              ++cursor;
            }
          } else {
            break;
          }
        }

        return;
      }

      ++cursor;

      while (!curStack.empty() &&
             (curStack.back()->sc.end() == cursor ||
              Script::OPTION == curStack.back()->type)) {

        switch (curStack.back()->type)
        {
        case Script::ROOT:
        case Script::REPEAT:
        case Script::OPTION:
          if (Script::REPEAT == curStack.back()->type &&
              (-1 == count || 0 < --count)) {
            ++repeat;
            cursor = curStack.back()->sc.begin();
          } else {
            repeat = repStack.back();
            repStack.pop_back();
            count = repStack.back();
            repStack.pop_back();
            cursor = curStack.back();
            curStack.pop_back();
            ++cursor;
          }
          break;
        }
      }
    }

    void set(const Script* sc_, Expression::Context const* c)
    {
      sc = sc_;
      ctx = c;

      if (sc) {
        cursor = sc->sc.begin();
      }

      count = repeat = 0;
      curStack.clear();
      repStack.clear();
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
