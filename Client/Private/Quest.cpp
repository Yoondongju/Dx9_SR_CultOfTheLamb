#include "Quest.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CQuest)


CQuest::CQuest()
{
	Add_Quset(L"��Ÿ��� ��ȭ�ϱ�");

	Add_Quset(L"Ÿ�� ī�� �����ϱ�");

	Add_Quset(L"���ÿ� �����");

	Add_Quset(L"����ý��� �����ϱ�");

	Add_Quset(L"������ ��ĳ��");

	Add_Quset(L"�ŵ� �� ��Ű��");



	Add_Quset(L"����1 �����ϱ�");

	Add_Quset(L"���� �ŵ��� �����ϱ�");


	Add_Quset(L"����: ��ȸ ����");


	Add_Quset(L"����2 ���� óġ�ϱ�");


	Add_Quset(L"����: UpGrade��������");


	Add_Quset(L"��� �ŵ� �⵵��Ű��");


	Add_Quset(L"��ٸ��� �� óġ");


	Add_Quset(L"�γ��� ������ óġ");



	AssignCurQuest(L"��Ÿ��� ��ȭ�ϱ�", L"������ ��");
}

HRESULT CQuest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQuest::Initialize()
{
	return S_OK;
}

void CQuest::Update(_float fTimeDelta)
{



}

void CQuest::Free()
{
	__super::Free();

}
