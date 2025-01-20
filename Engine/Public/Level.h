#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel() = default;

public:
	_uint Get_StageIndex() { return m_iStageIndex; }

public:
	virtual HRESULT Initialize();
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual HRESULT Start_StageChange(_int iNextStageIndex) { return S_OK; }

	virtual HRESULT Change_Stage(_int iStageIndex) { return S_OK; }

protected:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	_uint						m_iStageIndex = {};
	_uint						m_iPreStageIndex = 0;
	_int						m_iNextStageIndex = {};

	_float3						m_vPlayerInitPos = { 0.f,0.f,0.f };
	_float						m_fSizeTimer = 0.f;

	_bool						m_isChangeStage = { false };
	_bool						m_bPlayerChangeOnce = false;
	_bool						m_bPlayerSizeOnce = true;

public:
	virtual void Free() override;
};

END