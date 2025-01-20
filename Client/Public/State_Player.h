#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CState_Player abstract : public CState
{
protected:
	CState_Player(class CFsm* pFsm);
	virtual ~CState_Player() = default;

public:
	virtual HRESULT Initialize(_uint iStateNum);
	virtual HRESULT	Start_State() PURE;
	virtual void	Update(_float fTimeDelta) PURE;
	virtual void	End_State() PURE;

protected:
	class CPlayer* m_pPlayer = nullptr;

public:
	virtual void Free() override;
};

END
