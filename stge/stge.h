
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/03/28 Waync created.
//

#pragma once

//
// STD.
//

#include <cmath>

#include <algorithm>
#include <list>
#include <map>
#include <string>
#include <sstream>
#include <vector>

//
// smallworld.
//

#include "swObjectPool.h"
#include "swUtil.h"

//
// stge.
//

enum STGE_CONTEXT_VALUE
{
  DIRECTION = 0,
  SPEED,
  X,
  Y,
  NUM_VAL
};

struct Context
{
  float const *param;
  int *repeat;
  float *val[NUM_VAL];                  // dir, spd, x, y.
  int w, h;                             // Window w/h.
};

#include "math.h"
#include "exp.h"
#include "p.h"
#include "o.h"
#include "sc.h"
#include "om.h"
#include "ac.h"

// end of stge.h
