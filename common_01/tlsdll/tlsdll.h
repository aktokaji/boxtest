#ifndef TLSDLL_H
#define TLSDLL_H

#include "tlsdll_global.h"

#include "windows.h"

class TLSDLLSHARED_EXPORT Tlsdll
{

public:
    Tlsdll();
};

TLSDLLSHARED_EXPORT int test_add(int x, int y);

#endif // TLSDLL_H
