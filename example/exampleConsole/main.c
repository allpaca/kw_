#include <stdio.h>
#include "example_TrRealRecv.h"
#include "example_sendCondition.h"

int main()
{
	int input = 0;
	puts("=========================");
	puts("1. �ü� ��ȸ ���� ����");
	puts("2. ���ǰ˻� ���� ����");
	puts("=========================");
	puts("��ȣ�� �Է����ּ���!");
	scanf("\n%d", &input);

	switch (input)
	{
	case 1:	return example_trRealRecv();
	case 2:	return example_sendCondition();
	}

	return 0;
}
