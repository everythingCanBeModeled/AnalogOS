/**
 * @Author: GWL
 * @Date:   2022-07-03 09:29:12
 * @Last Modified by:   GWL
 * @Last Modified time: 2022-07-03 22:16:30
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPACE_SIZE 100 * 1024 * 1024
#define STR_SIZE 100
typedef _Bool bool;
typedef long long ELEMENT_TYPE;

// �������ڵ�
typedef struct spaceNode
{
	void *pointer;
	struct spaceNode *next;
} SPACE_NODE;

// ��������ڵ�
typedef struct usedSpaceNode
{
	void *pointer;
	long long size;
	struct usedSpaceNode *pre;
	struct usedSpaceNode *next;
} FREE_SPACE_NODE;

// ����ڵ�
typedef struct listNode
{
	ELEMENT_TYPE val;
	struct listNode *next;
} LIST_NODE;

// �����
typedef struct
{
	ELEMENT_TYPE *nums;
	long long size, maxSize;
} HEAP;

// ջ
typedef struct
{
	LIST_NODE *head;
	long long size;
} STACK;

// ����
typedef struct
{
	LIST_NODE *head, *tail;
	long long size;
} QUEUE;

// ���ڵ�
typedef struct treeNode
{
	ELEMENT_TYPE val;
	struct treeNode *left, *right, *parent;
} TREE_NODE;

// ��
typedef struct
{
	TREE_NODE *root;
	long long size;
} TREE;

// ��
typedef struct edgeNode
{
	ELEMENT_TYPE val;
	LIST_NODE *listHead;
	struct edgeNode *next;
} EDGE_NODE;

// ͼ
typedef struct
{
	// �洢����
	LIST_NODE *nodeHead;
	// �洢��
	EDGE_NODE *edgeHead;
	long long edgeSize, nodeSize;
} MAP;

// �жϵ�ַ�Ƿ�Ϸ�
bool addressIsValid(void *address, void *space)
{
	if (address < space || address >= space + SPACE_SIZE)
	{
		printf("%p ������ַ��Χ��\n", address);
		printf("��ȷ��ַ��ΧΪ��[%p, %p]\n", space, space + SPACE_SIZE - 1);
		return 0;
	}
	return 1;
}

// �ֽ���תΪ����λ���ַ���
char *bytesToStr(long long size)
{
	char *s = (char *)calloc(100, sizeof(char));
	if (size == 0)
	{
		sprintf(s, "%7.2f MB", size);
		return s;
	}
	char unit[][3] = {" B", "KB", "MB", "GB", "TB", "PB", "EB"};
	int i = 0;
	while ((long long)pow(1024, i) <= size)
	{
		i++;
	}
	i--;
	float num = size / pow(1024, i);
	sprintf(s, "%7.2f %s", num, unit[i]);
	return s;
}

// �鿴�ռ�����ʹ�����
void checkSpace(int usedSpace, int freeSpace)
{
	// printf("\n");
	printf("��ʹ�ÿռ䣺%s\n", bytesToStr(usedSpace));
	printf("  ʣ��ռ䣺%s\n", bytesToStr(freeSpace));
	printf("    �ܿռ䣺%s\n", bytesToStr(usedSpace + freeSpace));
}

// �鿴�ڴ��ʹ�����
void checkBlock(void *space, char *blockStatus, void *a)
{
	// printf("\n");
	if (blockStatus[a - space] == '0')
	{
		printf("%p δʹ�ã�\n", a);
	}
	else
	{
		printf("%p ��ʹ�ã�\n", a);
	}
}

// �鿴���п�����
void checkFreeSpaceList(FREE_SPACE_NODE *freeSpaceListHead)
{
	printf("���п�����\n");
	FREE_SPACE_NODE *t = freeSpaceListHead->next;
	while (t != freeSpaceListHead)
	{
		printf("%s��%p\n", bytesToStr(t->size), t->pointer);
		t = t->next;
	}
}

// �鿴�������
void checkFreeSpaceArray(SPACE_NODE **freeSpaceArray, int maxBlock)
{
	printf("\n������飺\n");
	for (int i = maxBlock - 1; i >= 0; i--)
	{
		SPACE_NODE *t = freeSpaceArray[i]->next;
		while (t)
		{
			printf("%s��%p\n", bytesToStr(1 << i), t->pointer);
			t = t->next;
		}
	}
}

int countBit(long long size)
{
	int bit = 0;
	while ((1 << bit) < size)
	{
		bit++;
	}
	return bit;
}

// Ѱ��Ŀ��ڵ��ǰһ��
SPACE_NODE *findPreNodeInArray(void *pointer, SPACE_NODE **freeSpaceArray, long long size)
{
	int bit = countBit(size);
	SPACE_NODE *t = freeSpaceArray[bit];
	while (t->next && t->next->pointer != pointer)
	{
		t = t->next;
	}
	return t;
}

void myFree(void *pointer, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	long long size = usedHash[pointer - space];
	if (!size)
	{
		printf("��ַδʹ�ã������ͷţ�\n");
		return;
	}
	usedHash[pointer - space] = 0;
	*usedSpace -= size;
	*freeSpace += size;
	memset(&blockStatus[pointer - space], '0', sizeof(char) * size);
	memset(pointer, 0, size);
	// �ڿ�����������½ڵ�
	FREE_SPACE_NODE *nowNode = (FREE_SPACE_NODE *)calloc(1, sizeof(FREE_SPACE_NODE));
	nowNode->pointer = pointer;
	nowNode->size = size;
	FREE_SPACE_NODE *t = freeSpaceListHead;
	while (t->next->pointer < nowNode->pointer + size && t->next != freeSpaceListHead)
	{
		t = t->next;
	}
	nowNode->next = t->next;
	nowNode->pre = t;
	nowNode->next->pre = nowNode->pre->next = nowNode;
	// ���ܺϲ���ȫ�ϲ��������»���
	// �ϲ�Ҫ����һ������ͷ����ַ����
	// �Һϲ�
	while (nowNode->next != freeSpaceListHead && nowNode->pointer + nowNode->size == nowNode->next->pointer)
	{
		FREE_SPACE_NODE *temNowNode = nowNode->next;
		// �ϲ�
		nowNode->size += temNowNode->size;
		// �ڻ��������ɾ��
		int bit = countBit(temNowNode->size);
		SPACE_NODE *preNodeInArray = findPreNodeInArray(temNowNode->pointer, freeSpaceArray, temNowNode->size);
		SPACE_NODE *nowNodeInArray = preNodeInArray->next;
		preNodeInArray->next = preNodeInArray->next->next;
		free(nowNodeInArray);
		// �ڿ���������ɾ��
		temNowNode->next->pre = temNowNode->pre;
		temNowNode->pre->next = temNowNode->next;
		free(temNowNode);
	}
	// ��ϲ�
	while (nowNode->pre != freeSpaceListHead && nowNode->pointer == nowNode->pre->pointer + nowNode->pre->size)
	{
		FREE_SPACE_NODE *temNowNode = nowNode->pre;
		// �ϲ�
		nowNode->size += temNowNode->size;
		nowNode->pointer -= temNowNode->size;
		// �ڻ��������ɾ��
		int bit = countBit(temNowNode->size);
		SPACE_NODE *preNodeInArray = findPreNodeInArray(temNowNode->pointer, freeSpaceArray, temNowNode->size);
		SPACE_NODE *nowNodeInArray = preNodeInArray->next;
		preNodeInArray->next = preNodeInArray->next->next;
		free(nowNodeInArray);
		// �ڿ���������ɾ��
		temNowNode->next->pre = temNowNode->pre;
		temNowNode->pre->next = temNowNode->next;
		free(temNowNode);
	}
	// ���
	int i = maxBlock - 1;
	// resSize = temNowNode->size;
	FREE_SPACE_NODE *tail = nowNode->pre;
	// ��nowNode�ӿ���������ȡ����
	nowNode->pre->next = nowNode->next;
	nowNode->next->pre = nowNode->pre;
	while (nowNode->size)
	{
		for (; i >= 0; i--)
		{
			if (nowNode->size >= (1 << i))
			{
				FREE_SPACE_NODE *listNode = (FREE_SPACE_NODE *)calloc(1, sizeof(FREE_SPACE_NODE));
				listNode->pointer = nowNode->pointer;
				listNode->size = (1 << i);
				listNode->next = tail->next;
				listNode->pre = tail;
				listNode->next->pre = listNode;
				listNode->pre->next = listNode;
				tail = listNode;
				SPACE_NODE *arrNode = (SPACE_NODE *)calloc(1, sizeof(SPACE_NODE));
				arrNode->pointer = nowNode->pointer;
				arrNode->next = freeSpaceArray[i]->next;
				freeSpaceArray[i]->next = arrNode;
				nowNode->pointer += (1 << i);
				nowNode->size -= (1 << i);
				break;
			}
		}
	}
	free(nowNode);
	printf("%p �ͷųɹ���\n", pointer);
}

void *myMalloc(long long size, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (size <= 0)
	{
		printf("�ڴ��С���Ϸ���\n");
		return NULL;
	}
	// printf("\n");
	if (size > *freeSpace)
	{
		printf("ʣ��ռ䲻�㣡\n");
		printf("������Ŀռ��СΪ��%s�����ռ��ʣ��%s\n", bytesToStr(size), bytesToStr(*freeSpace));
		return NULL;
	}

	// ���������������С��
	int blockBit = countBit(size);
	// printf("����Ҫ�����С��Ϊ��%s\n", bytesToStr(1 << blockBit));
	bool flag = 0;
	SPACE_NODE *t = NULL;
	void *ans = NULL;
	for (int i = blockBit; i < maxBlock; i++)
	{
		if (freeSpaceArray[i]->next)
		{
			t = freeSpaceArray[i]->next;
			// ��t�ӻ������ȡ��
			freeSpaceArray[i]->next = t->next;
			flag = 1;
			*usedSpace += size, *freeSpace -= size;
			memset(&blockStatus[t->pointer - space], '1', size);
			ans = t->pointer;
			usedHash[ans - space] = size;

			FREE_SPACE_NODE *nowNode = freeSpaceListHead->next;
			while (nowNode->pointer != t->pointer)
			{
				nowNode = nowNode->next;
			}

			// ����ʣ�µ��ǲ���
			if (size < (1 << i))
			{
				long long resSize = (1 << i) - size;
				nowNode->pointer = ans + size;
				nowNode->size = resSize;
				// ���ܺϲ���ȫ�ϲ��������»���
				// �ϲ�Ҫ����һ������ͷ����ַ����
				while (nowNode->next != freeSpaceListHead && nowNode->pointer + nowNode->size == nowNode->next->pointer)
				{
					FREE_SPACE_NODE *temNowNode = nowNode->next;
					// �ϲ�
					nowNode->size += temNowNode->size;
					// �ڻ��������ɾ��
					int bit = countBit(temNowNode->size);
					SPACE_NODE *preNodeInArray = findPreNodeInArray(temNowNode->pointer, freeSpaceArray, temNowNode->size);
					SPACE_NODE *nowNodeInArray = preNodeInArray->next;
					preNodeInArray->next = preNodeInArray->next->next;
					free(nowNodeInArray);
					// �ڿ���������ɾ��
					temNowNode->next->pre = temNowNode->pre;
					temNowNode->pre->next = temNowNode->next;
					free(temNowNode);
				}
				// ���
				int i = maxBlock - 1;
				// resSize = temNowNode->size;
				FREE_SPACE_NODE *tail = nowNode->pre;
				// ��nowNode�ӿ���������ȡ����
				nowNode->pre->next = nowNode->next;
				nowNode->next->pre = nowNode->pre;
				while (nowNode->size)
				{
					for (; i >= 0; i--)
					{
						if (nowNode->size >= (1 << i))
						{
							FREE_SPACE_NODE *listNode = (FREE_SPACE_NODE *)calloc(1, sizeof(FREE_SPACE_NODE));
							listNode->pointer = nowNode->pointer;
							listNode->size = (1 << i);
							listNode->next = tail->next;
							listNode->pre = tail;
							listNode->next->pre = listNode;
							listNode->pre->next = listNode;
							tail = listNode;
							SPACE_NODE *arrNode = (SPACE_NODE *)calloc(1, sizeof(SPACE_NODE));
							arrNode->pointer = nowNode->pointer;
							arrNode->next = freeSpaceArray[i]->next;
							freeSpaceArray[i]->next = arrNode;
							nowNode->pointer += (1 << i);
							nowNode->size -= (1 << i);
							break;
						}
					}
				}
				free(nowNode);
			}
			else
			{
				nowNode->next->pre = nowNode->pre;
				nowNode->pre->next = nowNode->next;
				free(nowNode);
				free(t);
			}
			break;
		}
	}
	if (flag)
	{
		printf("%s �ڴ�����ɹ�����ַΪ��%p\n", bytesToStr(size), t->pointer);
		return ans;
	}
	else
	{
		printf("û������Ҫ����ڴ�飬�ռ����ʧ�ܣ�\n");
		return NULL;
	}
}

void *myCalloc(long long num, long long size, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	void *ans = myMalloc(num * size, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	if (ans)
	{
		memset(ans, 0, num * size);
	}
	return ans;
}

// --------------------����ʼ--------------------
// �����յ�����
LIST_NODE *initList(SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	LIST_NODE *head = (LIST_NODE *)myCalloc(1, sizeof(LIST_NODE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	if (head)
	{
		printf("�������ɹ�����ַΪ��%p\n", head);
	}
	else
	{
		printf("������ʧ�ܣ�\n");
	}
	return head;
}

// ��������
void delList(LIST_NODE *head, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)head - space])
	{
		printf("�����������ڣ�\n");
		return;
	}
	while (head->next)
	{
		LIST_NODE *t = head->next;
		// printf("%lld", usedHash[(void *)t - space]);
		head->next = t->next;
		myFree(t, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	}
	myFree(head, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	printf("����ɾ���ɹ���\n");
}

// �ж������Ƿ�Ϊ��
bool listEmpty(LIST_NODE *head, void *space, long long *usedHash)
{
	if (!usedHash[(void *)head - space])
	{
		printf("�������ڣ�\n");
		return 0;
	}
	if (head->next)
	{
		printf("����ǿգ�\n");
	}
	else
	{
		printf("����Ϊ�գ�\n");
	}
	return head->next != NULL;
}

// ��������
long long listLength(LIST_NODE *head, void *space, long long *usedHash)
{
	if (!usedHash[(void *)head - space])
	{
		printf("�����������ڣ�\n");
		return -1;
	}
	long long ans = 0;
	LIST_NODE *t = head->next;
	while (t)
	{
		ans++;
		t = t->next;
	}
	printf("������Ϊ%lld\n", ans);
	return ans;
}

// ��ȡ����ڵ�Ԫ��
int getListNodeVal(LIST_NODE *node, void *space, long long *usedHash)
{
	if (!usedHash[(void *)node - space])
	{
		printf("��ȡʧ�ܣ��ýڵ�Ϊ�գ�\n");
		return 0;
	}
	printf("�ڵ�Ԫ��Ϊ��%lld\n", node->val);
	return node->val;
}

int compare(ELEMENT_TYPE a, ELEMENT_TYPE b)
{
	return a - b;
}

// ��������ĳԪ�ص�λ�򣬲����ڷ���0
long long findListNode(LIST_NODE *head, ELEMENT_TYPE val, void *space, long long *usedHash)
{
	if (!usedHash[(void *)head - space])
	{
		printf("�����������ڣ�\n");
		return -1;
	}
	LIST_NODE *t = head->next;
	for (long long i = 1; t; i++, t = t->next)
	{
		if (compare(val, t->val) == 0)
		{
			printf("Ԫ��λ������ĵ�%lldλ\n", i);
			return i;
		}
	}
	printf("Ԫ�ز������������У�\n");
	return 0;
}

// ��������ǰ��
void listPreNode(LIST_NODE *head, ELEMENT_TYPE val, void *space, long long *usedHash)
{
	if (!usedHash[(void *)head - space])
	{
		printf("�����������ڣ�\n");
		return;
	}
	LIST_NODE *t = head;
	for (long long i = 1; t->next; i++, t = t->next)
	{
		if (compare(val, t->next->val) == 0)
		{
			if (t == head)
			{
				printf("Ԫ��λ�������1λ����ǰ��\n");
				return;
			}
			printf("Ԫ�ص�ǰ����ַΪ��%p��ֵΪ��%lld\n", t, t->val);
			return;
		}
	}
	printf("Ԫ�ز������������У�\n");
	return;
}

// ��������Ԫ�غ��
void listNextNode(LIST_NODE *head, ELEMENT_TYPE val, void *space, long long *usedHash)
{
	if (!usedHash[(void *)head - space])
	{
		printf("�����������ڣ�\n");
		return;
	}
	LIST_NODE *t = head->next;
	for (long long i = 1; t; i++, t = t->next)
	{
		if (compare(val, t->val) == 0)
		{
			if (t->next == NULL)
			{
				printf("Ԫ��λ���������һλ���޺��\n");
				return;
			}
			printf("Ԫ�صĺ�̵�ַΪ��%p��ֵΪ��%lld\n", t->next, t->next->val);
			return;
		}
	}
	printf("Ԫ�ز������������У�\n");
	return;
}

// ��������
void visitList(LIST_NODE *head, void *space, long long *usedHash)
{
	if (!usedHash[(void *)head - space])
	{
		printf("�����������ڣ�\n");
		return;
	}
	LIST_NODE *t = head->next;
	if (!t)
	{
		printf("����Ϊ�գ�\n");
		return;
	}
	printf("��������\n");
	for (long long i = 1; t; i++, t = t->next)
	{
		printf("��%lld��Ԫ��ֵΪ��%lld����ַΪ%p\n", i, t->val, t);
	}
	printf("�������������\n");
}

// �������ÿ�
void clearList(LIST_NODE *head, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)head - space])
	{
		printf("�����������ڣ�\n");
		return;
	}
	while (head->next)
	{
		LIST_NODE *t = head->next;
		// printf("%lld", usedHash[(void *)t - space]);
		head->next = t->next;
		myFree(t, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	}
	printf("�ɹ��������ÿգ�\n");
}

// �޸�����Ԫ�ص�ֵ
void changeListVal(LIST_NODE *head, long long pos, ELEMENT_TYPE val, void *space, long long *usedHash)
{
	if (!usedHash[(void *)head - space])
	{
		printf("�����������ڣ�\n");
		return;
	}
	LIST_NODE *t = head->next;
	for (long long i = 1; t; i++, t = t->next)
	{
		if (i == pos)
		{
			t->val = val;
			printf("�ɹ��������%lld��Ԫ�ظ���Ϊ%lld\n", pos, val);
			return;
		}
	}
	printf("����ʧ�ܣ��������ڵ�%lld��Ԫ�أ�\n", pos);
}

// �������i��Ԫ�غ������Ԫ��
void addListNode(LIST_NODE *head, long long pos, ELEMENT_TYPE val, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)head - space])
	{
		printf("�����������ڣ�\n");
		return;
	}
	if (pos < 0)
	{
		printf("����ʧ�ܣ��������ڵ�%lld��Ԫ�أ�\n", pos);
		return;
	}
	LIST_NODE *t = head;
	for (long long i = 0; t; i++, t = t->next)
	{
		if (i == pos)
		{
			LIST_NODE *newNode = (LIST_NODE *)myCalloc(1, sizeof(LIST_NODE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
			newNode->next = t->next;
			t->next = newNode;
			newNode->val = val;
			printf("�ɹ��������%lld��Ԫ�غ������Ԫ��%lld\n", pos, val);
			return;
		}
	}
	printf("����ʧ�ܣ��������ڵ�%lld��Ԫ�أ�\n", pos);
}

// ɾ�������i��Ԫ��
void delListNode(LIST_NODE *head, long long pos, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)head - space])
	{
		printf("�����������ڣ�\n");
		return;
	}
	if (pos <= 0)
	{
		printf("ɾ��ʧ�ܣ��������ڵ�%lld��Ԫ�أ�\n", pos);
		return;
	}
	LIST_NODE *t = head;
	for (long long i = 1; t; i++, t = t->next)
	{
		if (i == pos)
		{
			LIST_NODE *p = t->next;
			t->next = t->next->next;
			myFree(p, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
			printf("�ɹ�ɾ�������%lld��Ԫ��\n", pos);
			return;
		}
	}
	printf("ɾ��ʧ�ܣ��������ڵ�%lld��Ԫ�أ�\n", pos);
}

// --------------------�������--------------------

// --------------------���鿪ʼ--------------------

// ��������
ELEMENT_TYPE *initArray(long long n, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	ELEMENT_TYPE *arr = (ELEMENT_TYPE *)myCalloc(n, sizeof(ELEMENT_TYPE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	if (arr)
	{
		printf("���鴴���ɹ�����ַΪ��%p\n", arr);
	}
	else
	{
		printf("�������ʧ�ܣ�\n");
	}
	return arr;
}

// ɾ������
void delArray(ELEMENT_TYPE *arr, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)arr - space])
	{
		printf("ɾ��ʧ�ܣ����鲻���ڣ�\n");
	}
	myFree(arr, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	printf("����ɾ���ɹ���\n");
}

// ��ȡ����ָ��λ�õ�ֵ
ELEMENT_TYPE getArrayVal(ELEMENT_TYPE *arr, long long pos, void *space, long long *usedHash)
{
	if (!usedHash[(void *)arr - space])
	{
		printf("���鲻���ڣ�\n");
		return -1;
	}
	if (pos < 0 || pos >= usedHash[(void *)arr - space] / sizeof(ELEMENT_TYPE))
	{
		printf("����Խ�磡\n");
		return -1;
	}
	printf("�����%lldλԪ��Ϊ��%lld\n", pos, arr[pos]);
	return arr[pos];
}

// �޸�����ָ��λ�õ�ֵ
void changeArrayVal(ELEMENT_TYPE *arr, long long pos, ELEMENT_TYPE val, void *space, long long *usedHash)
{
	if (!usedHash[(void *)arr - space])
	{
		printf("���鲻���ڣ�\n");
		return;
	}
	if (pos < 0 || pos >= usedHash[(void *)arr - space] / sizeof(ELEMENT_TYPE))
	{
		printf("����Խ�磡\n");
		return;
	}
	arr[pos] = val;
	printf("�ɹ��������%lldλԪ���޸�Ϊ%lld\n", pos, arr[pos]);
}

// ��������
void visitArray(ELEMENT_TYPE *arr, void *space, long long *usedHash)
{
	if (!usedHash[(void *)arr - space])
	{
		printf("���鲻���ڣ�\n");
		return;
	}
	long long size = usedHash[(void *)arr - space] / sizeof(ELEMENT_TYPE);
	printf("����Ԫ��Ϊ��\n");
	printf("%lld", arr[0]);
	for (long long i = 1; i < size; i++)
	{
		printf(", %lld", arr[i]);
	}
	printf("\n����������\n");
}

// --------------------�������--------------------

// --------------------�ѿ�ʼ--------------------

// �����ն�(�����)
HEAP *initHeap(long long maxSize, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	HEAP *heap = (HEAP *)myCalloc(1, sizeof(HEAP), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	if (!heap)
	{
		printf("�Ѵ���ʧ�ܣ�\n");
		return NULL;
	}
	heap->maxSize = maxSize;
	heap->nums = (ELEMENT_TYPE *)myCalloc(1, sizeof(ELEMENT_TYPE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	if (!heap->nums)
	{
		myFree(heap, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		printf("�Ѵ���ʧ�ܣ�\n");
		return NULL;
	}
	printf("�Ѵ����ɹ�����ַΪ��%p\n", heap);
	return heap;
}

// ɾ����
void delHeap(HEAP *heap, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)heap - space])
	{
		printf("�Ѳ����ڣ�\n");
		return;
	}
	myFree(heap->nums, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	myFree(heap, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	printf("��ɾ���ɹ���\n");
}

// ��ն�
void clearHeap(HEAP *heap, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)heap - space])
	{
		printf("�Ѳ����ڣ�\n");
		return;
	}
	heap->size = 0;
	printf("����ճɹ���\n");
}

void swap(ELEMENT_TYPE *a, ELEMENT_TYPE *b)
{
	ELEMENT_TYPE t = *a;
	*a = *b;
	*b = t;
}

// �����
int adjust(ELEMENT_TYPE *nums, int x, int n)
{
	int pos = x + 1;
	int left = 2 * pos - 1, right = 2 * pos + 1 - 1;
	// ��������������
	if (right < n)
	{
		if (nums[left] >= nums[x] && nums[left] >= nums[right])
		{
			swap(&nums[left], &nums[x]);
			return -1;
		}
		else if (nums[right] >= nums[x] && nums[right] >= nums[left])
		{
			swap(&nums[right], &nums[x]);
			return 1;
		}
	}
	else if (left < n && right >= n)
	{
		// ֻ��������
		if (nums[left] > nums[x])
		{
			swap(&nums[left], &nums[x]);
			return -1;
		}
	}
	return 0;
}

void adjustHeap(ELEMENT_TYPE *nums, int x, int n)
{
	int pos = x + 1;
	int flag = adjust(nums, pos - 1, n);
	while (flag)
	{
		pos *= 2;
		if (flag == -1)
		{
			flag = adjust(nums, pos - 1, n);
		}
		else
		{
			pos++;
			flag = adjust(nums, pos - 1, n);
		}
	}
}

void adjustAllHeap(ELEMENT_TYPE *nums, int n)
{
	for (int i = n / 2; i >= 0; i--)
	{
		adjustHeap(nums, i, n);
	}
}

// �����
void heapPush(HEAP *heap, ELEMENT_TYPE val, void *space, long long *usedHash)
{
	if (!usedHash[(void *)heap - space])
	{
		printf("�Ѳ����ڣ�\n");
		return;
	}
	if (heap->size >= heap->maxSize)
	{
		printf("���������޷���ӣ�\n");
		return;
	}
	heap->nums[heap->size++] = val;
	adjustAllHeap(heap->nums, heap->size);
	printf("��ѳɹ���\n");
}

// ����Ѷ�Ԫ��
void heapPop(HEAP *heap, void *space, long long *usedHash)
{
	if (!usedHash[(void *)heap - space])
	{
		printf("�Ѳ����ڣ�\n");
		return;
	}
	if (!heap->size)
	{
		printf("��Ϊ�գ�\n");
		return;
	}
	printf("�Ѷ�Ԫ��Ϊ��%lld\n", heap->nums[0]);
	heap->nums[0] = heap->nums[--heap->size];
	adjustAllHeap(heap->nums, heap->size);
}

// ��ȡ�Ѷ�Ԫ��
void getHeapTop(HEAP *heap, void *space, long long *usedHash)
{
	if (!usedHash[(void *)heap - space])
	{
		printf("�Ѳ����ڣ�\n");
		return;
	}
	if (!heap->size)
	{
		printf("��Ϊ�գ�\n");
		return;
	}
	printf("�Ѷ�Ԫ��Ϊ��%lld\n", heap->nums[0]);
}

// ��ȡ�ѵĴ�С
void getHeapSize(HEAP *heap, void *space, long long *usedHash)
{
	if (!usedHash[(void *)heap - space])
	{
		printf("�Ѳ����ڣ�\n");
		return;
	}
	printf("������%lld��Ԫ�أ�\n", heap->size);
}

// �����
void outputHeap(HEAP *heap, void *space, long long *usedHash)
{
	if (!usedHash[(void *)heap - space])
	{
		printf("�Ѳ����ڣ�\n");
		return;
	}
	if (heap->size == 0)
	{
		printf("��Ϊ�գ�\n");
		return;
	}
	printf("����Ԫ������Ϊ��\n");
	printf("%lld", heap->nums[0]);
	for (long long i = 1; i < heap->size; i++)
	{
		printf(", %lld", heap->nums[i]);
	}
	printf("\n");
}

// --------------------�ѽ���--------------------

// --------------------ջ��ʼ--------------------

// ����ջ
STACK *initStack(SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	STACK *stack = (STACK *)myCalloc(1, sizeof(STACK), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	if (!stack)
	{
		printf("ջ����ʧ�ܣ�\n");
		return NULL;
	}
	stack->head = (LIST_NODE *)myCalloc(1, sizeof(LIST_NODE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	if (!stack->head)
	{
		myFree(stack, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		printf("ջ����ʧ�ܣ�\n");
		return NULL;
	}
	stack->size = 0;
	printf("ջ�����ɹ�����ַΪ��%p\n", stack);
	return stack;
}

// ɾ��ջ
void *delStack(STACK *stack, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)stack - space])
	{
		printf("ջ�����ڣ�\n");
	}
	while (stack->size)
	{
		LIST_NODE *t = stack->head->next;
		stack->head->next = t->next;
		myFree(t, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		stack->size--;
	}
	myFree(stack->head, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	myFree(stack, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);

	printf("ջɾ���ɹ���\n");
}

// �ÿ�ջ
void *clearStack(STACK *stack, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)stack - space])
	{
		printf("ջ�����ڣ�\n");
	}
	while (stack->size)
	{
		LIST_NODE *t = stack->head->next;
		stack->head->next = t->next;
		myFree(t, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		stack->size--;
	}

	printf("ջ�ÿճɹ���\n");
}

// ��ȡջ��Ԫ��
ELEMENT_TYPE getStackTopVal(STACK *stack, void *space, long long *usedHash)
{
	if (!usedHash[(void *)stack - space])
	{
		printf("ջ�����ڣ�\n");
		return 0;
	}
	if (stack->size == 0)
	{
		printf("ջΪ�գ�\n");
		return 0;
	}
	printf("ջ��Ԫ��Ϊ��%lld\n", stack->head->next->val);
	return stack->head->next->val;
}

// ��ȡջ�Ĵ�С
long long getStackSize(STACK *stack, void *space, long long *usedHash)
{
	if (!usedHash[(void *)stack - space])
	{
		printf("ջ�����ڣ�\n");
		return -1;
	}
	printf("ջ����%lld��Ԫ��\n", stack->size);
	return stack->size;
}

// ��ջ
void stackPush(STACK *stack, ELEMENT_TYPE val, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)stack - space])
	{
		printf("ջ�����ڣ�\n");
		return;
	}
	LIST_NODE *node = (LIST_NODE *)myCalloc(1, sizeof(LIST_NODE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	if (!node)
	{
		printf("�ռ�����ʧ�ܣ�\n");
		return;
	}
	node->val = val;
	node->next = stack->head->next;
	stack->head->next = node;
	stack->size++;
	printf("��ջ�ɹ���\n");
}

// ��ջ
void stackPop(STACK *stack, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)stack - space])
	{
		printf("ջ�����ڣ�\n");
		return;
	}
	if (stack->size == 0)
	{
		printf("ջΪ�գ�\n");
		return;
	}
	LIST_NODE *node = stack->head->next;
	stack->head->next = node->next;
	stack->size--;
	printf("��ջ�ɹ�����ջԪ��Ϊ%lld\n", node->val);
	myFree(node, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
}

// --------------------ջ����--------------------

// --------------------���п�ʼ--------------------

// ��������
QUEUE *initQueue(SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	QUEUE *queue = (QUEUE *)myCalloc(1, sizeof(QUEUE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	if (!queue)
	{
		printf("���д���ʧ�ܣ�\n");
		return NULL;
	}
	queue->head = myCalloc(1, sizeof(LIST_NODE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	if (!queue->head)
	{
		myFree(queue, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		printf("���д���ʧ�ܣ�\n");
		return NULL;
	}
	queue->tail = queue->head;
	queue->size = 0;
	printf("���д����ɹ�����ַΪ��%p\n", queue);
	return queue;
}

// ɾ������
void delQueue(QUEUE *queue, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)queue - space])
	{
		printf("���в����ڣ�\n");
		return;
	}
	while (queue->size)
	{
		LIST_NODE *node = queue->head->next;
		queue->head->next = node->next;
		myFree(node, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		queue->size--;
	}
	myFree(queue->head, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	myFree(queue, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	printf("����ɾ���ɹ�\n");
}

// �ÿն���
void clearQueue(QUEUE *queue, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)queue - space])
	{
		printf("���в����ڣ�\n");
		return;
	}
	while (queue->size)
	{
		LIST_NODE *node = queue->head->next;
		queue->head->next = node->next;
		myFree(node, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		queue->size--;
	}
	queue->tail = queue->head;
	printf("�����ÿճɹ�\n");
}

// ��ȡ����Ԫ��
ELEMENT_TYPE getQueueHeadVal(QUEUE *queue, void *space, long long *usedHash)
{
	if (!usedHash[(void *)queue - space])
	{
		printf("���в����ڣ�\n");
		return -1;
	}
	if (queue->size == 0)
	{
		printf("����Ϊ�գ�\n");
		return -1;
	}
	printf("����Ԫ��Ϊ��%lld\n", queue->head->next->val);
	return queue->head->next->val;
}

// ��ȡ���еĴ�С
long long getQueueSize(QUEUE *queue, void *space, long long *usedHash)
{
	if (!usedHash[(void *)queue - space])
	{
		printf("���в����ڣ�\n");
		return -1;
	}
	printf("��������%lld��Ԫ��\n", queue->size);
	return queue->size;
}

// �����
void queuePush(QUEUE *queue, ELEMENT_TYPE val, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)queue - space])
	{
		printf("���в����ڣ�\n");
		return;
	}
	LIST_NODE *node = (LIST_NODE *)myCalloc(1, sizeof(LIST_NODE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	if (!node)
	{
		printf("���ʧ�ܣ�\n");
		return;
	}
	node->val = val;
	node->next = queue->tail->next;
	queue->tail->next = node;
	queue->tail = node;
	queue->size++;
	printf("��ӳɹ���\n");
}

// ������
void queuePop(QUEUE *queue, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)queue - space])
	{
		printf("���в����ڣ�\n");
		return;
	}
	if (queue->size == 0)
	{
		printf("����Ϊ�գ�\n");
		return;
	}
	LIST_NODE *node = queue->head->next;
	queue->head->next = node->next;
	queue->size--;
	printf("���ӳɹ���Ԫ��ֵΪ%lld\n", node->val);
	myFree(node, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
}

// --------------------���н���--------------------

// --------------------����ʼ--------------------

// ��������
TREE *initTree(SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	TREE *tree = (TREE *)myCalloc(1, sizeof(TREE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	if (tree)
	{
		printf("�������ɹ�����ַΪ��%p\n", tree);
	}
	else
	{
		printf("������ʧ�ܣ�\n");
	}
	return tree;
}

// �ݹ�ɾ�����ڵ�
void recursiveDelTreeNode(TREE *tree, TREE_NODE *root, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (root)
	{
		TREE_NODE *left = root->left;
		TREE_NODE *right = root->right;
		myFree(root, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		tree->size--;
		recursiveDelTreeNode(tree, left, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		recursiveDelTreeNode(tree, right, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	}
}

// ɾ����
void delTree(TREE *tree, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)tree - space])
	{
		printf("�������ڣ�\n");
		return;
	}
	recursiveDelTreeNode(tree, tree->root, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	myFree(tree, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	printf("��ɾ���ɹ���\n");
}

// �����
void clearTree(TREE *tree, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)tree - space])
	{
		printf("�������ڣ�\n");
		return;
	}
	recursiveDelTreeNode(tree, tree->root, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	tree->size = 0;
	printf("����ճɹ���\n");
}

// �����Ľڵ�����
long long countTreeNode(TREE *tree, void *space, long long *usedHash)
{
	if (!usedHash[(void *)tree - space])
	{
		printf("�������ڣ�\n");
		return -1;
	}
	printf("������%lld���ڵ�\n", tree->size);
	return tree->size;
}

// �ݹ����������
long long recursiveCountTreeDeep(TREE_NODE *root, long long *maxDeep)
{
	long long deep = 0;
	if (root)
	{
		long long left = recursiveCountTreeDeep(root->left, maxDeep);
		long long right = recursiveCountTreeDeep(root->right, maxDeep);
		deep = fmax(left, right) + 1;
		*maxDeep = fmax(*maxDeep, deep);
	}
	return deep;
}

// ���������
long long countTreeDeep(TREE *tree, void *space, long long *usedHash)
{
	if (!usedHash[(void *)tree - space])
	{
		printf("�������ڣ�\n");
		return -1;
	}
	long long maxDeep = 0;
	recursiveCountTreeDeep(tree->root, &maxDeep);
	printf("�������Ϊ��%lld\n", maxDeep);
}

// �������ĸ��ڵ��ַ
TREE_NODE *treeRoot(TREE *tree, void *space, long long *usedHash)
{
	if (!usedHash[(void *)tree - space])
	{
		printf("�������ڣ�\n");
		return NULL;
	}
	printf("���ĸ��ڵ��ַΪ��%p\n", tree->root);
	return tree->root;
}

// ��ȡ���нڵ��ֵ
ELEMENT_TYPE getTreeNodeVal(TREE *tree, TREE_NODE *root, void *space, long long *usedHash)
{
	if (!usedHash[(void *)tree - space])
	{
		printf("�������ڣ�\n");
		return -1;
	}
	if (!usedHash[(void *)root - space])
	{
		printf("���ڵ㲻���ڣ�\n");
		return -1;
	}
	printf("�����ڵ��ֵΪ��%lld\n", root->val);
	return root->val;
}

// �޸����ڵ��ֵ
void changeTreeNodeVal(TREE *tree, TREE_NODE *root, ELEMENT_TYPE val, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)tree - space])
	{
		printf("�������ڣ�\n");
		return;
	}
	if (!usedHash[(void *)root - space])
	{
		printf("���ڵ㲻���ڣ�\n");
		return;
	}
	root->val = val;
	printf("�ɹ��޸ĸ����ڵ��ֵ��\n");
}

// �����ĸ��ڵ㸳ֵ
void addTreeRootVal(TREE *tree, ELEMENT_TYPE val, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)tree - space])
	{
		printf("�������ڣ�\n");
		return;
	}
	if (!tree->root)
	{
		tree->root = myCalloc(1, sizeof(TREE_NODE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		if (!tree->root)
		{
			printf("���ڵ㴴��ʧ�ܣ�\n");
			return;
		}
		else
		{
			printf("���ڵ㴴���ɹ�����ַΪ��%p\n", tree->root);
			tree->size++;
		}
	}
	tree->root->val = val;
	printf("�ɹ������ڵ㸳ֵ��\n");
}

// �������ڵ������
TREE_NODE *getTreeNodeParent(TREE *tree, TREE_NODE *root, void *space, long long *usedHash)
{
	if (!usedHash[(void *)tree - space])
	{
		printf("�������ڣ�\n");
		return NULL;
	}
	if (root == tree->root)
	{
		printf("�ýڵ������ĸ��ڵ㣬�����ȣ�\n");
		return NULL;
	}
	if (!usedHash[(void *)root - space])
	{
		printf("�ýڵ㲻���ڣ�\n");
		return NULL;
	}
	printf("�ýڵ�����ȵ�ַΪ��%p��ֵΪ��%lld\n", root->parent, root->parent->val);
	return root->parent;
}

// �������ڵ�ĺ���
void getTreeNodeChildren(TREE *tree, TREE_NODE *root, void *space, long long *usedHash)
{
	if (!usedHash[(void *)tree - space])
	{
		printf("�������ڣ�\n");
		return;
	}

	if (root->left)
	{
		printf("�ڵ�����ӵ�ַΪ��%p��ֵΪ��%lld\n", root->left, root->left->val);
	}
	else
	{
		printf("�ڵ������Ϊ�գ�\n");
	}
	if (root->right)
	{
		printf("�ڵ���Һ��ӵ�ַΪ��%p��ֵΪ��%lld\n", root->right, root->right->val);
	}
	else
	{
		printf("�ڵ���Һ���Ϊ�գ�\n");
	}
}

// �������ڵ���ֵ�
TREE_NODE *getTreeNodeBrother(TREE *tree, TREE_NODE *root, void *space, long long *usedHash)
{
	if (!usedHash[(void *)tree - space])
	{
		printf("�������ڣ�\n");
		return NULL;
	}
	TREE_NODE *parent = root->parent;
	if (root == parent->left)
	{
		if (parent->right)
		{
			printf("�ýڵ�����ֵܵ�ַΪ��%p��ֵΪ��%lld\n", parent->right, parent->right->val);
		}
		else
		{
			printf("�ýڵ����ֵ�Ϊ�գ�\n");
		}
		return parent->right;
	}
	else
	{
		if (parent->left)
		{
			printf("�ýڵ�����ֵܵ�ַΪ��%p��ֵΪ��%lld\n", parent->left, parent->left->val);
		}
		else
		{
			printf("�ýڵ����ֵ�Ϊ�գ�\n");
		}
		return parent->left;
	}
}

// ����������
void addTreeNodeInLeft(TREE *tree, TREE_NODE *root, ELEMENT_TYPE val, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)tree - space])
	{
		printf("�������ڣ�\n");
		return;
	}
	if (!usedHash[(void *)root - space])
	{
		printf("���ڵ㲻���ڣ�\n");
		return;
	}
	if (!root->left)
	{
		root->left = (TREE_NODE *)myCalloc(1, sizeof(TREE_NODE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		if (!root->left)
		{
			printf("�½ڵ㴴��ʧ�ܣ�\n");
			return;
		}
		root->left->parent = root;
		tree->size++;
		printf("�ɹ�������������\n");
	}
	else
	{
		printf("�������Ѵ��ڣ�ֵ�Ѹ��£�\n");
	}
	root->left->val = val;
}

// ����������
void addTreeNodeInRight(TREE *tree, TREE_NODE *root, ELEMENT_TYPE val, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)tree - space])
	{
		printf("�������ڣ�\n");
		return;
	}
	if (!usedHash[(void *)root - space])
	{
		printf("���ڵ㲻���ڣ�\n");
		return;
	}
	if (!root->right)
	{
		root->right = (TREE_NODE *)myCalloc(1, sizeof(TREE_NODE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		if (!root->right)
		{
			printf("�½ڵ㴴��ʧ�ܣ�\n");
			return;
		}
		root->right->parent = root;
		tree->size++;
		printf("�ɹ�������������\n");
	}
	else
	{
		printf("�������Ѵ��ڣ�ֵ�Ѹ��£�\n");
	}
	root->right->val = val;
}

// ɾ���ýڵ㼰������
void delTreeNodeAndChildren(TREE *tree, TREE_NODE *root, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)tree - space])
	{
		printf("�������ڣ�\n");
		return;
	}
	if (!usedHash[(void *)root - space])
	{
		printf("���ڵ㲻���ڣ�\n");
		return;
	}
	recursiveDelTreeNode(tree, root, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	printf("�ɹ�ɾ���ýڵ㼰��������\n");
}

void preOrderTree(TREE_NODE *root)
{
	if (root)
	{
		printf(" %lld", root->val);
		preOrderTree(root->left);
		preOrderTree(root->right);
	}
}

// ǰ�����
void preOrderVisitTree(TREE *tree, void *space, long long *usedHash)
{
	if (!usedHash[(void *)tree - space])
	{
		printf("�������ڣ�\n");
		return;
	}
	if (!tree->root)
	{
		printf("��Ϊ�գ�\n");
		return;
	}
	printf("ǰ�������\n");
	preOrderTree(tree->root);
	printf("\n");
}

void inOrderTree(TREE_NODE *root)
{
	if (root)
	{
		inOrderTree(root->left);
		printf(" %lld", root->val);
		inOrderTree(root->right);
	}
}

// �������
void inOrderVisitTree(TREE *tree, void *space, long long *usedHash)
{
	if (!usedHash[(void *)tree - space])
	{
		printf("�������ڣ�\n");
		return;
	}
	if (!tree->root)
	{
		printf("��Ϊ�գ�\n");
		return;
	}
	printf("���������\n");
	inOrderTree(tree->root);
	printf("\n");
}

void postOrderTree(TREE_NODE *root)
{
	if (root)
	{
		postOrderTree(root->left);
		postOrderTree(root->right);
		printf(" %lld", root->val);
	}
}

// �������
void postOrderVisitTree(TREE *tree, void *space, long long *usedHash)
{
	if (!usedHash[(void *)tree - space])
	{
		printf("�������ڣ�\n");
		return;
	}
	if (!tree->root)
	{
		printf("��Ϊ�գ�\n");
		return;
	}
	printf("���������\n");
	postOrderTree(tree->root);
	printf("\n");
}

// �������
void levelOrderVisitTree(TREE *tree, void *space, long long *usedHash)
{
	if (!usedHash[(void *)tree - space])
	{
		printf("�������ڣ�\n");
		return;
	}
	if (!tree->root)
	{
		printf("��Ϊ�գ�\n");
		return;
	}
	QUEUE *queue = (QUEUE *)calloc(1, sizeof(QUEUE));
	if (!queue)
	{
		printf("�ռ䲻�㣬�������ʧ�ܣ�\n");
		return;
	}
	queue->head = (LIST_NODE *)calloc(1, sizeof(LIST_NODE));
	if (!queue->head)
	{
		printf("�ռ䲻�㣬�������ʧ�ܣ�\n");
		return;
	}
	queue->tail = queue->head;
	LIST_NODE *node = (LIST_NODE *)calloc(1, sizeof(LIST_NODE));
	node->val = (long long)(tree->root);
	node->next = NULL;
	queue->tail->next = node;
	queue->tail = node;
	queue->size++;
	printf("���������\n");
	while (queue->size)
	{
		long long size = queue->size;
		for (long long i = 0; i < size; i++)
		{
			LIST_NODE *t = queue->head->next;
			if (t->val)
			{
				TREE_NODE *temTreeNode = (TREE_NODE *)(t->val);
				printf("%lld\t", temTreeNode->val);
				LIST_NODE *left = (LIST_NODE *)calloc(1, sizeof(LIST_NODE));
				left->next = NULL;
				queue->tail->next = left;
				queue->tail = left;
				left->val = (ELEMENT_TYPE)(temTreeNode->left);
				queue->size++;
				LIST_NODE *right = (LIST_NODE *)calloc(1, sizeof(LIST_NODE));
				right->next = NULL;
				queue->tail->next = right;
				queue->tail = right;
				right->val = (ELEMENT_TYPE)(temTreeNode->right);
				queue->size++;
				// printf("left=%p, right=%p\n", left->val, right->val);
			}
			else
			{
				// ����
				printf("NULL\t");
			}
			queue->head->next = t->next;
			free(t);
			queue->size--;
		}
		printf("\n");
	}
	printf("����������\n");
}

void recursiveVisualTree(TREE_NODE *root, long long deep)
{
	if (root)
	{
		recursiveVisualTree(root->right, deep + 1);
		for (long long i = 0; i < deep; i++)
		{
			printf("    ");
		}
		printf("%lld\n", root->val);
		recursiveVisualTree(root->left, deep + 1);
	}
}

// ���ӻ����Ľṹ
void visualTree(TREE *tree, void *space, long long *usedHash)
{
	if (!usedHash[(void *)tree - space])
	{
		printf("�������ڣ�\n");
		return;
	}
	if (!tree->root)
	{
		printf("��Ϊ�գ�\n");
		return;
	}
	recursiveVisualTree(tree->root, 0);
}

// --------------------������--------------------

// --------------------ͼ��ʼ--------------------

// ����ͼ
MAP *initMap(SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	MAP *map = (MAP *)myCalloc(1, sizeof(MAP), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	if (!map)
	{
		printf("ͼ����ʧ�ܣ�\n");
		return NULL;
	}
	map->nodeHead = (LIST_NODE *)myCalloc(1, sizeof(LIST_NODE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	map->edgeHead = (EDGE_NODE *)myCalloc(1, sizeof(EDGE_NODE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	if (!map->nodeHead || !map->edgeHead)
	{
		printf("ͼ����ʧ�ܣ�\n");
		myFree(map, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		return NULL;
	}
	printf("ͼ�����ɹ�����ַΪ��%p\n", map);
	return map;
}

// ɾ��ͼ
void delAllMap(MAP *map, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)map - space])
	{
		printf("ͼ�����ڣ�\n");
		return;
	}
	while (map->edgeHead->next)
	{
		EDGE_NODE *edge = map->edgeHead->next;
		map->edgeHead->next = edge->next;
		LIST_NODE *head = edge->listHead;
		while (head->next)
		{
			LIST_NODE *node = head->next;
			head->next = node->next;
			myFree(node, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
		myFree(head, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		myFree(edge, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	}
	myFree(map->edgeHead, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	while (map->nodeHead->next)
	{
		LIST_NODE *node = map->nodeHead->next;
		map->nodeHead->next = node->next;
		myFree(node, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	}
	myFree(map->nodeHead, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	myFree(map, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	printf("ͼɾ���ɹ���\n");
}

// ���ͼ
void clearMap(MAP *map, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)map - space])
	{
		printf("ͼ�����ڣ�\n");
		return;
	}
	while (map->edgeHead->next)
	{
		EDGE_NODE *edge = map->edgeHead->next;
		map->edgeHead->next = edge->next;
		LIST_NODE *head = edge->listHead;
		while (head->next)
		{
			LIST_NODE *node = head->next;
			head->next = node->next;
			myFree(node, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
		myFree(head, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		myFree(edge, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	}
	while (map->nodeHead->next)
	{
		LIST_NODE *node = map->nodeHead->next;
		map->nodeHead->next = node->next;
		myFree(node, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	}
	map->edgeSize = map->nodeSize = 0;
	printf("ͼ��ճɹ���\n");
}

// ���붥��
void addMapNode(MAP *map, ELEMENT_TYPE val, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)map - space])
	{
		printf("ͼ�����ڣ�\n");
		return;
	}
	LIST_NODE *node = map->nodeHead;
	while (node->next)
	{
		if (node->next->val == val)
		{
			printf("�ڵ��Ѵ��ڣ�\n");
			return;
		}
		node = node->next;
	}
	LIST_NODE *newNode = (LIST_NODE *)myCalloc(1, sizeof(LIST_NODE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	if (!newNode)
	{
		printf("�ڵ����ʧ�ܣ�\n");
		return;
	}
	newNode->val = val;
	node->next = newNode;
	map->nodeSize++;
	printf("�ڵ���ӳɹ���\n");
}

// ɾ������
void delMapNode(MAP *map, ELEMENT_TYPE val, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)map - space])
	{
		printf("ͼ�����ڣ�\n");
		return;
	}
	LIST_NODE *node = map->nodeHead;
	bool flag = 0;
	while (node->next)
	{
		if (node->next->val == val)
		{
			LIST_NODE *t = node->next;
			node->next = node->next->next;
			myFree(t, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
			flag = 1;
			map->nodeSize--;
			break;
		}
		node = node->next;
	}
	if (!flag)
	{
		printf("ɾ��ʧ�ܣ��ڵ㲻���ڣ�\n");
		return;
	}
	EDGE_NODE *edge = map->edgeHead;
	while (edge->next)
	{
		if (edge->next->val == val)
		{
			// ����ɾ��
			EDGE_NODE *temEdge = edge->next;
			edge->next = edge->next->next;
			LIST_NODE *head = temEdge->listHead;
			while (head->next)
			{
				LIST_NODE *temNode = head->next;
				head->next = head->next->next;
				myFree(temNode, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
				map->edgeSize--;
			}
			myFree(head, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
			myFree(temEdge, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
		else
		{
			// �����ڵ�������
			LIST_NODE *head = edge->next->listHead;
			while (head->next)
			{
				if (head->next->val == val)
				{
					LIST_NODE *temNode = head->next;
					head->next = head->next->next;
					map->edgeSize--;
					myFree(temNode, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
					if (!edge->next->listHead->next)
					{
						myFree(edge->next->listHead, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
						EDGE_NODE *temEdgeNode = edge->next;
						edge->next = temEdgeNode->next;
						myFree(temEdgeNode, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
					}
					break;
				}
				else
				{
					head = head->next;
				}
			}
			edge = edge->next;
		}
	}
	printf("�ڵ�ɾ���ɹ���\n");
}

// �����
void addMapEdge(MAP *map, ELEMENT_TYPE start, ELEMENT_TYPE end, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)map - space])
	{
		printf("ͼ�����ڣ�\n");
		return;
	}
	int flag = 0;
	LIST_NODE *node = map->nodeHead->next;
	while (node)
	{
		if (node->val == start)
		{
			flag++;
		}
		if (node->val == end)
		{
			flag += 2;
		}
		node = node->next;
	}
	if (flag == 0)
	{
		printf("�����յ㶼�����ڣ�\n");
	}
	else if (flag == 1)
	{
		printf("�յ㲻���ڣ�\n");
	}
	else if (flag == 2)
	{
		printf("��㲻���ڣ�\n");
	}
	else
	{
		EDGE_NODE *edge = map->edgeHead;
		while (edge->next)
		{
			if (edge->next->val == start)
			{
				LIST_NODE *node = edge->next->listHead;
				while (node->next)
				{
					if (node->next->val == end)
					{
						printf("���Ѵ��ڣ�\n");
						return;
					}
					node = node->next;
				}
				LIST_NODE *newNode = (LIST_NODE *)myCalloc(1, sizeof(LIST_NODE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
				if (!newNode)
				{
					printf("���ʧ�ܣ�\n");
					return;
				}
				newNode->val = end;
				node->next = newNode;
				map->edgeSize++;
				printf("����ӳɹ���\n");
				return;
			}
			edge = edge->next;
		}
		// �ö���û�б�
		EDGE_NODE *newEdge = (EDGE_NODE *)myCalloc(1, sizeof(EDGE_NODE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		if (!newEdge)
		{
			printf("�����ʧ�ܣ�\n");
			return;
		}
		newEdge->val = start;
		newEdge->listHead = (LIST_NODE *)myCalloc(1, sizeof(LIST_NODE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		if (!newEdge->listHead)
		{
			printf("�����ʧ�ܣ�\n");
			myFree(newEdge, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
			return;
		}
		LIST_NODE *temNode = (LIST_NODE *)myCalloc(1, sizeof(LIST_NODE), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		if (!temNode)
		{
			printf("�����ʧ�ܣ�\n");
			myFree(newEdge->listHead, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
			myFree(newEdge, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
			return;
		}
		temNode->val = end;
		newEdge->listHead->next = temNode;
		edge->next = newEdge;
		map->edgeSize++;
		printf("����ӳɹ���\n");
	}
}

// ɾ����
void delMapEdge(MAP *map, ELEMENT_TYPE start, ELEMENT_TYPE end, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (!usedHash[(void *)map - space])
	{
		printf("ͼ�����ڣ�\n");
		return;
	}
	EDGE_NODE *edge = map->edgeHead;
	while (edge->next)
	{
		if (edge->next->val == start)
		{
			LIST_NODE *node = edge->next->listHead;
			while (node->next)
			{
				if (node->next->val == end)
				{
					LIST_NODE *t = node->next;
					node->next = node->next->next;
					myFree(t, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
					map->edgeSize--;
					// ����õ�û�б��ˣ��Ͱѱ�����ɾ��
					if (!edge->next->listHead->next)
					{
						myFree(edge->next->listHead, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
						EDGE_NODE *temEdgeNode = edge->next;
						edge->next = temEdgeNode->next;
						myFree(temEdgeNode, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
					}
					printf("ɾ���ɹ���\n");
					return;
				}
				node = node->next;
			}
			printf("�߲����ڣ�\n");
			return;
		}
		edge = edge->next;
	}
	printf("�߲����ڣ�\n");
}

// �������ĳ��������б�
void inMapNode(MAP *map, ELEMENT_TYPE val, void *space, long long *usedHash)
{
	if (!usedHash[(void *)map - space])
	{
		printf("ͼ�����ڣ�\n");
		return;
	}
	// ���ڵ��Ƿ����
	bool flag = 0;
	LIST_NODE *node = map->nodeHead->next;
	while (node)
	{
		if (node->val == val)
		{
			flag = 1;
			break;
		}
		node = node->next;
	}
	if (!flag)
	{
		printf("�ýڵ㲻���ڣ�\n");
		return;
	}
	flag = 0;
	EDGE_NODE *edge = map->edgeHead->next;
	while (edge)
	{
		LIST_NODE *head = edge->listHead->next;
		while (head)
		{
			if (head->val == val)
			{
				if (flag == 0)
				{
					printf("����ýڵ�ĵ��У�");
				}
				flag = 1;
				printf("%lld  ", edge->val);
				break;
			}
			head = head->next;
		}
		edge = edge->next;
	}
	if (!flag)
	{
		printf("�����ڽ���ýڵ�ĵ㣡\n");
	}
	else
	{
		printf("\n");
	}
}

// ����뿪ĳ��������б�
void outMapNode(MAP *map, ELEMENT_TYPE val, void *space, long long *usedHash)
{
	if (!usedHash[(void *)map - space])
	{
		printf("ͼ�����ڣ�\n");
		return;
	}
	// ���ڵ��Ƿ����
	bool flag = 0;
	LIST_NODE *node = map->nodeHead->next;
	while (node)
	{
		if (node->val == val)
		{
			flag = 1;
			break;
		}
		node = node->next;
	}
	if (!flag)
	{
		printf("�ýڵ㲻���ڣ�\n");
		return;
	}
	flag = 0;
	EDGE_NODE *edge = map->edgeHead->next;
	while (edge)
	{
		if (edge->val == val)
		{
			LIST_NODE *head = edge->listHead->next;
			if (head)
			{
				printf("�뿪�ýڵ�ĵ��У�");
				flag = 1;
			}
			while (head)
			{
				flag = 1;
				printf("%lld  ", head->val);
				head = head->next;
			}
			break;
		}
		edge = edge->next;
	}
	if (!flag)
	{
		printf("�������뿪�ýڵ�ĵ㣡\n");
	}
	else
	{
		printf("\n");
	}
}

// �����ĸ���
long long mapNodeNum(MAP *map, void *space, long long *usedHash)
{
	if (!usedHash[(void *)map - space])
	{
		printf("ͼ�����ڣ�\n");
		return -1;
	}
	printf("ͼ�Ķ��㹲��%lld��\n", map->nodeSize);
	return map->nodeSize;
}

// ����ߵĸ���
long long mapEdgeNum(MAP *map, void *space, long long *usedHash)
{
	if (!usedHash[(void *)map - space])
	{
		printf("ͼ�����ڣ�\n");
		return -1;
	}
	printf("ͼ�ı߹���%lld��\n", map->edgeSize);
	return map->edgeSize;
}

// �������ж���
void visitAllMapNode(MAP *map, void *space, long long *usedHash)
{
	if (!usedHash[(void *)map - space])
	{
		printf("ͼ�����ڣ�\n");
		return;
	}
	if (map->nodeSize == 0)
	{
		printf("��ͼΪ�գ�\n");
		return;
	}
	printf("ͼ�Ķ����У�\n");
	LIST_NODE *node = map->nodeHead->next;
	while (node)
	{
		printf(" %lld", node->val);
		node = node->next;
	}
	printf("\n");
}

// �������б�
void visitAllMapEdge(MAP *map, void *space, long long *usedHash)
{
	if (!usedHash[(void *)map - space])
	{
		printf("ͼ�����ڣ�\n");
		return;
	}
	if (map->edgeSize == 0)
	{
		printf("��ͼû�бߣ�\n");
		return;
	}
	printf("ͼ�ı��У�\n");
	EDGE_NODE *edge = map->edgeHead->next;
	while (edge)
	{
		LIST_NODE *head = edge->listHead->next;
		while (head)
		{
			printf("%lld->%lld  ", edge->val, head->val);
			head = head->next;
		}
		printf("\n");
		edge = edge->next;
	}
}

// --------------------ͼ����--------------------

// ��ȡ����
char **getArguments(char *s, int n)
{
	char **arr = (char **)calloc(n, sizeof(char *));
	int flag1 = strstr(s, "(") - s;
	int flag2 = flag1;
	for (int i = 0; i < n - 1; i++)
	{
		arr[i] = (char *)calloc(STR_SIZE, sizeof(char));
		flag1 = flag2 + 1;
		flag2 = strstr(&s[flag1], ",") - s;
		strncpy(arr[i], &s[flag1], flag2 - flag1);
	}
	arr[n - 1] = (char *)calloc(STR_SIZE, sizeof(char));
	flag1 = flag2 + 1;
	flag2 = strstr(&s[flag1], ")") - s;
	strncpy(arr[n - 1], &s[flag1], flag2 - flag1);
	return arr;
}

// �ַ���ת��ַ
void *strToPath(char *s)
{
	return (void *)strtoll(s, NULL, 16);
}

void eval(char *s, SPACE_NODE **freeSpaceArray, int maxBlock, long long *usedSpace, long long *freeSpace, void *space, char *blockStatus, long long *usedHash, FREE_SPACE_NODE *freeSpaceListHead)
{
	if (strstr(s, "checkSpace"))
	{
		// �鿴�ռ�����ʹ�����
		checkSpace(*usedSpace, *freeSpace);
	}
	else if (strstr(s, "checkBlock"))
	{
		// �鿴�ڴ��ʹ�����
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			checkBlock(space, blockStatus, address);
		}
	}
	else if (strstr(s, "checkFreeSpaceList"))
	{
		// �鿴���п�����
		checkFreeSpaceList(freeSpaceListHead);
	}
	else if (strstr(s, "myFree"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			myFree(address, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "myMalloc"))
	{
		char **arr = getArguments(s, 1);
		void *p = myMalloc(atoll(arr[0]), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	}
	else if (strstr(s, "myCalloc"))
	{
		char **arr = getArguments(s, 2);
		void *p = myCalloc(atoll(arr[0]), atoll(arr[1]), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	}
	else if (strstr(s, "initList"))
	{
		LIST_NODE *head = initList(freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	}
	else if (strstr(s, "delAllList"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			delList(address, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "listEmpty"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			listEmpty(address, space, usedHash);
		}
	}
	else if (strstr(s, "listLength"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			listLength(address, space, usedHash);
		}
	}
	else if (strstr(s, "getListNodeVal"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			getListNodeVal(address, space, usedHash);
		}
	}
	else if (strstr(s, "findListNode"))
	{
		char **arr = getArguments(s, 2);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			findListNode(address, atoll(arr[1]), space, usedHash);
		}
	}
	else if (strstr(s, "listPreNode"))
	{
		char **arr = getArguments(s, 2);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			listPreNode(address, atoll(arr[1]), space, usedHash);
		}
	}
	else if (strstr(s, "listNextNode"))
	{
		char **arr = getArguments(s, 2);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			listNextNode(address, atoll(arr[1]), space, usedHash);
		}
	}
	else if (strstr(s, "visitList"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			visitList(address, space, usedHash);
		}
	}
	else if (strstr(s, "clearList"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			clearList(address, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "changeListVal"))
	{
		char **arr = getArguments(s, 3);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			changeListVal(address, atoll(arr[1]), atoll(arr[2]), space, usedHash);
		}
	}
	else if (strstr(s, "addListNode"))
	{
		char **arr = getArguments(s, 3);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			addListNode(address, atoll(arr[1]), atoll(arr[2]), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "delListNode"))
	{
		char **arr = getArguments(s, 2);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			delListNode(address, atoll(arr[1]), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "initArray"))
	{
		char **arr = getArguments(s, 1);
		initArray(atoll(arr[0]), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	}
	else if (strstr(s, "delAllArray"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			delArray(address, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "getArrayVal"))
	{
		char **arr = getArguments(s, 2);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			getArrayVal(address, atoll(arr[1]), space, usedHash);
		}
	}
	else if (strstr(s, "changeArrayVal"))
	{
		char **arr = getArguments(s, 3);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			changeArrayVal(address, atoll(arr[1]), atoll(arr[2]), space, usedHash);
		}
	}
	else if (strstr(s, "visitArray"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			visitArray(address, space, usedHash);
		}
	}
	else if (strstr(s, "initHeap"))
	{
		char **arr = getArguments(s, 1);
		initHeap(atoll(arr[0]), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	}
	else if (strstr(s, "delAllHeap"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			delHeap(address, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "clearHeap"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			clearHeap(address, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "heapPush"))
	{
		char **arr = getArguments(s, 2);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			heapPush(address, atoll(arr[1]), space, usedHash);
		}
	}
	else if (strstr(s, "heapPop"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			heapPop(address, space, usedHash);
		}
	}
	else if (strstr(s, "getHeapTop"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			getHeapTop(address, space, usedHash);
		}
	}
	else if (strstr(s, "getHeapSize"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			getHeapSize(address, space, usedHash);
		}
	}
	else if (strstr(s, "outputHeap"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			outputHeap(address, space, usedHash);
		}
	}
	else if (strstr(s, "initStack"))
	{
		initStack(freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	}
	else if (strstr(s, "delAllStack"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			delStack(address, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "clearStack"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			clearStack(address, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "getStackTopVal"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			getStackTopVal(address, space, usedHash);
		}
	}
	else if (strstr(s, "getStackSize"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			getStackSize(address, space, usedHash);
		}
	}
	else if (strstr(s, "stackPush"))
	{
		char **arr = getArguments(s, 2);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			stackPush(address, atoll(arr[1]), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "stackPop"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			stackPop(address, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "initQueue"))
	{
		initQueue(freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	}
	else if (strstr(s, "delAllQueue"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			delQueue(address, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "clearQueue"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			clearQueue(address, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "getQueueHeadVal"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			getQueueHeadVal(address, space, usedHash);
		}
	}
	else if (strstr(s, "getQueueSize"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			getQueueSize(address, space, usedHash);
		}
	}
	else if (strstr(s, "queuePush"))
	{
		char **arr = getArguments(s, 2);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			queuePush(address, atoll(arr[1]), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "queuePop"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			queuePop(address, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "initTree"))
	{
		initTree(freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	}
	else if (strstr(s, "delAllTree"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			delTree(address, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "clearTree"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			clearTree(address, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "countTreeNode"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			countTreeNode(address, space, usedHash);
		}
	}
	else if (strstr(s, "countTreeDeep"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			countTreeDeep(address, space, usedHash);
		}
	}
	else if (strstr(s, "treeRoot"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			treeRoot(address, space, usedHash);
		}
	}
	else if (strstr(s, "getTreeNodeVal"))
	{
		char **arr = getArguments(s, 2);
		void *address1 = strToPath(arr[0]);
		void *address2 = strToPath(arr[1]);
		bool flag1 = addressIsValid(address1, space);
		bool flag2 = addressIsValid(address2, space);
		if (flag1 && flag2)
		{
			getTreeNodeVal(address1, address2, space, usedHash);
		}
	}
	else if (strstr(s, "changeTreeNodeVal"))
	{
		char **arr = getArguments(s, 3);
		void *address1 = strToPath(arr[0]);
		void *address2 = strToPath(arr[1]);
		bool flag1 = addressIsValid(address1, space);
		bool flag2 = addressIsValid(address2, space);
		if (flag1 && flag2)
		{
			changeTreeNodeVal(address1, address2, atoll(arr[2]), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "addTreeRootVal"))
	{
		char **arr = getArguments(s, 2);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			addTreeRootVal(address, atoll(arr[1]), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "getTreeNodeParent"))
	{
		char **arr = getArguments(s, 2);
		void *address1 = strToPath(arr[0]);
		void *address2 = strToPath(arr[1]);
		bool flag1 = addressIsValid(address1, space);
		bool flag2 = addressIsValid(address2, space);
		if (flag1 && flag2)
		{
			getTreeNodeParent(address1, address2, space, usedHash);
		}
	}
	else if (strstr(s, "getTreeNodeChildren"))
	{
		char **arr = getArguments(s, 2);
		void *address1 = strToPath(arr[0]);
		void *address2 = strToPath(arr[1]);
		bool flag1 = addressIsValid(address1, space);
		bool flag2 = addressIsValid(address2, space);
		if (flag1 && flag2)
		{
			getTreeNodeChildren(address1, address2, space, usedHash);
		}
	}
	else if (strstr(s, "getTreeNodeBrother"))
	{
		char **arr = getArguments(s, 2);
		void *address1 = strToPath(arr[0]);
		void *address2 = strToPath(arr[1]);
		bool flag1 = addressIsValid(address1, space);
		bool flag2 = addressIsValid(address2, space);
		if (flag1 && flag2)
		{
			getTreeNodeBrother(address1, address2, space, usedHash);
		}
	}
	else if (strstr(s, "addTreeNodeInLeft"))
	{
		char **arr = getArguments(s, 3);
		void *address1 = strToPath(arr[0]);
		void *address2 = strToPath(arr[1]);
		bool flag1 = addressIsValid(address1, space);
		bool flag2 = addressIsValid(address2, space);
		if (flag1 && flag2)
		{
			addTreeNodeInLeft(address1, address2, atoll(arr[2]), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "addTreeNodeInRight"))
	{
		char **arr = getArguments(s, 3);
		void *address1 = strToPath(arr[0]);
		void *address2 = strToPath(arr[1]);
		bool flag1 = addressIsValid(address1, space);
		bool flag2 = addressIsValid(address2, space);
		if (flag1 && flag2)
		{
			addTreeNodeInRight(address1, address2, atoll(arr[2]), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "delTreeNodeAndChildren"))
	{
		char **arr = getArguments(s, 2);
		void *address1 = strToPath(arr[0]);
		void *address2 = strToPath(arr[1]);
		bool flag1 = addressIsValid(address1, space);
		bool flag2 = addressIsValid(address2, space);
		if (flag1 && flag2)
		{
			delTreeNodeAndChildren(address1, address2, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "preOrderVisitTree"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			preOrderVisitTree(address, space, usedHash);
		}
	}
	else if (strstr(s, "inOrderVisitTree"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			inOrderVisitTree(address, space, usedHash);
		}
	}
	else if (strstr(s, "postOrderVisitTree"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			postOrderVisitTree(address, space, usedHash);
		}
	}
	else if (strstr(s, "levelOrderVisitTree"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			levelOrderVisitTree(address, space, usedHash);
		}
	}
	else if (strstr(s, "visualTree"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			visualTree(address, space, usedHash);
		}
	}
	else if (strstr(s, "initMap"))
	{
		initMap(freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
	}
	else if (strstr(s, "delAllMap"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			delAllMap(address, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "clearMap"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			clearMap(address, freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "addMapNode"))
	{
		char **arr = getArguments(s, 2);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			addMapNode(address, atoll(arr[1]), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "delMapNode"))
	{
		char **arr = getArguments(s, 2);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			delMapNode(address, atoll(arr[1]), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "addMapEdge"))
	{
		char **arr = getArguments(s, 3);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			addMapEdge(address, atoll(arr[1]), atoll(arr[2]), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "delMapEdge"))
	{
		char **arr = getArguments(s, 3);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			delMapEdge(address, atoll(arr[1]), atoll(arr[2]), freeSpaceArray, maxBlock, usedSpace, freeSpace, space, blockStatus, usedHash, freeSpaceListHead);
		}
	}
	else if (strstr(s, "inMapNode"))
	{
		char **arr = getArguments(s, 2);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			inMapNode(address, atoll(arr[1]), space, usedHash);
		}
	}
	else if (strstr(s, "outMapNode"))
	{
		char **arr = getArguments(s, 2);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			outMapNode(address, atoll(arr[1]), space, usedHash);
		}
	}
	else if (strstr(s, "mapNodeNum"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			mapNodeNum(address, space, usedHash);
		}
	}
	else if (strstr(s, "mapEdgeNum"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			mapEdgeNum(address, space, usedHash);
		}
	}
	else if (strstr(s, "visitAllMapNode"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			visitAllMapNode(address, space, usedHash);
		}
	}
	else if (strstr(s, "visitAllMapEdge"))
	{
		char **arr = getArguments(s, 1);
		void *address = strToPath(arr[0]);
		bool flag = addressIsValid(address, space);
		if (flag)
		{
			visitAllMapEdge(address, space, usedHash);
		}
	}
	else
	{
		printf("δ�����ָ�����У�\n");
	}
}
