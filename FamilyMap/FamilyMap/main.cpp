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
	qNode *front, *rear; // 머리, 꼬리
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
	// 큐에 데이터 삽입
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
	// 큐에서 데이터 제거
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
	// 비어 있는지 체크
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
	fileIn.open(fileName); // 파일 열기
	if (!fileIn.is_open()) { // 파일 열기 오류 체크
		cout << "File Open error!" << endl;
		exit(1);
	}

	treeNode itemTable[100];
	int tableSize = 0;
	string buffer, strTemp[4];
	// 파일 읽기 및 itemTable에 저장
	while (!fileIn.eof()) {
		getline(fileIn, buffer); // buffer에 저장
		if (buffer.size() < 1)
			break;

		// 토큰 분리
		int index = -1, beginIndex = 0;
		for (int i = 0; i < 3; i++) {
			index = buffer.find('\t', ++index); // "\t" 찾기
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

	// itemTable 오름차순 정렬
	quickSort(itemTable, 0, tableSize - 1);

	// 트리에 삽입
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
		treeNode *pNode = queue.dequeue(); // 큐에서 데이터 제거
		fileOut << pNode->level << '\t' << pNode->myName << '\t' << pNode->parentName << '\t' << pNode->wife << endl;

		// 큐에 pNode의 형제와 자식 삽입
		if (pNode->bro)
			queue.enqueue(pNode->bro);
		if (pNode->son)
			queue.enqueue(pNode->son);
	}

	fileOut.close();
}

void FamilyMap::insert(int level, string myName, string parentName, string wife) {
	Queue queue;
	// 새 노드 생성 및 데이터 저장
	treeNode *newNode = new treeNode;
	newNode->level = level;
	newNode->myName = myName;
	newNode->parentName = parentName;
	newNode->wife = wife;
	newNode->bro = NULL;
	newNode->son = NULL;

	// root 노드인 경우
	if (level == 1)
		root = newNode;
	else { // 그 외의 경우
		queue.enqueue(root);
		while (!queue.isEmpty()) {
			treeNode *pNode = queue.dequeue();
			// 삽입할 노드의 부모이름을 찾은 경우
			if (pNode->myName == parentName) {
				// pNode의 자식이 없는 경우
				if (pNode->son == NULL)
					pNode->son = newNode; // 새 노드 삽입
				else { // 자식 존재
					pNode = pNode->son;
					while (pNode->bro) // 맨 우측 형제 탐색
						pNode = pNode->bro;
					pNode->bro = newNode; // 새 노드 삽입
				}
				break;
			}

			// 큐에 pNode의 형제, 자식 삽입
			if (pNode->bro)
				queue.enqueue(pNode->bro);
			if (pNode->son)
				queue.enqueue(pNode->son);
		}
	}

	// 큐 비우기
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
	cout << "1. 검색(이름)\t2. 전체 출력\t3. 종료" << endl;
}

int main() {
	string fileName = "Family.txt";
	FamilyMap fMap(fileName);
	string name;

	int n;
	do {
		fMap.printMenu();
		cout << "입력 : ";
		cin >> n;
		switch (n) {
		case 1:
			cout << "검색할 이름 : ";
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