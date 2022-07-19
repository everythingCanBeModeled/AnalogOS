/**
 * @Author: GWL
 * @Date:   2022-07-03 09:29:12
 * @Last Modified by:   GWL
 * @Last Modified time: 2022-07-03 21:04:56
 */

#include "myLib.h"

int main()
{
	// ����100MB�ڴ�
	void *space = calloc(1, SPACE_SIZE);
	if (!space)
	{
		printf("�ڴ�����ʧ�ܣ�\n");
		system("pause");
		exit(1);
	}
	printf("\n%s �ڴ�����ɹ�����ַΪ��%p\n", bytesToStr(SPACE_SIZE), space);

	// ���û���㷨�����п�����
	int maxBlock = 1;
	while ((1 << maxBlock) <= SPACE_SIZE)
	{
		maxBlock++;
	}
	// printf("����Ϊ %s\n", bytesToStr(1 << maxBlock - 1));
	SPACE_NODE **freeSpaceArray = (SPACE_NODE **)calloc(maxBlock, sizeof(SPACE_NODE *));
	for (int i = 0; i < maxBlock; i++)
	{
		freeSpaceArray[i] = (SPACE_NODE *)calloc(1, sizeof(SPACE_NODE));
	}
	// ��ʹ�ÿռ��ʣ��ռ�
	long long usedSpace = 0, freeSpace = SPACE_SIZE;
	// checkSpace(usedSpace, freeSpace);

	// ��¼�����ڴ���ַ�ʹ�С
	FREE_SPACE_NODE *freeSpaceListHead = (FREE_SPACE_NODE *)calloc(1, sizeof(FREE_SPACE_NODE));

	// ���ռ�װ����������(β�巨)
	if (1)
	{
		FREE_SPACE_NODE *tail = freeSpaceListHead;
		int i = maxBlock - 1;
		while (freeSpace)
		{
			for (; i >= 0; i--)
			{
				if (freeSpace >= (1 << i))
				{
					// printf("%s\n", bytesToStr(1 << i));
					SPACE_NODE *t = (SPACE_NODE *)calloc(1, sizeof(SPACE_NODE));
					t->pointer = space + usedSpace;
					t->next = freeSpaceArray[i]->next;
					freeSpaceArray[i]->next = t;
					FREE_SPACE_NODE *p = (FREE_SPACE_NODE *)calloc(1, sizeof(FREE_SPACE_NODE));
					p->pointer = space + usedSpace;
					p->size = (1 << i);
					p->next = freeSpaceListHead;
					p->pre = tail;
					p->next->pre = p;
					p->pre->next = p;
					tail = p;
					freeSpace -= (1 << i);
					usedSpace += (1 << i);
					break;
				}
			}
		}
	}
	// checkFreeSpaceList(freeSpaceListHead);
	usedSpace = 0, freeSpace = SPACE_SIZE;
	// �ռ�ʹ�����
	char *blockStatus = (char *)calloc(SPACE_SIZE, sizeof(char));
	memset(blockStatus, '0', SPACE_SIZE * sizeof(char));

	// ��¼��ʹ�õ�ָ��ָ����ֽ���
	long long *usedHash = (long long *)calloc(SPACE_SIZE, sizeof(long long));

	char *s = (char *)calloc(STR_SIZE, sizeof(char));
	while (1)
	{
		printf("\n���������");
		gets(s);
		if (strcmp(s, "exit()") == 0)
		{
			break;
		}
		eval(s, freeSpaceArray, maxBlock, &usedSpace, &freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	}

	//�ͷ�֮ǰ����Ŀռ�
	for (int i = 0; i < maxBlock; i++)
	{
		while (freeSpaceArray[i]->next)
		{
			SPACE_NODE *t = freeSpaceArray[i]->next;
			freeSpaceArray[i]->next = t->next;
			free(t);
		}
		free(freeSpaceArray[i]);
	}
	free(freeSpaceArray);
	free(blockStatus);
	free(usedHash);
	while (freeSpaceListHead->next != freeSpaceListHead)
	{
		FREE_SPACE_NODE *node = freeSpaceListHead->next;
		freeSpaceListHead->next = node->next;
		free(node);
	}
	free(freeSpaceListHead);
	free(space);
	space = NULL;
	system("pause");
}