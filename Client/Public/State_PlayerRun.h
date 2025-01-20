#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_PlayerRun final : public CState
{

private:
	enum RUN_STATE
	{
		RUN_HOR,
		RUN_UP,
		RUN_DOWN,
		RUN_DU,
		RUN_DD,
		RUN_END
	};

private:
	CState_PlayerRun(class CFsm* pFsm);
	virtual ~CState_PlayerRun() = default;

public:
	virtual HRESULT Initialize(_uint iStateNum) override;
	virtual HRESULT	Start_State() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	End_State() override;

private:
	class CPlayer*	m_pPlayer = nullptr;
	RUN_STATE		m_eCurState = RUN_HOR;
	RUN_STATE		m_ePreState = RUN_END;
	_float3			m_vPlayerPos = { 0.f,0.f,0.f };
	_float			m_fSoundTimer = 0.f;

private:
	_float2 Get_MoveVector();
	void	Check_Roll();
	void	Check_Hit();
	void	Check_Dead();
	void	Check_State(_float fTimeDelta);
	void	Check_State_Init();

private:
	void	Run_Up(_float fTimeDelta);
	void	Run_Down(_float fTimeDelta);
	void	Run_Horizontal(_float fTimeDelta);
	void	Run_Diagonal_Up(_float fTimeDelta);
	void	Run_Diagonal_Down(_float fTimeDelta);

	void	Run_Up_Init();
	void	Run_Down_Init();
	void	Run_Horizontal_Init();
	void	Run_Diagonal_Up_Init();
	void	Run_Diagonal_Down_Init();
	
public:
	static CState_PlayerRun* Create(class CFsm* pFsm, _uint iStateNum);
	virtual void Free() override;
};
END
