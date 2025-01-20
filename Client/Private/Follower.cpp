 
#include "Follower.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "Collider_Cube.h"
#include "Animator.h"
#include "Fsm.h"
#include "VIBuffer_Rect.h"
#include "UI.h"
#include <Player.h>
#include <Camera.h>

#include "State_Follower_Idle.h"
#include "State_Follower_Walk.h"
#include "State_Follower_Mining.h"
#include "State_Follower_Axe.h"
#include "State_Follower_Pray.h"
#include "State_Follower_Worship.h"
#include "State_Follower_Indo.h"
#include "InteractObject.h"

CFollower::CFollower(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CLandObject(pGraphic_Device)
{
}

CFollower::CFollower(const CFollower& Prototype)
    :CLandObject(Prototype)
{
}

HRESULT CFollower::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFollower::Initialize(void* pArg)
{
    FOLLOWER_INFO* tInfo = static_cast< FOLLOWER_INFO*>(pArg);
    m_tInfo.e_type = tInfo->e_type;
    m_tInfo.iHunger = tInfo->iHunger;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Animation()))
        return E_FAIL;

    if (FAILED(Ready_Fsm()))
        return E_FAIL;

    __super::Initialize(&tInfo->tLandObjectDesc);

    //초기 세팅
    _float3 vPosition = _float3{ tInfo->fX,0.f,tInfo->fZ };
    _float3 vScale = _float3{ 4.f,4.f,1.f };

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
    m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);   

    return S_OK;
}

void CFollower::Priority_Update(_float fTimeDelta)
{
    if (m_iStageIndex != m_pGameInstance->Get_StageIndex()
        || m_iLevelIndex != m_pGameInstance->Get_LevelIndex())
        return;

    if(m_pCommandUI == nullptr)
    {
        CUI* pUI = dynamic_cast<CUI*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Follower_Command_UI"), 0));
        if (pUI != nullptr)
        {
            m_pCommandUI = dynamic_cast<CFollower_Command_UI*>(pUI);
            if (m_pCommandUI == nullptr)
                return;
            m_pCommandUI->SetActivate(false);
        }
    }
}

void CFollower::Update(_float fTimeDelta)
{
    if (m_iStageIndex != m_pGameInstance->Get_StageIndex()
        || m_iLevelIndex != m_pGameInstance->Get_LevelIndex())
    {
       
        return;
    }

    __super::SetUp_OnTerrain(m_pTransformCom, 0.5f);

    _float3 vPos = {};

    if(m_bisIndo || m_pFsmCom->Get_CurrentState() == IDLE)
    {
        CulculateIsCanWorship();

        if (m_bCanWorship && m_pVIBufferCom->IsPick(m_pTransformCom, &vPos))
        {
            if (m_pCommandUI->GetActivate() == false)
            {
                //m_pCommandUI->SetActivate(true);
                m_pCommandUI->Set_Follower(this);
                m_pFsmCom->Change_State(WORSHIP);
            }
        }
    }
    m_pFsmCom->Update(fTimeDelta);
}

void CFollower::Late_Update(_float fTimeDelta)
{
    if (m_iStageIndex != m_pGameInstance->Get_StageIndex()
        || m_iLevelIndex != m_pGameInstance->Get_LevelIndex())
        return;

    m_pAnimatorCom->Update(fTimeDelta);

    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CFollower::Render()
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

    //디버그 모드 추가하기
    m_pColliderCom->Render_Collider();

    return S_OK;
}

void CFollower::Set_InitState(FOLLOWER_STATE eState, void* pArg)
{
    m_pFsmCom->Set_State(eState, pArg);
}

void CFollower::Change_State(FOLLOWER_STATE eState, void* pArg)
{
    m_pFsmCom->Change_State(eState, pArg);
}

void CFollower::Change_Animation(const _wstring& strAnimationTag, _uint iStartFrame)
{
    m_pAnimatorCom->Change_Animation(strAnimationTag, iStartFrame);
}

bool CFollower::IsEnd_CurrentAnimation()
{
    return m_pAnimatorCom->IsEnd_CurrentAnimation();
}

_uint CFollower::Get_CurrentFrame()
{
    return m_pAnimatorCom->Get_CurrentFrame();
}

HRESULT CFollower::Ready_Components()
{
    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    /* For.Com_Transform */
    CTransform::TRANSFORM_DESC		TransformDesc{};

    TransformDesc.fSpeedPerSec = 3.0f;
    TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;

    /* For.Com_Collider_Cube */
    CCollider_Cube::COLLIDER_CUBE_INFO ColliderInfo;

    ZeroMemory(&ColliderInfo, sizeof(CCollider_Cube::COLLIDER_CUBE_INFO));
    ColliderInfo.fSizeX = 0.15f;
    ColliderInfo.fSizeY = 0.15f;
    ColliderInfo.fSizeZ = 0.5f;
    ColliderInfo.fOffSetX = 0.f;
    ColliderInfo.fOffSetY = 0.1f;
    ColliderInfo.fOffSetZ = -0.2f;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
        TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderInfo)))
        return E_FAIL;

    return S_OK;
}

HRESULT CFollower::Ready_Animation()
{
    CTexture* pTextureCom = {};

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
        TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
        return E_FAIL;

    switch (m_tInfo.e_type)
    {
    case BEE:
        Ready_Animation_Bee(pTextureCom);
        break;

    case DEER:
        Ready_Animation_Deer(pTextureCom);
        break;

    case RABBIT:
        Ready_Animation_Rabbit(pTextureCom);
        break;

    case ELIGOS:
        Ready_Animation_Eligos(pTextureCom);
        break;

    default:
        break;
    }
    
    m_pAnimatorCom->Change_Animation(TEXT("INDO_LOOP"), 0);
    return S_OK;
}

HRESULT CFollower::Ready_Fsm() 
{
    /* For.Com_Fsm */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
        TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
        return E_FAIL;

    m_pFsmCom->Add_State(CState_Follower_Idle::Create(m_pFsmCom, IDLE));
    m_pFsmCom->Add_State(CState_Follower_Walk::Create(m_pFsmCom,WALK));
    m_pFsmCom->Add_State(CState_Follower_Mining::Create(m_pFsmCom, MINING));
    m_pFsmCom->Add_State(CState_Follower_Axe::Create(m_pFsmCom, AXE));
    m_pFsmCom->Add_State(CState_Follower_Pray::Create(m_pFsmCom, PRAY));
    m_pFsmCom->Add_State(CState_Follower_Worship::Create(m_pFsmCom, WORSHIP));
    m_pFsmCom->Add_State(CState_Follower_Indo::Create(m_pFsmCom, INDO));
    

    m_pFsmCom->Set_State(INDO);

    return S_OK;
}

HRESULT CFollower::SetUp_RenderState()
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

HRESULT CFollower::Reset_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    return S_OK;
}

HRESULT CFollower::Ready_Animation_Bee(CTexture* pTextureCom)
{
    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Bee_Idle"),
        TEXT("Com_Texture_Bee_Idle"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("IDLE"), pTextureCom, 0, 33, 0.03f);

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Bee_Walk"),
        TEXT("Com_Texture_Bee_Walk"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("WALK"), pTextureCom, 0, 29, 0.02f);

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Bee_Axe"),
        TEXT("Com_Texture_Bee_Axe"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("AXE"), pTextureCom, 0, 56, 0.02f);

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Bee_Mining"),
        TEXT("Com_Texture_Bee_Mining"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("MINING"), pTextureCom, 0, 38, 0.02f);

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Bee_Pray"),
        TEXT("Com_Texture_Bee_Pray"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("PRAY"), pTextureCom, 0, 42, 0.02f);

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Bee_Worship"),
        TEXT("Com_Texture_Bee_Worship"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("WORSHIP"), pTextureCom, 0, 29, 0.02f);

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Bee_Indo_Loop"),
        TEXT("Com_Texture_Bee_Indo_Loop"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("INDO_LOOP"), pTextureCom, 0, 22, 0.02f);

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Bee_Indo_End"),
        TEXT("Com_Texture_Bee_Indo_End"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("INDO_END"), pTextureCom, 0,67, 0.04f, false);

    return S_OK;
}

HRESULT CFollower::Ready_Animation_Deer(CTexture* pTextureCom)
{
    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Deer_Idle"),
        TEXT("Com_Texture_Deer_Idle"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("IDLE"), pTextureCom, 0, 33, 0.03f);

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Deer_Walk"),
           TEXT("Com_Texture_Deer_Walk"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
           return E_FAIL;
       m_pAnimatorCom->Add_Animation(TEXT("WALK"), pTextureCom, 0, 29, 0.02f);

       if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Deer_Axe"),
           TEXT("Com_Texture_Deer_Axe"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
           return E_FAIL;
       m_pAnimatorCom->Add_Animation(TEXT("AXE"), pTextureCom, 0, 56, 0.02f);

       if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Deer_Mining"),
           TEXT("Com_Texture_Deer_Mining"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
           return E_FAIL;
       m_pAnimatorCom->Add_Animation(TEXT("MINING"), pTextureCom, 0, 38, 0.02f);

       if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Deer_Pray"),
           TEXT("Com_Texture_Deer_Pray"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
           return E_FAIL;
       m_pAnimatorCom->Add_Animation(TEXT("PRAY"), pTextureCom, 0, 42, 0.02f);

       if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Deer_Worship"),
           TEXT("Com_Texture_Deer_Worship"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
           return E_FAIL;
       m_pAnimatorCom->Add_Animation(TEXT("WORSHIP"), pTextureCom, 0, 29, 0.02f);

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Deer_Indo_Loop"),
        TEXT("Com_Texture_Deer_Indo_Loop"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("INDO_LOOP"), pTextureCom, 0, 22, 0.02f);

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Deer_Indo_End"),
        TEXT("Com_Texture_Deer_Indo_End"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("INDO_END"), pTextureCom, 0, 67, 0.04f,false);

    return S_OK;
}

HRESULT CFollower::Ready_Animation_Rabbit(CTexture* pTextureCom)
{
    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Rabbit_Idle"),
        TEXT("Com_Texture_Rabbit_Idle"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("IDLE"), pTextureCom, 0, 33, 0.03f);

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Rabbit_Walk"),
          TEXT("Com_Texture_Rabbit_Walk"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
          return E_FAIL;
      m_pAnimatorCom->Add_Animation(TEXT("WALK"), pTextureCom, 0, 29, 0.02f);

      if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Rabbit_Axe"),
          TEXT("Com_Texture_Rabbit_Axe"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
          return E_FAIL;
      m_pAnimatorCom->Add_Animation(TEXT("AXE"), pTextureCom, 0, 56, 0.02f);

      if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Rabbit_Mining"),
          TEXT("Com_Texture_Rabbit_Mining"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
          return E_FAIL;
      m_pAnimatorCom->Add_Animation(TEXT("MINING"), pTextureCom, 0, 38, 0.02f);

      if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Rabbit_Pray"),
          TEXT("Com_Texture_Rabbit_Pray"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
          return E_FAIL;
      m_pAnimatorCom->Add_Animation(TEXT("PRAY"), pTextureCom, 0, 42, 0.02f);

      if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Rabbit_Worship"),
          TEXT("Com_Texture_Rabbit_Worship"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
          return E_FAIL;
      m_pAnimatorCom->Add_Animation(TEXT("WORSHIP"), pTextureCom, 0, 29, 0.04f);

      if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Rabbit_Indo_Loop"),
          TEXT("Com_Texture_Rabbit_Indo_Loop"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
          return E_FAIL;
      m_pAnimatorCom->Add_Animation(TEXT("INDO_LOOP"), pTextureCom, 0, 22, 0.02f);

      if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Rabbit_Indo_End"),
          TEXT("Com_Texture_Rabbit_Indo_End"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
          return E_FAIL;
      m_pAnimatorCom->Add_Animation(TEXT("INDO_END"), pTextureCom, 0,67, 0.04f, false);

    return S_OK;
}

HRESULT CFollower::Ready_Animation_Eligos(CTexture* pTextureCom)
{
    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Eligos_Idle"),
        TEXT("Com_Texture_Eligos_Idle"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("IDLE"), pTextureCom, 0, 33, 0.03f);

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Eligos_Walk"),
        TEXT("Com_Texture_EligosWalk"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("WALK"), pTextureCom, 0, 29, 0.02f);

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Eligos_Axe"),
        TEXT("Com_Texture_Eligos_Axe"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("AXE"), pTextureCom, 0, 56, 0.02f);

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Eligod_Mining"),
        TEXT("Com_Texture_Eligos_Mining"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("MINING"), pTextureCom, 0, 33, 0.02f);

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Eligos_Pray"),
        TEXT("Com_Texture_Eligos_Pray"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("PRAY"), pTextureCom, 0, 42, 0.02f);

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Eligos_Worship"),
        TEXT("Com_Texture_Eligos_Worship"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("WORSHIP"), pTextureCom, 0, 29, 0.04f);

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Eligos_Indo_Loop"),
        TEXT("Com_Texture_Eligos_Indo_Loop"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("INDO_LOOP"), pTextureCom, 0,32, 0.03f);

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Eligos_Indo_End"),
        TEXT("Com_Texture_Eligos_Indo_End"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("INDO_END"), pTextureCom, 0, 67, 0.04f,false);

    return S_OK;
}

void CFollower::OnCollisionEnter(CGameObject* pOther)
{
    if (pOther->Get_LayerTag() == TEXT("Layer_BackGround_Object"))
    {
        if (dynamic_cast<CInteractObject*>(pOther) == nullptr)
            return;

        if(m_bIsAxe && dynamic_cast<CInteractObject*>(pOther)->Get_Interact_Type() == CInteractObject::TREE)
        {
            m_pInteract_Object = dynamic_cast<CInteractObject*>(pOther);
            m_bCollision_Interact_Object = true;
        }

        if (m_bIsMining && dynamic_cast<CInteractObject*>(pOther)->Get_Interact_Type() == CInteractObject::ROCK)
        {
            m_pInteract_Object = dynamic_cast<CInteractObject*>(pOther);
            m_bCollision_Interact_Object = true;
        }
    }
}

void CFollower::OnCollisionStay(CGameObject* pOther)
{
}

void CFollower::OnCollisionExit(CGameObject* pOther)
{
    if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
        m_bCanWorship = false;

    if (pOther->Get_LayerTag() == TEXT("Layer_BackGround_Object"))
    {
        if (dynamic_cast<CInteractObject*>(pOther) == nullptr)
            return;

        m_bCollision_Interact_Object = false;
    }
}
void CFollower::CulculateIsCanWorship()
{
    _float3 vDistance = m_pGameInstance->Find_Player()->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _float fDistance = D3DXVec3Length(&vDistance);
    m_fDistance_with_Player = fDistance;
    if (fDistance < 3.f)
        m_bCanWorship = true;
    else
        m_bCanWorship = false;
}



CFollower* CFollower::Create(LPDIRECT3DDEVICE9 pGraphic_Device )
{
    CFollower* pInstance = new CFollower(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CFollower"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFollower::Clone(void* pArg)
{
    CFollower* pInstance = new CFollower(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CFollower"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFollower::Free()
{
    __super::Free();

    if (nullptr != m_pFsmCom)
        m_pFsmCom->Release_States();
    Safe_Release(m_pFsmCom);
    Safe_Release(m_pAnimatorCom);
    Safe_Release(m_pVIBufferCom);
}
