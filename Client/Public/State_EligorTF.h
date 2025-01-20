#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_EligorTF final : public CState
{
private:
	CState_EligorTF(class CFsm* pFsm);
	virtual ~CState_EligorTF() = default;

public:
	virtual	HRESULT Initialize(_uint iStateNum) override;
	virtual HRESULT Start_State() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	End_State() override;

private:
	class CBoss_Eligor* m_pEligor = nullptr;
	_bool				m_bDeadOnce = true;

public:
	static CState_EligorTF* Create(class CFsm* pFsm, _uint iStateNum);
	virtual void Free() override;
};
END
