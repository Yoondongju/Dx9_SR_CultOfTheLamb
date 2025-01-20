#include "../Public/MonsterHpUI.h"
#include "GameInstance.h"

#include "Player.h"
#include "Camera.h"

#include "Monster.h"
#include "Boss_DeathCat.h"
#include <Boss_Eligor.h>
#include "Boss_Eye.h"



_float CMonsterHpUI::m_fReduece_AmountTo_Boss = 0.f;


CMonsterHpUI::CMonsterHpUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CMonsterHpUI::CMonsterHpUI(const CMonsterHpUI& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CMonsterHpUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonsterHpUI::Initialize(void* pArg, void** pOut)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DESC* pDesc = reinterpret_cast<UI_DESC*>(pArg);


	if (nullptr != pOut)
	{
		*pOut = this;
	}


	m_fX = pDesc->m_vPos.x;
	m_fY = pDesc->m_vPos.y;

	m_fSizeX = pDesc->m_vSize.x;
	m_fSizeY = pDesc->m_vSize.y;
	m_fOffset = pDesc->m_vOffset;
	

	m_iBindNum = pDesc->m_iBindNum;

	m_eUItype = pDesc->eUItype;
	



	if (m_eUItype == UI_TYPE::UI_BAR)
	{
		m_pOwner = reinterpret_cast<CMonster*>(pDesc->pReferenceOwner);
		Safe_AddRef(m_pOwner);
	}




	if (nullptr != m_pOwner && TEXT("Layer_Boss") == m_pOwner->Get_LayerTag() && MONSTER_ID::EYE != m_pOwner->Get_MonsterId())
	{
		m_fX = g_iWinSizeX / 2.f;
		m_fY = g_iWinSizeY - 100.f;

		m_fSizeX = 1200;
		m_fSizeY = 40;
		m_iBindNum = 0;
		m_eUItype = UI_TYPE::UI_BAR;


		m_fX -= m_fSizeX / 4.f;

		D3DXMatrixIdentity(&m_ViewMatrix);
		D3DXMatrixIdentity(&m_ProjMatrix);


		/* 직교투영행렬을 만들어준다. */
		D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

		m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
			-m_fY + g_iWinSizeY * 0.5f, 0.f));
	}

	else if (nullptr != m_pOwner && TEXT("Layer_Boss") == m_pOwner->Get_LayerTag() &&  MONSTER_ID::EYE == m_pOwner->Get_MonsterId())
	{
		static _uint iCount = { 0 };
		m_fX = g_iWinSizeX / 2.f;
		m_fY = g_iWinSizeY - 130.f + (iCount * 50);

		iCount++;

		m_fSizeX = 1200;
		m_fSizeY = 30;
		m_iBindNum = 0;
		m_eUItype = UI_TYPE::UI_BAR;


		m_fX -= m_fSizeX / 4.f;

		D3DXMatrixIdentity(&m_ViewMatrix);
		D3DXMatrixIdentity(&m_ProjMatrix);


		/* 직교투영행렬을 만들어준다. */
		D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

		m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
			-m_fY + g_iWinSizeY * 0.5f, 0.f));
	}
	



	if (UI_TYPE::UI_BAR == m_eUItype)
	{
		CUI::UI_DESC Desc = {};
		Desc.m_vPos = { m_fX,m_fY };
		Desc.m_vSize = { m_fSizeX,m_fSizeY };
		Desc.m_iBindNum = 1;
		Desc.eUItype = UI_TYPE::UI_BAR_PREVIEW;
		if (FAILED(CreateMyChildUI(&Desc, L"Prototype_GameObject_MonsterHpUI")))
			return E_FAIL;


		Desc.m_vPos = { m_fX,m_fY };
		Desc.m_vSize = { m_fSizeX,m_fSizeY };
		Desc.m_iBindNum = 2;
		Desc.eUItype = UI_TYPE::UI_ELEMENT;
		if (FAILED(CreateMyChildUI(&Desc, L"Prototype_GameObject_MonsterHpUI")))
			return E_FAIL;

	}

	m_fOriginHp = m_fSizeX;

	m_bActivate = true;  

	return S_OK;
}

void CMonsterHpUI::Priority_Update(_float fTimeDelta)
{
	if(nullptr != m_pOwner 
		&& m_pOwner->Get_Dead() && 
		m_eUItype == UI_TYPE::UI_BAR)
	{
		m_pGameInstance->Event_DestoryObject(this);
	}
		


	CBoss_Eligor* pEligor = dynamic_cast<CBoss_Eligor*>(m_pOwner);
	if (nullptr != pEligor)
	{
		if (CBoss_Eligor::ELIGOR_STATE::TRANS_F == pEligor->Get_Eligor_State())
		{
			m_bActivate = false;			
		}
	}


	


	if (!m_bActivate)
		return;


	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Priority_Update(fTimeDelta);
	}
}




_float	g_fReduceLeftBoss = {};
_float	g_fReduceRightBoss = {};
_float	g_fReduceMiddleBoss = {};

void CMonsterHpUI::Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	CBoss_DeathCat* pDeathCat = dynamic_cast<CBoss_DeathCat*>(m_pOwner);
	if (nullptr != pDeathCat)		// 데스켓일대
	{
		if (false == pDeathCat->Get_IsActive())
			return;
	}


	if (nullptr != m_pOwner && TEXT("Layer_Boss") != m_pOwner->Get_LayerTag())
	{
		_float3 vOwnerPos = m_pOwner->Get_Position();
		CMonster::MONSTER_INFO tMonInfo = m_pOwner->Get_MonInfo();

		m_fX = vOwnerPos.x; 
		m_fY = vOwnerPos.y + m_fOffset.y;
		m_fZ = vOwnerPos.z;

		m_fX -= m_fSizeX / 4.f;
	}
	
	
	CMonsterHpUI* pParentUI = dynamic_cast<CMonsterHpUI*>(m_pParentUI);

	m_bIsBoss = (nullptr != m_pOwner && TEXT("Layer_Boss") == m_pOwner->Get_LayerTag()) ||
		(nullptr != pParentUI && TEXT("Layer_Boss") == pParentUI->Get_Owner()->Get_LayerTag());

	m_bIsEye = ((nullptr != pParentUI && TEXT("Layer_Boss") == pParentUI->Get_Owner()->Get_LayerTag() &&
			MONSTER_ID::EYE == pParentUI->Get_Owner()->Get_MonsterId()));


	if (nullptr != m_pParentUI)
	{
		m_fX = m_pParentUI->Get_fX();
		m_fY = m_pParentUI->Get_fY();
		m_fZ = m_pParentUI->Get_fZ();

	
		if (m_eUItype == UI_TYPE::UI_ELEMENT)
		{		
			CMonster::MONSTER_INFO tMonInfo = m_pOwner->Get_MonInfo();
	
			_float fHpRatio = (_float)tMonInfo.iHp / tMonInfo.iMaxHp;    // << 나중에 여기 바꾸세용 

			if (fHpRatio <= 0.f)
				fHpRatio = 0.f;

			if (m_bIsBoss && !m_bIsEye)
			{	
				m_fReduece_AmountTo_Boss = fHpRatio;
			}
			else if (m_bIsEye)
			{
				switch (dynamic_cast<CBoss_Eye*>(m_pOwner)->Get_EyeType())
				{
				case CBoss_Eye::EYE_TYPE::LEFT:
					g_fReduceLeftBoss = fHpRatio;
					break;
				case CBoss_Eye::EYE_TYPE::RIGHT:
					g_fReduceRightBoss = fHpRatio;
					break;
				case CBoss_Eye::EYE_TYPE::MIDDLE:
					g_fReduceMiddleBoss = fHpRatio;
					break;
				default:
					break;
				}
					
			}
			else
			{
				m_fReduece_AmountTo_Base = fHpRatio;
			}
			

			m_fSizeX = m_fOriginHp * fHpRatio;
		}


		if (m_eUItype == UI_TYPE::UI_BAR_PREVIEW)
		{
			_float fTargetSize = {};

			if (m_bIsBoss && !m_bIsEye)
			{
				fTargetSize = m_fOriginHp * m_fReduece_AmountTo_Boss;
			}

			else if (m_bIsEye)
			{
				switch (dynamic_cast<CBoss_Eye*>(m_pOwner)->Get_EyeType())
				{
				case CBoss_Eye::EYE_TYPE::LEFT:
					fTargetSize = m_fOriginHp * g_fReduceLeftBoss;
					break;
				case CBoss_Eye::EYE_TYPE::RIGHT:
					fTargetSize = m_fOriginHp * g_fReduceRightBoss;
					break;
				case CBoss_Eye::EYE_TYPE::MIDDLE:
					fTargetSize = m_fOriginHp * g_fReduceMiddleBoss;
					break;
				default:
					break;
				}				
			}

			else
			{		
				for (auto& pUI : m_pParentUI->GetChildUI())
				{
					if (UI_TYPE::UI_ELEMENT == pUI->GetUI_Type())
					{
						m_fReduece_AmountTo_Base = dynamic_cast<CMonsterHpUI*>(pUI)->Get_Reduece_AmountTo_Base();
					}
				}

				fTargetSize = m_fOriginHp * m_fReduece_AmountTo_Base;
			}
						
			if (fTargetSize < 0.f)
			{
				fTargetSize = 0.f;
				m_fSizeX = 0.f;
			}
					
			if (m_fSizeX > fTargetSize)
			{
				_float fAddSpeed = 3.f;
				if(m_fOriginHp == 4.f)		
					m_fSizeX -= (m_fOriginHp / 6.f) * fTimeDelta * fAddSpeed;	// 일반몬스터 체력
				else
					m_fSizeX -= (m_fOriginHp / 6.f) * fTimeDelta;   // 보스체력
				

				if (m_fSizeX <= fTargetSize) {
					m_fSizeX = fTargetSize;
				}

				m_fAlpha = 0.5f;
			}			

		}	
	}
	

	if (m_bIsBoss)
	{
		D3DXMatrixIdentity(&m_ProjMatrix);
		D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

		m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
			-m_fY + g_iWinSizeY * 0.5f, 0.f));
	}
	else
	{
		m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX, m_fY, m_fZ));
	}




	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Update(fTimeDelta);
	}

}

void CMonsterHpUI::Late_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CMonsterHpUI::Render()
{
	if (!m_bActivate)
		return S_OK;


	CBoss_DeathCat* pDeathCat = dynamic_cast<CBoss_DeathCat*>(m_pOwner);
	if (nullptr != pDeathCat)		// 데스켓일대
	{
		if (false == pDeathCat->Get_IsActive())
			return S_OK;
	}



	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;



	if (m_bIsBoss)
	{			
		// 보스는 직교로 그려라
		D3DXMatrixIdentity(&m_ViewMatrix);
		/* 직교투영행렬을 만들어준다. */
		
		m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
		m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);
	}
	else
	{
		// 일몬은 원근으으로 그려라
		CCamera* pCamera = static_cast<CCamera*>(m_pGameInstance->Find_Camera());
		_float4x4 ViewMat = pCamera->Get_Transform()->Get_WorldMatrix_Inverse();
		_float4x4 ProjMat = pCamera->Get_Transform()->Get_CameraProjMatrix();

		m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMat);
		m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &ProjMat);
	}
	



	if (FAILED(m_pTextureCom->Bind_Texture(m_iBindNum)))
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
	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(m_iBindNum));
	m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float));

	

	m_pShaderCom->Begin_Shader(1);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;



	CMonsterHpUI* pParentUI = dynamic_cast<CMonsterHpUI*>(m_pParentUI);

	if (m_eUItype == UI_TYPE::UI_ELEMENT && TEXT("Layer_Boss") == pParentUI->Get_Owner()->Get_LayerTag())
	{
		_wstring strMonName = L"";
		MONSTER_ID eMyName = (MONSTER_ID)m_pOwner->Get_MonsterId();

		switch (eMyName)
		{
		case Client::GUSION:
			strMonName = L"구시옹";
			break;
		case Client::ELIGOS:
			strMonName = L"엘리고스";
			break;
		case Client::HEKAT:
			strMonName = L"헤 켓";
			break;
		case Client::AYM:
			strMonName = L"에 임";
			break;
		case Client::DEATHCAT:
			strMonName = L"기다리는 자";
			break;
		case Client::EYE:
		{
			CBoss_Eye::EYE_TYPE eType = dynamic_cast<CBoss_Eye*>(pParentUI->Get_Owner())->Get_EyeType();
			switch (eType)
			{
			case Client::CBoss_Eye::LEFT:
				strMonName = L"인내의 좌안";
				break;
			case Client::CBoss_Eye::MIDDLE:
				strMonName = L"인내의 회랑";
				break;
			case Client::CBoss_Eye::RIGHT:
				strMonName = L"인내의 우안";
				break;
			case Client::CBoss_Eye::EYE_TYPE_END:
				break;
			default:
				break;
			}

		}		
			break;
		case Client::MONSTER_END:
			break;
		default:
			break;
		}
	
		RECT rt = {
				   LONG((m_fX + m_fOriginHp / 4.f) - (strMonName.size() * 10.f)),
				   LONG(m_fY - 15),
				   LONG(m_fX + m_fOriginHp / 4.f) + 200,
				   LONG(m_fY + 50),
		};

		m_pGameInstance->RenderText(strMonName, &rt, D3DCOLOR_XRGB(255, 255, 255), FONT_TYPE::MEDIUM);
	}


	m_pShaderCom->End_Shader();



	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Render();
	}

	return S_OK;
}

HRESULT CMonsterHpUI::CreateMyChildUI(void* pArg, _wstring strPrototypeTag, _uint iPosNum)
{
	CGameObject* pPrototype = m_pGameInstance->Find_Prototype(strPrototypeTag);
	if (nullptr != pPrototype)	// 존재한다면
	{
		CUI* pChildUI = dynamic_cast<CUI*>(pPrototype->Clone(pArg)); // 프로토타입의 태그명을 보고 객체의 실 타입을 유추할수있을것 같다.

		m_childUI_List.push_back(pChildUI);
		pChildUI->SetParentUI(this);
		pChildUI->SetActivate(true);

		dynamic_cast<CMonsterHpUI*>(pChildUI)->Set_Owner(m_pOwner);
		Safe_AddRef(m_pOwner);
	}

	return S_OK;
}


HRESULT CMonsterHpUI::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MonsterHp"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterHpUI::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CMonsterHpUI::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}



void CMonsterHpUI::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CMonsterHpUI"));
}

CMonsterHpUI* CMonsterHpUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMonsterHpUI* pInstance = new CMonsterHpUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonsterHpUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CMonsterHpUI::Clone(void* pArg)
{
	CMonsterHpUI* pInstance = new CMonsterHpUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CMonsterHpUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonsterHpUI::Clone_UI(void* pArg, void** pOut)
{
	CMonsterHpUI* pInstance = new CMonsterHpUI(*this);

	if (FAILED(pInstance->Initialize(pArg, pOut)))
	{
		MSG_BOX(TEXT("Failed to Created : CMonsterHpUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterHpUI::Free()
{
	__super::Free();


	Safe_Release(m_pOwner);


	if (m_childUI_List.size() > 0)
	{
		for (auto& pChildUI : m_childUI_List)
			Safe_Release(pChildUI);
		m_childUI_List.clear();
	}

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
