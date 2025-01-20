#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_PlayerWarp final : public CState
{
	enum WARP_STATE
	{
		WARP_IN,
		WARP_OUT,
		WARP_END
	};

private:
	CState_PlayerWarp(class CFsm* pFsm);
	virtual ~CState_PlayerWarp() = default;

public:
	virtual HRESULT Initialize(_uint iStateNum) override;
	virtual HRESULT	Start_State() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	End_State() override;

private:
	class CPlayer* m_pPlayer = nullptr;
	WARP_STATE m_eCurState = WARP_IN;
	WARP_STATE m_ePreState = WARP_END;

private:
	void	Check_State();
	void	Check_State_Init();
	void	Warp_In();
	void	Warp_Out();
	void	Warp_In_Init();
	void	Warp_Out_Init();

public:
	static CState_PlayerWarp* Create(class CFsm* pFsm, _uint iStateNum);
	virtual void Free() override;
};
END



