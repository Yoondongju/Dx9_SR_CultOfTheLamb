#pragma once

#include "Base.h"

/* 모든 게임내에 사용되는 게임 오브젝트들의 부모 클래스다. */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	_bool	Get_Dead() { return m_isDead; }
	void	Set_Dead(_bool isDead) { m_isDead = isDead; }

	_bool	Get_isFilp() { return m_isFilp; }
	void    Set_isFilp(_bool bFilp)
	{
		m_isFilp = bFilp;
	}
	_wstring Get_LayerTag() { return m_strLayerTag; }
	void Set_LayerTag(_wstring strLayerTag) { m_strLayerTag = strLayerTag; }

	void Set_LevelIndex(_uint iLevelIndex) { m_iLevelIndex = iLevelIndex; }
	
	_uint Get_StageIndex() { return m_iStageIndex; }
	void Set_StageIndex(_uint iStageIndex) { m_iStageIndex = iStageIndex; }

	class CTransform* Get_Transform() { return m_pTransformCom; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	class CComponent* Find_Component(const _wstring& strComponentTag);
	class CComponent* Find_Component(const COMPONENT_TYPE eComponentType);

	void Set_Position(_float3 vPosition);
	_float3 Get_Position();

public:
	virtual void OnCollisionEnter(CGameObject* pOther) {};
	virtual void OnCollisionStay(CGameObject* pOther) {};
	virtual void OnCollisionExit(CGameObject* pOther) {};

protected:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

	_uint						m_iLevelIndex = {};
	_uint						m_iStageIndex = {};
	_wstring					m_strLayerTag = {};

	_bool						m_isCloned = { false };
	_bool						m_isDead = { false };
	_bool						m_isFilp = { false };

	_float						m_fRadius = { 0.5f };
	_uint						m_iAlpha = { 255 };
	

protected:
	map<const _wstring, class CComponent*>			m_Components;

protected:
	class CTransform*	  m_pTransformCom = { nullptr };
	class CCollider_Cube* m_pColliderCom = { nullptr };

protected:
	HRESULT Add_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);



public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual CGameObject* Clone_UI(void* pArg, void** pOut) { return nullptr; }

	virtual void Free() override;
};

END