// hpp.h: include file for standart system include files or include files for specific project.

#pragma once

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN // Exclude rarely used components from Windows headers.

#include <Windows.h>
#include <TlHelp32.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <mutex>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <deque>
#include <map>
#include <ctime>
#include <thread>
#include <functional>
#include <tchar.h>
#include <io.h>
#include <shellapi.h>
#include <assert.h>

//#define LICENSING
#define DEBUG