#include "Door.h"
#include "GameInstance.h"
#include "Level_Loading.h"

#include "Player.h"

CDoor::CDoor(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CLandObject{ pGraphic_Device }
{
}

CDoor::CDoor(const CDoor& Prototype)
	:CLandObject{ Prototype }
{
}

HRESULT CDoor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDoor::Initialize(void* pArg)
{
    DOOR_DESC* tDoorDesc = static_cast<DOOR_DESC*>(pArg);

    m_eDoorId = tDoorDesc->eDoorId;
    m_isOpen = tDoorDesc->isOpen;

    if (FAILED(__super::Initialize(&tDoorDesc->tLandDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, tDoorDesc->vPosition);

    _float3 vScale = { 5.3f,5.7f,1.f };
    m_pTransformCom->Scaling(vScale);

	return S_OK;
}

void CDoor::Priority_Update(_float fTimeDelta)
{
    if (m_iStageIndex != m_pGameInstance->Get_StageIndex()
        || m_iLevelIndex != m_pGameInstance->Get_LevelIndex())
        return;
}

void CDoor::Update(_float fTimeDelta)
{
    if (m_iStageIndex != m_pGameInstance->Get_StageIndex()
        || m_iLevelIndex != m_pGameInstance->Get_LevelIndex())
        return;

    __super::SetUp_OnTerrain(m_pTransformCom, 0.5f);
}

void CDoor::Late_Update(_float fTimeDelta)
{
    if (m_iStageIndex != m_pGameInstance->Get_StageIndex()
        || m_iLevelIndex != m_pGameInstance->Get_LevelIndex())
        return;

    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CDoor::Render()
{
    m_pVIBufferCom->Filp_Texture(false);

    if (FAILED(m_pTransformCom->Bind_Transform()))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Texture()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;


    _float4x4		WorldMatrix, ViewMatrix, ProjMatrix;

    WorldMatrix = *D3DXMatrixTranspose(&WorldMatrix, &m_pTransformCom->Get_WorldMatrix());
    m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
    m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

    m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
    m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4));
    m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_float4x4));

    m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
    m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(0));

    if (m_isOpen)
        m_pShaderCom->Begin_Shader(1);
    else
        m_pShaderCom->Begin_Shader(2);

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    m_pShaderCom->End_Shader();

    m_pColliderCom->Render_Collider();
    return S_OK;
}

void CDoor::OnCollisionEnter(CGameObject* pOther)
{
    if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
    {
        if (m_isOpen)
        {
            if (m_eDoorId == DOOR_DUNGEON1)
            {
                m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, LEVEL_DUNGEON1));
            }
            else if (m_eDoorId == DOOR_DUNGEON2)
            {
                m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, LEVEL_DUNGEON2));
            }
            else if (m_eDoorId == DOOR_DUNGEON3)
            {
                m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, LEVEL_DUNGEON3));
            }


            static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_Transform()->Scaling(_float3(3.5f, 3.5f, 0.1f));
        }
    }
}

void CDoor::OnCollisionStay(CGameObject* pOther)
{
}

void CDoor::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CDoor::Ready_Components()
{
    // Texture Com
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Door"), TEXT("Com_Texture_Door"),
        reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    // VIBuffer Com
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("VIBuffer_Com"),
        reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    // Collider Com
    CCollider_Cube::COLLIDER_CUBE_INFO ColliderDesc = {};
    ColliderDesc.fSizeX = 0.5f;
    ColliderDesc.fSizeY = 0.5f;
    ColliderDesc.fSizeZ = 0.8f;
    ColliderDesc.fOffSetX = 0.0f;
    ColliderDesc.fOffSetY = 0.0f;
    ColliderDesc.fOffSetZ = -0.1f;
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), TEXT("Collider_Com"),
        reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    // Transform Com
    CTransform::TRANSFORM_DESC TransformDesc = {};
    TransformDesc.fSpeedPerSec = 10.f;
    TransformDesc.fRotationPerSec = D3DXToRadian(60.f);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Transform_Com"),
        reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Door"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

CDoor* CDoor::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CDoor* pInstance = new CDoor(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CDoor"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDoor::Clone(void* pArg)
{
    CDoor* pInstance = new CDoor(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CDoor"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDoor::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
}
