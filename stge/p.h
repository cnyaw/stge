
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/07/17 Waync created.
//

#pragma once

namespace stge {

class Param
{
public:

  bool bConst;
  float value;
  std::string exp;

  Param() : bConst(true), value(.0f)
  {
  }

  Param(std::string const& e) : bConst(false), value(.0f), exp(e)
  {
    if (isConst()) {
      bConst = true;
      value = Expression(exp, Expression::Context()).eval();
    }
  }

  Param(float v) : bConst(true), value(v)
  {
  }

  Param(int v) : bConst(true), value((float)v)
  {
  }

  Param(Param const& p) : bConst(p.bConst), value(p.value), exp(p.exp)
  {
  }

  Param& operator=(Param const& p)
  {
    if (this != &p) {
      bConst = p.bConst;
      value = p.value;
      exp = p.exp;
    }
    return *this;
  }

  float operator()(Expression::Context const& ctx) const
  {
    return bConst ? value : Expression(exp, ctx).eval();
  }

  bool isConst() const
  {
    if (std::string::npos != exp.find("$")) {
      return false;
    }

    for (std::string::size_type i = 0; i < exp.length(); i++) {
      if (::isalpha(exp[i])) {
        return false;
      }
    }

    return true;
  }
};

} // namespace stge

// end of file p.h
