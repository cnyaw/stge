
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/03/28 Waync created.
//

#pragma once

namespace stge {

template<typename ObjectT> class Action;

template<class T, class ObjectT, int MAX_OBJ = 1024>
class ObjectManager
{
public:
  enum CONSTANT
  {
    MAX_BULLETS = MAX_OBJ,
    MAX_ACTIONS = MAX_OBJ
  };

  typedef Action<ObjectT> ActionT;

  ScriptManager const* scm;

  sw2::ObjectPool<ObjectT, MAX_BULLETS> objects;
  sw2::ObjectPool<ActionT, MAX_ACTIONS> actions;

  ObjectManager() : scm(0)
  {
  }

  void init(ScriptManager const* scm_)
  {
    scm = scm_;
  }

  void freeAction(int id)
  {
    if (!actions.isUsed(id)) {
      return;
    }

    Action<ObjectT>& a = actions[id];

    if (-1 != a.idCaller && actions.isUsed(a.idCaller)) {
      assert(actions[a.idCaller].idCallee == id);
      actions[a.idCaller].idCallee = -1;
    }

    if (-1 != a.idObj && objects.isUsed(a.idObj)) {
      objects[a.idObj].idAction = -1;
    }

    actions.free(id);
  }

  void freeObject(int id)
  {
    if (!objects.isUsed(id)) {
      return;
    }

    ObjectT& o = objects[id];

    if (-1 != o.idAction &&
        actions.isUsed(o.idAction) &&
        actions[o.idAction].idObj == id) {
      actions.free(o.idAction);
    }

    objects.free(id);
  }

  void reset()
  {
    objects.clear();
    actions.clear();
  }

  int run(std::string const& name, float x, float y)
  {
    if (scm->end() == scm->find(name)) {
      return -1;
    }

    int idAction = actions.alloc();
    if (-1 == idAction) {
      return -1;
    }

    Script const& sc = scm->find(name)->second;
    actions[idAction].set(*((T*)this), idAction, -1, x, y, .0f, .0f, &sc);

    return idAction;
  }

  template<typename PlayerT>
  void update(float fElapsed, PlayerT& player)
  {
    updateActions(fElapsed, player);
    updateObjects(fElapsed, player);
  }

  //
  // Internal helper.
  //

  template<typename PlayerT>
  void updateActions(float fElapsed, PlayerT& player)
  {
    for (int i = actions.first(); -1 != i;) {

      int next = actions.next(i);
      if (!actions[i].update(fElapsed, *((T*)this), player)) {
        static_cast<T*>(this)->freeAction(i);
      }

      if (!actions.isUsed(next)) {      // May freed in actions[i].update.
        break;
      }

      i = next;
    }
  }

  template<typename PlayerT>
  void updateObjects(float fElapsed, PlayerT& player)
  {
    for (int i = objects.first(); -1 != i;) {

      int next = objects.next(i);
      if (!objects[i].update(fElapsed, *((T*)this), player)) {
        static_cast<T*>(this)->freeObject(i);
      }

      if (!objects.isUsed(next)) {      // May freed in objects[i].update.
        break;
      }

      i = next;
    }
  }
};

} // namespace stge

// end of file om.h
