#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "application.h"

int main()
{
    Application SandSimul;
    SandSimul.run();
}
