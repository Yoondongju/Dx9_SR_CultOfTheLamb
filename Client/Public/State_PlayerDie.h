#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_PlayerDie final : public CState
{
private:
	CState_PlayerDie(class CFsm* pFsm);
	virtual ~CState_PlayerDie() = default;

public:
	virtual HRESULT Initialize(_uint iStateNum) override;
	virtual HRESULT	Start_State() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	End_State() override;

private:
	class CPlayer* m_pPlayer = nullptr;

public:
	static CState_PlayerDie* Create(class CFsm* pFsm, _uint iStateNum);
	virtual void Free() override;
};
END
