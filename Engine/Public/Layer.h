#pragma once

#include "Base.h"

/* ��ü���� �����Ѵ�. */

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	list<class CGameObject*>&  Get_ObjectList() { return m_GameObjects; }

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	HRESULT Priority_Update(_float fTimeDelta);
	HRESULT Update(_float fTimeDelta);
	HRESULT Late_Update(_float fTimeDelta);

public:
	class CComponent*	Find_Component(const _wstring& strComponentTag, _uint iIndex);
	class CGameObject*	Find_Object(_uint iIndex);

private:
	list<class CGameObject*>			m_GameObjects;
	

	

public:
	static CLayer* Create();
	virtual void Free() override;
};

END