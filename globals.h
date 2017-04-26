#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include "View.h"


namespace global
{
    View* hgView;

    bool isRunning = true;

    extern std::wstring s2ws(const std::string&, int*);
}

#endif // GLOBALS_H_INCLUDED
