#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_PlayerSpell final : public CState
{
private:
	CState_PlayerSpell(class CFsm* pFsm);
	virtual ~CState_PlayerSpell() = default;

public:
	_float	Get_BulletTimer() { return m_fBulletTimer; }


public:
	virtual HRESULT Initialize(_uint iStateNum) override;
	virtual HRESULT	Start_State() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	End_State() override;

private:
	class CPlayer*	m_pPlayer = nullptr;
	_float			m_fBulletTimer = 0.f;
	POINT			m_ptMousePos = {};
	_float3			vMousePos = { 0.f,0.f,0.f };
	_float3			m_vRayPos = { 0.f,0.f,0.f };
	_float3			m_vRayDir = { 0.f,0.f,0.f };
	_float3			m_vInterPos = { 0.f,0.f,0.f };
	_bool			m_bEffectOnce = true;

public:
	static CState_PlayerSpell* Create(class CFsm* pFsm, _uint iStateNum);
	virtual void Free() override;
};
END


