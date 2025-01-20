#pragma once
#include "Base.h"

#include "Collider_Cube.h"
#include "Collider_Rect.h"

BEGIN(Engine)

class CCollider_Manager :
    public CBase
{
private:
	CCollider_Manager();
	virtual ~CCollider_Manager() = default;

public:
    HRESULT Initialize(); 

public:
    void Collision_Cube(_wstring strLayerTag1, _wstring strLayerTag2, _uint iLevelIndex1, _uint iLevelIndex2);

private:
	class CGameInstance* m_pGameInstance = { nullptr };


public:
	static CCollider_Manager* Create();
	virtual void Free() override;
};

END