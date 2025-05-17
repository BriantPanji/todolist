#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <ctime>
#include <windows.h>
#include <winnls.h>
// #include <cstdlib>
// #include <thread>
// #include <chrono>

// karakter: https://www.compart.com/en/unicode/block/U+2500

using namespace std;

struct Task
{
    int id;
    string title;
    string desc;
    bool isDone = false;
    time_t deadline = 0;
    time_t createdAt = 0;
};

// Tambah tugas -> Panji
// tampilkan semua tugas -> Sutanto
// tampilkan tugas terlambat -> Sutanto
// selesaikan tugas -> Andreas
// tampilkan to-do selesai -> Andreas
// detail tugas -> Delrico
// hapus tugas -> Feri
// edit tugas -> Delrico
// Exit

class TodoList
{
private:
    int maxLength = 49;

    vector<Task> listTasks;

    vector<string> listError;

    bool loadTasks();
    bool saveTasks();
    bool formatTaskId();
    char getChoice();

    void printHeader();

    string truncStr(string str, int len);
    void printBorder(char pos, bool isLight = false);
    void printText(string text);
    void printAsk(string text);
    void printCenter(string text);
    string getTanggal(time_t t);

    time_t currentTime;

public:
    TodoList();
    ~TodoList();

    bool addTask(string title, string desc, time_t deadline);
    void editTask();
    int binarySearch(int id);
    void detailTask(int pilihan);

    void printAllTasks();
    void printLateTasks();
    int sumOfTasks();

    bool start();
    bool formAddTask();

        void printDebug();

        bool deleteTask();

};

int TodoList::sumOfTasks()
{
    return this->listTasks.size();
}

string TodoList::truncStr(string str, int len) {
    if (str.length() > len)
        return str.substr(0, len - 3) + "...";
    else
        return str;
}

void TodoList::printBorder(char pos, bool isLight)
{
    switch (pos)
    {
    case 'h':
        cout << (!isLight ? "╔═══════════════════════════════════════════════════╗\n" : "╓───────────────────────────────────────────────────╖");
        break;
    case 'i':
        cout << (!isLight ? "╠═══════════════════════════════════════════════════╣\n" : "╟───────────────────────────────────────────────────╢\n");
        break;
    case 'f':
        cout << (!isLight ? "╚═══════════════════════════════════════════════════╝\n" : "╙───────────────────────────────────────────────────╜\n");
        break;
    default:
        listError.push_back("ERR (TodoList::printBorder): Invalid border type.");
        break;
    }
}
void TodoList::printText(string text)
{
    cout << "║ " << setw(maxLength) << truncStr(text, maxLength) << " ║\n";
}
void TodoList::printAsk(string text)
{
    cout << "║ " << truncStr(text, maxLength);
}
void TodoList::printCenter(string text)
{
    text = truncStr(text, maxLength);
    int textLength = text.length();
    int padding = ((maxLength - textLength) / 2) + 1;
    int extra = (maxLength - textLength) % 2;

    cout << "║" << string(padding, ' ')
         << text
         << string(padding + extra, ' ')
         << "║\n";
}

void TodoList::printHeader() {
    cout << left;
    printBorder('h');
    printCenter("TODO LIST");
    printBorder('i');
    printText("1. Tambah Tugas");
    printText("2. Tampilkan Semua Tugas");
    printText("3. Tampilkan Tugas Selesai");
    printText("4. Tampilkan Tugas Terlambat");
    printText("5. Edit Tugas");
    printText("6. Hapus Tugas");
    printText("7. Selesaikan Tugas");
    printText("8. Detail Tugas");
    printText("0. Keluar dari Program");
    printBorder('i');
}

TodoList::TodoList()
{
    SetConsoleOutputCP(CP_UTF8);
    cout << left;
    currentTime = time(0);
    if (!loadTasks())
    {
        cerr << "ERR (TodoList::TodoList): Failed to load tasks.\n";
    }
}
TodoList::~TodoList()
{
    if (!saveTasks())
    {
        cerr << "ERR (TodoList::~TodoList): Failed to save tasks.\n";
    }
}

bool TodoList::loadTasks()
{
    ifstream file("tasks.todo.txt");
    if (!file.is_open())
    {
        return false;
    }

    Task task;
    string line;
    while (getline(file, line))
    {
        // cth: 1|lorem ipsum|lorem ipsum dolor sit amet|1|1747224142|1747224001
        stringstream strs(line);
        string temp;

        getline(strs, temp, '|');
        task.id = stoi(temp);

        getline(strs, task.title, '|');

        getline(strs, task.desc, '|');

        getline(strs, temp, '|');
        task.isDone = (temp == "1") ? true : false;

        getline(strs, temp, '|');
        task.deadline = (temp == "0") ? currentTime : (time_t)stoi(temp);

        getline(strs, temp, '|');
        task.createdAt = (temp == "0") ? currentTime : (time_t)stoi(temp);

        listTasks.push_back(task);
    }

    file.close();
    return true;
}

bool TodoList::saveTasks()
{
    ofstream file("tasks.todo.txt");
    if (!file.is_open())
    {
        return false;
    }
    formatTaskId();

    for (auto i = listTasks.begin(); i != listTasks.end(); ++i)
    {
        file << i->id << "|" << i->title << "|" << i->desc << "|" << i->isDone << "|" << i->deadline << "|" << i->createdAt << endl;
    }

    file.close();
    return true;
}

bool TodoList::formatTaskId()
{
    int lastId = 1;
    for (auto i = listTasks.begin(); i != listTasks.end(); ++i)
    {
        i->id = lastId++;
        if (i->deadline == 0)
            i->deadline = currentTime;
        if (i->createdAt == 0)
            i->createdAt = currentTime;
    }
    return true;
}

bool TodoList::addTask(string title, string desc, time_t deadline)
{
    if (title.find("|") != string::npos)
    {
        listError.push_back("ERR (TodoList::addTask): Title cannot contain '|'.\n");
        return false;
    }
    if (desc.find("|") != string::npos)
    {
        listError.push_back("ERR (TodoList::addTask): Description cannot contain '|'.\n");
        return false;
    }

    Task task;

    task.id = listTasks.empty() ? 1 : listTasks.back().id + 1;
    task.title = title;
    task.desc = desc;
    task.deadline = deadline;
    task.createdAt = currentTime;

    listTasks.push_back(task);
    return saveTasks();
}

void TodoList::editTask()
{
    int id;
    printAllTasks();
    do
    {
        cout << "Mau ubah task ke berapa? ";
        cin >> id;
    } while (id < 1 || id > TodoList::sumOfTasks());

    int tasksSum = sumOfTasks();
    for (int i = 0; i < tasksSum; i++)
    {
        Task task;
        if (listTasks[i].id == id)
        {
            string newTitle, newDesc, newDeadline;
            struct tm deadline = {0};

            TodoList::detailTask(id);
            cin.ignore();
            cout << "Ubah Judul: ";
            getline(cin, newTitle);
            cout << "Ubah Deskripsi: ";
            getline(cin, newDesc);
            cout << "Ubah Deadline: (dd/mm/yy hh:mm) ";
            getline(cin, newDeadline);

            listTasks[i].title = newTitle;
            listTasks[i].desc = newDesc;

            if (newDeadline.empty())
            {
                printBorder('h');
                printText("ERR: Tenggat tidak boleh kosong. Ulangi lagi!");
                printBorder('f');
                return editTask();
            }
            if (newDeadline.length() < 14)
            {
                printBorder('h');
                printText("ERR: Format tenggat tidak valid. Ulangi lagi!");
                printBorder('f');
                return editTask();
            }

            deadline.tm_mday = stoi(newDeadline.substr(0, 2));
            deadline.tm_mon = stoi(newDeadline.substr(3, 2)) - 1;
            deadline.tm_year = stoi("20" + newDeadline.substr(6, 2)) - 1900;
            deadline.tm_hour = stoi(newDeadline.substr(9, 2));
            deadline.tm_min = stoi(newDeadline.substr(12, 2));

            time_t dlt = mktime(&deadline);
            if (dlt <= currentTime)
            {
                printBorder('h');
                printText("ERR: Tenggat tidak boleh lebih kecil dari waktu sekarang. Ulangi lagi!");
                printBorder('f');
                return editTask();
            }

            listTasks[i].deadline = dlt;

            printBorder('i');

            saveTasks();
            printCenter("Berhasil mengubah tugas.");
            printBorder('f');
        }
    }
}

int TodoList::binarySearch(int target)
{
    sort(listTasks.begin(), listTasks.end(), [](const Task &a, const Task &b)
         { return a.id < b.id; });

    auto it = lower_bound(listTasks.begin(), listTasks.end(), target,
                          [](const Task &task, int id)
                          {
                              return task.id < id;
                          });

    if (it != listTasks.end() && it->id == target)
    {
        return distance(listTasks.begin(), it);
    }
    return -1;
}

void TodoList::detailTask(int pilihan)
{
    int posisi = TodoList::binarySearch(pilihan);

    int isDone = listTasks[posisi].isDone;
    string tampilDone;
    if (isDone)
        tampilDone = "Tugas telah selesai";
    else
        tampilDone = "Tugas belum selesai";

    // cout << "╠════╤══════════════════════════════════════════════════╤══════════════════════════════════════════════╣\n";
    // cout << "║ " << setw(2) << "No" << " │ "
    //      << setw(29) << left << "Tugas"
    //      << setw(29) << left << "Deskripsi"
    //      << " " << right << setw(14) << "Status" << " ║\n"
    //      << " " << right << setw(14) << "Tenggat" << " ║\n";
    // cout << "╟────┼──────────────────────────────────────────────╢\n";
    // cout << "║ " << right << setw(2) << setfill('0') << listTasks[posisi].id << " │ "
    //      << setfill(' ') << setw(29) << left << truncStr(listTasks[posisi].title, 29)
    //      << setfill(' ') << setw(29) << left << truncStr(listTasks[posisi].desc, 29)
    //      << setfill(' ') << setw(29) << left << tampilDone << right << setw(14) << getTanggal(listTasks[posisi].deadline) << left << " ║\n";
    // cout << "╚════╧══════════════════════════════════════════════════╧══════════════════════════════════════════════╝\n";
    int sizeTitle = listTasks[posisi].title.size();
    if (sizeTitle < 9)
        sizeTitle = 10;
    int sizeDesc = listTasks[posisi].desc.size();
    if (sizeDesc < 13)
        sizeDesc = 14;

    int sizeBorder = 3 + 2 + sizeTitle + 2 + sizeDesc + 2 + 5 + 20 + 16 + 3;
    string garis = "";
    string simbol = "═";
    for (int i = 0; i <= sizeBorder; ++i)
    {
        garis += simbol;
    }
    cout << "╔" << garis << "╗" << endl;
    cout << "║ " << setw(2) << "No" << " │ "
         << setw(sizeTitle) << left << "Judul" << " │ "
         << setw(sizeDesc) << left << "Deskripsi" << " │ "
         << setw(20) << left << "Status" << " │ "
         << setw(16) << left << "Tenggat" << "   ║\n";
    cout << "╟" << garis << "╢\n";

    cout << "║ " << right << setfill('0') << setw(2) << listTasks[posisi].id << setfill(' ') << " │ "
         << setw(sizeTitle) << left << listTasks[posisi].title << " │ "
         << setw(sizeDesc) << left << listTasks[posisi].desc << " │ "
         << setw(20) << left << tampilDone << " │ "
         << setw(16) << left << getTanggal(listTasks[posisi].deadline) << "   ║\n";

    cout << "╚" << garis << "╝\n";
}

string TodoList::getTanggal(time_t t)
{
    struct tm dt = *localtime(&t);
    stringstream ss;
    ss << setfill('0') << setw(2) << dt.tm_mday << "/"
       << setfill('0') << setw(2) << dt.tm_mon + 1 << "/"
       << to_string(dt.tm_year + 1900).substr(2, 2)
       << " " << setfill('0') << setw(2) << dt.tm_hour << ":"
       << setfill('0') << setw(2) << dt.tm_min;
    return ss.str();
}

void TodoList::printAllTasks()
{
    printBorder('h');
    printCenter("TODO LIST");
    cout << "╠════╤══════════════════════════════════════════════╣\n";
    cout << "║ " << setw(2) << "No" << " │ "
         << setw(29) << left << "Tugas"
         << " " << right << setw(14) << "Tenggat" << " ║\n";
    cout << "╟────┼──────────────────────────────────────────────╢\n";
    for (auto i = listTasks.begin(); i != listTasks.end(); ++i) {
        cout << "║ " << right << setw(2) << setfill('0') << i->id << " │ "
             << setfill(' ') << setw(29) << left << truncStr(i->title, 29)
             << " " << right << setw(14) << getTanggal(i->deadline) << left << " ║\n";
    }
    cout << "╚════╧══════════════════════════════════════════════╝\n";
}

void TodoList::printLateTasks() {
    printBorder('h');
    printCenter("TUGAS TERLAMBAT");
    cout << "╠════╤══════════════════════════════════════════════╣\n";
    cout << "║ " << setw(2) << "No" << " │ "
        << setw(29) << left << "Tugas"
        << " " << right << setw(14) << "Tenggat" << " ║\n";
    cout << "╟────┼──────────────────────────────────────────────╢\n";
    bool found = false;
    for (const auto& task : listTasks) {
        if (!task.isDone && task.deadline < currentTime) {
            found = true;
            cout << "║ " << setw(2) << setfill('0') << task.id << " │ "
                << setfill(' ') << setw(29) << left << truncStr(task.title, 29)
                << " " << right << setw(14) << getTanggal(task.deadline) << " ║\n";
        }
    }
    if (!found) {
        cout << "║ Tidak ada tugas yang terlambat.                  ║\n";
    }
    cout << "╚════╧══════════════════════════════════════════════╝\n";
}


void TodoList::printDebug() {
    for (auto i = listTasks.begin(); i != listTasks.end(); ++i)
    {
        cout << "+++++++++++++++++++++++++++";
        cout << "ID: " << i->id << endl;
        cout << "Title: " << i->title << endl;
        cout << "Desc: " << i->desc << endl;
        cout << "Is Done: " << (i->isDone ? "true" : "false") << endl;
        cout << "Deadline: " << i->deadline << endl;
        cout << "Created At: " << i->createdAt << endl;
    }
}

bool TodoList::formAddTask()
{
    string temp;
    Task task;
    struct tm deadline = {0};

    printBorder('h');
    printCenter("TAMBAH TUGAS");
    printBorder('i');
    printCenter("Ketik 'x' untuk membatalkan menambah tugas.");
    printBorder('i', true);

    printText("Masukkan judul tugas, maksimal 35");
    printAsk("Judul: ");
    getline(cin, temp);
    printBorder('i', true);
    if (temp.empty())
    {
        printBorder('h');
        printText("ERR: Judul tidak boleh kosong. Ulangi lagi!");
        printBorder('f');
        return formAddTask();
    }
    if (temp == "x" || temp == "X")
    {
        printBorder('h');
        printText("Batal menambah tugas.");
        printBorder('f');
        return true;
    }
    task.title = truncStr(temp, 35);

    printText("Masukkan deskripsi tugas, maksimal 256");
    printAsk("Deskripsi: ");
    getline(cin, temp);
    printBorder('i', true);
    if (temp.empty())
    {
        printBorder('h');
        printText("ERR: Deskripsi tidak boleh kosong. Ulangi lagi!");
        printBorder('f');
        return formAddTask();
    }
    if (temp == "x" || temp == "X")
    {
        printBorder('h');
        printText("Batal menambah tugas.");
        printBorder('f');
        return true;
    }
    task.desc = truncStr(temp, 256);
    temp.clear();

    printText("Masukkan tenggat tugas, format: dd/mm/yy hh:mm");
    printAsk("Tenggat: ");
    getline(cin, temp);
    if (temp.empty())
    {
        printBorder('h');
        printText("ERR: Tenggat tidak boleh kosong. Ulangi lagi!");
        printBorder('f');
        return formAddTask();
    }
    if (temp == "x" || temp == "X")
    {
        printBorder('h');
        printText("Batal menambah tugas.");
        printBorder('f');
        return true;
    }
    if (temp.length() < 14)
    {
        printBorder('h');
        printText("ERR: Format tenggat tidak valid. Ulangi lagi!");
        printBorder('f');
        return formAddTask();
    }

    deadline.tm_mday = stoi(temp.substr(0, 2));
    deadline.tm_mon = stoi(temp.substr(3, 2)) - 1;
    deadline.tm_year = stoi("20" + temp.substr(6, 2)) - 1900;
    deadline.tm_hour = stoi(temp.substr(9, 2));
    deadline.tm_min = stoi(temp.substr(12, 2));

    time_t dlt = mktime(&deadline);
    if (dlt <= currentTime)
    {
        printBorder('h');
        printText("ERR: Tenggat tidak boleh lebih kecil dari waktu sekarang. Ulangi lagi!");
        printBorder('f');
        return formAddTask();
    }

    printBorder('i');

    printCenter("Berhasil menambahkan tugas.");
    printBorder('f');
    return addTask(task.title, task.desc, dlt);
}

char TodoList::getChoice()
{
    char choice;
    printAsk("Pilih menu (0-8): ");
    choice = cin.get();
    cin.ignore();
    printBorder('f');
    if (choice < '0' || choice > '8')
    {
        printBorder('h');
        printText("Pilihan tidak valid. Silakan coba lagi.");
        printBorder('f');
        printBorder('h');
        return getChoice();
    }
    return choice;
}

// Fungsi untuk menghapus tugas (Feri)
bool TodoList::deleteTask(){
    string temp;
    int id;

    printBorder('h');
    printCenter("HAPUS TUGAS");
    printBorder('i');
    printCenter("Ketik 'x' untuk membatalkan menghapus tugas.");
    printBorder('i', true);

    // viewTasks(); // Tampilkan daftar tugas terlebih dahulu

    printText("Masukkan ID tugas yang ingin dihapus.");
    printAsk("ID: ");
    getline(cin, temp);

    // Validasi x
    if (temp == "x" || temp == "X") {
        printBorder('h');
        printText("Batal menghapus tugas.");
        printBorder('f');
        return true;
    }
// Validasi ID dalam bentuk angka
    try {
        id = stoi (temp);
    } catch (...){
        printBorder('h');
        printText("ERR: ID tidak valid, harus berupa angka. Ulangi lagi!");
        printBorder('f');
        return deleteTask();
    }

    printBorder('i', true);

    // Cari id di listTasks
    auto it = find_if(listTasks.begin(), listTasks.end(), [&](Task& t){
        return t.id == id;
    });

    if ( it == listTasks.end()){
        printBorder('h');
        printText("ERR: ID tugas tidak ditemukan. Ulangi lagi!");
        printBorder('f');
        return deleteTask();
    }

    // Detail tugas yg mau dihapus
    printBorder('h');
    printText("Tugas ditemukan:");
    printText("ID: " + to_string(it->id));
    printText("Judul: " + it->title);   
    printText("Deskripsi: " + it->desc);
    printText("Tenggat: " + getTanggal(it->deadline));
    printText("Dibuat pada: " + getTanggal(it->createdAt)); 
    printText("Status: " + string(it->isDone ? "Selesai" : "Belum Selesai"));
    printBorder('i', true);
    printBorder('i');
    printText("Yakin ingin menghapus tugas ini? (Y/N)");
    printAsk("Pilihan: ");
    getline(cin, temp);
    printBorder('i', true);

    if (temp == "y" || temp == "Y") {
        listTasks.erase(it);
        saveTasks();
        printBorder('h');
        printText("Tugas berhasil dihapus.");
        printBorder('f');
        return true;
    } else if (temp == "n" || temp == "N") {
        printBorder('h');
        printText("Tugas batal dihapus.");
        printBorder('f');
        return true;
    } else {
        printBorder('h');
        printText("ERR: Input tidak valid. Harus 'Y' atau 'N'.");
        printBorder('f');
        return deleteTask(); 
    }
}

bool TodoList::start() {
    printHeader();
    char choice = getChoice();
    while (choice) {
        switch (choice) {
            case '0':
                printBorder('h');
                printCenter("Program Selesai");
                printCenter("Terima kasih!");
                printBorder('f');
                return true;
            case '1': 
                formAddTask();
                break;
            case '2':
            printAllTasks();
            break;
            case '4':
                printLateTasks();
                break;
            case '5':
            editTask();
            break;
            case '6':
                deleteTask();
                break;
            case '8':
            int pilihan;
            TodoList::printAllTasks();
            do
            {
                cout << "Mau Lihat Detail task ke berapa? ";
                cin >> pilihan;
            } while (pilihan < 1 || pilihan > TodoList::sumOfTasks());
            detailTask(pilihan);
            break;
        }

        system("pause");
        system("cls");
        printHeader();
        choice = getChoice();
    }
    return true;
}

int main()
{
    TodoList todo;

    todo.start();

    return 0;
}