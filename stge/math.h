
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/03/28 Waync created.
//

#pragma once

namespace stge {

class Math
{
public:

  struct Table
  {
    float table[360];

    explicit Table(bool bCos)
    {
      const float PI = 3.1415926f;
      for (int i = 0; i < 360; i++) {
        table[i] = bCos ? ::cos(i * PI / 180.0f) : ::sin(i * PI / 180.0f);
      }
    }
  };

  float atan2(float y, float x) const
  {
    const float PI = 3.1415926f;

    float coeff1 = PI / 4.0f;
    float coeff2 = 3 * coeff1;
    float absy = std::abs(y) + 1e-10f;
    float angle;

    if (x >= 0.) {
      float r = (x - absy) / (x + absy);
      angle = coeff1 - coeff1 * r;
    } else {
      float r = (x + absy) / (absy - x);
      angle = coeff2 - coeff1 * r;
    }

    angle = angle * 180.0f / PI;

    if (y < .0f) {
      return - angle;
    } else {
      return angle;
    }
  }

  float cos(float angle) const
  {
    static Table t(true);
    return t.table[(int)fmod(360000.0f + angle, 360.0f)];
  }

  float sin(float angle) const
  {
    static Table t(false);
    return t.table[(int)fmod(360000.0f + angle, 360.0f)];
  }
};

} // namespace stge

// end of file math.h
