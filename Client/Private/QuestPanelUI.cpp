#include "../Public/QuestPanelUI.h"
#include "GameInstance.h"

#include "QuestOrnamentUI.h"
#include "Player.h"

#include "Quest.h"

CQuestPanelUI::CQuestPanelUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CQuestPanelUI::CQuestPanelUI(const CQuestPanelUI& Prototype)
	: CUI{ Prototype }
{
}


HRESULT CQuestPanelUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQuestPanelUI::Initialize(void* pArg)
{
	CQuest::Get_Instance();

	if (FAILED(Ready_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = 300;
	m_fSizeY = 130;

	m_fTargetX = g_iWinSizeX - (m_fSizeX / 2.f);
	m_fX = g_iWinSizeX + (m_fSizeX / 2.f);
	m_fY = 200.f;


	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);



	CGameObject* pPrototype = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_QuestOrnamentUI"));
	if (nullptr != pPrototype)	// 존재한다면
	{	
		CUI* pQuestOrnamentUI = dynamic_cast<CUI*>(pPrototype->Clone(pArg)); // 프로토타입의 태그명을 보고 객체의 실 타입을 유추할수있을것 같다.
	
		pQuestOrnamentUI->Set_fX(m_fX - 50);  // 내 자식의 위치 = 내위치 + offset
		pQuestOrnamentUI->Set_fY(m_fY - 50);

		m_childUI_List.push_back(pQuestOrnamentUI);
		pQuestOrnamentUI->SetParentUI(this);
	}

	

	return S_OK;
}

void CQuestPanelUI::Priority_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Priority_Update(fTimeDelta);
	}
}

void CQuestPanelUI::Update(_float fTimeDelta)
{
	if (!m_bActivate || m_pGameInstance->Get_LevelIndex() == LEVEL_LOADING)
		return;

	CQuestOrnamentUI* pOrnamentUI = dynamic_cast<CQuestOrnamentUI*>(m_childUI_List.back());
	
	if(pOrnamentUI->Get_BindNum() == 0)
		ShowQuest(fTimeDelta);


	CQuest::tCurQuest quest = CQuest::Get_Instance()->Get_CurQuest();

	_bool bSucceed = CQuest::Get_Instance()->Get_CurQuest().bCurQuestSucceed;

	if (bSucceed)
		pOrnamentUI->Set_BindNum(1);
	else
		pOrnamentUI->Set_BindNum(0);



	if (pOrnamentUI->Get_BindNum() == 1)     // 성공햇다면
	{
		OutQuest(fTimeDelta);			// m_bActivate 다시 활성화 되는시점 생각해봐야겠다..
	}
	
		



	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Update(fTimeDelta);
	}
}

void CQuestPanelUI::Late_Update(_float fTimeDelta)
{
	if (!m_bActivate || m_pGameInstance->Get_LevelIndex() == LEVEL_LOADING)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CQuestPanelUI::Render()
{
	if (!m_bActivate || m_pGameInstance->Get_LevelIndex() == LEVEL_LOADING)
		return S_OK;


	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	_float4x4 ViewMat = {};
	D3DXMatrixIdentity(&ViewMat);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_Texture(0)))
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
	m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float));

	m_pShaderCom->Begin_Shader(1);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pShaderCom->End_Shader();


	CQuest::tCurQuest CurQuest = CQuest::Get_Instance()->Get_CurQuest();

	RECT rt = {
		(LONG)(m_fX - (m_fSizeX / 2.f) + 35),
		(LONG)(m_fY - (m_fSizeY / 2.f) + 25),
		(LONG)(m_fX + (m_fSizeX / 2.f) + 85),
		(LONG)(m_fY + (m_fSizeY / 2.f) + 35),
	};
	m_pGameInstance->RenderText(CurQuest.strAdditialnalName, &rt, D3DCOLOR_ARGB( _uint(255 * m_fAlpha) , 255, 255, 0),FONT_TYPE::MEDIUM);

	rt.top += (LONG)(40.f);
	rt.bottom = rt.top + 20;
	m_pGameInstance->RenderText(CurQuest.strCurQuestName, &rt, D3DCOLOR_ARGB(_uint(255 * m_fAlpha) , 255, 255, 255), FONT_TYPE::SMALL);



	for (auto& pChildUI : m_childUI_List)
		pChildUI->Render();

	return S_OK;
}

void CQuestPanelUI::ShowQuest(_float fTimeDelta)
{
	if (m_fAlpha < 1.f)
		m_fAlpha += 2.f / 60;
	else
		m_fAlpha = 1.f;


	if (m_fX > m_fTargetX)
		m_fX -= 100 * fTimeDelta;
	else if (abs(m_fX - m_fTargetX) < EPSILON || m_fX < m_fTargetX)
		m_fX = m_fTargetX;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));
}


void CQuestPanelUI::OutQuest(_float fTimeDelta)
{
	if (m_fAlpha > 0.f)
		m_fAlpha -= 2.f / 60;
	else
	{
		
		m_strCurQuestName = CQuest::Get_Instance()->Get_CurQuest().strCurQuestName;

		if(m_strCurQuestName == L"라타우와 대화하기")							// 이런식으로 가는거 말안되는데 구현어려워서 걍 이렇게함
			CQuest::Get_Instance()->AssignCurQuest(L"타로 카드 선택하기", L"세상의 빛");      // 항상 다음 퀘스트 정해라

		else if (m_strCurQuestName == L"타로 카드 선택하기")
			CQuest::Get_Instance()->AssignCurQuest(L"구시옹 무찌르기", L"세상의 빛");			// 항상 다음 퀘스트 정해라

		else if (m_strCurQuestName == L"구시옹 무찌르기")
			CQuest::Get_Instance()->AssignCurQuest(L"건축시스템 이해하기", L"적응 시간");

		else if (m_strCurQuestName == L"건축시스템 이해하기")
			CQuest::Get_Instance()->AssignCurQuest(L"나무와 돌캐기", L"적응 시간");

		else if (m_strCurQuestName == L"나무와 돌캐기")
			CQuest::Get_Instance()->AssignCurQuest(L"신도 일 시키기", L"적응 시간");




		else if (m_strCurQuestName == L"신도 일 시키기")
			CQuest::Get_Instance()->AssignCurQuest(L"던전1 조사하기", L"적을 용서하다");

		else if (m_strCurQuestName == L"던전1 조사하기")
			CQuest::Get_Instance()->AssignCurQuest(L"적을 신도로 영입하기", L"적을 용서하다");





		else if (m_strCurQuestName == L"적을 신도로 영입하기")
			CQuest::Get_Instance()->AssignCurQuest(L"건축: 교회 짓기", L"내실 다지기");

		else if (m_strCurQuestName == L"건축: 교회 짓기")
			CQuest::Get_Instance()->AssignCurQuest(L"던전2 헤켓 처치하기", L"내실 다지기");

		else if (m_strCurQuestName == L"던전2 헤켓 처치하기")
			CQuest::Get_Instance()->AssignCurQuest(L"건축: UpGrade석상짓기", L"내실 다지기");

		else if (m_strCurQuestName == L"건축: UpGrade석상짓기")
			CQuest::Get_Instance()->AssignCurQuest(L"모든 신도 기도시키기", L"내실 다지기");




		else if (m_strCurQuestName == L"모든 신도 기도시키기")
			CQuest::Get_Instance()->AssignCurQuest(L"기다리는 자 처치", L"마지막 임무");

		else if (m_strCurQuestName == L"기다리는 자 처치")
			CQuest::Get_Instance()->AssignCurQuest(L"인내의 눈동자 처치", L"마지막 임무");


		m_fAlpha = 0.f;
	}
		


	_float fOutTargetX = g_iWinSizeX + (m_fSizeX / 2.f);

	if (m_fX < fOutTargetX)
		m_fX += 100 * fTimeDelta;
	else if (abs(m_fX - fOutTargetX) < EPSILON || m_fX > fOutTargetX)
	{
		m_fX = fOutTargetX;
		m_fTime = 0.f;

		CQuestOrnamentUI* pOrnamentUI = dynamic_cast<CQuestOrnamentUI*>(m_childUI_List.back());
		pOrnamentUI->Set_BindNum(0);
	}
		

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

}



HRESULT CQuestPanelUI::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuestPanelUI"),
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


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CQuestPanelUI::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CQuestPanelUI::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

void CQuestPanelUI::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CQuestPanelUI"));
}

CQuestPanelUI* CQuestPanelUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CQuestPanelUI* pInstance = new CQuestPanelUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CQuestPanelUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CQuestPanelUI::Clone(void* pArg)
{
	CQuestPanelUI* pInstance = new CQuestPanelUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CQuestPanelUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuestPanelUI::Free()
{
	__super::Free();

	for (auto& pChildUI : m_childUI_List)
		Safe_Release(pChildUI);
	m_childUI_List.clear();


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
