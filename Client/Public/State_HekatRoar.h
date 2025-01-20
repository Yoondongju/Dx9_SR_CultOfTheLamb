#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_HekatRoar final : public CState
{
private:
    CState_HekatRoar(class CFsm* pFsm);
    virtual ~CState_HekatRoar() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    HRESULT Create_Frog();
    HRESULT Create_Bat();
    HRESULT Create_Human();
    HRESULT Create_Turret();

private:
    class CBoss_Hekat*  m_pHekat = { nullptr };
    _bool               m_bMonsterNot = true;
    _uint               m_iMonsterNumber = 0;

public:
    static CState_HekatRoar* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};
END





