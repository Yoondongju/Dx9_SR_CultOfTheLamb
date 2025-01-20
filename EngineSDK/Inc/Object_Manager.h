#pragma once

#include "Base.h"

/* ��ü���� ��Ƽ� �׷�(Layer)���� �����Ѵ�. */
/* ��� ���� ��ü���� �ݺ����� Update�� ȣ�����ش�. */
/* ��� ���� ��ü���� �ݺ����� Render�� ȣ�����ش�.(x) : ��ü���� �׸��� ������ ���� ���� �� ������ �ؾ��� �ʿ䰡 �ִ�. */

BEGIN(Engine)

class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CGameObject* Get_Player() { return m_pPlayer; }
	class CLayer* Get_CurLevel_Layer(_uint iCurLevel, _wstring strLayerTag) { return m_pLayers[iCurLevel][strLayerTag]; }

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);	

	HRESULT Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg);
	HRESULT Add_CloneObject_ToLayer_UI(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg, void** pOut);

	HRESULT	Add_Player(CGameObject* pPlayer);
	HRESULT Priority_Update(_float fTimeDelta);
	HRESULT Update(_float fTimeDelta);
	HRESULT Late_Update(_float fTimeDelta);
	void Clear(_uint iLevelIndex);

public:
	class CComponent*	Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex);
	class CLayer*		Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	class CGameObject*  Find_Prototype(const _wstring& strPrototypeTag);

	class CGameObject*  Find_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex);

	class CGameObject*	Find_Player();
	class CGameObject*  Find_Camera();

private:

	/* ������ü���� �����Ѵ�. */
	map<const _wstring, class CGameObject*>		m_Prototypes;

	/* ��������, �纻��ü���� �׷캰�� ��Ƽ� �����Ѵ�. */
	_uint										m_iNumLevels = {};
	map<const _wstring, class CLayer*>*			m_pLayers = { nullptr };
	typedef map<const _wstring, class CLayer*>		LAYERS;

	class CGameObject*							m_pPlayer = { nullptr };



public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END