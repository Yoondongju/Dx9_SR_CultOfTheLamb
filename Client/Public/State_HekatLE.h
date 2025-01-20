#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_HekatLE final : public CState
{
private:
    CState_HekatLE(class CFsm* pFsm);
    virtual ~CState_HekatLE() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CBoss_Hekat* m_pHekat = { nullptr };
    _float m_fBulletTimer = 0.f;
    _float3 m_vPlayerPos = { 0.f,0.f,0.f };
    _float Temp = 0.f;

private:
    void Create_Bullet();

public:
    static CState_HekatLE* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};
END


