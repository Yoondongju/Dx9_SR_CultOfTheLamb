
#include "..\Public\Terrain.h"

#include "GameInstance.h"
#include "Player.h"

#include "Boss_Gusion.h"
#include "Boss_Eligor.h"
#include "Boss_Hekat.h"

#include "Grid.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject { pGraphic_Device }
{
}

CTerrain::CTerrain(const CTerrain & Prototype)
	: CGameObject { Prototype }
	, m_iBindTexNum {Prototype.m_iBindTexNum}
	, m_strViBufferTag{Prototype.m_strViBufferTag}
{
}


HRESULT CTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	_uint* pIndex = static_cast<_uint*>(pArg);
	m_iStageIndex = *pIndex;

	// 마을에서 건축할때 그리드가 보여야하는데 미리 세팅해주고 활성화를 이용하자 그래야 생성타이밍때 부하 덜수있을거같아

	if (m_iLevelIndex == LEVEL_GAMEPLAY)
	{
		if (FAILED(Ready_BuildingGrid()))
			return E_FAIL;
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

	return S_OK;
}

void CTerrain::Priority_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;
}

void CTerrain::Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player());

	_float3 vPlayerPos = pPlayer->Get_Position();

	if (pPlayer->Get_Player_State() != CPlayer::CHANGE_LEVEL)
	{
		if (m_fBorderLeft.x >= vPlayerPos.x)
		{
			vPlayerPos.x = m_fBorderLeft.x;
			pPlayer->Set_Position(vPlayerPos);
		}
		else if (m_fBorderRight.x <= vPlayerPos.x)
		{
			vPlayerPos.x = m_fBorderRight.x;
			pPlayer->Set_Position(vPlayerPos);
		}

		if ((m_fBorderUp.z - 1) <= vPlayerPos.z)     // << 얘만 예외걸게 이상해먼가
		{
			vPlayerPos.z = m_fBorderUp.z - 1;
			pPlayer->Set_Position(vPlayerPos);
		}
		else if (m_fBorderDown.z >= vPlayerPos.z)
		{
			vPlayerPos.z = m_fBorderDown.z;
			pPlayer->Set_Position(vPlayerPos);
		}
	}

	if (m_pGameInstance->Get_LevelIndex() == LEVELID::LEVEL_DUNGEON0)
	{
		if (nullptr == m_pGusion && m_iStageIndex == 4)
		{
			m_pGusion = static_cast<CBoss_Gusion*>(m_pGameInstance->Find_Object(LEVELID::LEVEL_DUNGEON0, TEXT("Layer_Boss"), 0));
			Safe_AddRef(m_pGusion);
		}

		if (m_pGusion != nullptr)
		{
			vGusionPos = m_pGusion->Get_Position();

			if (m_fBorderLeft.x >= vGusionPos.x)
			{
				vGusionPos.x = m_fBorderLeft.x;
				m_pGusion->Set_Position(vGusionPos);
			}
			else if (m_fBorderRight.x <= vGusionPos.x)
			{
				vGusionPos.x = m_fBorderRight.x;
				m_pGusion->Set_Position(vGusionPos);
			}

			if ((m_fBorderUp.z - 1) <= vGusionPos.z)
			{
				vGusionPos.z = m_fBorderUp.z - 1;
				m_pGusion->Set_Position(vGusionPos);
			}
			else if (m_fBorderDown.z >= vGusionPos.z)
			{
				vGusionPos.z = m_fBorderDown.z;
				m_pGusion->Set_Position(vGusionPos);
			}
		}
	}

	if (m_pGameInstance->Get_LevelIndex() == LEVELID::LEVEL_DUNGEON1)
	{
		if (nullptr == m_pEligor && m_iStageIndex == 3)
		{
			m_pEligor = static_cast<CBoss_Eligor*>(m_pGameInstance->Find_Object(LEVELID::LEVEL_DUNGEON1, TEXT("Layer_Boss"), 0));
			Safe_AddRef(m_pEligor);
		}

		if (nullptr != m_pEligor)
		{
			vEligorPos = m_pEligor->Get_Position();

			if (m_fBorderLeft.x >= vEligorPos.x)
			{
				vEligorPos.x = m_fBorderLeft.x;
				m_pEligor->Set_Position(vEligorPos);
			}
			else if (m_fBorderRight.x <= vEligorPos.x)
			{
				vEligorPos.x = m_fBorderRight.x;
				m_pEligor->Set_Position(vEligorPos);
			}

			if ((m_fBorderUp.z - 1) <= vEligorPos.z)
			{
				vEligorPos.z = m_fBorderUp.z - 1;
				m_pEligor->Set_Position(vEligorPos);
			}
			else if (m_fBorderDown.z >= vEligorPos.z)
			{
				vEligorPos.z = m_fBorderDown.z;
				m_pEligor->Set_Position(vEligorPos);
			}
		}
	}

	if (m_pGameInstance->Get_LevelIndex() == LEVELID::LEVEL_DUNGEON2)
	{
		if (nullptr == m_pHekat && m_iStageIndex == 3)
		{
			m_pHekat = static_cast<CBoss_Hekat*>(m_pGameInstance->Find_Object(LEVELID::LEVEL_DUNGEON2, TEXT("Layer_Boss"), 0));
			Safe_AddRef(m_pHekat);
		}

		if (nullptr != m_pHekat)
		{
			vHekatPos = m_pHekat->Get_Position();

			if (m_fBorderLeft.x >= vHekatPos.x)
			{
				vHekatPos.x = m_fBorderLeft.x;
				m_pHekat->Set_Position(vHekatPos);
			}
			else if (m_fBorderRight.x <= vHekatPos.x)
			{
				vHekatPos.x = m_fBorderRight.x;
				m_pHekat->Set_Position(vHekatPos);
			}

			if ((m_fBorderUp.z - 1) <= vHekatPos.z)
			{
				vHekatPos.z = m_fBorderUp.z - 1;
				m_pHekat->Set_Position(vHekatPos);
			}
			else if (m_fBorderDown.z >= vHekatPos.z)
			{
				vHekatPos.z = m_fBorderDown.z;
				m_pHekat->Set_Position(vHekatPos);
			}
		}
	}


}

void CTerrain::Late_Update(_float fTimeDelta)
{
	if (m_iStageIndex != m_pGameInstance->Get_StageIndex())
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTerrain::Render()
{	
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;
	
	if (FAILED(m_pTextureCom->Bind_Texture(m_iBindTexNum, 0)))
		return E_FAIL;

	if (m_pGameInstance->Get_LevelIndex() == LEVEL_GAMEPLAY)
	{
		if (FAILED(m_pLightTextureCom->Bind_Texture(1, 1)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pLightTextureCom->Bind_Texture(0, 1)))
			return E_FAIL;
	}

	if (FAILED(m_pCircleMapTextureCom->Bind_Texture(0, 2)))
		return E_FAIL;

	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);

	m_pGraphic_Device->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);

	m_pGraphic_Device->SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, 1);

	// 텍스처 맵핑을 할때 확대 필터를 사용하고, Linear 방식으로 보간한다.
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	// D3DTSS_COLOROP, D3DTSS_ALPHAOP과 함께
	// D3DTOP_SELECTARG1를 사용하면 이번 stage에 input를 바로 output으로 전달한다.
	// 0번 스테이지의 컬러 값과 알파 값이 그대로 output으로 전달된다.
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	// color operation을 modulate(곱연산)
	m_pGraphic_Device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphic_Device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphic_Device->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);// 컬러의 첫 번째 인자는 현재 스테이지의 텍스처의 컬러 값
	m_pGraphic_Device->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);// 컬러의 두 번째 인자는 이전 스테이지에서 전달된 컬러 값
	m_pGraphic_Device->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);// 알파의 첫 번째 인자는 현재 스테이지의 텍스처의 알파 값
	m_pGraphic_Device->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);// 알파의 두 번째 인자는 이전 스테이지에서 전달된 알파 값

	// color operation을 modulate로 설정
	m_pGraphic_Device->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphic_Device->SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphic_Device->SetTextureStageState(2, D3DTSS_COLORARG1, D3DTA_TEXTURE);// 컬러의 첫 번째 인자는 현재 스테이지의 텍스처의 컬러 값
	m_pGraphic_Device->SetTextureStageState(2, D3DTSS_COLORARG2, D3DTA_CURRENT);// 컬러의 두 번째 인자는 이전 스테이지에서 전달된 컬러 값
	m_pGraphic_Device->SetTextureStageState(2, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);// 알파의 첫 번째 인자는 현재 스테이지의 텍스처의 알파 값
	m_pGraphic_Device->SetTextureStageState(2, D3DTSS_ALPHAARG2, D3DTA_CURRENT);// 알파의 두 번째 인자는 이전 스테이지에서 전달된 알파 값


	// 4번째 스테이지에서부터는 컬러와 알파값에 대한 연산을 하지 않는다.
	m_pGraphic_Device->SetTextureStageState(4, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pGraphic_Device->SetTextureStageState(4, D3DTSS_ALPHAOP, D3DTOP_DISABLE);


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
	m_pShaderCom->Set_Texture("g_Texture", m_pLightTextureCom->Get_Texture(0));

	//m_pShaderCom->Begin_Shader(1);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	//m_pShaderCom->End_Shader();

	m_pGraphic_Device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pGraphic_Device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	return S_OK;
}

_bool CTerrain::Check_InTerrain(_float3 vPos)
{
	if ((m_fBorderLeft.x + 1.f) > vPos.x)
		return false;

	if ((m_fBorderRight.x  -1.f) < vPos.x)
		return false;

	if ((m_fBorderUp.z - 1.f) < vPos.z)     
		return false;

	else if ((m_fBorderDown.z + 1.f) > vPos.z)
		return false;

	return true;
}

HRESULT CTerrain::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LightMap"),
		TEXT("Com_Texture_Light"), reinterpret_cast<CComponent**>(&m_pLightTextureCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_CircleMap"),
		TEXT("Com_Texture_CircleMap"), reinterpret_cast<CComponent**>(&m_pCircleMapTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(m_iLevelIndex, m_strViBufferTag,
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;


	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 20.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Example"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Ready_BuildingGrid()
{

	CGrid::GRID_DESC GridDesc = {};

	GridDesc.vScale.x = 1.9f;
	GridDesc.vScale.y = 1.f;
	GridDesc.vScale.z = 1.9f;

	_uint iVerticesNum = m_pVIBufferCom->Get_NumVertices();
	_float3* pVerticesPos = m_pVIBufferCom->Get_VerticesPos();



	_uint iZ = m_pVIBufferCom->Get_NumVerticesZ();
	_uint iX = m_pVIBufferCom->Get_NumVerticesX();

	// 70 ,50
	
	for (_uint i = 3; i < iZ - 9; ++i)
	{
		for (_uint j = 3; j < (iX - 9) / 2; ++j)
		{
			GridDesc.pOwner = this;
			GridDesc.iBindNum = 0;

		
			_float offsetX = j * (GridDesc.vScale.x + 0.3f);
			_float offsetZ = i * (GridDesc.vScale.z + 0.3f);

			// 홀수 행의 경우 x 좌표를 절반만큼 이동
			if (i % 2 != 0) 
			{
				offsetX += (GridDesc.vScale.x + 0.3f) / 2.0f;				
			}
			
			offsetZ -= i * ((GridDesc.vScale.z + 0.3f) / 2.f);

			
	
			// 그리드의 위치를 설정
			GridDesc.vPos = _float3(offsetX, 0.6f, offsetZ);


			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround_Grid"), TEXT("Prototype_GameObject_Grid"), &GridDesc)))
				return E_FAIL;
		}

	}

	return S_OK;
}

_float3 CTerrain::Compute_TerrainCenter()
{
	_float3 vCenterPos{ 0.f,0.f,0.f };

	vCenterPos.x = (m_fBorderLeft.x + m_fBorderRight.x) * 0.5f;
	vCenterPos.y = Get_Position().y;
	vCenterPos.z = (m_fBorderUp.z + m_fBorderDown.z) * 0.5f;

	return vCenterPos;
}

CTerrain * CTerrain::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTerrain*		pInstance = new CTerrain(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CTerrain::Clone(void* pArg)
{
	CTerrain*		pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	//if (LEVEL_DUNGEON0 == m_pGameInstance->Get_LevelIndex() && nullptr != m_pGusion)
	//	Safe_Release(m_pGusion);

	//if (LEVEL_DUNGEON1 == m_pGameInstance->Get_LevelIndex() && nullptr != m_pEligor)
	//	Safe_Release(m_pEligor);

	//if (LEVEL_DUNGEON2 == m_pGameInstance->Get_LevelIndex() && nullptr != m_pHekat)
	//	Safe_Release(m_pHekat);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pCircleMapTextureCom);
	Safe_Release(m_pLightTextureCom);
	Safe_Release(m_pVIBufferCom);
}
