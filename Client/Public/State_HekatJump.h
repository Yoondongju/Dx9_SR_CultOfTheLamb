#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_HekatJump final : public CState
{
    enum HEKAT_JUMP_STATE
    {
        JUMP_START,
        JUMP,
        JUMP_END,
        STATE_JUMP_END
    };

private:
    CState_HekatJump(class CFsm* pFsm);
    virtual ~CState_HekatJump() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    void Check_State(_float fTimeDelta);
    void Check_State_Init();

    void Jump_Start();
    void Jump(_float fTimeDelta);
    void Jump_End();

    void Jump_Start_Init();
    void Jump_Init();
    void Jump_End_Init();

private:
    class CBoss_Hekat* m_pHekat = { nullptr };
    HEKAT_JUMP_STATE m_eCurState = JUMP_START;
    HEKAT_JUMP_STATE m_ePreState = STATE_JUMP_END;
    _float3 m_vPos = { 0.f,0.f,0.f };
    _float3 m_vDir = { 0.f,0.f,0.f };

public:
    static CState_HekatJump* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};
END

