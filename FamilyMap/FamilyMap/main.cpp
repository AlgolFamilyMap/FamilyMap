#include <iostream>
#include <fstream>
#include <string>
using namespace std;

typedef struct treeNode {
	int level;
	string myName, parentName, wife;
	treeNode *bro, *son;
} treeNode;

void swapInAry(treeNode ary[], int i, int j) {
	treeNode temp = ary[i];
	ary[i] = ary[j];
	ary[j] = temp;
}

int partition(treeNode ary[], int left, int right) {
	treeNode pivot = ary[left];
	int l = left;
	int r = right + 1;

	while (l < r) {
		while (ary[++l].level < pivot.level);
		while (ary[--r].level > pivot.level);

		if (l >= r)
			break;
		swapInAry(ary, l, r);
	}

	swapInAry(ary, left, r);

	return r;
}

void quickSort(treeNode ary[], int left, int right) {
	if (left < right) {
		int pivot = partition(ary, left, right);
		quickSort(ary, left, pivot - 1);
		quickSort(ary, pivot + 1, right);
	}
}

class Queue {
private:
	typedef struct qNode {
		treeNode *item;
		qNode *next;
	} qNode;
	qNode *front, *rear; // �Ӹ�, ����
public:
	Queue();
	void enqueue(treeNode *item);
	treeNode* dequeue();
	bool isEmpty();
};

Queue::Queue() {
	front = rear = NULL;
}

void Queue::enqueue(treeNode *item) {
	// ť�� ������ ����
	qNode *newNode = new qNode;
	newNode->item = item;
	newNode->next = NULL;

	if (isEmpty())
		front = rear = newNode;
	else {
		rear->next = newNode;
		rear = newNode;
	}
}

treeNode* Queue::dequeue() {
	// ť���� ������ ����
	if (isEmpty()) {
		return NULL;
	}

	qNode *delNode = front;
	treeNode *delItem = delNode->item;
	front = front->next;
	delete delNode;

	return delItem;
}

bool Queue::isEmpty() {
	// ��� �ִ��� üũ
	if (front == NULL)
		return true;
	else
		return false;
}

class FamilyMap {
private:
	treeNode *root;
	string fileName;
public:
	FamilyMap(string fileName);
	~FamilyMap();
	void insert(int level, string myName, string parentName, string wife);
	void search(string name);
	void printAll();
	void printMenu();
};

FamilyMap::FamilyMap(string fileName) {
	this->fileName = fileName;
	root = NULL;

	ifstream fileIn;
	fileIn.open(fileName); // ���� ����
	if (!fileIn.is_open()) { // ���� ���� ���� üũ
		cout << "File Open error!" << endl;
		exit(1);
	}

	treeNode itemTable[100];
	int tableSize = 0;
	string buffer, strTemp[4];
	// ���� �б� �� itemTable�� ����
	while (!fileIn.eof()) {
		getline(fileIn, buffer); // buffer�� ����
		if (buffer.size() < 1)
			break;

		// ��ū �и�
		int index = -1, beginIndex = 0;
		for (int i = 0; i < 3; i++) {
			index = buffer.find('\t', ++index); // "\t" ã��
			strTemp[i] = buffer.substr(beginIndex, index - beginIndex);
			beginIndex = index + 1;
		}
		strTemp[3] = buffer.substr(beginIndex);

		tableSize++;
		itemTable[tableSize - 1].level = stoi(strTemp[0]);
		itemTable[tableSize - 1].myName = strTemp[1];
		itemTable[tableSize - 1].parentName = strTemp[2];
		itemTable[tableSize - 1].wife = strTemp[3];
	}

	// itemTable �������� ����
	quickSort(itemTable, 0, tableSize - 1);

	// Ʈ���� ����
	for (int i = 0; i < tableSize; i++)
		insert(itemTable[i].level, itemTable[i].myName, itemTable[i].parentName, itemTable[i].wife);

	fileIn.close();
}

FamilyMap::~FamilyMap() {
	ofstream fileOut;
	fileOut.open(fileName);
	if (!fileOut.is_open()) {
		cout << "File Writer Error!" << endl;
		exit(1);
	}

	Queue queue;
	queue.enqueue(root);
	while (!queue.isEmpty()) {
		treeNode *pNode = queue.dequeue(); // ť���� ������ ����
		fileOut << pNode->level << '\t' << pNode->myName << '\t' << pNode->parentName << '\t' << pNode->wife << endl;

		// ť�� pNode�� ������ �ڽ� ����
		if (pNode->bro)
			queue.enqueue(pNode->bro);
		if (pNode->son)
			queue.enqueue(pNode->son);
	}

	fileOut.close();
}

void FamilyMap::insert(int level, string myName, string parentName, string wife) {
	Queue queue;
	// �� ��� ���� �� ������ ����
	treeNode *newNode = new treeNode;
	newNode->level = level;
	newNode->myName = myName;
	newNode->parentName = parentName;
	newNode->wife = wife;
	newNode->bro = NULL;
	newNode->son = NULL;

	// root ����� ���
	if (level == 1)
		root = newNode;
	else { // �� ���� ���
		queue.enqueue(root);
		while (!queue.isEmpty()) {
			treeNode *pNode = queue.dequeue();
			// ������ ����� �θ��̸��� ã�� ���
			if (pNode->myName == parentName) {
				// pNode�� �ڽ��� ���� ���
				if (pNode->son == NULL)
					pNode->son = newNode; // �� ��� ����
				else { // �ڽ� ����
					pNode = pNode->son;
					while (pNode->bro) // �� ���� ���� Ž��
						pNode = pNode->bro;
					pNode->bro = newNode; // �� ��� ����
				}
				break;
			}

			// ť�� pNode�� ����, �ڽ� ����
			if (pNode->bro)
				queue.enqueue(pNode->bro);
			if (pNode->son)
				queue.enqueue(pNode->son);
		}
	}

	// ť ����
	while (!queue.isEmpty())
		queue.dequeue();
}

void FamilyMap::search(string name) {
	Queue queue;
	queue.enqueue(root);
	while (!queue.isEmpty()) {
		treeNode *pNode = queue.dequeue();
		if (pNode->myName == name) {
			cout << pNode->level << '\t' << pNode->myName << '\t' << pNode->parentName << '\t' << pNode->wife << endl;
			break;
		}

		if (pNode->bro)
			queue.enqueue(pNode->bro);
		if (pNode->son)
			queue.enqueue(pNode->son);
	}
}

void FamilyMap::printAll() {
	Queue queue;
	queue.enqueue(root);
	while (!queue.isEmpty()) {
		treeNode *pNode = queue.dequeue();
		cout << pNode->level << '\t' << pNode->myName << '\t' << pNode->parentName << '\t' << pNode->wife << endl;

		if (pNode->bro)
			queue.enqueue(pNode->bro);
		if (pNode->son)
			queue.enqueue(pNode->son);
	}
}

void FamilyMap::printMenu() {
	cout << "1. �˻�(�̸�)\t2. ��ü ���\t3. ����" << endl;
}

int main() {
	string fileName = "Family.txt";
	FamilyMap fMap(fileName);
	string name;

	int n;
	do {
		fMap.printMenu();
		cout << "�Է� : ";
		cin >> n;
		switch (n) {
		case 1:
			cout << "�˻��� �̸� : ";
			cin >> name;
			fMap.search(name);
			break;
		case 2:
			fMap.printAll();
			break;
		case 3:
			break;
		}
	} while (n != 3);
}