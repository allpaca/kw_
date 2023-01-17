#include <stdio.h>

#include "kw_.h"
#pragma comment(lib, "kw_.lib")

void OnEventConnect(long errCode);
void OnReceiveTrCondition(PCSTR sScrNo,
	PCSTR strCodeList, PCSTR strConditionName, int nIndex, int nNext);
void OnReceiveConditionVer(long lRet, PCSTR sMsg);
void OnReceiveRealCondition(PCSTR sTrCode, PCSTR strType, PCSTR strConditionName, 
	PCSTR strConditionIndex);

int example_sendCondition() {
	kw_Initialize(0);

	// �̺�Ʈ �ڵ鷯 ����
	kw_SetOnEventConnect(OnEventConnect);
	kw_SetOnReceiveTrCondition(OnReceiveTrCondition);
	kw_SetOnReceiveConditionVer(OnReceiveConditionVer);
	kw_SetOnReceiveRealCondition(OnReceiveRealCondition);

	// �α���
	kw_CommConnect();

	kw_Wait();

	kw_Uninitialize();
	return 0;
}

void OnEventConnect(long errCode) {
	if (errCode == 0) {
		puts("�����");

		// ���ǰ˻�
		// SendConditon ȣ������ �ݵ�� GetConditionLoad�� ���� 1ȸ ȣ��Ǿ� �� ��.
		long ret = kw_GetConditionLoad();
		printf("kw_GetConditionLoad=%d\n", ret);

		// ���ǰ˻��� ��� ��ȸ
		char* names = kw_GetConditionNameList();
		printf("ConditionNameList=%s\n", names);
		kw_FreeString(names);
	}
	else {
		puts("���� �����");

	}
}


void OnReceiveTrCondition(PCSTR sScrNo,
	PCSTR strCodeList, PCSTR strConditionName, int nIndex, int nNext) {
	printf("OnReceiveTrCondition) scrNo=%s, conditionName=%s, index=%d, codelist=%s\n", 
		sScrNo, strConditionName, nIndex, strCodeList);
}

void OnReceiveConditionVer(long lRet, PCSTR sMsg) {
	printf("OnReceiveConditionVer) %s\n", sMsg);

	// ȭ���ȣ
	char* strScrNo = "testScr1";
	// ���ǽ� �̸� (GetConditionNameList����)
	char* strConditionName = "���ǽ�1";
	// ���ǽ� �ε���(GetConditionNameList����)
	int nIndex = 5;
	// ��ȸ����(0:�Ϲ���ȸ, 1:�ǽð���ȸ, 2:������ȸ)
	int nSearch = 1; 	
	long ret = kw_SendCondition(strScrNo, strConditionName, nIndex, nSearch);
	printf("SendCondition=%d\n", ret);
}

void OnReceiveRealCondition(PCSTR sTrCode, PCSTR strType, PCSTR strConditionName,
	PCSTR strConditionIndex) {
	printf("OnReceiveRealCondition) trCode=%s, type=%s, conditionName=%s, conditionIndex=%s\n", 
		sTrCode, strType, strConditionName, strConditionIndex);
}