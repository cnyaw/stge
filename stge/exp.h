
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/03/18 Waync created.
//

#pragma once

namespace stge {

class Expression
{
public:
  enum CONSTANT
  {
    DIRECTION = 0,
    SPEED,
    X,
    Y
  };

  struct Context
  {
    float const *param;
    int *repeat;
    float *val[4];                      // dir, spd, x, y.
    int w, h;                           // Window w/h.
  };

  class Exception : public std::exception
  {
  public:

    Exception(std::string const& exp, std::string const& s, std::string::size_type p) throw () : pos(p)
    {
      str = std::string("\"") + exp + std::string("\" ") + s;
    }

    virtual ~Exception() throw ()
    {
    }

    virtual const char * what() const throw()
    {
      return str.c_str();
    }

    std::string::size_type pos;
    std::string str;
  };

  Expression(std::string const& str, Context const& c) : exp(str), ctx(c)
  {
  }

  float eval()
  {
    if (exp.empty()) {
      return .0f;
    }

    p = 0;
    skipWhites();

    return expression();
  }

private:

  float evalName()
  {
    std::string name = getName();

    char funcName[][32] = {
           "rand",
           "sin", "cos",
           "$1", "$2", "$3", "$4", "$5", "$6",
           "$rep",
           "$dir", "$speed",
           "$x", "$y",
           "$w", "$h",
           "dummy"};

    const int numFunc = sizeof(funcName) / sizeof(funcName[0]);

    switch (std::find(funcName, funcName + numFunc, name) - funcName)
    {
    case 0:                             // rand.
      {
        match('(');

        if (isMatch(')')) {
          return rand() / (float)RAND_MAX;
        }

        float r1 = expression(), r2 = 0;
        bool b = isMatch(',');

        if (b) {
          r2 = expression();
        }

        match(')');

        return b ? sw2::rangeRand(r1,r2) : sw2::rangeRand(.0f,r1);
      }

    case 1:                             // sin.
      {
        match ('(');
        float r = expression();
        match(')');
        return Math ().sin(r);
      }

    case 2:                             // cos.
      {
        match('(');
        float r = expression();
        match(')');
        return Math().cos(r);
      }

    case 3:                             // param 1.
      return ctx.param[0];

    case 4:                             // param 2.
      return ctx.param[1];

    case 5:                             // param 3.
      return ctx.param[2];

    case 6:                             // param 4.
      return ctx.param[3];

    case 7:                             // param 5.
      return ctx.param[4];

    case 8:                             // param 6.
      return ctx.param[5];

    case 9:                             // Repeat count.
      return (float)*ctx.repeat;

    case 10:                            // Direction.
      return *ctx.val[DIRECTION];

    case 11:                            // Speed.
      return *ctx.val[SPEED];

    case 12:                            // x.
      return *ctx.val[X];

    case 13:                            // y.
      return *ctx.val[Y];

    case 14:                            // Window width.
      return (float)ctx.w;

    case 15:                            // Window height.
      return (float)ctx.h;

    default:
      throw Exception(exp, "funcion or name expected", p);
    }

    return 0.0f;
  }

  float expression()
  {
    float val = ('+' == c || '-' == c) ? .0f : term();

    while ('+' == c || '-' == c) {
      if ('+' == c) {
        match('+');
        val += term();
      } else {
        match('-');
        val -= term();
      }
    }

    return val;
  }

  float factor()
  {
    float val = .0f;

    if ('(' == c) {
      match('(');
      val = expression();
      match(')');
    } else if (isalpha(c) || '$' == c) {
      val = evalName();
    } else {
      val = getNumber();
    }

    return val;
  }

  void getChar()                        // Read next char.
  {
    if (exp.empty() || exp.size() == p) { // Empty exp str or end of str.
      c = 0;
      return;
    }

    c = exp[p ++];
  }

  std::string getName()
  {
    if (!::isalpha(c) && '$' != c) {
      throw Exception(exp, "Token name expected", p);
    }

    std::string name;
    while (::isalnum(c) || '$' == c) {
      name += c;
      getChar();
    }

    p -= 1;                             // Put back 1.
    skipWhites();

    return name;
  }

  float getNumber()                     // Read a number.
  {
    if (!::isdigit(c) && '.' != c) {
      throw Exception(exp, "Number expected", p);
    }

    char* ps = NULL;
    float val = (float)strtod(exp.c_str() + p - 1, &ps);

    p = ps - exp.c_str();

    skipWhites();

    return val;
  }

  bool isMatch(std::string::value_type c_) // Check is the char maths the expected char.
  {
    if (c != c_) {
      return false;
    }

    skipWhites();

    return true;
  }

  void match(std::string::value_type c_) // Check is the char maths the expected char.
  {
    if (c != c_) {
      throw Exception(exp, std::string() + c_ + " expected", p);
    }

    skipWhites();
  }

  void skipWhites()                     // Skip white chars.
  {
    getChar();
    while ((' ' == c || '\t' == c)) {
      getChar();
    }
  }

  float term()
  {
    float val = factor();

    while ('*' == c || '/' == c) {
      if ('*' == c) {
        match('*');
        val *= factor();
      } else {
        match('/');
        val /= factor();
      }
    }

    return val;
  }

  std::string const& exp;
  Context const& ctx;

  std::string::size_type p;             // Pointer.
  std::string::value_type c;            // Current char read.
};

} // namespace stge

// end of file exp.h
