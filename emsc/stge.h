//
// 2021/9/19 Waync Cheng.
//

#include <sstream>

#include "swGeometry.h"

#include "../stge/stge.h"
#include "../stge/par_yard.h"

sw2::IntRect Bounding;
sw2::IntRect OutterBounding;

template<class ObjMgrT>
class MyObj : public stge::Object
{
public:

  bool init(ObjMgrT& fac, int idAction, int idNewObj)
  {
    //
    // Return false if init failed, and clear this object.
    //

    return true;
  }

  template<typename PlayerT>
  bool update(float fElapsed, ObjMgrT& om, PlayerT& player)
  {
    //
    // Return false, and clear this object.
    //

    stge::Object::update(fElapsed, om, player);

    return OutterBounding.ptInRect(sw2::POINT_t<int>((int)x, (int)y));
  }
};

class MyObjectManager : public stge::ObjectManager<MyObjectManager, MyObj<MyObjectManager> >
{
public:
  static int getWindowWidth()
  {
    return Bounding.width();
  }

  static int getWindowHeight()
  {
    return Bounding.height();
  }
};

stge::ScriptManager scm;
MyObjectManager om;

extern "C" {

int EMSCRIPTEN_KEEPALIVE stge_init(int wbound, int hbound)
{
  om.init(&scm);
  Bounding = sw2::IntRect(-wbound/2, -hbound/2, wbound/2, hbound/2);
  OutterBounding = Bounding;
  OutterBounding.inflate(Bounding.width()/10, Bounding.height()/10);
  return 1;
}

int EMSCRIPTEN_KEEPALIVE stge_parse(const char *pScript)
{
  scm.clear();
  if (stge::Parser::parseFromStream(pScript, scm)) {
    return 1;
  }
  return 0;
}

const char* EMSCRIPTEN_KEEPALIVE stge_get_last_parse_error()
{
  static std::string s;
  s = stge::Parser::getLastError();
  return s.c_str();
}

int EMSCRIPTEN_KEEPALIVE stge_run(const char *pScript)
{
  om.reset();
  int id = om.run(pScript, 0, 0);
  return id;
}

int EMSCRIPTEN_KEEPALIVE stge_update(int dt, int xplayer, int yplayer)
{
  stge::Object p;
  p.x = (float)(xplayer - Bounding.width() / 2);
  p.y = (float)(yplayer - Bounding.height() / 2);
  om.update(dt / 1000.0f, p);
  return 0 != om.actions.size() || 0 != om.objects.size();
}

int EMSCRIPTEN_KEEPALIVE stge_get_script_count()
{
  return (int)scm.size();
}

const char* EMSCRIPTEN_KEEPALIVE stge_get_script(int idx)
{
  stge::ScriptManager::const_iterator it = scm.begin();
  for (int i = 0; i < idx && i < scm.size(); i++, it++) {
  }
  return it->first.c_str();
}

int EMSCRIPTEN_KEEPALIVE stge_get_action_count()
{
  return om.actions.size();
}

int EMSCRIPTEN_KEEPALIVE stge_get_obj_count()
{
  return om.objects.size();
}

static std::string getParam(const std::string &s)
{
  int i = atoi(s.c_str());
  switch (i) {
    case stge::Script::AIM:
      return "aim";
    case stge::Script::ADD:
      return "add";
    case stge::Script::OBJ:
      return "obj";
  }
  return "";
}

static bool generateParsedDataToStream(std::stringstream &ss, const std::string &type, const std::string &param, const stge::Script &sc)
{
  ss << "{\"script\":\"" + type + "\",\"param\":[\"" + param + "\"],\"sc\":[";
  bool firstElement = true;
  for (std::list<stge::Script>::const_iterator its = sc.sc.begin(); sc.sc.end() != its; ++its) {
    if (!firstElement) {
      ss << ",";
    }
    switch (its->type) {
      case stge::Script::REPEAT:
        generateParsedDataToStream(ss, "repeat", its->param[0].exp, *its);
        break;

      case stge::Script::OPTION:
        generateParsedDataToStream(ss, "option", its->param[0].exp, *its);
        break;

      case stge::Script::ELSE:
        ss << "{\"script\":\"else\",\"param\":[]}";
        break;

      case stge::Script::FORK:
      case stge::Script::CALL:
      case stge::Script::FIRE:
        {
          static const char* scmd[] = {"fork", "call", "fire"};
          ss << "{\"script\":\"" << scmd[its->type - stge::Script::FORK] << "\",\"param\":[" ;
          ss << "\"" << its->param[0].exp << "\",";
          ss << "\"" << its->param[1].exp << "\",";
          ss << "\"" << its->param[2].exp << "\",";
          ss << "\"" << its->param[3].exp << "\",";
          ss << "\"" << its->param[4].exp << "\",";
          ss << "\"" << its->param[5].exp << "\",";
          ss << "\"" << its->param[6].exp << "\"";
          ss << "]}";
        }
        break;

      case stge::Script::SLEEP:
        ss << "{\"script\":\"sleep\",\"param\":[" ;
        ss << "\"" << its->param[0].exp << "\"";
        ss << "]}";
        break;

      case stge::Script::DIRECTION:
        ss << "{\"script\":\"direction\",\"param\":[" ;
        ss << "\"" << its->param[0].exp << "\",";
        ss << "\"" << getParam(its->param[1].exp) << "\"";
        ss << "]}";
        break;

      case stge::Script::SPEED:
        ss << "{\"script\":\"speed\",\"param\":[" ;
        ss << "\"" << its->param[0].exp << "\",";
        ss << "\"" << getParam(its->param[1].exp) << "\"";
        ss << "]}";
        break;

      case stge::Script::CHANGEDIRECTION:
      case stge::Script::CHANGESPEED:
      case stge::Script::CHANGEX:
      case stge::Script::CHANGEY:
        {
          static const char* scmd[] = {"changedirection", "changespeed", "changex", "changey"};
          ss << "{\"script\":\"" << scmd[its->type - stge::Script::CHANGEDIRECTION] << "\",\"param\":[" ;
          ss << "\"" << its->param[0].exp << "\",";
          ss << "\"" << its->param[1].exp << "\",";
          ss << "\"" << getParam(its->param[2].exp) << "\"";
          ss << "]}";
        }
        break;

      case stge::Script::CLEAR:
        ss << "{\"script\":\"clear\",\"param\":[]}";
        break;

      case stge::Script::USERDATA:
        ss << "{\"script\":\"userdata\",\"param\":[" ;
        ss << "\"" << its->param[0].exp << "\",";
        ss << "\"" << its->param[1].exp << "\",";
        ss << "\"" << its->param[2].exp << "\",";
        ss << "\"" << its->param[3].exp << "\"";
        ss << "]}";
        break;
    }
    firstElement = false;
  }
  ss << "]}";
  return true;
}

const char* EMSCRIPTEN_KEEPALIVE stge_get_parsed_data()
{
  static std::string s;
  s = "[";
  bool firstScript = true;
  for (stge::ScriptManager::const_iterator it = scm.begin(); scm.end() != it; ++it) {
    std::stringstream ss;
    if (!generateParsedDataToStream(ss, "script", it->first, it->second)) {
      break;
    }
    if (!firstScript) {
      s += ",";
    }
    s += ss.str();
    firstScript = false;
  }
  s += "]";
  return s.c_str();
}

int EMSCRIPTEN_KEEPALIVE stge_get_obj_xpos(int idx)
{
  int id = om.objects.first();
  for (int i = 0; i < idx && -1 != id; id = om.objects.next(id), i++) {
  }
  if (-1 != id) {
    const stge::Object &o = om.objects[id];
    return (int)o.x;
  }
  return 10000;
}

int EMSCRIPTEN_KEEPALIVE stge_get_obj_ypos(int idx)
{
  int id = om.objects.first();
  for (int i = 0; i < idx && -1 != id; id = om.objects.next(id), i++) {
  }
  if (-1 != id) {
    const stge::Object &o = om.objects[id];
    return (int)o.y;
  }
  return 10000;
}

} // extern "C"
