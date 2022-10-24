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
const int StepOfLinearSampling = 1;

int NumOfSegments = 4;
int MaxNumOfCollisions = NumOfSegments * 7 / 10; //for expanding
int FilledSegments = 0;

// ��� �������� ������� �����������, �� ������
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

		//���� ��������� ����� ����� ����� �� �������
		for (int j = 0; j < 6; j++) {
			int r_number = TypeOfKey[j] == '�' ? rand() % 10 + 48 : rand() % 26 + 65;
			tmp->key += char(r_number);
		}


		//������������ ���-�������
		hashNum = 0 - minHash;
		for (int j = 0; j < 6; j++)
			hashNum = hashNum + (tmp->key[j] * tmp->key[j]); // ��� ���, �������� ��������� � �� ������
		hashNum = hashNum % NumOfSegments;


		MassiveOfSegments[hashNum].numOfH++; //����������� ���������� ����������� ������ ���������� ���� ������
		for (int j = 0; j <= MaxNumOfCollisions; j++) //�������� ���� ��� �������� ���������
		{
			//���� �� ����� ������ ����� ��� ���,
			if (MassiveOfSegments[hashNum].key.empty()) 
			{
				//�������� ��� ����
				MassiveOfSegments[hashNum].key = tmp->key; //�������. ����� �������� ��������
				break; //� �����
			}//������ ���� ����� ��� ����� � �� ����� ��������� ���������� ��������� �������� ���
			else if (j == MaxNumOfCollisions - 1) 
			{
				//�������� ���� ���� � ������ �����
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
			//����� ���������� �������� �������� ���� � ������ ������ ����� �������� �������������
			else
			{
				hashNum += StepOfLinearSampling;
				if (hashNum > NumOfSegments)
					hashNum -= NumOfSegments;
			}
		}
	}
}
void PutThisInFile() {
	srand(time(NULL));
	ofstream out_file("D:\\in-outs\\hash results.txt");
	Keys* MassiveOfSegments = new Keys[NumOfSegments]; //������� ������ ��� ������� � ����, ��� �� �� ������� � ����
	Keys* BadKeys = NULL;
	HashFunctionCounter(MassiveOfSegments, BadKeys);
	for (int i = 0; i < NumOfSegments; i++) {
		out_file << MassiveOfSegments[i].numOfH << endl;
	}
	out_file.close();
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
int my_hash(string key) { //�������� ��������� ����� � �� ������
	int hashNum = -19587;
	for (int j = 0; j < 6; j++)
		hashNum = hashNum + (key[j] * key[j]);
	hashNum = hashNum % NumOfSegments;
	return hashNum;
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
	cout << "������� ����� ��� ��������: ";
	cin >> val;
	while (cin.fail()) {
		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
		cout << "�������� ����. �����: ";
		cin >> val;
	}
	return val;
}


//MANIPULATION
int add(Entry*& table, Entry e);

void expand(Entry*& table) {

	cout << "���������� �������." << endl;
	int oldNum = NumOfSegments;
	NumOfSegments *= 2;
	MaxNumOfCollisions = NumOfSegments * 7 / 10;
	FilledSegments = 0;
	Entry* newTable = new Entry[NumOfSegments];

	for (int i = 0; i < oldNum; i++) {
		if (table[i].state == 1)
			add(newTable, table[i]);
	}

	Entry* tmp = table;
	delete[]tmp;
	table = newTable;
}
int add(Entry*& table, Entry e) {
	int hashNum = my_hash(e.key);

	for (int j = 0; j < MaxNumOfCollisions; j++)
	{
		//���� �� ������ �����,
		if (table[hashNum].state < 1)
		{
			//�������� ��� ���� � �������� �����������. ��������� ���� �����
			table[hashNum] = e;
			FilledSegments += 1;
			if (FilledSegments > MaxNumOfCollisions) {
				expand(table);
				return add(table, e);
			}
			return hashNum;
		}
		//���� �����, ��������� ���� �� ������������, ���� ���������� �� ������ ��� ��� ����
		else if (table[hashNum].key == e.key)
				return hashNum;
		//����� ���������� �������� �������� ���� � ������ ������ �������� �������������
		hashNum += StepOfLinearSampling;
		if (hashNum >= NumOfSegments)
			hashNum -= NumOfSegments;
		
	}
	//��������� ������ �� ���������, ������ �� ������ 70% ������� � ��� ���� ���������. �����������
	expand(table);
	return add(table, e);
}
bool remove(Entry*& table, string key) { //false if wrong index or no elem; prints out collided keys
	int hashNum = my_hash(key);
	for (int j = 0; j < MaxNumOfCollisions; j++)
	{
		//���� �� ����� ������ ������ �������, �� �������� � ������� ���
		if (table[hashNum].state == 0)
			return false;

		//���� �� ����, ��������� �� ������������
		else if (table[hashNum].state == 1 && table[hashNum].key == key) {
			table[hashNum].state = -1;
			FilledSegments -= 1;
			return true;
		}
		//���� �������� ��� �� ���, ���������� �����
		cout << "������� � ���������: " << table[hashNum].key << ". ���������: " << (table[hashNum].state == 1 ? "��������" : "�����");
		hashNum += StepOfLinearSampling;
		if (hashNum >= NumOfSegments)
			hashNum -= NumOfSegments;

	}
	return false;
}

//SEARCH
int search_key(Entry* table, string key) {
	int hashNum = my_hash(key);
	for (int j = 0; j < MaxNumOfCollisions; j++)
	{
		//���� �� ����� ������ ������ �������, �� ����� � ������� ���
		if (table[hashNum].state == 0)
			return -1;

		//���� �� ����, ��������� �� ������������
		else if (table[hashNum].state == 1 && table[hashNum].key == key)
			return hashNum;

		//���� �������� ��� �� ���, ���������� �����
		hashNum += StepOfLinearSampling;
		if (hashNum >= NumOfSegments)
			hashNum -= NumOfSegments;

	}
	return -1;
}
int search_value(Entry* table, int value) { //checking all of the table
	for (int j = 0; j < NumOfSegments; j++) {
		if (table[j].value == value && table[j].state == 1) return j;
	}
	return -1;
}

//DISPLAY
void print(Entry* table) {
	int start, finish;
	cout << "������� ������� ������ � ����� ������������ ����� ������� ����� ������ (���� �� �� ������� ������� ��� �������): ";
	cin.clear();
	cin >> start >> finish;

	if (cin.fail() || cin.rdbuf()->in_avail() != 1 || start > finish || start >= NumOfSegments || finish >= NumOfSegments) {
		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
		start = 0;
		finish = NumOfSegments - 1;
	}

	cout << "�������:" << endl;
	for (int i = start; i <= finish; i++) {
		cout << "[ " << i << " ] " << setw(6) << table[i].key << " ��������: " << table[i].value << " ������: " << table[i].state << endl;
	}
}
void print_in_file(Entry* table) {
	ofstream out_file("D:\\in-outs\\hash table.txt");
	for (int i = 0; i < NumOfSegments; i++) {
		out_file << "[ " << i << " ] " << setw(6) <<  table[i].key << endl;
	}
	out_file.close();
	cout << "������� ��������� � ����!" << endl;
}


int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "rus");

	Entry* table = new Entry[NumOfSegments]; //�������� ����� �������
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
		cout << "������� 7 ��� ������." << endl;

		cin >> menu_state;
		if (menu_state == 1) {
			key = inp_key();
			value = inp_val();
			Entry neww = Entry(key, value);
			value = add(table, neww);
			cout << "������ ��������� � ������� " << value << endl;
			menu_state = 0;
		}
		else if (menu_state == 2) {
			key = inp_key();
			if (remove(table, key))
				cout << "������ �������." << endl;
			else 
				cout << "������ �� ����." << endl;
			menu_state = 0;
		}
		else if (menu_state == 3) {
			print(table);
			menu_state = 0;
		}
		else if (menu_state == 4) {
			print_in_file(table);
			menu_state = 0;
		}
		else if (menu_state == 5) {
			key = inp_key();
			int a = search_key(table, key);
			if (a == -1) cout << "������ � ����� ������ ��� � �������." << endl;
			else cout << "������ ��������� � �������� " << a << endl;
			menu_state = 0;
		}
		else if (menu_state == 6) {
			value = inp_val();
			int a = search_value(table, value);
			if (a == -1) cout << "������ � ����� ��������� ��� � �������." << endl;
			else cout << "������ ��������� � �������� " << a << endl;
			menu_state = 0;
		}
		else if (menu_state == 7) {
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