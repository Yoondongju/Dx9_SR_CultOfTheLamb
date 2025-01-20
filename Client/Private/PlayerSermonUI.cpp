#include "../Public/PlayerSermonUI.h"
#include "GameInstance.h"

#include "Player.h"
#include "MonologueUI.h"

CPlayerSermonUI::CPlayerSermonUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CPlayerSermonUI::CPlayerSermonUI(const CPlayerSermonUI& Prototype)
	: CUI{ Prototype }
{


}


HRESULT CPlayerSermonUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerSermonUI::Initialize(void* pArg, void** pOut)
{
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
	

	if (FAILED(Ready_Components()))
		return E_FAIL;



	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);


	if (m_eUItype == UI_TYPE::UI_NONE)
	{
		UI_DESC Desc = {};

		Desc.m_vPos = { m_fX, m_fY };
		Desc.m_vSize = { m_fSizeX,m_fSizeY };
		Desc.m_iBindNum = 1;
		Desc.eUItype = UI_TYPE::UI_ELEMENT;

		CUI* pGameObj = dynamic_cast<CUI*>(this->Clone(&Desc)); // 프로토타입의 태그명을 보고 객체의 실 타입을 유추할수있을것 같다.

		m_childUI_List.push_back(pGameObj);
		pGameObj->SetParentUI(this);
		pGameObj->SetActivate(true);
	}
	
	

	m_fAlpha = 0.f;
	m_bActivate = false;  // 얘는 키 눌러야 켜져

	return S_OK;
}

void CPlayerSermonUI::Priority_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;


	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Priority_Update(fTimeDelta);
	}
}

void CPlayerSermonUI::Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	if (m_eUItype != UI_TYPE::UI_ELEMENT)
	{
		if (m_fAlpha < 1.f)
			m_fAlpha += 0.02f;
		else
		{
			if (false == m_bInitCheck)
			{
				LEVELID   eCurLevel = (LEVELID)m_pGameInstance->Get_LevelIndex();

				CMonologueUI::MONOLOGUE_DESC Desc = {};

				switch (eCurLevel)
				{
				case Client::LEVEL_DUNGEON0:
				{
					Desc.strText = L"어둠의 심연";
					Desc.rt = {
					(LONG)(g_iWinSizeX / 2.f - 130.f),
					(LONG)(g_iWinSizeY / 2.f),
					(LONG)(g_iWinSizeX / 2.f + 200.f),
					(LONG)(g_iWinSizeY / 2.f + 100.f)
					};
				}
				break;
				case Client::LEVEL_DUNGEON1:
				{
					Desc.strText = L"저주받은 숲";
					Desc.rt = {
					(LONG)(g_iWinSizeX / 2.f - 130.f),
					(LONG)(g_iWinSizeY / 2.f),
					(LONG)(g_iWinSizeX / 2.f + 200.f),
					(LONG)(g_iWinSizeY / 2.f + 100.f)
					};
				}
				break;
				case Client::LEVEL_DUNGEON2:
				{
					Desc.strText = L"불타는 지옥";
					Desc.rt = {
					(LONG)(g_iWinSizeX / 2.f - 130.f),
					(LONG)(g_iWinSizeY / 2.f),
					(LONG)(g_iWinSizeX / 2.f + 200.f),
					(LONG)(g_iWinSizeY / 2.f + 100.f)
					};
				}
				break;
				case Client::LEVEL_DUNGEON3:
				{
					Desc.strText = L"망자의 왕국";
					Desc.rt = {
					(LONG)(g_iWinSizeX / 2.f - 130.f),
					(LONG)(g_iWinSizeY / 2.f),
					(LONG)(g_iWinSizeX / 2.f + 200.f),
					(LONG)(g_iWinSizeY / 2.f + 100.f)
					};
				}
				break;
				case Client::LEVEL_END:
					break;
				default:
					break;
				}

				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(eCurLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonologueUI"), &Desc)))
					MSG_BOX(TEXT("Can't Create MonologueUI"));
				m_pGameInstance->Play_Sound(TEXT("UI_AreaText.wav"), SOUND_EFFECT, g_fBgmVolume);
				m_bInitCheck = true;
			}
			m_fAlpha = 1.f;
		}
	}
	else if (m_eUItype == UI_TYPE::UI_ELEMENT)
	{
		m_fAlpha = static_cast<CPlayer*>(m_pGameInstance->Find_Player())->Get_SermonGauge();
	}



	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));




	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Update(fTimeDelta);
	}

	MouseOnCheck();
}

void CPlayerSermonUI::Late_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CPlayerSermonUI::Render()
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

	m_pShaderCom->End_Shader();




	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Render();
	}

	return S_OK;
}

HRESULT CPlayerSermonUI::CreateMyChildUI(void* pArg, _wstring strPrototypeTag, _uint iPosNum)
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

HRESULT CPlayerSermonUI::Ready_Components()
{
	/* For.Com_Texture */

	_wstring strTexName = L"";
	switch (m_eUItype)
	{
	case Engine::UI_NONE:
		strTexName = TEXT("Prototype_Component_Texture_UI_PlayerSermon");
		break;
	case Engine::UI_ELEMENT:
		strTexName = TEXT("Prototype_Component_Texture_UI_PlayerSermon");
		break;
	case Engine::UI_ICON:
		strTexName = TEXT("Prototype_Component_Texture_UI_PlayerSermonIcon");
		break;
	default:
		break;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, strTexName,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC      TransformDesc{};

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

HRESULT CPlayerSermonUI::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CPlayerSermonUI::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}



void CPlayerSermonUI::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CPlayerSermonUI"));
}

CPlayerSermonUI* CPlayerSermonUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayerSermonUI* pInstance = new CPlayerSermonUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayerSermonUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CPlayerSermonUI::Clone(void* pArg)
{
	CPlayerSermonUI* pInstance = new CPlayerSermonUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayerSermonUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayerSermonUI::Clone_UI(void* pArg, void** pOut)
{
	CPlayerSermonUI* pInstance = new CPlayerSermonUI(*this);

	if (FAILED(pInstance->Initialize(pArg, pOut)))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayerSermonUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerSermonUI::Free()
{
	__super::Free();

	for (auto& pChildUI : m_childUI_List)
		Safe_Release(pChildUI);
	m_childUI_List.clear();


	Safe_Release(m_pOwner);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
