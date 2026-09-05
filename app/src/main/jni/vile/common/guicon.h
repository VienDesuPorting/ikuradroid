#ifndef _GUICON_H_
#define _GUICON_H_

#ifdef VILE_ARCH_MICROSOFT
#ifdef VILE_BUILD_W32CONSOLE
#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>

void RedirectIOToConsole();
#endif
#endif
#endif
