#include "RatNPC.h"
#include "Player.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Transform.h"
#include "Animator.h"
#include "Camera.h"

#include "Quest.h"

CRatNPC::CRatNPC(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CLandObject(pGraphic_Device)
{
}

CRatNPC::CRatNPC(const CRatNPC& Prototype)
    :CLandObject( Prototype)
{
}

HRESULT CRatNPC::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRatNPC::Initialize(void* pArg)
{
    Rat_INFO* tInfo = static_cast<Rat_INFO*>(pArg);
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

void CRatNPC::Priority_Update(_float fTimeDelta)
{
   if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
        return;
   if ((m_pAnimatorCom->Get_AnimationTag()).compare(TEXT("IDLE")) == 0)
   {
       if (Culculate_Distance_with_Player() < 8.f && m_iFirstEnter == 1)
       {  
           m_bActive = true;
           m_iFirstEnter++;
           m_bDirection = true;
           dynamic_cast<CCamera*> (m_pGameInstance->Find_Camera())->Zoom_In(1.5f);
           dynamic_cast<CCamera*> (m_pGameInstance->Find_Camera())->Target_to(this);
           m_pAnimatorCom->Change_Animation(TEXT("ENTER"));
           m_pGameInstance->Play_Sound(TEXT("Ratau_Dig.wav"), SOUND_NPC, g_fEffectVolume);
       }
   }
}

void CRatNPC::Update(_float fTimeDelta)
{
    if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
        return;

    __super::SetUp_OnTerrain(m_pTransformCom, 0.5f);


    if (m_bStartSound && m_pGameInstance->Check_IsPlaying(SOUND_NPC) == false)
        Sound_Play();

    //등장 후 기본 상태
    if ((m_pAnimatorCom->Get_AnimationTag()).compare(TEXT("ENTER")) == 0)
    {
        if (m_pAnimatorCom->IsEnd_CurrentAnimation() && m_bDirection == true)
        {
            m_pAnimatorCom->Change_Animation(TEXT("IDLE"));
            dynamic_cast<CCamera*> (m_pGameInstance->Find_Camera())->Zoom_Out(1.5f);
            dynamic_cast<CCamera*> (m_pGameInstance->Find_Camera())->Target_to_Player();
            m_bDirection = false;
        }
    }

    CTalkPanelUI* pTalkUI{ nullptr };
    if (nullptr != m_pGameInstance->Find_Object(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_TalkUI"), 0))
    {
        pTalkUI = dynamic_cast<CTalkPanelUI*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_TalkUI"), 0));
    }

    //일정 거리 이상 가까워지면 대화
    if ((m_pAnimatorCom->Get_AnimationTag()).compare(TEXT("IDLE")) == 0)
    {
        if (Culculate_Distance_with_Player() < 4.f)
        {
            CCamera* pCamera = dynamic_cast<CCamera*> (m_pGameInstance->Find_Camera());

            m_bStartSound = true;
            m_bFinishTalk = false;
            if (m_bDirection == false)
            {

                m_pAnimatorCom->Change_Animation(TEXT("TALK"));
                pCamera->Zoom_In(2.f);
                pCamera->Target_to(this, _float3(0.f, 0.f, 0.f), 5.f);
                m_bDirection = true;

                CTalkPanelUI::TALK_DESC Desc = {};
                Desc.UIDesc.m_vPos = { g_iWinSizeX / 2.f , g_iWinSizeY / 2.f - 200.f };
                Desc.UIDesc.m_vSize = { 600.f,300.f };
                Desc.UIDesc.m_iBindNum = 0;
                Desc.UIDesc.bActivate = true;
                Desc.strLayerOpponent = L"Layer NPC";
                Desc.strNameOpponent = L"라타우";

                if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_TalkUI"), TEXT("Prototype_GameObject_TalkUI"), &Desc)))
                    MSG_BOX(L"Can't Create TalkUI");

                pTalkUI = dynamic_cast<CTalkPanelUI*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_TalkUI"), 0));

                vector<_wstring>& vecSpeech = pTalkUI->Get_vecSpeech((LEVELID)m_pGameInstance->Get_LevelIndex());

                vecSpeech.push_back(L"드디어...\n드디어 새로운 신성을 가진 자가\n나타났구려!!");
                vecSpeech.push_back(L"아직은 어리지만 신성을 가진 양이여\n그대의 이름이 무엇인가?");
                vecSpeech.push_back(L"램.....\n램이라는 이름이구려");
                vecSpeech.push_back(L"악신을 물리칠 수 있는 것은\n신성을 가지고 있는 자 뿐이오...");
                vecSpeech.push_back(L"부디 힘을 길러 악신을 무찌르고 새로운\n신의 자리에 올라주시길 바라오!!!");
                vecSpeech.push_back(L"이건 내 염원을 담은 선물이오!!!");
                vecSpeech.push_back(L"일단 이 거지같은 곳을 빠져나가시구려!");
            }
        }
        return;
    }

    /*  if (dynamic_cast<CTalkPanelUI*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_TalkUI"), 0))->Get_TalkEnd() == true
          && (m_pAnimatorCom->Get_AnimationTag()).compare(TEXT("TALK")) == 0)
          m_bFinishTalk = true;*/

          //대화 끝
    if (pTalkUI != nullptr && pTalkUI->Get_TalkEnd() && m_bDirection == true)
    {
        m_bStartSound = false;
        m_bDirection = false;
        m_pAnimatorCom->Change_Animation(TEXT("EXIT"));
        m_pGameInstance->Play_Sound(TEXT("Ratau_Dig.wav"), SOUND_NPC, g_fEffectVolume);

        dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Change_State(CPlayer::GET_SWORD);

        
        CQuest::Get_Instance()->Succeed_Quest(L"라타우와 대화하기");      
    }
}



void CRatNPC::Late_Update(_float fTimeDelta)
{
    if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
        return;

    m_pAnimatorCom->Update(fTimeDelta);
    if (m_bActive)
         m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CRatNPC::Render()
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

HRESULT CRatNPC::Ready_Components()
{
    /* For.Com_VIBuffer */
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

HRESULT CRatNPC::Ready_Animation()
{
    CTexture* pTextureCom = {};

    if (FAILED(__super::Add_Component(Client::LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
        TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_NPC_RatNPC_Enter"),
        TEXT("Com_Texture_RatNPC_Enter"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("ENTER"), pTextureCom, 0, 24, 0.09f, false);

    if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_NPC_RatNPC_Exit"),
        TEXT("Com_Texture_RatNPC_Exit"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("EXIT"), pTextureCom, 0, 39, 0.05f, false);

    if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_NPC_RatNPC_IDLE"),
        TEXT("Com_Texture_RatNPC_IDLE"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("IDLE"), pTextureCom, 0, 45, 0.05f, true);

    if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Texture_NPC_RatNPC_Talk"),
        TEXT("Com_Texture_RatNPC_TALK"), reinterpret_cast<CComponent**>(&pTextureCom), nullptr)))
        return E_FAIL;
    m_pAnimatorCom->Add_Animation(TEXT("TALK"), pTextureCom, 0, 45, 0.05f, true);

    m_pAnimatorCom->Change_Animation(TEXT("IDLE"), 0);
    return S_OK;
}

HRESULT CRatNPC::SetUp_RenderState()
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

HRESULT CRatNPC::Reset_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    return S_OK;
}

_float CRatNPC::Culculate_Distance_with_Player()
{
    _float3 vDistance = m_pGameInstance->Find_Player()->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _float fDistance = D3DXVec3Length(&vDistance);
    return fDistance;
}

void CRatNPC::Sound_Play()
{
    switch (m_iSoundNum)
    {
    case 0:
        m_pGameInstance->Play_Sound(TEXT("Ratau_Talk1.wav"), SOUND_NPC, 0.8f);
        m_iSoundNum++;
        break;
    case 1:
        m_pGameInstance->Play_Sound(TEXT("Ratau_Talk2.wav"), SOUND_NPC, 0.8f);
        m_iSoundNum++;
        break;
    case 2:
        m_pGameInstance->Play_Sound(TEXT("Ratau_Talk3.wav"), SOUND_NPC, 0.8f);
        m_iSoundNum = 0;
        break;
    default:
        break;
    } 
}

CRatNPC* CRatNPC::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CRatNPC* pInstance = new CRatNPC(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CRatNPCvvvv"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRatNPC::Clone(void* pArg)
{

    CRatNPC* pInstance = new CRatNPC(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CRatNPC"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRatNPC::Free()
{
    __super::Free();

    Safe_Release(m_pAnimatorCom);
    Safe_Release(m_pVIBufferCom);
}
