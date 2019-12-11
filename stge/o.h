
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/03/28 Waync created.
//

#pragma once

namespace stge {

class Object
{
public:
  enum CONSTANT
  {
    NUM_USERDATA = 4
  };

  int idAction;

  float x, y;
  float direction;
  float speed;

  float user[NUM_USERDATA];

  void set(float x_, float y_, float direction_, float speed_, float userdata[])
  {
    idAction = -1;

    x = x_;
    y = y_;
    direction = fmod(360000.0f + direction_, 360.0f);
    speed = speed_;

    for (int i = 0; i < NUM_USERDATA; i++) {
      user[i] = userdata[i];
    }
  }

  //
  // Overridable.
  //

  template<typename BulletSystemT>
  bool init(BulletSystemT& bs, int idAction, int idNewObj)
  {
    //
    // Return false if init failed, and clear this object.
    //

    return true;
  }

  template<typename BulletSystemT, typename PlayerT>
  bool update(float fElapsed, BulletSystemT& bs, PlayerT&)
  {
    if (-1 == idAction) {
      x += fElapsed * speed * Math().cos(direction);
      y += fElapsed * speed * Math().sin(direction);
    } else {
      typename BulletSystemT::ActionT &a = bs.actions[idAction];
      if (.0f >= a.durations[X]) {
        x += fElapsed * speed * Math().cos(direction);
      }
      if (.0f >= a.durations[Y]) {
        y += fElapsed * speed * Math().sin(direction);
      }
    }

    //
    // Return false, and clear this object.
    //

    return true;
  }
};

} // namespace stge

// end of file o.h
