#include<fstream>
#include<iostream>
#include<string>
#include<ctime>
#include <windows.h>
#include <stdlib.h>
#include<iomanip>
using namespace std;
//������ �����:  ������ 
//���������� ���������: 2000
// �������� �����������


//DONE- �������� ���-�������
//DONE - ���������� ���������
//DONE- ��������
//DONE - ����� ��������� �� �����;
//DONE- ����� ��������� �� ������ ��������
//DONE- ��������� ���������� ��� - ������� � ���� ��� ���������� ����������� � MS Excel
//DONE- �������� ���������;
//DONE - �������� ������� ��������� �����;
//DONE- ��� �������� ��������� �� ��� - �������, � ��������� ������ ���� ���������� ��������,
//		 ����������� ������ ��������, ��������� �������� � ���������;
//DONE- �������� ��� ������������



const string TypeOfKey = "������";


// ��� �������� ������� �����������, �� ������
int NumOfSegments = 2000;
int MaxNumOfCollisions = 2000;
const int StepOfLinearSampling = 1;
const int NumOfKeys = 6000;
struct Keys {
	string key = "";
	int numOfH = 0;
	Keys* next = NULL;
	Keys() {};
};
int length(Keys* head) {
	int counter = 0;
	while (head != NULL) {
		counter++;
		head = head->next;
	}
	return counter;
}
void HashFunctionCounter(Keys* MassiveOfSegments, Keys*& BadKeys)
{
	int hashNum = 0;
	Keys* tmp = new Keys;
	Keys* TmpBadKeys = BadKeys; //empty
	int minHash = 19587;

	for (int i = 0; i < NumOfKeys; i++)
	{
		tmp->key = "";
		//���� ��������� ����� ����� ����� �� �������
		for (int j = 0; j < 6; j++) {
			int r_number = TypeOfKey[j] == '�' ? rand() % 10 + 48 : rand() % 26 + 65;
			tmp->key += char(r_number);
		}

		hashNum = 0 - minHash;
		for (int j = 0; j < 6; j++)
			hashNum = hashNum + (tmp->key[j] * tmp->key[j]); //�������� ��������� � �� ������
		hashNum = hashNum % NumOfSegments;


		MassiveOfSegments[hashNum].numOfH++; 
		for (int j = 0; j <= MaxNumOfCollisions; j++) 
		{
			if (MassiveOfSegments[hashNum].key.empty())
			{
				MassiveOfSegments[hashNum].key = tmp->key;
				break;
			}
			else if (j == MaxNumOfCollisions - 1)
			{
				if (BadKeys == NULL) 
				{
					TmpBadKeys = new Keys;
					TmpBadKeys->key = tmp->key;
					TmpBadKeys->next = NULL;
					BadKeys = TmpBadKeys;
					break;
				}
				else
				{
					TmpBadKeys->next = new Keys;
					TmpBadKeys = TmpBadKeys->next;
					TmpBadKeys->next = NULL;
					TmpBadKeys->key = tmp->key;
					break;
				}
			}
			else
			{
				hashNum += StepOfLinearSampling;
				if (hashNum > NumOfSegments)
					hashNum -= NumOfSegments;
			}
		}
	}
}
int main_test() {
	srand(time(NULL));
	ofstream out_file("D:\\in-outs\\hash results.txt");
	Keys* MassiveOfSegments = new Keys[NumOfSegments]; //������� ������ ��� ������� � ����, ��� �� �� ������� � ����
	Keys* BadKeys = NULL;
	HashFunctionCounter(MassiveOfSegments, BadKeys);
	for (int i = 0; i < NumOfSegments; i++) {
		out_file << MassiveOfSegments[i].numOfH << endl;
	}
	out_file.close();
	cout << "All done!" << endl;
	return 0;
}





//BASE
struct Entry {
	string key = "";
	int value = 0;
	int state = 0; //0 for free, 1 for full, -1 for deleted 
	Entry(string key, int value) {
		this->key = key;
		this->value = value;
		this->state = 1;
	}
	Entry() {};
};
struct HTable {
	Entry* storage;
	int size;
	double coeff;
	int step;
	int filled = 0;

	HTable(int size) {
		this->size = size;
		this->storage = new Entry[size];
		this->step = 1;
		this->coeff = 0.7;
	}

	HTable(int size, double coeff, int step) {
		this->size = size;
		this->storage = new Entry[size];
		this->step = step;
		this->coeff = coeff;
	}

	HTable() {
		this->size = 2000;
		this->storage = new Entry[size];
		this->coeff = 0.7;
		this->step = 1;
	}

	int my_hash(string key);
	int add(Entry e);
	bool remove(string key);
	void expand();
	int search_key(string key);
	int search_value(int value);

	void print();
	void print_in_file();

};

int HTable::my_hash(string key) {
	int hashNum = -19587;
	for (int j = 0; j < 6; j++)
		hashNum = hashNum + (key[j] * key[j]);
	hashNum = hashNum % size;
	return hashNum;
}

//MANIPULATION
int HTable::add(Entry e) {
	int hashNum = my_hash(e.key);

	for (int j = 0; j < (size * coeff); j++)
	{
		//���� �� ������ �����,
		if (storage[hashNum].state < 1)
		{
			//�������� ��� ���� � �������� �����������. ��������� ���� �����
			storage[hashNum] = e;
			filled += 1;
			if (filled > (size * coeff)) {
				expand();
				return add(e);
			}
			return hashNum;
		}
		//���� �����, ��������� ���� �� ������������, ���� ���������� �� ������ ��� ��� ����
		else if (storage[hashNum].key == e.key)
			return hashNum;
		//����� ���������� �������� �������� ���� � ������ ������ �������� �������������
		hashNum += step;
		if (hashNum >= size)
			hashNum -= size;

	}
	//��������� ������ �� ���������, ������ �� ������ 70% ������� � ��� ���� ���������. �����������
	expand();
	return add(e);
}
void HTable::expand() {
	cout << "���������� �������." << endl;
	int oldSize = size;
	size *= 2;
	filled = 0;
	Entry* newTable = new Entry[size];
	Entry* tmp = storage;
	storage = newTable;

	for (int i = 0; i < oldSize; i++) {
		if (tmp[i].state == 1)
			add(tmp[i]);
	}
	delete[]tmp;
}
bool HTable::remove(string key) { //false if wrong index or no elem; prints out collided keys
	int hashNum = my_hash(key);
	for (int j = 0; j < size * coeff; j++)
	{
		//���� �� ����� ������ ������ �������, �� �������� � ������� ���
		if (storage[hashNum].state == 0)
			return false;

		//���� �� ����, ��������� �� ������������
		else if (storage[hashNum].state == 1 && storage[hashNum].key == key) {
			storage[hashNum].state = -1;
			filled -= 1;
			return true;
		}
		//���� �������� ��� �� ���, ���������� �����
		cout << "������� � ���������: " << storage[hashNum].key << ". ���������: " << (storage[hashNum].state == 1 ? "��������" : "�����");
		hashNum += step;
		if (hashNum >= size)
			hashNum -= size;

	}
	return false;
}

//SEARCH
int HTable:: search_key(string key) {
	int hashNum = my_hash(key);
	for (int j = 0; j < size * coeff; j++)
	{
		//���� �� ����� ������ ������ �������, �� ����� � ������� ���
		if (storage[hashNum].state == 0)
			return -1;

		//���� �� ����, ��������� �� ������������
		else if (storage[hashNum].state == 1 && storage[hashNum].key == key)
			return hashNum;

		//���� �������� ��� �� ���, ���������� �����
		hashNum += step;
		if (hashNum >= size)
			hashNum -= size;

	}
	return -1;
}
int HTable:: search_value(int value) { //checking all of the table
	for (int j = 0; j < size; j++) {
		if (storage[j].value == value && storage[j].state == 1) return j;
	}
	return -1;
}

//DISPLAY
void HTable:: print() {
	int start, finish;
	cout << "������� ������� ������ � ����� ������������ ����� ������� ����� ������ (���� �� �� ������� ������� ��� �������): ";
	cin.clear();
	cin >> start >> finish;

	if (cin.fail() || cin.rdbuf()->in_avail() != 1 || start > finish || start >= size || finish >= size) {
		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
		start = 0;
		finish = size - 1;
	}

	cout << "�������:" << endl;
	for (int i = start; i <= finish; i++) {
		cout << "[ " << i << " ] " << setw(6) << storage[i].key << " ��������: " << storage[i].value << " ������: " << storage[i].state << endl;
	}
}
void HTable::print_in_file() {
	ofstream out_file("D:\\in-outs\\hash table.txt");
	for (int i = 0; i < size; i++) {
		out_file << "[ " << i << " ] " << setw(6) << storage[i].key << endl;
	}
	out_file.close();
	cout << "������� ��������� � ����!" << endl;
}

//CHECKS
bool check_key(string key) {
	if (key.length() != 6) return false;
	for (int i = 0; i < 6; i++) {
		if (TypeOfKey[i] == '�') {
			if (key[i] < 48 || key[i] > 57) return false;
		}
		if (TypeOfKey[i] == '�') {
			if (key[i] < 65 || key[i] > 90) return false;
		}
	}
	return true;
}
string inp_key() {
	string key;
	cout << "������� ���� � ������� ������: ";
	cin >> key;
	while (!check_key(key) || cin.fail()) {
		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
		cout << "�������� ����. ��� ���: ";
		cin >> key;
	}
	return key;
}
int inp_val() {
	int val;
	cout << "������� ������������� ����� �����: ";
	cin >> val;
	while (cin.fail() || val < 0) {
		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
		cout << "�������� ����. ������������� ����� �����: ";
		cin >> val;
	}
	return val;
}


void gen(HTable &table) {
	for (int i = 0; i < table.size * 0.7 - 1; i++)
	{
		string key = "";
		//���� ��������� ����� ����� ����� �� �������
		for (int j = 0; j < 6; j++) {
			int r_number = TypeOfKey[j] == '�' ? rand() % 10 + 48 : rand() % 26 + 65;
			key += char(r_number);
		}
		table.add(Entry(key, i));
	}
}


int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "rus");

	int size = 4;

	HTable table(size);
	string key;
	int value;

	int menu_state = 0;
	while (menu_state == 0) {
		cout << "������� 1, ����� �������� ������." << endl;
		cout << "������� 2, ����� ������� ������." << endl;
		cout << "������� 3, ����� ������� �������." << endl;
		cout << "������� 4, ����� �������� � ����." << endl;
		cout << "������� 5, ����� ����� �� �����." << endl;
		cout << "������� 6, ����� ����� �� ��������." << endl;
		cout << "������� 7, ����� ��������� ������ 70% ������� �����������." << endl;
		cout << "������� 8, ����� ������� ����� �������." << endl;
		cout << "������� 9 ��� ������." << endl;

		cin >> menu_state;
		if (menu_state == 1) {
			key = inp_key();
			value = inp_val();
			Entry e = Entry(key, value);
			value = table.add(e);
			cout << "������ ��������� � ������� " << value << endl;
			menu_state = 0;
		}
		else if (menu_state == 2) {
			key = inp_key();
			if (table.remove(key))
				cout << "������ �������." << endl;
			else 
				cout << "������ �� ����." << endl;
			menu_state = 0;
		}
		else if (menu_state == 3) {
			table.print();
			menu_state = 0;
		}
		else if (menu_state == 4) {
			table.print_in_file();
			menu_state = 0;
		}
		else if (menu_state == 5) {
			key = inp_key();
			int a = table.search_key(key);
			if (a == -1) cout << "������ � ����� ������ ��� � �������." << endl;
			else cout << "������ ��������� � �������� " << a << endl;
			menu_state = 0;
		}
		else if (menu_state == 6) {
			value = inp_val();
			int a = table.search_value(value);
			if (a == -1) cout << "������ � ����� ��������� ��� � �������." << endl;
			else cout << "������ ��������� � �������� " << a << endl;
			menu_state = 0;
		}
		else if (menu_state == 7) {
			gen(table);
			cout << "������� ���������." << endl;
			menu_state = 0;
		}
		else if (menu_state == 8) {
			cout << "������� ������" << endl;
			value = inp_val();
			table = HTable(value);
			cout << "������� �������� " << value << " �������." << endl;
			menu_state = 0;
		}
		else if (menu_state == 9) {
			break;
		}
		else {
			cout << "�������� ����." << endl;
			menu_state = 0;
			while (cin.fail()) {
				cin.clear();
				cin.ignore(cin.rdbuf()->in_avail());
			}
		}
	}
	return 0;
}