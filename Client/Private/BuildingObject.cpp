#include "BuildingObject.h"

#include "GameInstance.h"
#include "Player.h"
#include "Grid.h"

#include "Camera.h"
#include "Layer.h"

#include "ItemGetUI.h"

#include "MonologueUI.h"

#include "Quest.h"

CBuildingObject::CBuildingObject(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CInteractObject{ pGraphic_Device }
{
}

CBuildingObject::CBuildingObject(const CBuildingObject& Prototype)
    : CInteractObject{ Prototype }
    , m_bLand{ Prototype.m_bLand }
{
}

HRESULT CBuildingObject::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CBuildingObject::Initialize(void* pArg)
{
    if (FAILED(CLandObject::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;


    LANDOBJECT_DESC* pDesc = static_cast<LANDOBJECT_DESC*>(pArg);

    _float3 vSacling = pDesc->vInitScale;
    m_iBindTexNum = pDesc->iInitBindTexNum;




    m_pTransformCom->Scaling(vSacling.x, vSacling.y, vSacling.z);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vInitPos);

    m_iLevelIndex = pDesc->iLevelIndex;
    m_iStageIndex = pDesc->iStageIndex;


    if (FAILED(CreateLandGrid()))
        return E_FAIL;

    if (FAILED(CreateLandUI(this)))
        return E_FAIL;


    return S_OK;
}

void CBuildingObject::Priority_Update(_float fTimeDelta)
{
    _uint iCurLevelIndex = m_pGameInstance->Get_LevelIndex();

    if (LEVELID::LEVEL_GAMEPLAY != iCurLevelIndex)
        return;
    else
    {
        if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
            return;
    }

}

void CBuildingObject::Update(_float fTimeDelta)
{
    _uint iCurLevelIndex = m_pGameInstance->Get_LevelIndex();

    if (LEVELID::LEVEL_GAMEPLAY != iCurLevelIndex)
        return;
    else
    {
        if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
            return;
    }

    if (!m_bLand || m_bInitSetPos)
        __super::SetUp_OnTerrain(m_pTransformCom, 0.5f);

    if (FAILED(__super::Compute_ViewZ(TEXT("Com_Transform"))))
        return;


    if (!m_bLand)  // 아직 건설못했다면
    {
        static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Target_to(this);

        if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::TAP)
            m_pTransformCom->Go_Straight_BuildingObj();
        if (m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::TAP)
            m_pTransformCom->Go_Backward_BuildingObj();
        if (m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::TAP)
            m_pTransformCom->Go_Left_BuildingObj();
        if (m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::TAP)
            m_pTransformCom->Go_Right_BuildingObj();


        if (m_pGameInstance->Get_KeyState(KEY::SPACE) == KEY_STATE::TAP)
        { 
            if (FAILED(CreateBuildObject()))
                m_pGameInstance->Event_DestoryObject(this);
            else
            {
                _float3 vCurPos = Get_Position();
                Set_Position(_float3{ vCurPos.x, vCurPos.y + 20.f, vCurPos.z });
                m_fTargetY = vCurPos.y;


                m_bLand = true;

                CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player());
                CCamera* pCamera = static_cast<CCamera*>(m_pGameInstance->Find_Camera());

                pCamera->Set_NULL_PlayerTransform();
                pCamera->Target_to_Player();

                pPlayer->Set_Player_State(CPlayer::PLAYERSTATE::IDLE);

                m_pGameInstance->Play_Sound(TEXT("UI_Confirm_Selection.wav"), SOUND_UI, g_fEffectVolume);
                
                CGameObject* pGameObj = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_GuideUI"), 0);
                if (nullptr != pGameObj && !pGameObj->Get_Dead())
                {
                    if (FAILED(m_pGameInstance->Event_DestoryObject(pGameObj)))
                        MSG_BOX(L"Can't Delete GuideUI");
                }
            }

            CQuest::Get_Instance()->Succeed_Quest(L"건축시스템 이해하기");

        }
    }
    else if (m_bLand)               // 건설햇어
    {
        if (false == m_bInitSetPos) // 초기 위치 안잡으면
        {
            _float3 vCurPos = Get_Position();


            m_fTime += fTimeDelta;

            if (m_fTargetY < vCurPos.y)
            {
                vCurPos.y -= 2.f * m_fTime;

            }
            else
            {
                vCurPos.y = m_fTargetY;
                m_bInitSetPos = true;
                m_fTime = 0.f;
            }

            Set_Position(_float3{ vCurPos.x, vCurPos.y, vCurPos.z });

            if (m_bInitSetPos)
                __super::SetUp_OnTerrain(m_pTransformCom, 0.5f);
        }

    }





}

void CBuildingObject::Late_Update(_float fTimeDelta)
{
    _uint iCurLevelIndex = m_pGameInstance->Get_LevelIndex();

    if (LEVELID::LEVEL_GAMEPLAY != iCurLevelIndex)
        return;
    else
    {
        if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
            return;
    }

    m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CBuildingObject::Render()
{
    _uint iCurLevelIndex = m_pGameInstance->Get_LevelIndex();

    if (LEVELID::LEVEL_GAMEPLAY != iCurLevelIndex)
        return S_OK;
    else
    {
        if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
            return S_OK;
    }

    m_pVIBufferCom->Filp_Texture(false);

    if (FAILED(m_pTransformCom->Bind_Transform()))
        return E_FAIL;


    if (FAILED(m_pTextureCom->Bind_Texture(m_iBindTexNum)))         // << 여기서 
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;


    SetUp_RenderState();

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    Reset_RenderState();

    return S_OK;
}

HRESULT CBuildingObject::CreateLandGrid()
{
    _float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _float3 vSacling = m_pTransformCom->Get_Scaled();

    CGrid::GRID_DESC GridDesc = {};

    GridDesc.pOwner = this;
    GridDesc.iBindNum = 1;

    GridDesc.vPos = vPos;           // << 초기위치 중요해 !!!!!!!!!!!!!!!!!!!!!!!!!!!
    GridDesc.vPos.y = 0.6f;         // << 상관없다 어짜피 부모 pos 따라가니까

    GridDesc.vScale.x = 1.9f;
    GridDesc.vScale.y = 1.f;
    GridDesc.vScale.z = 1.9f;


    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_BackGround_Grid"), TEXT("Prototype_GameObject_Grid"), &GridDesc)))
        return E_FAIL;

    GridDesc.vPos.x -= (0.43f * GridDesc.vScale.x) + 0.3f;
    GridDesc.vPos.z += (0.43f * GridDesc.vScale.z) + 0.3f;
    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_BackGround_Grid"), TEXT("Prototype_GameObject_Grid"), &GridDesc)))
        return E_FAIL;



    GridDesc.vPos.x += (0.43f * GridDesc.vScale.x) + 0.3f;
    GridDesc.vPos.z += (0.43f * GridDesc.vScale.z) + 0.3f;
    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_BackGround_Grid"), TEXT("Prototype_GameObject_Grid"), &GridDesc)))
        return E_FAIL;



    GridDesc.vPos.x += (0.43f * GridDesc.vScale.x) + 0.3f;
    GridDesc.vPos.z -= (0.43f * GridDesc.vScale.z) + 0.3f;
    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_BackGround_Grid"), TEXT("Prototype_GameObject_Grid"), &GridDesc)))
        return E_FAIL;


    GridDesc.vPos.x += (0.43f * GridDesc.vScale.x) + 0.3f;
    GridDesc.vPos.z -= (0.43f * GridDesc.vScale.z) + 0.3f;
    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_BackGround_Grid"), TEXT("Prototype_GameObject_Grid"), &GridDesc)))
        return E_FAIL;


    GridDesc.vPos.x -= (0.43f * GridDesc.vScale.x) + 0.3f;
    GridDesc.vPos.z -= (0.43f * GridDesc.vScale.z) + 0.3f;
    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_BackGround_Grid"), TEXT("Prototype_GameObject_Grid"), &GridDesc)))
        return E_FAIL;


    GridDesc.vPos.x -= (0.43f * GridDesc.vScale.x) + 0.3f;
    GridDesc.vPos.z -= (0.43f * GridDesc.vScale.z) + 0.3f;
    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_BackGround_Grid"), TEXT("Prototype_GameObject_Grid"), &GridDesc)))
        return E_FAIL;


    GridDesc.vPos.x -= (0.43f * GridDesc.vScale.x) + 0.3f;
    GridDesc.vPos.z += (0.43f * GridDesc.vScale.z) + 0.3f;
    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_BackGround_Grid"), TEXT("Prototype_GameObject_Grid"), &GridDesc)))
        return E_FAIL;


    GridDesc.vPos.x -= (0.43f * GridDesc.vScale.x) + 0.3f;
    GridDesc.vPos.z += (0.43f * GridDesc.vScale.z) + 0.3f;
    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_BackGround_Grid"), TEXT("Prototype_GameObject_Grid"), &GridDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBuildingObject::CreateLandUI(class CBuildingObject* pOwner)
{

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_BuildingPanelUI"), pOwner)))
        MSG_BOX(L"BuildingObj_UI Error");


    return S_OK;
}

HRESULT CBuildingObject::CreateBuildObject()
{
    CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player());

    CMonologueUI::MONOLOGUE_DESC MonologueDesc = {};
    MonologueDesc.strText = L"배치에 필요한 아이템이 부족합니다..";
    MonologueDesc.rt = {
               (LONG)((g_iWinSizeX / 2.f) - 350),
               (LONG)((g_iWinSizeY / 2.f)),
               (LONG)((g_iWinSizeX / 2.f) + 400),
               (LONG)((g_iWinSizeY / 2.f) + 100),
    };


    switch ((BUILDING_TYPE)m_iBindTexNum)
    {
    case BUILDING_TYPE::SHRINE:
    {
        m_iNeedItem0_Count = 3;
        m_iNeedItem1_Count = 3;

        if (m_iNeedItem0_Count > pPlayer->Get_ItemCount(CItem::ITEM_ID::COIN) ||
            m_iNeedItem1_Count > pPlayer->Get_ItemCount(CItem::ITEM_ID::WOOD))  // 돈없어?
        {

            if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonologueUI"), &MonologueDesc)))
                MSG_BOX(TEXT("Can't Create MonologueUI"));

            m_bLand = true;
            CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player());
            CCamera* pCamera = static_cast<CCamera*>(m_pGameInstance->Find_Camera());

            pCamera->Set_NULL_PlayerTransform();
            pCamera->Target_to_Player();

            pPlayer->Set_Player_State(CPlayer::PLAYERSTATE::IDLE);

            list<CGameObject*>& GameObjects = m_pGameInstance->Find_Layer(LEVEL_STATIC, L"Layer_GuideUI")->Get_ObjectList();

            CGameObject* pGameObj = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_GuideUI"), 0);
            if (nullptr != pGameObj && !pGameObj->Get_Dead())
            {
                if (FAILED(m_pGameInstance->Event_DestoryObject(pGameObj)))
                    MSG_BOX(L"Can't Delete GuideUI");
            }
            return E_FAIL;
        }
        else
        {
            _uint iCurCount0 = pPlayer->Get_ItemCount(CItem::ITEM_ID::COIN);
            _uint iCurCount1 = pPlayer->Get_ItemCount(CItem::ITEM_ID::WOOD);

            pPlayer->Discount_ItemCount(CItem::ITEM_ID::COIN, m_iNeedItem0_Count);
            pPlayer->Discount_ItemCount(CItem::ITEM_ID::WOOD, m_iNeedItem1_Count);


            CItemGetUI::ITEM_GETUI_DESC Desc = {};

            Desc.eUIDesc.m_vPos = { -100.f , 190.f };
            Desc.eUIDesc.m_vSize = { 200.f,40.f };
            Desc.eUIDesc.m_iBindNum = 1;
            Desc.eUIDesc.eUItype = UI_TYPE::UI_NONE;
            Desc.eItemId = CItem::ITEM_ID::COIN;

            Desc.iLevelIndex = m_iLevelIndex;
            Desc.iNeedItem_Count = m_iNeedItem0_Count;

            if (FAILED(m_pGameInstance->Event_CreateObject(LEVEL_STATIC, TEXT("Layer_ItemGetUI"), TEXT("Prototype_GameObject_ItemGetUI"), sizeof(CItemGetUI::ITEM_GETUI_DESC), &Desc)))
                MSG_BOX(L"Can't Create ItemGetUI");

            Desc.eUIDesc.m_vPos = { -100.f , 190.f + 50.f };
            Desc.eUIDesc.m_vSize = { 200.f,40.f };
            Desc.eUIDesc.m_iBindNum = 1;
            Desc.eUIDesc.eUItype = UI_TYPE::UI_NONE;
            Desc.eItemId = CItem::ITEM_ID::WOOD;

            Desc.iLevelIndex = m_iLevelIndex;
            Desc.iNeedItem_Count = m_iNeedItem1_Count;

            if (FAILED(m_pGameInstance->Event_CreateObject(LEVEL_STATIC, TEXT("Layer_ItemGetUI"), TEXT("Prototype_GameObject_ItemGetUI"), sizeof(CItemGetUI::ITEM_GETUI_DESC), &Desc)))
                MSG_BOX(L"Can't Create ItemGetUI");

        }

    }
    break;

    case BUILDING_TYPE::SANCTUARY:
    {
        m_iNeedItem0_Count = 7;
        m_iNeedItem1_Count = 10;


        if (m_iNeedItem0_Count > pPlayer->Get_ItemCount(CItem::ITEM_ID::COIN) ||
            m_iNeedItem1_Count > pPlayer->Get_ItemCount(CItem::ITEM_ID::ROCK))  // 돈없어?
        {

            if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonologueUI"), &MonologueDesc)))
                MSG_BOX(TEXT("Can't Create MonologueUI"));

            m_bLand = true;
            CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player());
            CCamera* pCamera = static_cast<CCamera*>(m_pGameInstance->Find_Camera());

            pCamera->Set_NULL_PlayerTransform();
            pCamera->Target_to_Player();

            pPlayer->Set_Player_State(CPlayer::PLAYERSTATE::IDLE);

            CGameObject* pGameObj = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_GuideUI"), 0);
            if (nullptr != pGameObj && !pGameObj->Get_Dead())
            {
                if (FAILED(m_pGameInstance->Event_DestoryObject(pGameObj)))
                    MSG_BOX(L"Can't Delete GuideUI");
            }
            return E_FAIL;
        }
        else
        {
            _uint iCurCount0 = pPlayer->Get_ItemCount(CItem::ITEM_ID::COIN);
            _uint iCurCount1 = pPlayer->Get_ItemCount(CItem::ITEM_ID::ROCK);

            pPlayer->Discount_ItemCount(CItem::ITEM_ID::COIN, m_iNeedItem0_Count);
            pPlayer->Discount_ItemCount(CItem::ITEM_ID::ROCK, m_iNeedItem1_Count);



            CItemGetUI::ITEM_GETUI_DESC Desc = {};

            Desc.eUIDesc.m_vPos = { -100.f , 190.f };
            Desc.eUIDesc.m_vSize = { 200.f,40.f };
            Desc.eUIDesc.m_iBindNum = 1;
            Desc.eUIDesc.eUItype = UI_TYPE::UI_NONE;
            Desc.eItemId = CItem::ITEM_ID::COIN;

            Desc.iLevelIndex = m_iLevelIndex;
            Desc.iNeedItem_Count = m_iNeedItem0_Count;

            if (FAILED(m_pGameInstance->Event_CreateObject(LEVEL_STATIC, TEXT("Layer_ItemGetUI"), TEXT("Prototype_GameObject_ItemGetUI"), sizeof(CItemGetUI::ITEM_GETUI_DESC), &Desc)))
                MSG_BOX(L"Can't Create ItemGetUI");

            Desc.eUIDesc.m_vPos = { -100.f , 190.f + 50.f };
            Desc.eUIDesc.m_vSize = { 200.f,40.f };
            Desc.eUIDesc.m_iBindNum = 1;
            Desc.eUIDesc.eUItype = UI_TYPE::UI_NONE;
            Desc.eItemId = CItem::ITEM_ID::ROCK;

            Desc.iLevelIndex = m_iLevelIndex;
            Desc.iNeedItem_Count = m_iNeedItem1_Count;

            if (FAILED(m_pGameInstance->Event_CreateObject(LEVEL_STATIC, TEXT("Layer_ItemGetUI"), TEXT("Prototype_GameObject_ItemGetUI"), sizeof(CItemGetUI::ITEM_GETUI_DESC), &Desc)))
                MSG_BOX(L"Can't Create ItemGetUI");


            CQuest::Get_Instance()->Succeed_Quest(L"건축: 교회 짓기");
        }

    }
    break;

    case BUILDING_TYPE::UPGRADE_SHRINE:
    {
        m_iNeedItem0_Count = 6;
        m_iNeedItem1_Count = 15;

        if (m_iNeedItem0_Count > pPlayer->Get_ItemCount(CItem::ITEM_ID::ROCK) ||
            m_iNeedItem1_Count > pPlayer->Get_ItemCount(CItem::ITEM_ID::WOOD))  // 돈없어?
        {

            if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonologueUI"), &MonologueDesc)))
                MSG_BOX(TEXT("Can't Create MonologueUI"));


            m_bLand = true;
            CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player());
            CCamera* pCamera = static_cast<CCamera*>(m_pGameInstance->Find_Camera());

            pCamera->Set_NULL_PlayerTransform();
            pCamera->Target_to_Player();

            pPlayer->Set_Player_State(CPlayer::PLAYERSTATE::IDLE);

            CGameObject* pGameObj = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_GuideUI"), 0);
            if (nullptr != pGameObj && !pGameObj->Get_Dead())
            {
                if (FAILED(m_pGameInstance->Event_DestoryObject(pGameObj)))
                    MSG_BOX(L"Can't Delete GuideUI");
            }
            return E_FAIL;
        }
        else
        {
            _int iCurCount0 = pPlayer->Get_ItemCount(CItem::ITEM_ID::ROCK);
            _int iCurCount1 = pPlayer->Get_ItemCount(CItem::ITEM_ID::WOOD);


            pPlayer->Discount_ItemCount(CItem::ITEM_ID::ROCK, m_iNeedItem0_Count);
            pPlayer->Discount_ItemCount(CItem::ITEM_ID::WOOD, m_iNeedItem1_Count);


            CItemGetUI::ITEM_GETUI_DESC Desc = {};

            Desc.eUIDesc.m_vPos = { -100.f , 190.f };
            Desc.eUIDesc.m_vSize = { 200.f,40.f };
            Desc.eUIDesc.m_iBindNum = 1;
            Desc.eUIDesc.eUItype = UI_TYPE::UI_NONE;
            Desc.eItemId = CItem::ITEM_ID::ROCK;

            Desc.iLevelIndex = m_iLevelIndex;
            Desc.iNeedItem_Count = m_iNeedItem0_Count;

            if (FAILED(m_pGameInstance->Event_CreateObject(LEVEL_STATIC, TEXT("Layer_ItemGetUI"), TEXT("Prototype_GameObject_ItemGetUI"), sizeof(CItemGetUI::ITEM_GETUI_DESC), &Desc)))
                MSG_BOX(L"Can't Create ItemGetUI");

            Desc.eUIDesc.m_vPos = { -100.f , 190.f + 50.f };
            Desc.eUIDesc.m_vSize = { 200.f,40.f };
            Desc.eUIDesc.m_iBindNum = 1;
            Desc.eUIDesc.eUItype = UI_TYPE::UI_NONE;
            Desc.eItemId = CItem::ITEM_ID::WOOD;

            Desc.iLevelIndex = m_iLevelIndex;
            Desc.iNeedItem_Count = m_iNeedItem1_Count;

            if (FAILED(m_pGameInstance->Event_CreateObject(LEVEL_STATIC, TEXT("Layer_ItemGetUI"), TEXT("Prototype_GameObject_ItemGetUI"), sizeof(CItemGetUI::ITEM_GETUI_DESC), &Desc)))
                MSG_BOX(L"Can't Create ItemGetUI");


            CQuest::Get_Instance()->Succeed_Quest(L"건축: UpGrade석상짓기");
        }


    }
    break;
    default:
        break;
    }

    return S_OK;
}



HRESULT CBuildingObject::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Village_Building"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;


    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;



    /* For.Com_Transform */
    CTransform::TRANSFORM_DESC      TransformDesc{};

    TransformDesc.fSpeedPerSec = 20.0f;
    TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBuildingObject::SetUp_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

    return S_OK;
}

HRESULT CBuildingObject::Reset_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    return S_OK;
}

CBuildingObject* CBuildingObject::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CBuildingObject* pInstance = new CBuildingObject(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBuildingObject"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBuildingObject::Clone(void* pArg)
{
    CBuildingObject* pInstance = new CBuildingObject(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CBuildingObject"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBuildingObject::Free()
{
    __super::Free();


}
