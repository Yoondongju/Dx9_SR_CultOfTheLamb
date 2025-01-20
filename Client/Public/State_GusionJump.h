#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_GusionJump final : public CState
{
	enum GUSION_JUMP
	{
		JUMP_START,
		JUMP_END,
		JUMP_STATE_END
	};

private:
	CState_GusionJump(class CFsm* pFsm);
	virtual ~CState_GusionJump() = default;

public:
	virtual HRESULT Initialize(_uint iStateNum) override;
	virtual HRESULT Start_State() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	End_State() override;

private:
	class CBoss_Gusion* m_pGusion = nullptr;
	_float3		 m_vDir = { 0.f,0.f,0.f };
	_float3 m_vDistance = { 0.f,0.f,0.f };
	_float  m_fDistanceCheck = 0.f;
	_uint		 m_iJumpCount = 0;
	GUSION_JUMP m_eCurState = JUMP_START;
	GUSION_JUMP m_ePreState = JUMP_STATE_END;

private:
	void Check_State(_float fTimeDelta);
	void Check_State_Init();

	void Jump_Start(_float fTimeDelta);
	void Jump_End(_float fTimeDelta);

	void Jump_Start_Init();
	void Jump_End_Init();

public:
	static CState_GusionJump* Create(class CFsm* pFsm, _uint iStateNum);
	virtual void Free() override;
};
END
