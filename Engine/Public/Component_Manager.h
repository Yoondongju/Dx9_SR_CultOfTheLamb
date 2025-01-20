#pragma once


#include "VIBuffer_Terrain.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_GridRect.h"
#include "VIBuffer_Rhombus.h"

#include "Transform.h"
#include "Texture.h"
#include "Animator.h"
#include "Collider_Rect.h"
#include "Collider_Cube.h"
#include "Fsm.h"
#include "Shader.h"
#include "RigidBody.h"


/* 컴포넌트의 원형을 레벨별로 보관한다. */
/* 복제하여 리턴한다.*/

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg);

private:
	map<const _wstring, class CComponent*>*		m_pPrototypes = { nullptr };
	_uint										m_iNumLevels = { 0 };
private:
	class CComponent* Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag);

public:
	static CComponent_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END