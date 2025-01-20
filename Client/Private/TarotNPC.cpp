#include "TarotNPC.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "Animator.h"
#include "TarotPanel.h"
#include <UI.h>
#include <Player.h>

CTarotNPC::CTarotNPC(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CLandObject(pGraphic_Device)
{
}

CTarotNPC::CTarotNPC(const CTarotNPC& Prototype)
    :CLandObject(Prototype)
{
}

HRESULT CTarotNPC::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTarotNPC::Initialize(void* pArg)
{
    TAROTNPC_INFO* tInfo = static_cast<TAROTNPC_INFO*>(pArg);
    m_tInfo.vPos = tInfo->vPos;

    if (FAILED(__super::Initialize(&tInfo->tLandObjectDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Animation()))
        return E_FAIL;

    //초기 세팅
    _float3 vScale = _float3{ 6.f,6.f,0.1f };

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);
    m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);

    return S_OK;
}

void CTarotNPC::Priority_Update(_float fTimeDelta)
{
    if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
        return;

    if (m_pTarotPanel == nullptr)
    {
        CUI* pUI = dynamic_cast<CUI*>(m_pGameInstance->Find_Object(m_iLevelIndex, TEXT("Layer_TarotUI"), 0));
        if (pUI != nullptr)
        {
            m_pTarotPanel = dynamic_cast<CTarotPanel*>(pUI);
            if (m_pTarotPanel == nullptr)
                return;
        }
    }
}

void CTarotNPC::Update(_float fTimeDelta)
{
    if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
        return;

    __super::SetUp_OnTerrain(m_pTransformCom, 0.5f);

    Compute_PlayerFisrtCard();

    if (m_pTarotPanel->GetActivate())
        m_bCanSelcetCard = false;   //E버튼 안내창 닫기
    else if (m_pTarotPanel->GetActivate() == false && m_bPlayerPickedCard == false)
        Calculate_CanSelcetCard();  //거리로 상호작용 가능 여부 확인
    else if(m_bPlayerPickedCard == true)
        m_bCanSelcetCard = false;   //이미 카드를 뽑았으면 아예 다시 못뽑게 함

    if (m_bCanSelcetCard && m_pGameInstance->Get_KeyState(KEY::E) == KEY_STATE::TAP)
    {
        m_pTarotPanel->SetActivate(true);
        dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_State(CPlayer::TAROT);
    }
}

void CTarotNPC::Late_Update(_float fTimeDelta)
{
    if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
        return;

    m_pAnimatorCom->Update(fTimeDelta);

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTarotNPC::Render()
{
    m_pVIBufferCom->Filp_Texture(m_isFilp);

    if (FAILED(m_pTransformCom->Bind_Transform()))
        return E_FAIL;

    m_pAnimatorCom->Render(NULL);

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(SetUp_RenderState()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    if (FAILED(Reset_RenderState()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTarotNPC::Ready_Components()
{  /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    /* For.Com_Transform */
    CTransform::TRANSFORM_DESC		TransformDesc{};

    TransformDesc.fSpeedPerSec = 5.0f;
    TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTarotNPC::Ready_Animation()
{
    CTexture* pTextureCom = {};

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
        TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_NPC_TarotNPC"),
        TEXT("Com_Texture_TarotNPC_Idle"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("IDLE"), pTextureCom, 0, 87, 0.05f);
    m_pAnimatorCom->Change_Animation(TEXT("IDLE"), 0);
    return S_OK;
}

HRESULT CTarotNPC::SetUp_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);


    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

    /* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 127);

    return S_OK;
}

HRESULT CTarotNPC::Reset_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    return S_OK;
}

void CTarotNPC::Calculate_CanSelcetCard()
{
    _float3 vDistance = m_pGameInstance->Find_Player()->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _float fDistance = D3DXVec3Length(&vDistance);

    if (fDistance < 5.f)
        m_bCanSelcetCard = true;
    else
        m_bCanSelcetCard = false;
}

void CTarotNPC::Compute_PlayerFisrtCard()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player());
    if (pPlayer == nullptr)
        return;

    m_bPlayerPickedCard = pPlayer->Get_PickCard();
}

CTarotNPC* CTarotNPC::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CTarotNPC* pInstance = new CTarotNPC(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CTarotNPC"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTarotNPC::Clone(void* pArg)

{
    CTarotNPC* pInstance = new CTarotNPC(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CTarotNPC"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTarotNPC::Free()
{
    __super::Free();

    Safe_Release(m_pAnimatorCom);
    Safe_Release(m_pVIBufferCom);
}
