#include "../Public/PlayerHpUI.h"
#include "GameInstance.h"

#include "Player.h"

#include "Layer.h"

#include "TarotPanel.h"
#include "Camera.h"

_uint CPlayerHpUI::m_iInitCount = 0;

CPlayerHpUI::CPlayerHpUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CPlayerHpUI::CPlayerHpUI(const CPlayerHpUI& Prototype)
	: CUI{ Prototype }
{

}


HRESULT CPlayerHpUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerHpUI::Initialize(void* pArg , void** pOut)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);


	if (nullptr != pOut)
	{
		*pOut = this;
	}


	_float4x4 ViewMat = {};
	_float4x4 ProjectionMat = {};

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjectionMat);


	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = pDesc->m_vSize.x;
	m_fSizeY = pDesc->m_vSize.y;

	m_fX = pDesc->m_vPos.x;
	m_fY = pDesc->m_vPos.y;
	m_iBindNum = pDesc->m_iBindNum;

	m_eUItype = pDesc->eUItype;
	m_vTargetPos = pDesc->vTargetPos;


	if (m_eUItype == UI_TYPE::UI_ELEMENT)
	{
		m_pOwner = static_cast<CPlayer*>(m_pGameInstance->Find_Player());
		Safe_AddRef(m_pOwner);
	}
	


	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);


	

	 if(nullptr != pDesc->pReferenceOwner)			// << Taro Panel
	 {			 
		 m_pReferenceOwner = reinterpret_cast<CTarotPanel*>(pDesc->pReferenceOwner);
		 Safe_AddRef(m_pReferenceOwner);

		 if (UI_TYPE::UI_BAR == m_eUItype)
		 {
			 CUI::UI_DESC Desc = {};
			 Desc.m_vPos = { m_fX ,m_fY };
			 Desc.m_vSize = { 30.f,30.f };
			 Desc.m_iBindNum = 3;
			 Desc.eUItype = UI_TYPE::UI_ELEMENT;
			 if (FAILED(CreateMyChildUI(&Desc, L"Prototype_GameObject_PlayerHpUI")))
				 return E_FAIL;
		 }
	 }
	 else
	 {
		 if (UI_TYPE::UI_BAR == m_eUItype)
		 {
			 CUI::UI_DESC Desc = {};
			 Desc.m_vPos = { m_fX ,m_fY };
			 Desc.m_vSize = { 30.f,30.f };
			 Desc.m_iBindNum = 2;
			 Desc.eUItype = UI_TYPE::UI_ELEMENT;
			 if (FAILED(CreateMyChildUI(&Desc, L"Prototype_GameObject_PlayerHpUI")))
				 return E_FAIL;

			 m_iInitCount = 0;
		 }
				 
	 }

	m_bActivate = false;  // 얘는 키 눌러야 켜져


	return S_OK;
}

void CPlayerHpUI::Priority_Update(_float fTimeDelta)
{
	
	if (nullptr != m_pReferenceOwner &&
		false == m_pReferenceOwner->GetActivate())
	{
		m_bActivate = true;

		CUI* pElementUI = m_childUI_List.back();

		if (false == m_bEventCheck)
		{
			static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(1.f, 0.4f, 1);
			pElementUI->SetActivate(true);

			m_bEventCheck = true;
		}

		if (true == dynamic_cast<CPlayerHpUI*>(pElementUI)->Get_InitSet())
		{
			if (!m_bInitSet)
				MoveTo_TargetPos(fTimeDelta);
		}
	}


	if (!m_bActivate)
		return;


	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Priority_Update(fTimeDelta);
	}
}

void CPlayerHpUI::Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;


	if (nullptr == m_pReferenceOwner)
	{
		if (m_eUItype == UI_TYPE::UI_BAR)
		{
			CUI* pElementUI = m_childUI_List.back();

			if (true == dynamic_cast<CPlayerHpUI*>(pElementUI)->Get_InitSet())
			{
				if (!m_bInitSet)
					MoveTo_TargetPos(fTimeDelta);
				else if (m_iInitCount == 3)
				{
					list<CGameObject*>& GameObjects = m_pGameInstance->Find_Layer(m_pGameInstance->Get_LevelIndex(), L"Layer_SermonUI")->Get_ObjectList();

					for (auto& GameObj : GameObjects)
					{
						CUI* pUI = dynamic_cast<CUI*>(GameObj);

						if (nullptr != pUI)
							pUI->SetActivate(true);
					}

				}
			}
		}

		if (m_eUItype == UI_TYPE::UI_ELEMENT)
		{
			if (false == m_bInitSet)
			{
				if (m_fSizeX < 150.f && m_fSizeY < 150.f)
				{
					m_fSizeX += 3.f;
					m_fSizeY += 3.f;
				}
				else
				{
					m_bInitSet = true;
					m_fSizeX = 150.f;
					m_fSizeY = 150.f;
				}
			}
			else if (true == m_bInitSet)	  // 초기세팅이 끝났으면
			{
				m_fX = m_pParentUI->Get_fX();
				m_fY = m_pParentUI->Get_fY();

				m_fSizeX = m_pParentUI->Get_fSizeX();
				m_fSizeY = m_pParentUI->Get_fSizeY();

				m_iMyOrder = dynamic_cast<CPlayerHpUI*>(m_pParentUI)->Get_MyOrder();
				Compute_PlayerHp();		// 실제 플레이어 Hp와 연동
			}
		}
	}
	




	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));




	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Update(fTimeDelta);
	}


}

void CPlayerHpUI::Late_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CPlayerHpUI::Render()
{
	if (!m_bActivate)
		return S_OK;

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	_float4x4 ViewMat = {};
	D3DXMatrixIdentity(&ViewMat);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);




	if (FAILED(m_pTextureCom->Bind_Texture(m_iBindNum)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;


	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;




	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Render();
	}

	return S_OK;
}

HRESULT CPlayerHpUI::CreateMyChildUI(void* pArg, _wstring strPrototypeTag, _uint iPosNum)
{
	CGameObject* pPrototype = m_pGameInstance->Find_Prototype(strPrototypeTag);
	if (nullptr != pPrototype)	// 존재한다면
	{
		CUI* pChildUI = dynamic_cast<CUI*>(pPrototype->Clone(pArg)); // 프로토타입의 태그명을 보고 객체의 실 타입을 유추할수있을것 같다.

		m_childUI_List.push_back(pChildUI);
		pChildUI->SetParentUI(this);	
	}

	return S_OK;
}

void CPlayerHpUI::MoveTo_TargetPos(_float fDeletaTime)
{
	if (m_fX == m_vTargetPos.x &&
		m_fY == m_vTargetPos.y &&
		m_fSizeX == 30.f &&
		m_fSizeY == 30.f)
	{
		m_bInitSet = true;
		
		++m_iInitCount;	// 1,2,3  초록이는 4일듯     /    1 ,  2 , 3		 이새기는 Bar임 

		m_iMyOrder = m_iInitCount;
	}

	m_vOriginPos = { m_fX ,m_fY };

	_float2 vDir = { m_vTargetPos - m_vOriginPos };
	D3DXVec2Normalize(&vDir, &vDir);
	
	m_fX += vDir.x * 400.f * fDeletaTime;
	m_fY += vDir.y * 400.f * fDeletaTime;

	m_fSizeX -= 40.f * fDeletaTime;
	m_fSizeY -= 40.f * fDeletaTime;

	

	if (fabs(m_vTargetPos.x - m_fX) < EPSILON
		&& fabs(m_vTargetPos.y - m_fY) < EPSILON ||
		m_fX < m_vTargetPos.x || m_fY < m_vTargetPos.y)
	{
		m_fX = m_vTargetPos.x;
		m_fY = m_vTargetPos.y;	
	}

	if (fabs(30.f - m_fSizeX) < EPSILON
		&& fabs(30.f - m_fSizeY) < EPSILON ||
		m_fSizeX < 30 || m_fSizeY < 30)
	{
		m_fSizeX = 30.f;
		m_fSizeY = 30.f;
	}
	
}

void CPlayerHpUI::Compute_PlayerHp()
{
	_int iPlayerHp = (_int)m_pOwner->Get_Player_Info().iHp;

	
	if (m_iMyOrder == 4)
	{
		if (m_iBindNum !=0 && iPlayerHp <= 6)
		{
			m_iBindNum = 0;
		}
			
		return;
	}
	
	switch (iPlayerHp)
	{
	case 6:
	{
		m_iBindNum = 2;				// 모든 애들의 이미지를 풀피로 바꾼다.
	}
	break;

	case 5:
	{
		if (m_iMyOrder == 3)		// 마지막 애만 반피로만들고 나머진 풀피다.
			m_iBindNum = 1;
		else
			m_iBindNum = 2;
	}
	break;

	case 4:
	{
		if (m_iMyOrder == 3)		// 마지막 애만 피없고 나머진 풀피다.
			m_iBindNum = 0;
		else
			m_iBindNum = 2;
	}
	break;

	case 3:
	{
		if (m_iMyOrder == 2)
			m_iBindNum = 1;
		else if (m_iMyOrder == 3)
			m_iBindNum = 0;
		else if (m_iMyOrder == 1)
			m_iBindNum = 2;
	}
	break;

	case 2:
	{
		if (m_iMyOrder == 2)
			m_iBindNum = 0;
		else if (m_iMyOrder == 3)
			m_iBindNum = 0;
		else if (m_iMyOrder == 1)
			m_iBindNum = 2;
	}
	break;

	case 1:
	{
		if (m_iMyOrder == 2)
			m_iBindNum = 0;
		else if (m_iMyOrder == 3)
			m_iBindNum = 0;
		else if (m_iMyOrder == 1)
			m_iBindNum = 1;
	}
	break;

	case 0:
	{
		if (m_iMyOrder == 2)
			m_iBindNum = 0;
		else if (m_iMyOrder == 3)
			m_iBindNum = 0;
		else if (m_iMyOrder == 1)
			m_iBindNum = 0;
	}
	break;

	default:
		break;
	}


}

HRESULT CPlayerHpUI::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerHp"),
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

	return S_OK;
}

HRESULT CPlayerHpUI::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CPlayerHpUI::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}





void CPlayerHpUI::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CPlayerHpUI"));
}

CPlayerHpUI* CPlayerHpUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayerHpUI* pInstance = new CPlayerHpUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayerHpUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CPlayerHpUI::Clone(void* pArg)
{
	CPlayerHpUI* pInstance = new CPlayerHpUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayerHpUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayerHpUI::Clone_UI(void* pArg, void** pOut)
{
	CPlayerHpUI* pInstance = new CPlayerHpUI(*this);

	if (FAILED(pInstance->Initialize(pArg, pOut)))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayerHpUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerHpUI::Free()
{
	__super::Free();

	for (auto& pChildUI : m_childUI_List)
		Safe_Release(pChildUI);
	m_childUI_List.clear();


	Safe_Release(m_pReferenceOwner);
	Safe_Release(m_pOwner);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
