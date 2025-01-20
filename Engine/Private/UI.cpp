#include "UI.h"

#include "GameInstance.h"

CUI::CUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CBlendObject{ pGraphic_Device }
{
}

CUI::CUI(const CUI& Prototype)
	:CBlendObject{ Prototype }
{
}

HRESULT CUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
	return S_OK;
}

void CUI::Priority_Update(_float fTimeDelta)
{
}

void CUI::Update(_float fTimeDelta)
{
}

void CUI::Late_Update(_float fTimeDelta)
{
}

HRESULT CUI::Render()
{
	return S_OK;
}

void CUI::MouseOnCheck()
{
	POINT vMousePos = m_pGameInstance->Get_MosePos();
	D3DXVECTOR3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	RECT tUIRect;
	SetRect(&tUIRect, int(m_fX - m_fSizeX * 0.5f), int(m_fY - m_fSizeY * 0.5f),
		int(m_fX + m_fSizeX * 0.5f), int(m_fY + m_fSizeY * 0.5f));

	if (vMousePos.x > tUIRect.left
		&& vMousePos.x < tUIRect.right
		&& vMousePos.y > tUIRect.top
		&& vMousePos.y < tUIRect.bottom)
	{
		m_bMouseOn = true;
	}
	else
		m_bMouseOn = false;

}

void CUI::MouseLBtnDown()
{
}

void CUI::MouseLBtnUp()
{
}

void CUI::MouseClick()
{
}

void CUI::MouseOn()
{
}

void CUI::MouseOut()
{
}

HRESULT CUI::Compute_ViewZ(CTransform* pTransform)
{
	if (nullptr == pTransform)
		return E_FAIL;

	_float3			vWorldPos = pTransform->Get_State(CTransform::STATE_POSITION);

	_float4x4		ViewMatrix{};

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	m_fViewZ = (*D3DXVec3TransformCoord(&vWorldPos, &vWorldPos, &ViewMatrix)).z;

	return S_OK;
}

HRESULT CUI::Compute_ViewZ(const _float3 vWorlPos)
{
	_float3			vWorldPos = vWorlPos;

	_float4x4		ViewMatrix{};

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	m_fViewZ = (*D3DXVec3TransformCoord(&vWorldPos, &vWorldPos, &ViewMatrix)).z;

	return S_OK;
}

HRESULT CUI::Compute_ViewZ(const _wstring& strTransformComTag)
{
	CTransform* pTransformCom = dynamic_cast<CTransform*>(Find_Component(strTransformComTag));
	if (nullptr == pTransformCom)
		return E_FAIL;

	_float3			vWorldPos = pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float4x4		ViewMatrix{};

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	m_fViewZ = (*D3DXVec3TransformCoord(&vWorldPos, &vWorldPos, &ViewMatrix)).z;

	return S_OK;
}


void CUI::Free()
{
	__super::Free();
}
