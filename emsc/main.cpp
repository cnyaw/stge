//
// 2021/9/18 Waync Cheng.
//

#include <emscripten.h>

#include "stge.h"

int main(int argc, char* argv[])
{
  emscripten_run_script("ongameload()");
  emscripten_exit_with_live_runtime();
}
