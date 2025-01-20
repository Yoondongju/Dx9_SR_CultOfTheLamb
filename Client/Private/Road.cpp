#include "Road.h"
#include "GameInstance.h"
#include "Layer.h"

#include "Level_Loading.h"

CRoad::CRoad(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CGameObject(pGraphic_Device)
{
}

CRoad::CRoad(const CRoad& Prototype)
    :CGameObject(Prototype)
    , m_iBindTexNum(Prototype.m_iBindTexNum)
    , m_strViBufferTag{ Prototype.m_strViBufferTag }
{
}

HRESULT CRoad::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRoad::Initialize(void* pArg)
{
    ROAD_DESC* RoadDesc = static_cast<ROAD_DESC*>(pArg);

    m_iLevelIndex = RoadDesc->iLevelIndex;
    m_iStageIndex = RoadDesc->iStageIndex;
    m_iChangeStageIndex = RoadDesc->iWhereWeGo;
    m_eRoadDir = RoadDesc->eRoadDir;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_Transform()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, RoadDesc->vPos);

    if (m_eRoadDir == UP || m_eRoadDir == DOWN)
    {
        _float3 vAxis = { 0.f, 1.f, 0.f };
        m_pTransformCom->Rotation(vAxis, D3DXToRadian(90.f));
    }

    _float fLeft = m_pVIBufferCom->Get_Border_LeftPos();
    _float fRight = m_pVIBufferCom->Get_Border_RightPos();
    _float fUp = m_pVIBufferCom->Get_Border_UpPos();
    _float fDown = m_pVIBufferCom->Get_Border_DownPos();

    m_fBorderLeft = _float3{ fLeft, 0.f, 0.f };
    m_fBorderRight = _float3{ fRight, 0.f, 0.f };
    m_fBorderUp = _float3{ 0.f, 0.f, fUp };
    m_fBorderDown = _float3{ 0.f, 0.f, fDown };

    _float4x4 WorldMat = {};
    WorldMat = m_pTransformCom->Get_WorldMatrix();

    D3DXVec3TransformCoord(&m_fBorderLeft, &m_fBorderLeft, &WorldMat);
    D3DXVec3TransformCoord(&m_fBorderRight, &m_fBorderRight, &WorldMat);
    D3DXVec3TransformCoord(&m_fBorderUp, &m_fBorderUp, &WorldMat);
    D3DXVec3TransformCoord(&m_fBorderDown, &m_fBorderDown, &WorldMat);

    if (FAILED(Ready_Collider()))
        return E_FAIL;

    return S_OK;
}

void CRoad::Priority_Update(_float fTimeDelta)
{
    if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
        return;
}

void CRoad::Update(_float fTimeDelta)
{
    if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
        return;
}

void CRoad::Late_Update(_float fTimeDelta)
{
    if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
        return;

    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CRoad::Render()
{
    if (FAILED(m_pTransformCom->Bind_Transform()))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Texture(m_iBindTexNum)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    m_pColliderCom->Render_Collider();

    return S_OK;
}

HRESULT CRoad::Ready_Component()
{
    /* For.Com_Texture */
    // 프로토타입 텍스처 컴포넌트 터레인 가져와서 
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(m_iLevelIndex, m_strViBufferTag,
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRoad::Ready_Transform()
{
    /* For.Com_Transform */
    CTransform::TRANSFORM_DESC		TransformDesc{};

    TransformDesc.fSpeedPerSec = 20.0f;
    TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRoad::Ready_Collider()
{
    // Collider Component
    CCollider_Cube::COLLIDER_CUBE_INFO Collider_Info = {};

    if (m_eRoadDir == RIGHT || m_eRoadDir == LEFT)
    {
        Collider_Info.fSizeX = (m_pVIBufferCom->Get_NumVerticesX() - 3.f) * 0.5f;
        Collider_Info.fSizeY = 2.f;
        Collider_Info.fSizeZ = (m_pVIBufferCom->Get_NumVerticesZ() - 1.f) * 0.5f;
        Collider_Info.fOffSetX = (m_pVIBufferCom->Get_NumVerticesX() * 0.5f) - 0.5f;
        Collider_Info.fOffSetY = 0.f;
        Collider_Info.fOffSetZ = (m_pVIBufferCom->Get_NumVerticesZ()) * 0.5f;
    }
    else if (m_eRoadDir == UP || m_eRoadDir == DOWN)
    {

        Collider_Info.fSizeX = (m_pVIBufferCom->Get_NumVerticesX() - 2.f) * 0.5f;
        Collider_Info.fSizeY = 2.f;
        Collider_Info.fSizeZ = (m_pVIBufferCom->Get_NumVerticesZ() - 1.f) * 0.5f;
        Collider_Info.fOffSetX = (m_pVIBufferCom->Get_NumVerticesZ() - 1.f) * 0.5f;
        Collider_Info.fOffSetY = 0.f;
        Collider_Info.fOffSetZ = -(m_pVIBufferCom->Get_NumVerticesX() * 0.5f);
    }

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &Collider_Info)))
        return E_FAIL;

    return S_OK;
}

_float3 CRoad::Compute_TerrainCenter()
{
    _float3 vCenterPos{ 0.f,0.f,0.f };

    vCenterPos.x = (m_fBorderLeft.x + m_fBorderRight.x) * 0.5f;
    vCenterPos.y = Get_Position().y;
    vCenterPos.z = (m_fBorderUp.z + m_fBorderDown.z) * 0.5f;

    return vCenterPos;
}

void CRoad::OnCollisionEnter(CGameObject* pOther)
{
    if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
    {
        if (nullptr == m_pGameInstance->Find_Layer(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Monster"))
            || nullptr == m_pGameInstance->Find_Layer(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Boss"))
            || m_pGameInstance->Find_Layer(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Monster"))->Get_ObjectList().size() <= 0
            || m_pGameInstance->Find_Layer(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Boss"))->Get_ObjectList().size() <= 0)
        {
            m_bChage_Level = true;
        }
    }
}

void CRoad::OnCollisionStay(CGameObject* pOther)
{
}

void CRoad::OnCollisionExit(CGameObject* pOther)
{
}

CRoad* CRoad::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CRoad* pInstance = new CRoad(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CRoad"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRoad::Clone(void* pArg)
{
    CRoad* pInstance = new CRoad(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CRoad"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRoad::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
