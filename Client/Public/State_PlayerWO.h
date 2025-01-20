#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_PlayerWO final : public CState
{

private:
	CState_PlayerWO(class CFsm* pFsm);
	virtual ~CState_PlayerWO() = default;

public:
	virtual HRESULT Initialize(_uint iStateNum) override;
	virtual HRESULT	Start_State() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	End_State() override;

private:
	class CPlayer* m_pPlayer = nullptr;


public:
	static CState_PlayerWO* Create(class CFsm* pFsm, _uint iStateNum);
	virtual void Free() override;
};
END





