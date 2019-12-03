#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <cwchar>
#include <winuser.h>
#include <tchar.h>
using namespace std;

//======================
// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ
//======================
string filename;
string All_bd = "mainBD.txt";
float total_el = 0;
int num_pages	 = 5, // Кол-во элементов на одной странице
	width		 = 0, // Ширина окна
	height		 = 0, // Высота окна
	sum_all_time = 0, // Сумма общего времени
	sum_time_cpu = 0, // Сумма времени ЦП
	average_percent_time_cpu = 0; // Средний процент процессорного времени

//===================
// КОНСТАНТЫ
//===================
// коды клавиш
const int up = 72,
down = 80,
right_btn = 77,
left_btn = 75,
enter = 13,
esc = 27,
del = 83;

const int SIZE_arr_filename = 50;

// названия пунктов
const string items[7] = {
	"   Ввод данных               ",
	"   Печать данных             ",
	"   Сохранить данные          ",
	"   Сохранить в другой файл   ",
	"   Поиск                     ",
	"   Выбрать другой файл       ",
	"   Выход из программы        " };

// названия для сортировки 
const string sort_items[5] = {
	"| Шифр задания ",
	"Код отдела ",
	"ФИО ",
	"Общее время ",
	"Время ЦП" };

// массив всех названий файлов
string arr_filename[SIZE_arr_filename] = {
"    Добавить новую БД    "
};

//===================
// ДАННЫЕ
//===================
struct info {
	string cipher;
	string department_code;
	string fio;
	string all_time;
	string time_cpu;
};


//===================
// СПИСОК
//===================
struct time_task {
	info d;
	time_task* next = 0;
	time_task* prev = 0;
};


//===================
// ИНТЕРФЕЙС
//===================
time_task* print(time_task* end, time_task* real_beg ,time_task* beg, int active, int edit_el, int print_count_num_pages, int print_page); // ВЫВОД ДАННЫХ
void print_info(const time_task& t, int active); // ПЕЧАТЬ СОДЕРЖИМОГО
void print_menu(int sym, const string items[], const int N_ITEMS); // ШАБЛОН ПЕЧАТИ МЕНЮ
void input(time_task*& beg, time_task*& end, const time_task& info);
time_task input_info(time_task* beg); // ВВОД ДАННЫХ
time_task* delete_el(time_task* beg, int num_del); // УДАЛЕНИЕ
int read_file(string filename, time_task** beg, time_task** end); // ЧТЕНИЕ ИЗ ФАЙЛА
int read_bin_file(string filename, time_task** beg, time_task** end); // ЧТЕНИЕ ИЗ БИНАРНОГО ФАЙЛА
int write_file(time_task* temp); // ЗАПИСЬ В ФАЙЛ
void write_filetype(time_task* temp, string filename, int el, int filetype); // ОПРЕДЕЛЕНИЕ ТИПА ДЛЯ ЗАПИСИ
int menu(int& active, const string items[], int num_el); // МЕНЮ
void SetColor(int text, int bg); // установка цвета текста и фона 
void find(time_task* beg); // ПОИСК
void edit(time_task* end, time_task* real_beg, time_task* beg, int active, time_task* _edit_ob, int edit_count_num_pages, int edit_page, int edit_i); // РЕДАКТИРОВАНИЕ ЭЛЕМЕНТА
void cls(); // ОЧИСТКА ЭКРАНА БЕЗ МЕРЦАНИЯ
void sort(time_task* beg, int field_for_sort, int sort_direction); // СОРТИРОВКА
int compare(time_task* t_i, time_task* t_j, int num, int compare_direction); // СРАВНЕНИЕ ДАННЫХ ДЛЯ СОРТИРОВКИ
void gotoxy(int xpos, int ypos); // ПЕРЕМЕЩЕНИЕ КУРСОРА НА ВЫБРАННУЮ ПОЗИЦИЮ
time_task* first_start(time_task** beg, time_task** end); // ВЫБОР БД
string sets(size_t size); // АНАЛОГ setw()
float percent_time_cpu(float a, float b); // РАСЧЁТ ПРОЦЕНТА ПРОЦЕССОРНОГО ВРЕМЕНИ
void show_filename(size_t posX, size_t posY); // ПОКАЗЫВАЕМ КАКОЙ РЕДАКТИРУЕТСЯ ФАЙЛ
void show_cursor(bool show); // ВИДИМОСТЬ КУРСОРА
string check_num(string field, int posX, int posY, int max_length, int is_text); // ПРОВЕРКИ НА СИМВОЛ И ДЛИНУ СТРОКИ

//===================
// ОСНОВНАЯ ПРОГРАММА
//===================
int main() {
	//========================
	//========================
	//========================
	HANDLE hCon;
	// вытаскиваем ширину и высоту
	hCon = GetStdHandle(-12);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	if (GetConsoleScreenBufferInfo(hCon, &consoleInfo))
	{
		width = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
		height = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;
	}

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

	SetColor(7, 0); // устанавливаем цвет текста и заднего фона чёрным
	ShowWindow(GetConsoleWindow(), SW_MAXIMIZE); // полноэкранный режим
	show_cursor(FALSE); // убираем курсор
	int item = 0,
		current = 1;
	time_task* beg = 0,
		* end = 0;

	first_start(&beg, &end);

	while (1) {
		show_cursor(FALSE); // убираем курсор
		system("cls");

		show_filename(width - filename.length() - 9, 0);

		// выводим название раздела
		SetColor(7, 5);
		gotoxy(width / 2 + 1, 3);
		cout << "            ";
		gotoxy(width / 2 + 1, 4);
		cout << "    МЕНЮ    ";
		gotoxy(width / 2 + 1, 5);
		cout << "            ";

		switch (menu(current, items, 7)) {
		// Добавление элемента в список
		case 1:
			system("cls");
			show_filename(width - filename.length() - 9, 0);

			input(beg, end, input_info(beg));
			break;

			// Печать элементов
		case 2:
			system("cls");
			show_filename(width - filename.length() - 9, 0);

			beg = print(end, beg, beg, 1, 0, 1, 0);
			break;

			// Запись в файл
		case 3:
			system("cls");
			show_filename(width - filename.length() - 9, 0);
			write_filetype(beg, filename, -1, filename[filename.length() - 3] == 't' ? 1 : 0);
			break;

			// Запись в другой файл
		case 4:
			system("cls");
			show_filename(width - filename.length() - 9, 0);
			write_file(beg);
			break;

			// Поиск элемента
		case 5:
			system("cls");
			show_filename(width - filename.length() - 9, 0);
			find(beg);
			break;

			// выбор другого файла
		case 6:
			if (beg) {
				if (MessageBox(0, L"Хотите сохранить данные?", L"Сохранение", MB_ICONQUESTION | MB_SETFOREGROUND | MB_YESNO) == 6) {
					write_filetype(beg, filename, -1, filename[filename.length() - 3] == 't' ? 1 : 0);
				}
			}
			beg = end = first_start(&beg, &end);
			break;
		case 7:
		case -1:
			if (MessageBox(0, L"Вы уверены, что хотите выйти?", L"Уведомление", MB_ICONQUESTION | MB_SETFOREGROUND | MB_YESNO) == 6) {
				return 0;
			} else break;
		}
	}
}

//===================
// ФУНКЦИИ
//===================

// =========ПОКАЗЫВАЕМ КАКОЙ РЕДАКТИРУЕТСЯ ФАЙЛ=========
void show_filename(size_t posX, size_t posY) {
	gotoxy(posX, posY);
	cout << "Редактируемый файл: ";
	SetColor(7, 8);
	cout << " " << filename << " ";
	SetColor(7, 0);
	gotoxy(0, 0);
}

// =========ВИДИМОСТЬ КУРСОРА=========
void show_cursor(bool show) {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO structCursorInfo;
	GetConsoleCursorInfo(handle, &structCursorInfo);
	structCursorInfo.bVisible = show;
	SetConsoleCursorInfo(handle, &structCursorInfo);
}

// ==========ВЫБОР БД (первый запуск)==========
time_task* first_start(time_task** beg, time_task** end) {
	int k = 1,
		current = 1,
		fl = 0,
		new_line = 0;
	num_pages = 5;
	total_el = 0;

	while (1) {
		k = 1;

		int exit_fl = 0;
		// +++++++ОТКРЫВАЕМ ФАЙЛ СО ВСЕМИ БД+++++++
		ifstream fin;
		fin.open(All_bd);

		if (!fin) {
			MessageBox(0, L"Невозможно открыть файл!", L"Ошибка", MB_ICONERROR | MB_SETFOREGROUND);
			return 0;
		}

		// записываем считанные данные в массив
		while (getline(fin, arr_filename[k++])) {
			if (k >= 50) {
				MessageBox(0, L"Выведены первые 50 файлов!", L"Предупреждение", MB_ICONWARNING | MB_SETFOREGROUND);
			}
		}

		fin.close();
		// ++++++++++++++++++++++++++++++++++++++++++

		system("cls");

		gotoxy(0, 3);
		SetColor(7, 8);
		cout << "     ГОРЯЧИЕ КЛАВИШИ     " << endl;
		SetColor(7, 0);
		cout << "del - удалить файл" << endl;
		cout << "enter - выбрать файл" << endl;
		cout << "esc - выйти из программы" << endl;

		// выводим название раздела
		SetColor(7, 5);
		gotoxy(width / 2 - 12, 3);
		cout << sets(36);
		gotoxy(width / 2 - 12, 4);
		cout << "             ВЫБОР ФАЙЛА            ";
		gotoxy(width / 2 - 12, 5);
		cout << "  выберите откуда считывать данные  ";
		gotoxy(width / 2 - 12, 6);
		cout << sets(36);

		size_t pos = 0;
		int main_bd = menu(current, arr_filename, k - 1);

		// если нажали delete
		if (main_bd <= -2) {
			exit_fl = 1;
			if (MessageBox(0, L"Вы уверены, что хотите удалить?", L"Удаление", MB_ICONQUESTION | MB_YESNO | MB_SETFOREGROUND) == 6) {
				ofstream file_All_bd(All_bd);
				main_bd = abs(main_bd);

				if (main_bd == k - 1) current--; // если удаляется последний элемент

				remove(arr_filename[main_bd - 1].c_str());

				// удаляем элемент из массива
				for (int i = 1; i < k; i++) {
					if (i == main_bd - 1) {
						for (int j = i; j < k - 1; j++) arr_filename[j] = arr_filename[j + 1];
						k--;
						break;
					}
				}

				// запись новых данных в файл
				for (int i = 1; i < k - 1; i++) {
					file_All_bd << arr_filename[i] << endl;
				}

				file_All_bd.close();
			}
		} else if (main_bd == -1) { // если нажали esc
			if (MessageBox(0, L"Вы уверены, что хотите выйти?", L"Уведомление", MB_ICONQUESTION | MB_SETFOREGROUND | MB_YESNO) == 6) {
				exit(0);
			} else exit_fl = 1;
		}
		else {
			filename = arr_filename[main_bd - 1];

			if (main_bd - 1 != 0) {
				pos = filename.find(".txt", filename.length() - 4); // ищем .txt в названии файла, если вернёт -1, то это бинарный

				if (pos != -1) {
					read_file(filename, beg, end);
				}
				else {
					read_bin_file(filename, beg, end);
				}

				// считаем сумму общего времени и сумму времени ЦП
				sum_all_time = sum_time_cpu = average_percent_time_cpu = 0;
				for (time_task* temp = *beg; temp; temp = temp->next) {
					sum_all_time += stoi(temp->d.all_time);
					sum_time_cpu += stoi(temp->d.time_cpu);
					average_percent_time_cpu += percent_time_cpu(stof(temp->d.all_time), stof(temp->d.time_cpu));
				}

				return *beg;
			}
			else return 0;
		}

		if (exit_fl == 0) return 0;
	}
}

// ==========ПЕРЕМЕЩЕНИЕ КУРСОРА НА ВЫБРАННУЮ ПОЗИЦИЮ==========
void gotoxy(int xpos, int ypos)
{
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = xpos; scrn.Y = ypos;
	SetConsoleCursorPosition(hOuput, scrn);
}

// ==========ОЧИСТКА СТРОКИ==========
void clearRow(int row)
{
	DWORD a;
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE); // получаем хэндл окна консоли*/
	COORD coord = { 0, row - 1 }; // получаем координаты строки для очистки 
	CONSOLE_SCREEN_BUFFER_INFO csbi; 
	GetConsoleScreenBufferInfo(hStdOut, &csbi); // получаем данные из буфера вывода консоли
	FillConsoleOutputCharacter(hStdOut, ' ', width, coord, &a); // заполняем строку пробелами
}

// ==========ВЫДЕЛЕНИЕ ПАМЯТИ==========
void input(time_task*& beg, time_task*& end, const time_task& info) {
	if (info.d.cipher == "-1" || info.d.all_time == "-1" || info.d.department_code == "-1" || info.d.fio == "-1" || info.d.time_cpu == "-1") return;
	time_task* newel = new time_task;
	newel->next = NULL;
	newel->prev = NULL;
	newel->d = info.d;
	if (!beg) {
		beg = end = newel;
	}
	else {
		end->next = newel;
		newel->prev = end;
		end = newel;
		num_pages = 5;
	}
}

// ==========ПРОВЕРКИ НА СИМВОЛ И ДЛИНУ СТРОКИ==========
string check_num(string field, int posX, int posY, int max_length, int is_text) {
	/*
	если is_text = 0 - ввод чисел
	если is_text = 1 - ввод символов
	если is_text = 2 - ввод чисел и ввод символов
	*/
	int length = 0;
	int pospos = 0;
	field = "";
	int* posarrays = new int[(__int64)max_length + 1];
	for (int i = 0; i < max_length; i++) {
		posarrays[i] = i;
		if(max_length != i) field += (is_text == 2 ? " " : "_"); // создаём маску
	}
	int pos = posarrays[pospos];
	cout << field;
	while (true) {
		int ch = _getch();
		if (ch == enter && length >= 1) {
			if (is_text == 0 && stoi(field) <= 0) { // если поле <= 0
				MessageBox(0, L"Данные должны быть >= 1!", L"Предупреждение", MB_ICONWARNING | MB_SETFOREGROUND);
			} else break; // выходим, если нажали enter
		} 
		if (ch == esc) return "-1";

		if (ch == 8 && length >= 1) { // если нажали backspace
			length--;
			gotoxy(posX, posY);
			pospos--;
			pos = posarrays[pospos];
			field[pos] = (is_text == 2 ? ' ' : '_');
			cout << field;
		}
		else if (length != max_length && ch != esc && ch != enter && is_text == 2 && ch != 8) {
			length++;
			field[pos] = ch;
			gotoxy(posX, posY);
			pospos++;
			pos = posarrays[pospos];
			cout << field;
		} else if (length != max_length && ch != esc && ch != enter && ch != 8 && (is_text == 1 ? !(ch >= '0' && ch <= '9') : (ch >= '0' && ch <= '9'))) {
			length++;
			field[pos] = ch;
			gotoxy(posX, posY);
			pospos++;
			pos = posarrays[pospos];
			cout << field;
		}
	}

	delete[] posarrays;
	return field.substr(0, length);
}

// ==========ВВОД ДАННЫХ==========
time_task input_info(time_task * beg) {
	time_task t;
	time_task* temp = beg;
	int fl = 0;

	// выводим название раздела
	SetColor(7, 5);
	gotoxy(width / 2 + 1, 3);
	cout << "                   ";
	gotoxy(width / 2 + 1, 4);
	cout << "    ВВОД ДАННЫХ    ";
	gotoxy(width / 2 + 1, 5);
	cout << "                   ";
	SetColor(7, 0);

	gotoxy(width / 2 - 6, 8);
	cout << "Введите шифр задания (8 символов)" << endl;
	gotoxy(width / 2 - 6, 9);
	t.d.cipher = check_num(t.d.cipher, width / 2 - 6,  9, 8, 0);
	if (t.d.cipher == "-1") return t;

	gotoxy(width / 2 - 6, 10);
	cout << "Введите код отдела (3 символа)" << endl;
	gotoxy(width / 2 - 6, 11);
	t.d.department_code = check_num(t.d.department_code, width / 2 - 6, 11, 3, 0);
	if (t.d.department_code == "-1") return t;

	gotoxy(width / 2 - 6, 12);
	cout << "Введите ФИО (15 символов)" << endl;
	gotoxy(width / 2 - 6, 13);
	t.d.fio = check_num(t.d.fio, width / 2 - 6, 13, 15, 1);
	if(t.d.fio == "-1") return t;

	do {
		gotoxy(width / 2 - 6, 14);
		cout << "Введите общее время прохождения задания (5 символов)" << endl;
		gotoxy(width / 2 - 6, 15);
		t.d.all_time = check_num(t.d.all_time, width / 2 - 6, 15, 5, 0);
		if (t.d.all_time == "-1") return t;

		gotoxy(width / 2 - 6, 16);
		cout << "Введите время центрального процессора (5 символов)" << endl;
		gotoxy(width / 2 - 6, 17);
		t.d.time_cpu = check_num(t.d.time_cpu, width / 2 - 6, 17, 5, 0);
		if (t.d.time_cpu == "-1") return t;

		if (stoi(t.d.all_time) < stoi(t.d.time_cpu)) {
			MessageBox(0, L"Общее время должно быть больше времени центрального процессора!", L"Предупреждение", MB_ICONWARNING | MB_SETFOREGROUND);
			clearRow(15);
			clearRow(16);
			clearRow(17);
			clearRow(18);
			gotoxy(0, 14);
		}
		else break;

	} while (1);

	total_el++;

	return t;
}

// ==========РАСЧЁТ ПРОЦЕНТА ПРОЦЕССОРНОГО ВРЕМЕНИ==========
float percent_time_cpu(float a, float b) {
	return (b * 100) / a;
}

// ==========АНАЛОГ setw()==========
string sets(size_t size) {
	string res_s;
	for (int i = 0; i < size; i++) {
		res_s += " ";
	}
	return res_s;
}


// ==========ОЧИСТКА ЭКРАНА БЕЗ МЕРЦАНИЯ==========
void cls() {
	HANDLE hd = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD cd;
	cd.X = 0;
	cd.Y = 0;
	SetConsoleCursorPosition(hd, cd);
}

// ==========ПЕЧАТЬ СОДЕРЖИМОГО==========
void print_info(const time_task & t, int active) {
	if (active) SetColor(7, 0);

	if (active == 1) {
		SetColor(7, 0);
		SetColor(7, 5);
		cout << "| " << t.d.cipher << sets(12 - t.d.cipher.length());
		SetColor(7, 0);
		cout << setw(6 + t.d.department_code.length());
	}
	else {
		cout << "| " << t.d.cipher << sets((13 - t.d.cipher.length()) + t.d.department_code.length()) << sets(5 - t.d.department_code.length());
	}

	if (active == 2) {
		SetColor(7, 0);
		SetColor(7, 5);
		cout << t.d.department_code << sets(10 - t.d.department_code.length());
		SetColor(7, 0);
		cout << setw(6 + t.d.fio.length());
	}
	else {
		cout << t.d.department_code << sets(16 - t.d.department_code.length());
	}

	if (active == 3) {
		SetColor(7, 0);
		SetColor(7, 5);
		cout << t.d.fio << sets(15 - t.d.fio.length());
		SetColor(7, 0);
		cout << setw(3 + t.d.all_time.length());
	}
	else {
		cout << t.d.fio << sets(18 - t.d.fio.length());
	}

	if (active == 4) {
		SetColor(7, 0);
		SetColor(7, 5);
		cout << t.d.all_time << sets(11 - t.d.all_time.length());
		SetColor(7, 0);
		cout << setw(6 + t.d.time_cpu.length());
	}
	else {
		cout << t.d.all_time << sets(17 - t.d.all_time.length());
	}

	if (active == 5) {
		SetColor(7, 0);
		SetColor(7, 5);
		cout << t.d.time_cpu << sets(9 - t.d.time_cpu.length()) << "|";
		SetColor(7, 0);
	}
	else {
		cout << t.d.time_cpu << setw(10 - t.d.time_cpu.length()) << "|";
	}

	cout << setfill(' ') << setw(8) << percent_time_cpu(stof(t.d.all_time), stof(t.d.time_cpu)) << setfill(' ') << setw(24) << setprecision(4) << fixed << "|";
}

// ==========ВЫВОД ДАННЫХ==========
// если active = 0, то это режим сортировки
// если active = -1, то это изменение кол-ва элементов на странице
time_task* print(time_task* end, time_task* real_beg, time_task * beg, int active, int edit_el, int print_count_num_pages, int print_page) {
	wint_t buf;

	time_task* temp = beg,
		* buf_el = beg,
		* first_buf_el = beg,
		* edit_ob = beg;

	int i				= 1, // номер текущего элемента
		first_i			= 0, // первый элемент в каждой странице
		count_num_pages = print_count_num_pages, // счётчик для i (название не соответствует применению)
		page			= print_page, // текущая страница
		np				= 0, // новая страница
		sort_field		= 1, // поле для сортировки
		remember_active = 0, // запоминающая переменная для active
		direction		= 0, // направление сортировки (0 - от меньшего к большему, 1 - наоборот)
		fl				= 0,
		show_hotkey		= 0; // видимость горячих клавиш

	// пересчитываем сумму общего времени и сумму времени ЦП
	if (edit_el == 0) {
		sum_all_time = sum_time_cpu = average_percent_time_cpu = 0;
		for (temp = beg; temp; temp = temp->next) {
			sum_all_time += stoi(temp->d.all_time);
			sum_time_cpu += stoi(temp->d.time_cpu);
			average_percent_time_cpu += percent_time_cpu(stof(temp->d.all_time), stof(temp->d.time_cpu));
		}
	}

	do {
		float total_pages = ceil(total_el / num_pages); // общее кол-во страниц

		cls();
		show_filename(width - filename.length() - 9, 0);

		// если пустой список
		if (!beg) {
			MessageBox(0, L"Список пуст", L"Уведомление", MB_ICONINFORMATION | MB_SETFOREGROUND);
			return beg;
		}

		// +++++ОСНОВНОЙ ВЫВОД+++++
		int num_del = 0; // номер для удаления 

		if (total_el < num_pages) {
			num_pages = total_el;
		}

		if (active > num_pages || active == 0 || active == -1) {
			temp = buf_el;
			edit_ob = buf_el;
		} else {
			temp = beg;
			edit_ob = beg;
		}
		
		gotoxy(0, 0);
		cout << "\nОбщее количесвто элементов: " << total_el << setw(20) << endl;
		cout << "Количество элементов на странице: ";

		if (active == -1) {
			SetColor(7, 5);
		} else SetColor(7, 2);

		cout << " " << num_pages << " " << endl << endl;
		SetColor(7, 0);
		cout << "+———————————————————————————————————————————————————————————————————————————————+———————————————————————————————+" << endl;
		
		if (active == 0) {
			for (int i = 1; i <= 5; i++) {
				if (i == sort_field) {
					SetColor(7, 5);
				}
				cout << sort_items[i - 1];

				if (i == sort_field) {
					if (direction == 0) {
						cout << "<";
						fl = 0;
					}
					else {
						fl = 1;
						cout << ">";
					}

					switch (i) {
					case 1:
					case 2:
					case 4:
						cout << sets(4);
						break;
					case 3:
						cout << sets(13);
						break;
					case 5: // если это последний пункт
						cout << "|"; 
						break;
					}
				} else {
					switch (i) {
					case 1:
					case 2:
					case 4:
						cout << sets(5);
						break;
					case 3:
						cout << sets(14);
						break;
					case 5: // если это последний пункт
						cout << " |"; 
						break;
					}
				}
				SetColor(7, 0);
			}
		} else {
			cout << "| Шифр задания      Код отдела      ФИО               Общее время      Время ЦП |";
		}

		cout << " Процент процессорного времени |" << endl;
		cout << "+———————————————————————————————————————————————————————————————————————————————+———————————————————————————————+" << endl;

		for (i = count_num_pages; temp; temp = temp->next, i++) {
			// достаём первые элементы
			if (i % num_pages == 1 && page != 0) {
				first_i = i;
				buf_el = temp;
				for (int j = 0; j < num_pages; j++, temp = temp->prev) {
					first_buf_el = temp->prev;
				}
				temp = buf_el;
			}

			if (i % num_pages != 0) {
				np = 0;
			}

			// разукрашивание выбранного элемента
			if (i == active) {
				SetColor(7, 5);
				num_del = stoi(temp->d.cipher);
				edit_ob = temp; // редактируемый объект
				print_info(*temp, edit_el);
			}
			else {
				print_info(*temp, 0);
			}
			SetColor(7, 0);

			cout << endl;
			cout << "+———————————————————————————————————————————————————————————————————————————————+———————————————————————————————+" << endl;

			if (i % num_pages == 0 && np == 0) {
				np = 1;
				break;
			}
		}

		cout << "Сумма общего времени: " << sum_all_time << endl;
		cout << "Сумма времени ЦП: " << sum_time_cpu << endl;
		cout << "Средний процент времени ЦП: " << average_percent_time_cpu / total_el << endl;
		cout << "+———————————————————————————————————————————————————————————————————————————————+———————————————————————————————+" << endl << endl;
		cout << setw(width / 2) << "Страница " << page + 1 << " из " << setprecision(0) << total_pages;
		// +++++++++++++++++

		// горячие клавиши
		gotoxy(0, 0);
		SetColor(7, 4);
		if (show_hotkey == 0) {
			cout << "    Горячие клавиши (H)    " << endl;
		}
		else {
			cout << "         ГОРЯЧИЕ КЛАВИШИ         закрыть(H)" << endl;
			cout << " del - удалить файл                        " << endl;
			cout << " enter - редактировать поле                " << endl;
			cout << " s - сортировка                            " << endl;
			cout << " n - изменить кол-во элементов на странице " << endl;
			cout << " esc - выйти в меню                        " << endl;
		}
		SetColor(7, 0);
		
		int height_el_num_pages = 0; // высота элементов
		if ((page + 1 != total_pages)) {
			height_el_num_pages = i - count_num_pages;
		}
		else {
			height_el_num_pages = i - count_num_pages - 1;
		}

		if (edit_el) {
			gotoxy(0, 14 + height_el_num_pages + (height_el_num_pages + 1));
			return beg; // если редактируется какой-нибудь элемент, то выходим из ф-ии чтобы не было рекурсии
		} else {
			show_cursor(FALSE);
		}

		// считывание клавиш
		buf = _getwch();
		switch (buf) {
		case up:
			if (show_hotkey == 1) {
				system("cls");
				show_hotkey = 0;
			}
			if (active == 0 || active == -1 || total_el == 1) break;
			if (active > 1) active--;
			if (active % num_pages == 0 || page == total_pages) {
				page--;
				buf_el = first_buf_el;
				count_num_pages = first_i - num_pages; // формируем i
				system("cls");
			}
			break;
		case down:
			if (show_hotkey == 1) {
				system("cls");
				show_hotkey = 0;
			}
			if (active == -1 || total_el == 1) break;
			if (active != 0) {
				if (active % (num_pages) == 0) {
					if (i == total_el) { // если это самый последний элемент
						break;
					}
					page++;
					buf_el = temp->next;
					count_num_pages = i + 1; // формируем i
					system("cls");
				}
				if (active < total_el) active++;
				if (page == 0) {
					if (active % (num_pages + 1) == 0 && temp != 0) {
						page++;
						buf_el = temp->next;
					}
				}
			}
			break;
		case right_btn:
			if (show_hotkey == 1) break; // если включен блок горячих клавиш
			if (active == -1 || total_el == 1) break;
			if (i + 1 <= total_el && active != 0) {
				buf_el = temp->next;
				count_num_pages += num_pages;
				active = i + 1;
				page++;
				system("cls");
			}

			if (active % num_pages == 0) {
				if (sort_field < 5) {
					sort_field++;
				}
				break;
			}
			break;
		case left_btn:
			if (show_hotkey == 1) break; // если включен блок горячих клавиш
			if (active == -1 || total_el == 1) break;
			if (active == 0) {
				if (sort_field > 1) {
					sort_field--;
				}
				break;
			}
			if (active - num_pages >= 1) {
				buf_el = first_buf_el;
				count_num_pages -= num_pages;
				active = first_i - num_pages;
				page--;
				system("cls");
			}
			break;
		case del:
			if (show_hotkey == 1) {
				system("cls");
				show_hotkey = 0;
			}
			if (MessageBox(0, L"Вы уверены, что хотите удалить?", L"Удаление", MB_ICONQUESTION | MB_YESNO | MB_SETFOREGROUND) == 6) {
				beg = delete_el(beg, num_del);
				if (active != 1) {
					active--;
				}
				total_el--;
				if (active % num_pages == 0) {
					buf_el = first_buf_el;
					count_num_pages -= num_pages;
					page--;
				} else if (active < num_pages) buf_el = beg;

				// пересчитываем сумму общего времени и сумму времени ЦП
				sum_all_time = sum_time_cpu = average_percent_time_cpu = 0;
				for (temp = beg; temp; temp = temp->next) {
					sum_all_time += stoi(temp->d.all_time);
					sum_time_cpu += stoi(temp->d.time_cpu);
					average_percent_time_cpu += percent_time_cpu(stof(temp->d.all_time), stof(temp->d.time_cpu));
				}
			}
			system("cls");
			break;
		case esc:
			if (show_hotkey == 1) {
				system("cls");
				show_hotkey = 0;
				break;
			}
			if (active == 0 || active == -1) {
				active = remember_active;
				break;
			} else {
				return beg;
			}
		case enter:
			if (show_hotkey == 1) break; // если включен блок горячих клавиш
			if (active == -1) { // изменение кол-ва элементов на одной странице
				show_cursor(TRUE);
				do {
					string str;
					int fl = 0;
					// установка курсора в правильное место
					gotoxy(34, 2);
					SetColor(7, 0);
					cout << sets(50);
					gotoxy(35, 2);
					// =====================================
					str = to_string(num_pages);
					cin >> str;

					// проверки на ошибки
					try {
						if ( !(str[0] >= '0' && str[0] <= '9') ) { // если это символ
							throw 1;
						} else if (stoi(str) >= 2 && stoi(str) <= 10) {
							num_pages = stoi(str);
							break;
						} else throw 2; // если число не попало в диапозон от 2 до 10
					}
					catch (int ex) {
						if (ex == 1) {
							MessageBox(0, L"Нельзя вводить символы!", L"Предупреждение", MB_ICONWARNING | MB_SETFOREGROUND);
						} else if (ex == 2) {
							MessageBox(0, L"Значение должно быть в диапозоне от 2 до 10!", L"Предупреждение", MB_ICONWARNING | MB_SETFOREGROUND);
						}
					}
				} while (1);
				
				// делаем активным первый элемент
				i = 1;
				first_i = 0;
				count_num_pages = 1;
				page = 0; 
				temp = buf_el = first_buf_el = beg;
				active = 1;
				system("cls");
				show_cursor(FALSE);
				break;
			}
			if (active == 0) {
				sort(beg, sort_field, direction);
				if (direction == 0) direction = sort_field; else direction = 0;
				break;
			}
			edit(end, beg, buf_el, active, edit_ob, count_num_pages, page, height_el_num_pages);
			system("cls");
			break;
		case 115: // символ s
		case 1099: // символ ы
			if (active == -1) break;
			if (show_hotkey == 1) { 
				system("cls");
				show_hotkey = 0; 
			}
			if (active == 0) {
				active = remember_active;
			} else {
				remember_active = active;
				active = 0;
			}
			break;
		case 110: // символ n
		case 1090: // символ т
			if (show_hotkey == 1) {
				system("cls");
				show_hotkey = 0;
			}
			if (active == -1) {
				active = remember_active;
			}
			else {
				remember_active = active;
				active = -1;
			}
			break;
		case 104: // символ h
		case 1088: // символ р
			if (show_hotkey == 0) {
				show_hotkey = 1;
			} else  show_hotkey = 0;
			system("cls");
			break;
		}
	} while (1);
}

// ==========РЕДАКТИРОВАНИЕ ЭЛЕМЕНТА==========
void edit(time_task* end, time_task* real_beg ,time_task * beg, int active, time_task * _edit_ob, int edit_count_num_pages, int edit_page, int edit_i) {
	int edit_el = 1;
	string remember_field;

	do {
		print(end, real_beg, beg, active, edit_el, edit_count_num_pages, edit_page);

		switch (_getwch()) {
		case right_btn:
			if (edit_el >= 1 && edit_el < 5) print(end, real_beg, beg, active, ++edit_el, edit_count_num_pages, edit_page);
			break;
		case left_btn:
			if (edit_el <= 5 && edit_el > 1) print(end, real_beg, beg, active, --edit_el, edit_count_num_pages, edit_page);
			break;
		case esc:
			return;
			break;
		case enter:
			switch (edit_el) {
			case 1:
				cout << "\nВведите новый шифр задания (8 символов):" << endl;
				remember_field = _edit_ob->d.cipher;
				_edit_ob->d.cipher = check_num(_edit_ob->d.cipher, 0, 17 + edit_i * 2, 8, 0);
				if (_edit_ob->d.cipher == "-1") {
					_edit_ob->d.cipher = remember_field;
					break;
				} 
				break;
			case 2:
				cout << "\nВведите новый код отдела (3 символа):" << endl;
				remember_field = _edit_ob->d.department_code;
				_edit_ob->d.department_code = check_num(_edit_ob->d.department_code, 0, 17 + edit_i * 2, 3, 0);
				if (_edit_ob->d.department_code == "-1") {
					_edit_ob->d.department_code = remember_field;
					break;
				}
				break;
			case 3:
				cout << "\nВведите ФИО (15 символов):" << endl;
				remember_field = _edit_ob->d.fio;
				_edit_ob->d.fio = check_num(_edit_ob->d.fio, 0, 17 + edit_i * 2, 15, 1);
				if (_edit_ob->d.fio == "-1") {
					_edit_ob->d.fio = remember_field;
					break;
				}
				break;
			case 4:
				cout << "\nВведите новое общее время (5 символов):" << endl;
				do {
					remember_field = _edit_ob->d.all_time;
					_edit_ob->d.all_time = check_num(_edit_ob->d.all_time, 0, 17 + edit_i * 2, 5, 0);
					if (_edit_ob->d.all_time == "-1") {
						_edit_ob->d.all_time = remember_field;
						break;
					}

					if (stoi(_edit_ob->d.all_time) < stoi(_edit_ob->d.time_cpu)) {
						MessageBox(0, L"Общее время должно быть больше времени центрального процессора!", L"Ошибка", MB_ICONWARNING | MB_SETFOREGROUND);
						clearRow(16 + edit_i * 2);
						gotoxy(0, 17 + edit_i * 2);
					} else break;
				} while (1);
				break;
			case 5:
				cout << "\nВведите новое время ЦП (5 символов):" << endl;
				do {
					remember_field = _edit_ob->d.time_cpu;
					_edit_ob->d.time_cpu = check_num(_edit_ob->d.time_cpu, 0, 17 + edit_i * 2, 5, 0);
					if (_edit_ob->d.time_cpu == "-1") {
						_edit_ob->d.time_cpu = remember_field;
						break;
					}

					if (stoi(_edit_ob->d.all_time) < stoi(_edit_ob->d.time_cpu)) {
						MessageBox(0, L"Общее время должно быть больше времени центрального процессора!", L"Ошибка", MB_ICONWARNING | MB_SETFOREGROUND);
						clearRow(16 + edit_i * 2);
						gotoxy(0, 17 + edit_i * 2);
					} else break;
				} while (1);
				break;
			}

			// пересчитываем сумму общего времени и сумму времени ЦП
			sum_all_time = sum_time_cpu = average_percent_time_cpu = 0;
			for (time_task* temp = real_beg; temp; temp = temp->next) {
				sum_all_time += stoi(temp->d.all_time);
				sum_time_cpu += stoi(temp->d.time_cpu);
				average_percent_time_cpu += percent_time_cpu(stof(temp->d.all_time), stof(temp->d.time_cpu));
			}

			system("cls");
			print(end, real_beg, beg, active, edit_el, edit_count_num_pages, edit_page);
			return;
		}
	} while (1);
}

// ==========УДАЛЕНИЕ==========
time_task* delete_el(time_task * beg, int num_del) {
	time_task* temp;
	time_task* buf;

	if (!beg) {
		MessageBox(0, L"Список пуст", L"Уведомление", MB_ICONINFORMATION | MB_SETFOREGROUND);
		return 0;
	}

	temp = beg;

	// если один элемент в списке
	if (beg->next == 0) {
		delete temp;
		return 0;
	}

	while (temp) {
		if (num_del == stoi(temp->d.cipher)) { // если введённый номер совпал с шифром задания
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

	MessageBox(0, L"Произошла ошибка!", L"Ошибка", MB_ICONERROR | MB_SETFOREGROUND);
	return beg;
}

// ==========СРАВНЕНИЕ ДАННЫХ ДЛЯ СОРТИРОВКИ==========
int compare(time_task* t_i, time_task* t_j, int num, int compare_direction) {
	switch (num) {
	case 1:
		if (compare_direction == 0) {
			return stoi(t_i->d.cipher) > stoi(t_j->d.cipher);
		} else return stoi(t_i->d.cipher) < stoi(t_j->d.cipher);
	case 2:
		if (compare_direction == 0) {
			return stoi(t_i->d.department_code) > stoi(t_j->d.department_code);
		} else return stoi(t_i->d.department_code) < stoi(t_j->d.department_code);
		
	case 3:
		if (compare_direction == 0) {
			return t_i->d.fio > t_j->d.fio;
		} else return t_i->d.fio < t_j->d.fio;
		
	case 4:
		if (compare_direction == 0) {
			return stoi(t_i->d.all_time) > stoi(t_j->d.all_time);
		} else return stoi(t_i->d.all_time) < stoi(t_j->d.all_time);
		
	case 5:
		if (compare_direction == 0) {
			return stoi(t_i->d.time_cpu) > stoi(t_j->d.time_cpu);
		} else return stoi(t_i->d.time_cpu) < stoi(t_j->d.time_cpu);
	}
	return 0;
}

// ==========СОРТИРОВКА==========
void sort(time_task* beg, int field_for_sort, int sort_direction){
	time_task *temp_i = beg,
			  *temp_j = beg;

	for (; temp_i; temp_i = temp_i->next) {
		for (temp_j = temp_i; temp_j; temp_j = temp_j->next) {
			if (compare(temp_i, temp_j, field_for_sort, sort_direction)) {
				swap(temp_i->d, temp_j->d);
			}
		}
	}
}

// ==========ПОИСК ЭЛЕМЕНТА ПО ФАМИЛИИ==========
void find(time_task * beg) {
	time_task* temp = beg;
	string find_el;
	bool fl = 0;
	string str;

	if (!beg) {
		MessageBox(0, L"Список пуст", L"Уведомление", MB_ICONINFORMATION | MB_SETFOREGROUND);
		return;
	}

	// выводим название раздела
	SetColor(7, 5);
	gotoxy(width / 2 - 2, 3);
	cout << sets(21);
	gotoxy(width / 2 - 2, 4);
	cout << "        ПОИСК        ";
	gotoxy(width / 2 - 2, 5);
	cout << sets(21);
	SetColor(7, 0);

	gotoxy(width / 2 - 4, height / 2 - 4);
	cout << "Введите данные для поиска" << endl;
	gotoxy(width / 2 - 4, height / 2 - 3);
	SetColor(7, 5);
	find_el = check_num(find_el, width / 2 - 4, height / 2 - 3, 15, 2);
	SetColor(7, 0);
	if (find_el == "-1") return;

	system("cls");

	while (temp) {
		if (find_el == temp->d.all_time || find_el == temp->d.cipher || find_el == temp->d.department_code || find_el == temp->d.time_cpu || find_el == temp->d.fio) {
			if (fl == 0) {
				cout << "+———————————————————————————————————————————————————————————————————————————————+———————————————————————————————+" << endl;
				cout << "| Шифр задания      Код отдела      ФИО               Общее время      Время ЦП | Процент процессорного времени |" << endl;
				cout << "+———————————————————————————————————————————————————————————————————————————————+———————————————————————————————+" << endl;
			}
			fl = 1;
			print_info(*temp, 0);
			cout << "\n+———————————————————————————————————————————————————————————————————————————————+———————————————————————————————+" << endl;
		}
		temp = temp->next;
	}

	if (fl == 0) {
		MessageBox(0, L"Сотрудник с такими данными не найден", L"Уведомление", MB_ICONINFORMATION | MB_SETFOREGROUND);
		return;
	} else system("pause");
}


// ==========ЧТЕНИЕ ИЗ БИНАРНОГО ФАЙЛА==========
int read_bin_file(string filename, time_task** beg, time_task** end) {
	total_el = 0;

	ifstream fin;
	fin.open(filename, ios::binary);

	if (!fin) {
		MessageBox(0, L"Нет файла!", L"Ошибка", MB_ICONERROR | MB_SETFOREGROUND);
		return 1;
	}

	fin.seekg(ios_base::beg);
	time_task* t = new time_task;
	t->next = NULL;
	t->prev = NULL;
	*beg = 0;

	while (fin.read((char*)& t->d, sizeof(t->d)) ) {
		input(*beg, *end, *t);
		total_el++;
	}

	fin.close();
	return 0;
}

// ==========ЧТЕНИЕ ИЗ ФАЙЛА==========
int read_file(string filename, time_task * *beg, time_task * *end) {
	int k = 0;
	total_el = 0;
	ifstream fin;
	fin.open(filename);

	if (!fin) {
		MessageBox(0, L"Нет файла!", L"Ошибка", MB_ICONERROR | MB_SETFOREGROUND);
		return 1;
	}

	time_task t;
	*beg = 0;
	while (getline(fin, t.d.cipher)) {
		getline(fin, t.d.department_code);
		getline(fin, t.d.fio);
		getline(fin, t.d.all_time);
		getline(fin, t.d.time_cpu);

		input(*beg, *end, t);
		total_el++;
	}

	fin.close();
	return 0;
}

// ==========ЗАПИСЬ В ФАЙЛ==========
int write_file(time_task * temp) {

	if (!temp) {
		MessageBox(0, L"Список пуст", L"Уведомление", MB_ICONINFORMATION | MB_SETFOREGROUND);
		return 0;
	}

	// МЕНЮ ДЛЯ ЗАПИСИ В ФАЙЛ
	int active_file = 1,
		k = 1;

	string write_filename;

	// выводим название раздела
	SetColor(7, 5);
	gotoxy(width / 2 - 16, 3);
	cout << sets(41);
	gotoxy(width / 2 - 16, 4);
	cout << "          ЗАПИСЬ ДАННЫХ В ФАЙЛ           ";
	gotoxy(width / 2 - 16, 5);
	cout << "    Выберите тип файла для сохранения.   ";
	gotoxy(width / 2 - 16, 6);
	cout << "    (Расширение файла вводить не надо)   ";
	gotoxy(width / 2 - 16, 7);
	cout << sets(41);

	const string items[2] = {
		"   Текстовый          ",
		"   Бинарный           "};

	int is_text = menu(active_file, items, 2);
	if (is_text == -1) return 0;
	// ======================
	show_cursor(TRUE);
	cout << "Введите название файла:" << endl;
	do {
		try {
			write_filename = check_num(write_filename, 0, 17, 20, 2);
			if (write_filename == "-1") return 0;

			for (int i = 0; write_filename[i]; i++) write_filename[i] = tolower(write_filename[i]); // приводим к нижнему регистру для правильного сравнения
			if (write_filename == "mainbd") throw 1;

			// проверка на повторение названия файла
			for (k; k < SIZE_arr_filename; k++) {
				if (arr_filename[k].find(write_filename) != -1) {
					throw 2;
				}
				if (arr_filename[k] == "") break; // если дальше пустые элементы
			}
			break;

		} catch (int ex) { // если такое название уже есть
			if (ex == 1) {
				MessageBox(0, L"Недопустимое название файла!", L"Предупреждение", MB_ICONWARNING | MB_SETFOREGROUND);
			}
			else if (ex == 2) {
				MessageBox(0, L"Файл с таким именем уже существует!", L"Предупреждение", MB_ICONWARNING | MB_SETFOREGROUND);
			}
			clearRow(18);
			gotoxy(0, 17);
		}
	} while (1);

	if (is_text == 1) {
		write_filetype(temp, write_filename + ".txt", k, 1);
	} else write_filetype(temp, write_filename + ".bin", k, 0);
	return 0;
}

// ==========ОПРЕДЕЛЕНИЕ ТИПА ДЛЯ ЗАПИСИ==========
void write_filetype(time_task *temp, string filename, int el, int filetype) {

	if (!temp) {
		MessageBox(0, L"Список пуст", L"Уведомление", MB_ICONINFORMATION | MB_SETFOREGROUND);
		return;
	}

	ofstream fout; // файл в который производится запись данных
	ofstream fout_all_bd; // файл в который производится запись названия файла
	if (el != -1) fout_all_bd.open("mainBD.txt", ios_base::app);

	// обработка ошибок
	if (!fout || !fout_all_bd) {
		MessageBox(0, L"Невозможно открыть файл для записи!", L"Ошибка", MB_ICONERROR | MB_SETFOREGROUND);
		return;
	}

	fout.open(filename, (filetype == 1) ? ios_base::out : ios::binary | ios::out);
	if (el != -1) fout_all_bd << filename << endl; // записываем в общий файл названия БД

	if (el != -1) arr_filename[el++] = filename; // добавялем в массив новое название файла

	if (filetype == 1) {
		while (temp) {
			fout << temp->d.cipher << endl;
			fout << temp->d.department_code << endl;
			fout << temp->d.fio << endl;
			fout << temp->d.all_time << endl;
			fout << temp->d.time_cpu << endl;
			temp = temp->next;
		}
	} else {
		while (temp) {
			fout.write((char*)& temp->d, sizeof temp->d);
			temp = temp->next;
		}
	}

	// закрываем файлы
	fout.close();
	fout_all_bd.close();

	MessageBox(0, L"БД Сохранена", L"Сохранение", MB_ICONINFORMATION | MB_SETFOREGROUND);
}

// ==========ШАБЛОН ПЕЧАТИ МЕНЮ==========
void print_menu(int sym, const string items[], const int N_ITEMS) {
	for (int i = 1; i <= N_ITEMS; i++) {
		SetColor(7, 0);
		gotoxy((width / 2) - 6, (height / 2) + i - 3); // ставим меню в центр
		if (i == sym) {
			SetColor(7, 5);
		}
		cout << items[i - 1] << endl;
		SetColor(7, 0);
	}
}

// ==========МЕНЮ==========
int menu(int& active, const string items[], int num_el) {
	wint_t buf;

	do {
		cls();
		print_menu(active, items, num_el);

		buf = _getwch();
		switch (buf) {
		case up: // клавиша вверх
			if (active > 1) active--;
			break;
		case down: // клавиша вниз
			if (active < num_el) active++;
			break;
		case enter: // клавиша enter
			return active;
		case esc: // клавиша escape
			return -1;
		case del:
			return -active;
		}
	} while (1);
}


// ==========УСТАНОВКА ЦВЕТА ТЕКСТА И ФОНА==========
void SetColor(int text, int bg) {
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdOut, (WORD)((bg << 4) | text));
}