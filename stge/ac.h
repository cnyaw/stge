
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/03/28 Waync created.
//

#pragma once

namespace stge {

template<typename ObjectT>
class Action
{
public:

  int id;

  int idCallee;                         // Callee script.
  int idCaller;                         // Caller script.

  int idObj;
  ObjectT *pobj;

  float x, y;
  float direction;
  float speed;

  Context ctx;
  float *val[NUM_VAL];                   // dir, speed, x, y.

  float *target[NUM_VAL], durations[NUM_VAL], totals[NUM_VAL], amounts[NUM_VAL];

  Script::Cursor sc;
  float wait;

  float param[Script::NUM_PARAM];
  float user[Object::NUM_USERDATA];

  template<class ObjectManagerT>
  void set(ObjectManagerT& om, int id_, int idObj_, float x_, float y_, float direction_, float speed_, const Script* sc_)
  {
    id = id_;

    idObj = idObj_;
    if (-1 != idObj) {
      pobj = &om.objects[idObj];
    } else {
      pobj = 0;
    }

    x = x_;
    y = y_;
    direction = fmod(360000.0f + direction_, 360.0f);
    speed = speed_;

    ctx.param = param;
    if (pobj) {
      ctx.val[DIRECTION] = &pobj->direction;
      ctx.val[SPEED] = &pobj->speed;
      ctx.val[X] = &pobj->x;
      ctx.val[Y] = &pobj->y;
    } else {
      ctx.val[DIRECTION] = &direction;
      ctx.val[SPEED] = &speed;
      ctx.val[X] = &x;
      ctx.val[Y] = &y;
    }

    ctx.w = ObjectManagerT::getWindowWidth();
    ctx.h = ObjectManagerT::getWindowHeight();

    val[DIRECTION] = &direction;
    val[SPEED] = &speed;
    val[X] = &x;
    val[Y] = &y;

    sc.set(sc_, &ctx);
    ctx.repeat = &sc.repeat;

    wait = -1.0f;
    idCallee = idCaller = -1;

    for (int i = 0; i < Script::NUM_PARAM; i++) {
      param[i] = .0f;
    }

    for (int i = 0; i < Object::NUM_USERDATA; i++) {
      user[i] = .0f;
    }

    for (int i = 0; i < 4; i++) {
      durations[i] = .0f;
    }
  }

  //
  // Process script, return false if done.
  //

  template<class ObjectManagerT, class PlayerT>
  bool update(float fElapsed, ObjectManagerT& om, PlayerT& player)
  {
    //
    // Reference die?
    //

    if (pobj && !om.objects.isUsed(idObj)) {
      return false;
    }

    //
    // Callee script finished?
    //

    if (-1 != idCallee) {
      return true;
    }

    //
    // Progressive changes.
    //

    updateProgressiveChanges_i(fElapsed);

    //
    // Is still pending?
    //

    if (.0f < wait) {
      wait -= fElapsed;
      return true;
    }

    //
    // Process action.
    //

    while (!sc.end()) {

      switch (sc.curr().type)
      {
      case Script::ROOT:
      case Script::REPEAT:
      case Script::OPTION:
        sc.next();
        break;

      case Script::FORK:
        {
          int idAction = -1;
          if (!fork(om, idObj, &idAction)) {
            return false;
          }
          if (-1 != idAction) {
            (void)om.actions[idAction].update(fElapsed, om, player);
          } else {
            return true;
          }
        }
        break;

      case Script::CALL:
        if (!fork(om, idObj, &idCallee)) {
          return false;
        }
        if (-1 != idCallee) {
          om.actions[idCallee].idCaller = id;
          (void)om.actions[idCallee].update(fElapsed, om, player);
        }
        return true;

      case Script::FIRE:
        {
          bool ret;
          if (updateFire_i(fElapsed, om, player, ret)) {
            return ret;
          }
        }
        break;

      case Script::SLEEP:
        wait = sc.curr().param[0](ctx);
        sc.next();
        return true;

      case Script::DIRECTION:
      case Script::SPEED:
        updateDirSpeed_i(player);
        break;

      case Script::CHANGEDIRECTION:
      case Script::CHANGESPEED:
      case Script::CHANGEX:
      case Script::CHANGEY:
        updateChangeVals_i(player);
        break;

      case Script::CLEAR:
        if (-1 != idObj) {
          if (om.objects.isUsed(idObj)) {
            om.freeObject(idObj);
          }
        }
        return false;                   // End of script.

      case Script::USERDATA:
        {
          for (int i = 0; i < Object::NUM_USERDATA; i++) {
            user[i] = sc.curr().param[i](ctx);
          }
          sc.next();
        }
        break;

      default:
        return false;
      }
    }

    //
    // Still running.
    //

    return !sc.end();
  }

  void updateProgressiveChanges_i(float fElapsed)
  {
    for (int i = 0; i < 4; ++i) {
      if (.0f < durations[i]) {
        durations[i] -= fElapsed;
        *target[i] += amounts[i] * fElapsed;
        if (.0f < amounts[i]) {
          if (*target[i] > totals[i]) {
            *target[i] = totals[i];
          }
        } else {
          if (*target[i] < totals[i]) {
            *target[i] = totals[i];
          }
        }
      }
    }
  }

  template<class ObjectManagerT, class PlayerT>
  bool updateFire_i(float fElapsed, ObjectManagerT& om, PlayerT& player, bool &ret)
  {
    int idObj_ = om.objects.alloc();
    if (-1 == idObj_) {
      ret = true;
      return true;
    }

    ObjectT &o = om.objects[idObj_];
    o.set(*ctx.val[X], *ctx.val[Y], direction, speed, user);

    if (!o.init(om, id, idObj_)) {
      om.objects.free(idObj_);
      ret = true;
      return true;
    }

    if (!om.objects.isUsed(idObj_)) {   // Been killed in init?
      ret = true;
      return true;
    }

    if (sc.curr().param[0].exp.empty()) {
      sc.next();
      return false;
    }

    int idAction_ = -1;
    if (!fork(om, idObj_, &idAction_)) {
      ret = false;
      return true;
    }

    if (-1 == idAction_) {              // Fork not success, wait next chance.
      om.objects.free(idObj_);
      ret = true;
      return true;
    }

    o.idAction = idAction_;

    (void)om.actions[idAction_].update(fElapsed, om, player);

    return false;
  }

  template<class PlayerT>
  void updateDirSpeed_i(PlayerT& player)
  {
    int idx = sc.curr().type - Script::DIRECTION;

    float value = sc.curr().param[0](ctx);
    int p = (int)sc.curr().param[1](ctx);

    switch (p)
    {
    case Script::AIM:                   // Only valid for direction.
      {
        float x = player.x - *ctx.val[X];
        float y =player.y - *ctx.val[Y];
        *val[idx] = Math().atan2(y, x) + value;
      }
      break;

    case Script::ADD:
      *val[idx] += value;
      break;

    case Script::OBJ:
      *val[idx] = *ctx.val[idx] + value;
      break;

    default:
      *val[idx] = value;
      break;
    }

    sc.next();
  }

  template<class PlayerT>
  void updateChangeVals_i(PlayerT& player)
  {
    int idx = sc.curr().type - Script::CHANGEDIRECTION;

    float term_ = sc.curr().param[0](ctx);
    float value_ = sc.curr().param[1](ctx), value;
    int p = (int)sc.curr().param[2](ctx);

    switch (p)
    {
    case Script::AIM:                   // Only valid for changedirection.
      {
        float x = player.x - *ctx.val[X];
        float y = player.y - *ctx.val[Y];
        float a2 = Math().atan2(y, x);
        value = fmod(360000.0f + a2 + value_, 360.0f);

        //
        // Find smallest angle.
        //

        if (std::abs(value - *ctx.val[DIRECTION]) >
            std::abs(*ctx.val[DIRECTION] - value + 360.0f)) {
          value -= 360.0f;
        }
      }
      break;

    case Script::ADD:
      value = *val[idx] + value_;
      break;

    case Script::OBJ:
      value = *ctx.val[idx] + value_;
      break;

    default:
      value = value_;
      break;
    }

    if (.0f < term_) {
      durations[idx] = term_;
      totals[idx] = value;
      amounts[idx] = (value - *ctx.val[idx]) / term_;
      target[idx] = ctx.val[idx];
    } else {
      *ctx.val[idx] = value;
    }

    sc.next();
  }

  template<class ObjectManagerT>
  bool fork(ObjectManagerT& om, int idObj_, int* idAction_ = 0)
  {
    if (!sc.curr().param[0].exp.empty()) {

      std::map<std::string, Script>::const_iterator itAction;
      itAction = om.scm->find(sc.curr().param[0].exp);

      if (om.scm->end() == itAction) {

        //
        // Action not exist.
        //

        return false;
      }

      int idAction = om.actions.alloc();
      if (-1 == idAction) {

        //
        // Wait next chance to fire.
        //

        return true;
      }

      Action &a = om.actions[idAction];
      a.set(
         om, idAction,
         idObj_,
         *ctx.val[X], *ctx.val[Y],
         direction, speed,
         &itAction->second);

      if (idAction_) {
        *idAction_ = idAction;
      }

      a.param[0] = sc.curr().param[1](ctx);
      a.param[1] = sc.curr().param[2](ctx);
      a.param[2] = sc.curr().param[3](ctx);
      a.param[3] = sc.curr().param[4](ctx);
      a.param[4] = sc.curr().param[5](ctx);
      a.param[5] = sc.curr().param[6](ctx);
    }

    sc.next();
    return true;
  }
};

} // namespace stge

// end of file ac.h
