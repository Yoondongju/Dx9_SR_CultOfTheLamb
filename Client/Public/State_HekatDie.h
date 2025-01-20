#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_HekatDie final : public CState
{
private:
	CState_HekatDie(class CFsm* pFsm);
	virtual ~CState_HekatDie() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CBoss_Hekat*  m_pHekat = { nullptr };
    _bool               m_bDeadOnce = true;

public:
    static CState_HekatDie* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};
END
