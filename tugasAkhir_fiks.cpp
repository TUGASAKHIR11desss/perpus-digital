#include <iostream> 
#include <fstream> 
#include <string> 
#include <iomanip> 
#include <sstream> 
#include <ctime>
#include <cstdlib>
using namespace std;

struct Petugas {
    string id_petugas, username, nama,password;
};

struct Anggota {
    string id_anggota, kode_anggota, username, nama, email, alamat, password;
    int tanggal, bulan, tahun;

};

string buatIdAcak() {
    srand(time(0));
    int angkaAcak = rand() % 900000 + 100000;
    return to_string(angkaAcak);
}

void registerPetugas() {
    ofstream file("petugas.txt", ios::app);
    Petugas p;

    cout << "\n===== REGISTER PETUGAS =====\n";
    p.id_petugas = buatIdAcak();

    cout << "Masukkan Username: ";
    cin >> p.username;
    cout << "Masukkan Nama: ";
    cin.ignore();
    getline(cin, p.nama);
    cout << "Masukkan Password: ";
    cin >> p.password;

    file << "===== DATA PETUGAS =====" << "\n";
    file << "id_petugas = " << p.id_petugas  << "\n";
    file << "username = " << p.username << "\n";
    file << "nama = " << p.nama << "\n";
    file << "password = " << p.password << "\n";
    file << endl;
    file.close();

    cout << "\n==========================================\n";
    cout << "registrasi berhasil\n";
    cout << "ID kamu: " << p.id_petugas << endl;
    cout << "simpan id untuk login\n";
    cout << "==========================================\n";
}

void registerAnggota() {
    ofstream file("anggota.txt", ios::app);
    Anggota a;

    cout << "\n===== REGISTER ANGGOTA =====\n";
    a.id_anggota = buatIdAcak();

    cout << "Masukkan Username: ";
    cin >> a.username;
    cout << "Masukkan Nama: ";
    cin.ignore();
    getline(cin, a.nama);
    cout << "Masukkan Tanggal lahir: ";
    cin >> a.tanggal;
    cout << "Masukkan Bulan lahir: ";
    cin >> a.bulan;
    cout << "Masukkan Tahun lahir: ";
    cin >> a.tahun;
    cout << "Masukkan Alamat Email: ";
    cin >> a.email;
    cout << "Masukkan Alamat: ";
    cin.ignore();
    getline(cin, a.alamat);
    cout << "Masukkan Password: ";
    cin >> a.password;

    ifstream fileIn("anggota.txt");
    string baris, lastLine;
    while (getline(fileIn, baris)) {
        if (!baris.empty())
            lastLine = baris;
    }
    fileIn.close();

    int urutan = 0;
    if (!lastLine.empty())
        urutan = stoi(lastLine.substr(lastLine.length() - 3));
    urutan++;

    stringstream ss;
    ss << setw(4) << setfill('0') << a.tahun
       << setw(2) << setfill('0') << a.bulan
       << setw(2) << setfill('0') << a.tanggal
       << setw(3) << setfill('0') << urutan;
    a.kode_anggota = ss.str();

   file << "===== DATA ANGGOTA =====" << "\n";
    file << "id_anggota = " << a.id_anggota << "\n";
    file << "kode_anggota = " << a.kode_anggota << "\n";
    file << "username = " << a.username << "\n";
    file << "nama = " << a.nama << "\n";
    file << "tanggal_lahir = " << a.tanggal << "-" << a.bulan << "-" << a.tahun << "\n";
    file << "email = " << a.email << "\n";
    file << "alamat = " << a.alamat << "\n";
    file << "password = " << a.password << "\n";
    file << endl;
    file.close();

    cout << "\n==========================================\n";
    cout << "registrasi berhasil\n";
    cout << "ID kamu        : " << a.id_anggota << endl;
    cout << "Kode Anggota   : " << a.kode_anggota << endl;
    cout << "Simpan ID & kode kamu untuk login\n";
    cout << "==========================================\n";
}

void loginPetugas() {
    ifstream file("petugas.txt");
    string line;
    Petugas p;
    string id_input, password_input;
    bool berhasil = false;

    cout << "\n===== LOGIN PETUGAS =====\n";
    cout << "Masukkan ID Petugas: ";
    cin >> id_input;
    cout << "Masukkan Password: ";
    cin >> password_input;

    while (getline(file, line)) {
        if (line == "===== DATA PETUGAS =====") {
            // New format: read next 4 lines
            string id_line, user_line, nama_line, pass_line;
            getline(file, id_line);
            getline(file, user_line);
            getline(file, nama_line);
            getline(file, pass_line);

            // Extract values after " = "
            size_t pos = id_line.find(" = ");
            if (pos != string::npos) p.id_petugas = id_line.substr(pos + 3);
            pos = user_line.find(" = ");
            if (pos != string::npos) p.username = user_line.substr(pos + 3);
            pos = nama_line.find(" = ");
            if (pos != string::npos) p.nama = nama_line.substr(pos + 3);
            pos = pass_line.find(" = ");
            if (pos != string::npos) p.password = pass_line.substr(pos + 3);

            if (id_input == p.id_petugas && password_input == p.password) {
                berhasil = true;
                break;
            }
        } else if (line.find('|') != string::npos) {
            // Old format: pipe-separated
            stringstream ss(line);
            getline(ss, p.id_petugas, '|');
            getline(ss, p.username, '|');
            getline(ss, p.nama, '|');
            getline(ss, p.password, '|');

            if (id_input == p.id_petugas && password_input == p.password) {
                berhasil = true;
                break;
            }
        }
    }
    file.close();

    cout << "\n==========================================\n";
    if (berhasil) {
        cout << "login berhasil\n";
        cout << "\n==== welcome " << p.nama << " ====\n";
    } else {
        cout << "ID atau password salah.\n";
    }
    cout << "==========================================\n";
}

void loginAnggota() {
    ifstream file("anggota.txt");
    string line;
    Anggota a;
    string id_input, password_input;
    bool berhasil = false;

    cout << "\n===== LOGIN ANGGOTA =====\n";
    cout << "Masukkan ID Anggota: ";
    cin >> id_input;
    cout << "Masukkan Password: ";
    cin >> password_input;

    while (getline(file, line)) {
        if (line == "===== DATA ANGGOTA =====") {
            // New format: read next 8 lines (but we only need id and password)
            string id_line, kode_line, user_line, nama_line, tgl_line, email_line, alamat_line, pass_line;
            getline(file, id_line);
            getline(file, kode_line);
            getline(file, user_line);
            getline(file, nama_line);
            getline(file, tgl_line);
            getline(file, email_line);
            getline(file, alamat_line);
            getline(file, pass_line);

            // Extract values after " = "
            size_t pos = id_line.find(" = ");
            if (pos != string::npos) a.id_anggota = id_line.substr(pos + 3);
            pos = kode_line.find(" = ");
            if (pos != string::npos) a.kode_anggota = kode_line.substr(pos + 3);
            pos = user_line.find(" = ");
            if (pos != string::npos) a.username = user_line.substr(pos + 3);
            pos = nama_line.find(" = ");
            if (pos != string::npos) a.nama = nama_line.substr(pos + 3);
            pos = pass_line.find(" = ");
            if (pos != string::npos) a.password = pass_line.substr(pos + 3);

            if (id_input == a.id_anggota && password_input == a.password) {
                berhasil = true;
                break;
            }
        } else if (line.find(';') != string::npos) {
            // Old format: semicolon-separated (assuming id;...;password)
            stringstream ss(line);
            getline(ss, a.id_anggota, ';');
            getline(ss, a.kode_anggota, ';');
            getline(ss, a.username, ';');
            getline(ss, a.nama, ';');
            getline(ss, a.email, ';');
            getline(ss, a.alamat, ';');
            getline(ss, a.password, ';');

            if (id_input == a.id_anggota && password_input == a.password) {
                berhasil = true;
                break;
            }
        }
    }
    file.close();

    cout << "\n==========================================\n";
    if (berhasil) {
        cout << "login berhasil\n";
        cout << "\n==== welcome " << a.nama << " " << a.kode_anggota << " ====\n";
    } else {
        cout << "ID atau password salah.\n";
    }
    cout << "==========================================\n";
}

void halamanPilihRegister() {
    int pilih;
    cout << "\n===== PILIH JENIS REGISTER =====\n";
    cout << "1. Petugas\n";
    cout << "2. Anggota\n";
    cout << "Pilih: ";
    cin >> pilih;

    if (pilih == 1)
        registerPetugas();
    else if (pilih == 2)
        registerAnggota();
    else
        cout << "Pilihan tidak valid\n";
}

void halamanPilihLogin() {
    int pilih;
    cout << "\n===== PILIH JENIS LOGIN =====\n";
    cout << "1. Petugas\n";
    cout << "2. Anggota\n";
    cout << "Pilih: ";
    cin >> pilih;

    if (pilih == 1)
        loginPetugas();
    else if (pilih == 2)
        loginAnggota();
    else
        cout << "Pilihan tidak valid\n";
}

int main() {
    int menu;

    do {
        cout << "\n=========== SISTEM PERPUSTAKAAN ==========\n";
        cout << "1. Login\n";
        cout << "2. Register\n";
        cout << "3. Keluar\n";
        cout << "==========================================\n";
        cout << "Pilih menu: ";
        cin >> menu;

        switch (menu) {
            case 1: halamanPilihLogin(); break;
            case 2: halamanPilihRegister(); break;
            case 3:
                cout << "\nKeluar dari sistem...\n";
                break;
            default:
                cout << "Pilihan tidak valid\n";
        }
    } while (menu !=3);

    return 0;
}