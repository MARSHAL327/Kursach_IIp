#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <cwchar>
using namespace std;

//===================
// КОНСТАНТЫ
//===================
char filename[] = "person.txt";

// коды клавиш
const int up = 72,
down = 80,
enter = 13,
esc = 27,
del = 83;

//названия пунктов
const string items[5] = {	"   Ввод данных            ",	"   Печать данных          ",	"   Запись данных в файл   ",	"   Поиск по фамилии       ",	"   Выход                  " };


//===================
// ДАННЫЕ
//===================
struct info {
	string cipher;
	string department_code;
	string all_time;
	string time_cpu;
	string fio;
};


//===================
// СПИСОК
//===================
struct time_task {
	info d;
	time_task* next;
	time_task* prev;
};


//===================
// ИНТЕРФЕЙС
//===================
time_task* print(time_task* beg, int active); // ВЫВОД ДАННЫХ
void print_info(const time_task& t); // ПЕЧАТЬ СОДЕРЖИМОГО
void print_menu(int sym, const string items[]); // ШАБЛОН ПЕЧАТИ МЕНЮ
time_task* input(time_task* end, const time_task& s); // ВЫДЕЛЕНИЕ ПАМЯТИ
time_task* input(const time_task& s); // ВЫДЕЛЕНИЕ ПАМЯТИ ДЛЯ ПЕРВОГО ЭЛЕМЕНТА
time_task input_info(time_task* beg); // ВВОД ДАННЫХ
time_task* delete_el(time_task* beg, int num_del); // УДАЛЕНИЕ
int read_file(char* filename, time_task** beg, time_task** end); // ЧТЕНИЕ ИЗ ФАЙЛА
int write_file(char* filename, time_task* temp); // ЗАПИСЬ В ФАЙЛ
int menu(int& active, const string items[]); // МЕНЮ
void SetColor(int text, int bg); // установка цвета текста и фона 
void find(time_task* beg); // поик элемента по фамилии 



//===================
// ОСНОВНАЯ ПРОГРАММА
//===================
int main() {
	SetColor(0, 0); // устанавливаем цвет текста и заднего фона чёрным
	::SendMessage(::GetConsoleWindow(), WM_SYSKEYDOWN, VK_RETURN, 0x20000000);

	int item = 0;
	time_task* beg = 0,
		* end = 0;

	read_file(filename, &beg, &end);

	while (1) {
		int current = 1;
		while (1) {
			switch (menu(current, items)) {
				// Добавление элемента в список
			case 1:
				system("cls");
				if (beg) {
					end = input(end, input_info(beg));
				}
				else {
					beg = input(input_info(beg));
					end = beg;
				}
				break;

				// Печать элементов
			case 2:
				system("cls");
				beg = print(beg, 1);
				break;

				// Запись в файл
			case 3:
				system("cls");
				write_file(filename, beg);
				break;

				// Поиск элемента
			case 4:
				system("cls");
				find(beg);
				break;

				// выход из программы
			case 5: return 0;
			default:
				cout << "Неверно введён номер!" << endl;
				system("pause");
				break;
			}
		}
	}

}

//===================
// ФУНКЦИИ
//===================


// ==========ВЫДЕЛЕНИЕ ПАМЯТИ ДЛЯ ПЕРВОГО ЭЛЕМЕНТА==========
time_task* input(const time_task& s) {
	time_task* beg = new time_task;
	*beg = s;
	beg->next = 0;
	beg->prev = 0;
	return beg;
}

// ==========ВЫДЕЛЕНИЕ ПАМЯТИ==========
time_task* input(time_task* end, const time_task& s) {
	time_task* newE = new time_task;
	*newE = s;
	newE->next = 0;
	newE->prev = end;
	end->next = newE;
	end = newE;
	return end;
}

// ==========ВВОД ДАННЫХ==========
time_task input_info(time_task* beg) {
	time_task t;
	time_task* temp = beg;

	cout << "Введите шифр задания (8 символов)" << endl;
	cin >> t.d.cipher;
	cout << "Введите код отдела (3 символа)" << endl;
	cin >> t.d.department_code;
	cout << "Введите ФИО (15 символов)" << endl;
	cin >> t.d.fio;
	do {
		cout << "Введите общее время прохождения задания (5 символов)" << endl;
		cin.ignore();
		cin >> t.d.all_time;
		cout << "Введите время центрального процессора (5 символов)" << endl;
		cin.ignore();
		cin >> t.d.time_cpu;

		if (stoi(t.d.all_time) < stoi(t.d.time_cpu)) {
			cout << "\n\nОбщее время должно быть больше времени центрального процессора!\n";
		}
		else break;

	} while (true);
	system("pause");

	return t;
}

// ==========РАСЧЁТ ПРОЦЕНТА ПРОЦЕССОРНОГО ВРЕМЕНИ==========
float percent_time_cpu(float a, float b) {
	return (b * 100) / a;
}

// ПЕЧАТЬ СОДЕРЖИМОГО
void print_info(const time_task& t) {
	cout << "|" << setw((__int64)1 + t.d.cipher.length()) << t.d.cipher;
	cout << setw(((__int64)18 - t.d.cipher.length()) + t.d.department_code.length()) << t.d.department_code;
	cout << setw(((__int64)16 - t.d.department_code.length()) + t.d.fio.length()) << t.d.fio;
	cout << setw(((__int64)18 - t.d.fio.length()) + t.d.all_time.length()) << t.d.all_time;
	cout << setw(((__int64)17 - t.d.all_time.length()) + t.d.time_cpu.length()) << t.d.time_cpu << setw(9 - t.d.time_cpu.length()) << "|";
	cout << " " << percent_time_cpu(stof(t.d.all_time), stof(t.d.time_cpu)) << setprecision(4) << fixed << setw(23) << "|";
}

// ==========ВЫВОД ДАННЫХ==========
time_task* print(time_task * beg, int active) {

	if (!beg) {
		cout << "Список пуст" << endl;
		system("pause");
		return beg;
	}

	int sum_all_time = 0,
		sum_time_cpu = 0,
		i = 1,
		num_del = 0;

	time_task* temp = beg;

	cout << "+------------------------------------------------------------------------------+-------------------------------+" << endl;
	cout << "| Шифр задания      Код отдела      ФИО               Общее время      Время ЦП| Процент процессорного времени |" << endl;
	cout << "+------------------------------------------------------------------------------+-------------------------------+" << endl;

	for (; temp; temp = temp->next, i++) {
		if (i == active) {
			SetColor(7, 5);
			num_del = stoi(temp->d.cipher);
		}
		print_info(*temp);
		SetColor(7, 0);

		sum_all_time += stoi(temp->d.all_time); // Сумма общего времени
		sum_time_cpu += stoi(temp->d.time_cpu); // Сумма времени ЦП

		cout << endl;
	}

	cout << "+------------------------------------------------------------------------------+-------------------------------+" << endl;
	cout << "Сумма общего времени: " << sum_all_time << endl;
	cout << "Сумма времени ЦП: " << sum_time_cpu << endl;
	cout << "+------------------------------------------------------------------------------+-------------------------------+" << endl;
	
	// считывание клавиш
	do {
		switch (_getwch()) {
		case up: 
			if (active > 1) {
				system("cls");
				print(beg, --active);
			}
			break;
		case down: 
			if (active < i - 1) {
				system("cls");
				print(beg, ++active);
			}
			break;
		case esc:
			return beg;
			break;
		case del:
			beg = delete_el(beg, num_del);
			system("cls");
			return beg;
			break;
		case enter:
			
			break;
		}
	} while (1);
}

// ==========УДАЛЕНИЕ==========
time_task* delete_el(time_task * beg, int num_del) {
	time_task* temp;
	time_task* buf;

	if (!beg) {
		cout << "Список пуст" << endl;
		return 0;
	}

	temp = beg;

	// если один элемент в списке
	if (beg->next == 0) {
		delete temp;
		return 0;
	}
	
	while (temp) {
		if (num_del == stoi(temp->d.cipher)) { // если введённый номер совпал с id
			buf = temp->next;
			
			// если удаляется второй элемент
			if (temp->prev == 0) {
				buf->prev = 0;
				delete temp;
				return buf;
			}

			if (buf != 0) buf->prev = temp->prev; // если следующий элемент не 0 

			buf = temp->prev;
			buf->next = temp->next;
			delete temp;
			return beg;
		}
		
		temp = temp->next;
	}

	cout << "\nТакого шифр не существует!";
	return beg;
}

// ==========ПОИСК ЭЛЕМЕНТА ПО ФАМИЛИИ==========
void find(time_task* beg) {
	time_task* temp = beg;
	string find_el;

	system("cls");

	if (!beg) {
		cout << "Список пуст" << endl;
		return;
	}

	cout << "Введите ФИО сотрудника для поиска" << endl;
	cin >> find_el;

	while (temp) {
		if (find_el == temp->d.fio) {
			cout << "+------------------------------------------------------------------------------+-------------------------------+" << endl;
			cout << "| Шифр задания      Код отдела      ФИО               Общее время      Время ЦП| Процент процессорного времени |" << endl;
			cout << "+------------------------------------------------------------------------------+-------------------------------+" << endl;
			print_info(*temp);
			cout << "\n+------------------------------------------------------------------------------+-------------------------------+" << endl;
			cout << endl;
			system("pause");
			return;
		}
		temp = temp->next;
	}

	cout << "Сотрудника с такой фамилией нет!" << endl;
}

// ==========ЧТЕНИЕ ИЗ ФАЙЛА==========
int read_file(char* filename, time_task **beg, time_task **end) {
	int k = 0;
	ifstream fin;
	fin.open(filename);

	if (!fin) {
		cout << "Нет файла" << filename << endl;
		return 1;
	}

	time_task t;
	*beg = 0;
	while (getline(fin, t.d.cipher)) {
		getline(fin, t.d.department_code);
		getline(fin, t.d.fio);
		getline(fin, t.d.all_time);
		getline(fin, t.d.time_cpu);
		if (*beg)
			* end = input(*end, t);
		else {
			*beg = input(t); *end = *beg;
		}
	}

	return 0;
}

// ==========ЗАПИСЬ В ФАЙЛ==========
int write_file(char* filename, time_task * temp) {
	ofstream fout;
	fout.open(filename);

	if (!fout) {
		cout << "Невозможно открыть файл для записи" << endl;
		return 1;
	}

	while (temp) {
		fout << temp->d.cipher << endl;
		fout << temp->d.department_code << endl;
		fout << temp->d.fio << endl;
		fout << temp->d.all_time << endl;
		fout << temp->d.time_cpu << endl;
		temp = temp->next;
	}

	cout << "Данные сохранены в файле: " << filename << endl;
	cout << "==============================" << endl;
	system("pause");
	return 0;
}


// ==========ШАБЛОН ПЕЧАТИ МЕНЮ==========
void print_menu(int sym, const string items[]) {
	const int N_ITEMS = 5;

	//========================
	//========================
	//========================
	HANDLE hCon;
	COORD cPos;
	int width = 0, height = 0;

	// вытакиваем ширину и высоту
	hCon = GetStdHandle(-12);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	if (GetConsoleScreenBufferInfo(hCon, &consoleInfo))
	{
		width = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
		height = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;
	}

	// меняем положение меню
	hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	cPos.Y = 10;
	cPos.X = 0;
	SetConsoleCursorPosition(hCon, cPos);

	// меняем размер шрифта
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 0;                   // Width of each character in the font
	cfi.dwFontSize.Y = 24;                  // Height
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);

	//========================
	//========================
	//========================
	for (int i = 1; i <= N_ITEMS; i++) {
		
		SetColor(7, 0);
		if (i == sym) {
			SetColor(7, 5);
		}
		cout << items[i - 1] << endl;
		SetColor(7, 0); 
	}
}

// ==========МЕНЮ==========
int menu(int& active, const string items[]) {
	char buf;

	do {
		system("CLS");
		switch (active) {
		case 1:
			print_menu(active, items);
			break;
		case 2:
			print_menu(active, items);
			break;
		case 3:
			print_menu(active, items);
			break;
		case 4:
			print_menu(active, items);
			break;
		case 5:
			print_menu(active, items);
			break;
		}
		buf = _getwch();
		switch (buf) {
		case up: // клавиша вверх
			if (active > 1) active--;
			break;
		case down: // клавиша вниз
			if (active < 5) active++;
			break;
		case enter: // клавиша enter
			return active;
		case esc: // клавиша escape
			return 5;
		}
	} while (1);
}

// ==========УСТАНОВКА ЦВЕТА ТЕКСТА И ФОНА==========
void SetColor(int text, int bg) {
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdOut, (WORD)((bg << 4) | text));
}
