#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_PlayerRoll final : public CState
{
	enum PLAYER_ROLL
	{
		ROLL_UP,
		ROLL_DOWN,
		ROLL_HOR,
		ROLL_DU,
		ROLL_DD,
		STATE_ROLL_END
	};
private:
	CState_PlayerRoll(class CFsm* pFsm);
	virtual ~CState_PlayerRoll() = default;

public:
	virtual HRESULT Initialize(_uint iStateNum) override;
	virtual HRESULT	Start_State() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	End_State() override;

private:
	class CPlayer* m_pPlayer = nullptr;
	PLAYER_ROLL m_eCurState = ROLL_UP;
	PLAYER_ROLL m_ePreState = STATE_ROLL_END;
	_bool		m_bCheckState = true;
	_float		m_fRollEndTimer = 0.f;

private:
	_float2 Get_MoveVector();
	void	Check_State(_float fTimeDelta);
	void	Check_State_Init();

	void	Roll(_float fTimeDelta);
	void	Roll_Down(_float fTimeDelta);
	void	Roll_Horizontal(_float fTimeDelta);
	void	Roll_Diagonal_Up(_float fTimeDelta);
	void	Roll_Diagonal_Down(_float fTimeDelta);

	void	Roll_Up_Init();
	void	Roll_Down_Init();
	void	Roll_Horizontal_Init();
	void	Roll_Diagonal_Up_Init();
	void	Roll_Diagonal_Down_Init();

public:
	static CState_PlayerRoll* Create(class CFsm* pFsm, _uint iStateNum);
	virtual void Free() override;
};
END
