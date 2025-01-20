#pragma once

namespace Engine
{
	typedef struct
	{
		HWND			hWnd;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
		bool			isWindowsed;
	}ENGINE_DESC;

	typedef struct 
	{
		D3DXVECTOR3		vPosition;
		D3DXVECTOR2		vTexcoord;
	}VTXTEX;

	typedef struct
	{
		D3DXVECTOR3		vPosition;
		D3DXVECTOR3		vNormal;
		D3DXVECTOR2		vTexcoord;
	}VTXNORTEX;

	typedef struct
	{
		D3DXVECTOR3		vPosition;		
		D3DXVECTOR3		vTexcoord;
	}VTXCUBE;

	typedef struct
	{
		D3DXVECTOR3		vPosition;
		D3DXVECTOR2		vTexcoord1;
		D3DXVECTOR2		vTexcoord2;
	}VTXTERRAIN;

	struct VTXPARTICLE
	{
		D3DXVECTOR3		vPosition;
		D3DCOLOR		vColor;
	};

	struct ATTRIBUTE 
	{
		D3DXVECTOR3 vPosition;		// 월드 스페이스 내 파티클 위치
		D3DXVECTOR3 vVelocity;		// 파티클 속도
		D3DXVECTOR3 vAcceleration;	// 파티클 가속도
		float		fLifeTime;		// 파티클이 소멸할 때 까지 유지되는 시간
		float		fAge;			// 파티클의 현재 나이
		D3DXCOLOR   Color;			// 파티클의 색
		D3DXCOLOR   ColorFade;		// 파티클 색이 시간이 지나면서 흐려지는 경우
		bool		bIsAlive;		// 파티클이 생존한 경우 True , 소멸한 경우 False
		int			iIndex;
		int			iLevelIndex;
		int			iStageIndex;
	};

	struct RANDOM
	{
		static float Get_RandomFloat(float min, float max)
		{
			if (min > max)
				std::swap(min, max);

			float f = (rand() % 10000) * 0.0001f;
			return (f * (max - min) + min);
		}

		static D3DXVECTOR3 Get_RandomVector(const D3DXVECTOR3& min, const D3DXVECTOR3& max)
		{
			D3DXVECTOR3 tmp;
			tmp.x = Get_RandomFloat(min.x, max.x);
			tmp.y = Get_RandomFloat(min.y, max.y);
			tmp.z = Get_RandomFloat(min.z, max.z);
			return tmp;
		}

	};
}