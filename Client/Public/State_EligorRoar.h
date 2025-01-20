#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_EligorRoar final : public CState
{
private:
	CState_EligorRoar(class CFsm* pFsm);
	virtual ~CState_EligorRoar() = default;

public:
	virtual	HRESULT Initialize(_uint iStateNum) override;
	virtual HRESULT Start_State() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	End_State() override;

private:
	class CBoss_Eligor* m_pEligor = nullptr;
	_bool				m_bCameraEnd = true;
	_bool				m_bEffectOnce = true;

public:
	static CState_EligorRoar* Create(class CFsm* pFsm, _uint iStateNum);
	virtual void Free() override;
};
END
