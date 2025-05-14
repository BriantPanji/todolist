#include <iostream>
#include <vector>

// Tambah tugas -> Panji
// tampilkan semua tugas -> Sutanto
// tampilkan tugas terlambat -> Sutanto
// selesaikan tugas -> Andreas
// tampilkan to-do selesai -> Sutanto
// detail tugas -> Delrico
// hapus tugas -> Feri
// hapus tugas
// edit tugas -> Delrico
// Exit

struct Task
{
    int id;
    string title;
    string desc;
    time_t deadline;
    time_t waktuPembuatan;
    bool isDone = false;
};

vector<Task> listTasks;

using namespace std;

int main()
{
}