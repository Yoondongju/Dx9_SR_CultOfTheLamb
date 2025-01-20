#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_EligorUC final : public CState
{
private:
	CState_EligorUC(class CFsm* pFsm);
	virtual ~CState_EligorUC() = default;

public:
	virtual	HRESULT Initialize(_uint iStateNum) override;
	virtual HRESULT Start_State() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	End_State() override;

private:
	class CBoss_Eligor* m_pEligor = nullptr;

	_float m_fZoomAmount = { 0.f };
	_float m_fHoldTime = { 0.f };

public:
	static CState_EligorUC* Create(class CFsm* pFsm, _uint iStateNum);
	virtual void Free() override;
};
END
