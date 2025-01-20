#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_PlayerAttack1 final : public CState
{
private:
	CState_PlayerAttack1(class CFsm* pFsm);
	virtual ~CState_PlayerAttack1() = default;

public:
	virtual HRESULT Initialize(_uint iStateNum) override;
	virtual HRESULT	Start_State() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	End_State() override;

private:
	class CPlayer*	m_pPlayer = nullptr;
	_bool			m_bAttackMove = true;
	POINT			m_ptMousePos = {};
	_float3			vPlayer_Pos = { 0.f,0.f,0.f };
	_float3			vMousePos = { 0.f,0.f,0.f };
	_float3			m_vRayPos = { 0.f,0.f,0.f };
	_float3			m_vRayDir = { 0.f,0.f,0.f };
	_float3			m_vInterPos = { 0.f,0.f,0.f };
	_float			m_fComboTimer = 0.f;
	_bool			m_bEffectOnce = true;

private:
	void Check_Move();
	void Check_Mouse();

public:
	static CState_PlayerAttack1* Create(class CFsm* pFsm,_uint iStateNum);
	virtual void Free() override;
};
END


