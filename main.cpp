#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct Task {
    string title;
    string deadline; // Format: YYYY-MM-DD
    int priority;    // 1 = Tinggi, 2 = Sedang, 3 = Rendah
};

vector<Task> toDoList;

void tambahTugas() {
    Task task;
    cout << "Judul Tugas   : ";
    cin.ignore();
    getline(cin, task.title);
    cout << "Deadline (YYYY-MM-DD): ";
    getline(cin, task.deadline);
    cout << "Prioritas (1=tinggi, 2=sedang, 3=rendah): ";
    cin >> task.priority;
    toDoList.push_back(task);
    cout << "Tugas berhasil ditambahkan!\n";
}

void tampilkanTugas() {
    if (toDoList.empty()) {
        cout << "Belum ada tugas.\n";
        return;
    }

    cout << "=== Daftar Tugas ===\n";
    for (size_t i = 0; i < toDoList.size(); ++i) {
        cout << i+1 << ". " << toDoList[i].title 
             << " | Deadline: " << toDoList[i].deadline 
             << " | Prioritas: " << toDoList[i].priority << "\n";
    }
}

void hapusTugas() {
    tampilkanTugas();
    if (toDoList.empty()) return;

    int index;
    cout << "Masukkan nomor tugas yang ingin dihapus: ";
    cin >> index;

    if (index >= 1 && index <= (int)toDoList.size()) {
        toDoList.erase(toDoList.begin() + (index - 1));
        cout << "Tugas berhasil dihapus.\n";
    } else {
        cout << "Nomor tidak valid.\n";
    }
}

void urutkanTugas() {
    int pilihan;
    cout << "Urutkan berdasarkan:\n";
    cout << "1. Deadline (Ascending)\n";
    cout << "2. Prioritas (1 ke 3)\n";
    cout << "Pilih: ";
    cin >> pilihan;

    if (pilihan == 1) {
        sort(toDoList.begin(), toDoList.end(), [](const Task& a, const Task& b) {
            return a.deadline < b.deadline;
        });
        cout << "Tugas diurutkan berdasarkan deadline.\n";
    } else if (pilihan == 2) {
        sort(toDoList.begin(), toDoList.end(), [](const Task& a, const Task& b) {
            return a.priority < b.priority;
        });
        cout << "Tugas diurutkan berdasarkan prioritas.\n";
    } else {
        cout << "Pilihan tidak valid.\n";
    }
}

int main() {
    int pilihan;
    do {
        cout << "\n===== TO-DO LIST MENU =====\n";
        cout << "1. Tambah Tugas\n";
        cout << "2. Hapus Tugas\n";
        cout << "3. Tampilkan Semua Tugas\n";
        cout << "4. Urutkan Tugas\n";
        cout << "5. Keluar\n";
        cout << "Pilih menu: ";
        cin >> pilihan;

        switch (pilihan) {
            case 1: tambahTugas(); break;
            case 2: hapusTugas(); break;
            case 3: tampilkanTugas(); break;
            case 4: urutkanTugas(); break;
            case 5: cout << "Keluar dari program.\n"; break;
            default: cout << "Pilihan tidak valid!\n";
        }
    } while (pilihan != 5);

    return 0;
}