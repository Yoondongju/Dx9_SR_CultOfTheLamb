#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_PlayerIdle final : public CState
{
private:
    CState_PlayerIdle(class CFsm* pFsm);
    virtual ~CState_PlayerIdle() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CPlayer* m_pPlayer = nullptr;
    _float         m_fHitTimer = 0.f;
    _bool          m_bDontAttack = true;

private:
    void Check_Attack();
    void Check_Move();
    void Check_Dead();
    void Check_Hit();

public:
    static CState_PlayerIdle* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};
END
