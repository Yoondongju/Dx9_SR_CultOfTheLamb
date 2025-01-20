#pragma once

#pragma warning (disable : 4251)
#pragma warning(disable : 5208) //구조체 경고 끄기

#include <d3d9.h>
#include <d3dx9.h>

#include <map>
#include <list>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <string>

#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;


#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"

using namespace Engine;

#define			OBJ_NOEVENT		0
#define			OBJ_DEAD		1

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG
 
extern bool g_DebugMode;