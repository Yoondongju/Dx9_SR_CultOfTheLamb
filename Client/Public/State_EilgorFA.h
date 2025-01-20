#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_EligorFA final : public CState
{
	enum FLY_ATTACK
	{
		ATTACK_CHARGE,
		ATTACK,
		ATTACK_STOP,

		FLY_ATTACK_END
	};

private:
	CState_EligorFA(class CFsm* pFsm);
	virtual ~CState_EligorFA() = default;

public:
	virtual	HRESULT Initialize(_uint iStateNum) override;
	virtual HRESULT Start_State() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	End_State() override;

private:
	class CBoss_Eligor* m_pEligor = nullptr;
	_float3				m_vDistance = { 0.f,0.f,0.f };
	_float				m_fDistanceCheck = 0.f;
	_float2				m_vCenterPos = { 0.f,0.f };
	_float				m_fAngleSpeed = 0.f;
	_float				m_fAngle = 0.f;
	_float				m_fSoundTimer = 0.f;
	FLY_ATTACK m_eCurState = ATTACK_CHARGE;
	FLY_ATTACK m_ePreState = FLY_ATTACK_END;

private:
	void Check_State(_float fTimeDelta);
	void Check_State_Init();

	void Attack_Charge();
	void Attack(_float fTimeDelta);
	void Attack_Stop();

	void Attack_Charge_Init();
	void Attack_Init();
	void Attack_Stop_Init();

private:
	_float3 m_vDir = { 0.f,0.f,0.f };
	_float3 Temp = { 0.f,0.f,0.f };
	_float  m_fFlyTimer = 0.f;

public:
	static CState_EligorFA* Create(class CFsm* pFsm, _uint iStateNum);
	virtual void Free() override;
};
END

