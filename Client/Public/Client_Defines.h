#pragma once

//#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <ctime>

#include <fstream>
#include <iomanip>

#ifndef D3DXMATRIX_IO_H
#define D3DXMATRIX_IO_H

#include <iostream> 
#include <d3dx9math.h> 


#ifdef _D3DX9MATH_H_ 
std::istream& operator>>(std::istream& is, D3DXMATRIX& mat) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			is >> mat.m[i][j];
		}
	}
	return is;
}
#else
#endif

#endif 

#include <process.h>

#define EPSILON 0.01f

namespace Client
{
	const unsigned int			g_iWinSizeX = { 1280 };
	const unsigned int			g_iWinSizeY = { 720 };

	//플레이 상 레벨 총 17개
	enum LEVELID 
	{ 
		LEVEL_STATIC,	//0
		LEVEL_LOADING,  //1
		LEVEL_LOGO,		//2
		LEVEL_GAMEPLAY,	//3	//요게 Village, 던전 이동 문 2개
		LEVEL_DUNGEON0,	//4	// 구시옹 5개
		LEVEL_DUNGEON1,	//5	// 엘리고스 4개
		LEVEL_DUNGEON2,	//6	// 헤켓 4개
		LEVEL_DUNGEON3,	//7	// 최종 보스 맵, 최종 보스 전 타로 맵

		LEVEL_END 
	};

	enum SOUND_ID { SOUND_BGM, SOUND_PLAYER, SOUND_FOLLOWER, SOUND_EFFECT, SOUND_UI, SOUND_UI2, SOUND_MONSTER, SOUND_GUSION, SOUND_ELIGOR, SOUND_HEKAT, SOUND_DEATHCAT, SOUND_EYE, SOUND_MONSTERATTACK, SOUND_ITEM, SOUND_NPC, SOUND_END };	//BGM은 항상 0번

	enum ADORN_TYPE { VILLAGE, STAGE1, STAGE2, STAGE3 };
	enum MODEL_TYPE { MODELRECT , GRIDRECT};

	enum MONSTER_ID { FROG, FROWSUMMON, BAT, HUMAN, TURRET, GUSION, ELIGOS, HEKAT, AYM, DEATHCAT, EYE, MONSTER_END };;

	
}

extern HWND g_hWnd;
extern bool g_isLoaded[Client::LEVEL_END];
extern float g_fBgmVolume;
extern float g_fEffectVolume;
extern bool g_isFirstLoading;



using namespace Client;
using namespace std;