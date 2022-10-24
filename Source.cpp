#include<fstream>
#include<iostream>
#include<string>
#include<ctime>
#include <windows.h>
#include <stdlib.h>
#include<iomanip>
using namespace std;
//Формат ключа:  БцццББ 
//Количество сегментов: 2000
// Линейное опробование


//DONE- создание хэш-таблицы
//DONE - добавление элементов
//DONE- просмотр
//DONE - поиск элементов по ключу;
//DONE- поиск элементов по номеру сегмента
//DONE- выгружать содержимое хеш - таблицы в файл для построения гистограммы в MS Excel
//DONE- удаление элементов;
//DONE - проверка формата вводимого ключа;
//DONE- при удалении элементов из хэш - таблицы, в программе должен быть реализован алгоритм,
//		 позволяющий искать элементы, вызвавшие коллизию с удаленным;
//DONE- алгоритм для переполнения



const string TypeOfKey = "бцццбб";
const int StepOfLinearSampling = 1;

int NumOfSegments = 4;
int MaxNumOfCollisions = NumOfSegments * 7 / 10; //for expanding
int FilledSegments = 0;

// для проверки функции хэширования, по методе
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

		//ключ генерится сразу здесь чтобы не хранить
		for (int j = 0; j < 6; j++) {
			int r_number = TypeOfKey[j] == 'ц' ? rand() % 10 + 48 : rand() % 26 + 65;
			tmp->key += char(r_number);
		}


		//отыгрывается хэш-функция
		hashNum = 0 - minHash;
		for (int j = 0; j < 6; j++)
			hashNum = hashNum + (tmp->key[j] * tmp->key[j]); // это моя, сложение квадратов и по модулю
		hashNum = hashNum % NumOfSegments;


		MassiveOfSegments[hashNum].numOfH++; //увеличиваем количество сгенеренных ключей подходящих этой ячейке
		for (int j = 0; j <= MaxNumOfCollisions; j++) //начинаем цикл где пытаемся вставлять
		{
			//если по этому адресу ключа ещё нет,
			if (MassiveOfSegments[hashNum].key.empty()) 
			{
				//записать его туда
				MassiveOfSegments[hashNum].key = tmp->key; //хранить. чтобы избежать повторов
				break; //и выйти
			}//дальше если адрес уже занят и мы почти превысили количество возможных коллизий уже
			else if (j == MaxNumOfCollisions - 1) 
			{
				//добавить этот ключ в плохие ключи
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
			//иначе продолжать пытаться вставить ключ в другую ячейку своим линейным пробированием
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
	Keys* MassiveOfSegments = new Keys[NumOfSegments]; //выделяю память для массива в куче, ибо он не влезает в стек
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
int my_hash(string key) { //сложение квадратов кодов и по модулю
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
		if (TypeOfKey[i] == 'ц') {
			if (key[i] < 48 || key[i] > 57) return false;
		}
		if (TypeOfKey[i] == 'б') {
			if (key[i] < 65 || key[i] > 90) return false;
		}
	}
	return true;
}
string inp_key() {
	string key;
	cout << "Введите ключ в формате БцццББ: ";
	cin >> key;
	while (!check_key(key) || cin.fail()) {
		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
		cout << "Неверный ввод. Ещё раз: ";
		cin >> key;
	}
	return key;
}
int inp_val() {
	int val;
	cout << "Введите число для хранения: ";
	cin >> val;
	while (cin.fail()) {
		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
		cout << "Неверный ввод. Число: ";
		cin >> val;
	}
	return val;
}


//MANIPULATION
int add(Entry*& table, Entry e);

void expand(Entry*& table) {

	cout << "Расширение таблицы." << endl;
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
		//если по адресу пусто,
		if (table[hashNum].state < 1)
		{
			//записать его туда и обновить коэффициент. Расширить если нужно
			table[hashNum] = e;
			FilledSegments += 1;
			if (FilledSegments > MaxNumOfCollisions) {
				expand(table);
				return add(table, e);
			}
			return hashNum;
		}
		//если занят, проверить ключ на одинаковость, если одинаковые то запись уже там есть
		else if (table[hashNum].key == e.key)
				return hashNum;
		//иначе продолжать пытаться вставить ключ в другую ячейку линейным пробированием
		hashNum += StepOfLinearSampling;
		if (hashNum >= NumOfSegments)
			hashNum -= NumOfSegments;
		
	}
	//достигнут предел по коллизиям, значит мы обошли 70% таблицы и они были заполнены. увеличиваем
	expand(table);
	return add(table, e);
}
bool remove(Entry*& table, string key) { //false if wrong index or no elem; prints out collided keys
	int hashNum = my_hash(key);
	for (int j = 0; j < MaxNumOfCollisions; j++)
	{
		//если по этому адресу пустой элемент, то элемента в таблице нет
		if (table[hashNum].state == 0)
			return false;

		//если он есть, проверить на одинаковость
		else if (table[hashNum].state == 1 && table[hashNum].key == key) {
			table[hashNum].state = -1;
			FilledSegments -= 1;
			return true;
		}
		//если удалённый или не тот, продолжаем поиск
		cout << "Элемент с коллизией: " << table[hashNum].key << ". Состояние: " << (table[hashNum].state == 1 ? "заполнен" : "удалён");
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
		//если по этому адресу пустой элемент, то ключа в таблице нет
		if (table[hashNum].state == 0)
			return -1;

		//если он есть, проверить на одинаковость
		else if (table[hashNum].state == 1 && table[hashNum].key == key)
			return hashNum;

		//если удалённый или не тот, продолжаем поиск
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
	cout << "Введите индексы начала и конца интересующей части таблицы через пробел (ввод не по образцу покажет всю таблицу): ";
	cin.clear();
	cin >> start >> finish;

	if (cin.fail() || cin.rdbuf()->in_avail() != 1 || start > finish || start >= NumOfSegments || finish >= NumOfSegments) {
		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
		start = 0;
		finish = NumOfSegments - 1;
	}

	cout << "Таблица:" << endl;
	for (int i = start; i <= finish; i++) {
		cout << "[ " << i << " ] " << setw(6) << table[i].key << " значение: " << table[i].value << " статус: " << table[i].state << endl;
	}
}
void print_in_file(Entry* table) {
	ofstream out_file("D:\\in-outs\\hash table.txt");
	for (int i = 0; i < NumOfSegments; i++) {
		out_file << "[ " << i << " ] " << setw(6) <<  table[i].key << endl;
	}
	out_file.close();
	cout << "Таблица загружена в файл!" << endl;
}


int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "rus");

	Entry* table = new Entry[NumOfSegments]; //создание новой таблицы
	string key;
	int value;

	int menu_state = 0;
	while (menu_state == 0) {
		cout << "Введите 1, чтобы добавить запись." << endl;
		cout << "Введите 2, чтобы удалить запись." << endl;
		cout << "Введите 3, чтобы вывести таблицу." << endl;
		cout << "Введите 4, чтобы добавить в файл." << endl;
		cout << "Введите 5, чтобы найти по ключу." << endl;
		cout << "Введите 6, чтобы найти по значению." << endl;
		cout << "Введите 7 для выхода." << endl;

		cin >> menu_state;
		if (menu_state == 1) {
			key = inp_key();
			value = inp_val();
			Entry neww = Entry(key, value);
			value = add(table, neww);
			cout << "Запись добавлена в сегмент " << value << endl;
			menu_state = 0;
		}
		else if (menu_state == 2) {
			key = inp_key();
			if (remove(table, key))
				cout << "Запись удалена." << endl;
			else 
				cout << "Записи не было." << endl;
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
			if (a == -1) cout << "Записи с таким ключом нет в таблице." << endl;
			else cout << "Запись находится в сегменте " << a << endl;
			menu_state = 0;
		}
		else if (menu_state == 6) {
			value = inp_val();
			int a = search_value(table, value);
			if (a == -1) cout << "Записи с таким значением нет в таблице." << endl;
			else cout << "Запись находится в сегменте " << a << endl;
			menu_state = 0;
		}
		else if (menu_state == 7) {
			break;
		}
		else {
			cout << "Неверный ввод." << endl;
			menu_state = 0;
			while (cin.fail()) {
				cin.clear();
				cin.ignore(cin.rdbuf()->in_avail());
			}
		}
	}
	return 0;
}