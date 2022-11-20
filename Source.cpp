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


// для проверки функции хэширования, по методе
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
		//ключ генерится сразу здесь чтобы не хранить
		for (int j = 0; j < 6; j++) {
			int r_number = TypeOfKey[j] == 'ц' ? rand() % 10 + 48 : rand() % 26 + 65;
			tmp->key += char(r_number);
		}

		hashNum = 0 - minHash;
		for (int j = 0; j < 6; j++)
			hashNum = hashNum + (tmp->key[j] * tmp->key[j]); //сложение квадратов и по модулю
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
	Keys* MassiveOfSegments = new Keys[NumOfSegments]; //выделяю память для массива в куче, ибо он не влезает в стек
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
		//если по адресу пусто,
		if (storage[hashNum].state < 1)
		{
			//записать его туда и обновить коэффициент. Расширить если нужно
			storage[hashNum] = e;
			filled += 1;
			if (filled > (size * coeff)) {
				expand();
				return add(e);
			}
			return hashNum;
		}
		//если занят, проверить ключ на одинаковость, если одинаковые то запись уже там есть
		else if (storage[hashNum].key == e.key)
			return hashNum;
		//иначе продолжать пытаться вставить ключ в другую ячейку линейным пробированием
		hashNum += step;
		if (hashNum >= size)
			hashNum -= size;

	}
	//достигнут предел по коллизиям, значит мы обошли 70% таблицы и они были заполнены. увеличиваем
	expand();
	return add(e);
}
void HTable::expand() {
	cout << "Расширение таблицы." << endl;
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
		//если по этому адресу пустой элемент, то элемента в таблице нет
		if (storage[hashNum].state == 0)
			return false;

		//если он есть, проверить на одинаковость
		else if (storage[hashNum].state == 1 && storage[hashNum].key == key) {
			storage[hashNum].state = -1;
			filled -= 1;
			return true;
		}
		//если удалённый или не тот, продолжаем поиск
		cout << "Элемент с коллизией: " << storage[hashNum].key << ". Состояние: " << (storage[hashNum].state == 1 ? "заполнен" : "удалён");
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
		//если по этому адресу пустой элемент, то ключа в таблице нет
		if (storage[hashNum].state == 0)
			return -1;

		//если он есть, проверить на одинаковость
		else if (storage[hashNum].state == 1 && storage[hashNum].key == key)
			return hashNum;

		//если удалённый или не тот, продолжаем поиск
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
	cout << "Введите индексы начала и конца интересующей части таблицы через пробел (ввод не по образцу покажет всю таблицу): ";
	cin.clear();
	cin >> start >> finish;

	if (cin.fail() || cin.rdbuf()->in_avail() != 1 || start > finish || start >= size || finish >= size) {
		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
		start = 0;
		finish = size - 1;
	}

	cout << "Таблица:" << endl;
	for (int i = start; i <= finish; i++) {
		cout << "[ " << i << " ] " << setw(6) << storage[i].key << " значение: " << storage[i].value << " статус: " << storage[i].state << endl;
	}
}
void HTable::print_in_file() {
	ofstream out_file("D:\\in-outs\\hash table.txt");
	for (int i = 0; i < size; i++) {
		out_file << "[ " << i << " ] " << setw(6) << storage[i].key << endl;
	}
	out_file.close();
	cout << "Таблица загружена в файл!" << endl;
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
	cout << "Введите положительное целое число: ";
	cin >> val;
	while (cin.fail() || val < 0) {
		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
		cout << "Неверный ввод. Положительное целое число: ";
		cin >> val;
	}
	return val;
}


void gen(HTable &table) {
	for (int i = 0; i < table.size * 0.7 - 1; i++)
	{
		string key = "";
		//ключ генерится сразу здесь чтобы не хранить
		for (int j = 0; j < 6; j++) {
			int r_number = TypeOfKey[j] == 'ц' ? rand() % 10 + 48 : rand() % 26 + 65;
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
		cout << "Введите 1, чтобы добавить запись." << endl;
		cout << "Введите 2, чтобы удалить запись." << endl;
		cout << "Введите 3, чтобы вывести таблицу." << endl;
		cout << "Введите 4, чтобы добавить в файл." << endl;
		cout << "Введите 5, чтобы найти по ключу." << endl;
		cout << "Введите 6, чтобы найти по значению." << endl;
		cout << "Введите 7, чтобы заполнить меньше 70% таблицы генератором." << endl;
		cout << "Введите 8, чтобы создать новую таблицу." << endl;
		cout << "Введите 9 для выхода." << endl;

		cin >> menu_state;
		if (menu_state == 1) {
			key = inp_key();
			value = inp_val();
			Entry e = Entry(key, value);
			value = table.add(e);
			cout << "Запись добавлена в сегмент " << value << endl;
			menu_state = 0;
		}
		else if (menu_state == 2) {
			key = inp_key();
			if (table.remove(key))
				cout << "Запись удалена." << endl;
			else 
				cout << "Записи не было." << endl;
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
			if (a == -1) cout << "Записи с таким ключом нет в таблице." << endl;
			else cout << "Запись находится в сегменте " << a << endl;
			menu_state = 0;
		}
		else if (menu_state == 6) {
			value = inp_val();
			int a = table.search_value(value);
			if (a == -1) cout << "Записи с таким значением нет в таблице." << endl;
			else cout << "Запись находится в сегменте " << a << endl;
			menu_state = 0;
		}
		else if (menu_state == 7) {
			gen(table);
			cout << "Таблица заполнена." << endl;
			menu_state = 0;
		}
		else if (menu_state == 8) {
			cout << "Введите размер" << endl;
			value = inp_val();
			table = HTable(value);
			cout << "Таблица размером " << value << " создана." << endl;
			menu_state = 0;
		}
		else if (menu_state == 9) {
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