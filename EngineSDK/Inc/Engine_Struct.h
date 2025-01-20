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
		D3DXVECTOR3 vPosition;		// ���� �����̽� �� ��ƼŬ ��ġ
		D3DXVECTOR3 vVelocity;		// ��ƼŬ �ӵ�
		D3DXVECTOR3 vAcceleration;	// ��ƼŬ ���ӵ�
		float		fLifeTime;		// ��ƼŬ�� �Ҹ��� �� ���� �����Ǵ� �ð�
		float		fAge;			// ��ƼŬ�� ���� ����
		D3DXCOLOR   Color;			// ��ƼŬ�� ��
		D3DXCOLOR   ColorFade;		// ��ƼŬ ���� �ð��� �����鼭 ������� ���
		bool		bIsAlive;		// ��ƼŬ�� ������ ��� True , �Ҹ��� ��� False
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