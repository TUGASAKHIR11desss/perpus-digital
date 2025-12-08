// main.cpp - Sistem Perpustakaan (gabungan kode 1 + kode 2)
// Menggunakan struct Anggota versi B (dengan field status)

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <regex>
#include <iomanip>
#include <chrono>
#include <sstream>
using namespace std;

// ========== STRUCT ==========
struct Petugas {
    string id_petugas, username, nama, email, password;
};

struct Anggota {
    string id_anggota, kode_anggota, username, nama, email, alamat, password;
    string ID;
    int tanggal, bulan, tahun, status; // status: 0=belum verifikasi, 1=aktif
};

struct Buku {
    string id_buku, isbn, judul, pengarang, penerbit;
    int tahun_terbit, stok;
};

struct Peminjaman {
    string id_pinjam, id_anggota, id_buku;
    int tgl_pinjam, bln_pinjam, thn_pinjam;
    int tgl_kembali, bln_kembali, thn_kembali;
    int status; // 0 = sudah kembali, 1 = active loan, 2 = pending approval
    int denda;
};

// ========== UTILITY FUNCTIONS ==========
bool isValidEmail(const string& email) {
    regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return regex_match(email, emailRegex);
}

string simpleHash(const string& input, const string& salt = "salt123") {
    string salted = input + salt;
    hash<string> hasher;
    return to_string(hasher(salted));
}

bool confirmAction(const string& message) {
    char choice;
    cout << message << " (y/n): ";
    cin >> choice;
    return tolower(choice) == 'y';
}

chrono::system_clock::time_point parseDate(int day, int month, int year) {
    tm tm = {};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    return chrono::system_clock::from_time_t(mktime(&tm));
}

int calculateDaysDifference(int day1, int month1, int year1, int day2, int month2, int year2) {
    auto tp1 = parseDate(day1, month1, year1);
    auto tp2 = parseDate(day2, month2, year2);
    auto diff = chrono::duration_cast<chrono::hours>(tp2 - tp1);
    return diff.count() / 24;
}

// ========== HELPERS PETUGAS ==========
vector<Petugas> loadPetugas() {
    vector<Petugas> petugasList;
    ifstream file("petugas.txt");
    string line;
    Petugas p;
    while (getline(file, line)) {
        if (line.find("id_petugas = ") != string::npos) p.id_petugas = line.substr(13);
        else if (line.find("username = ") != string::npos) p.username = line.substr(11);
        else if (line.find("nama = ") != string::npos) p.nama = line.substr(7);
        else if (line.find("email = ") != string::npos) p.email = line.substr(8);
        else if (line.find("password = ") != string::npos) {
            p.password = line.substr(11);
            petugasList.push_back(p);
            p = Petugas();
        }
    }
    return petugasList;
}

// ========== REGISTER PETUGAS ==========
void registerPetugas() {
    ofstream file("petugas.txt", ios::app);
    if (!file) {
        cout << "Gagal membuka file petugas.txt\n";
        return;
    }
    Petugas etmin;
    cout << "\n===== REGISTER PETUGAS =====\n";
    cout << "Masukkan Username: "; cin >> etmin.username;
    cout << "Masukkan Nama: "; cin.ignore(); getline(cin, etmin.nama);
    cout << "Masukkan Email (harus esemkitabisa@gmail.com): "; cin >> etmin.email;
    cout << "Masukkan Password (harus esemkitabisa): "; cin >> etmin.password;

    if (etmin.email != "esemkitabisa@gmail.com" || etmin.password != "esemkitabisa") {
        cout << "Email atau password tidak valid untuk pendaftaran petugas.\n";
        return;
    }

    string id_petugas;
    bool unique = false;
    do {
        id_petugas = to_string(100000 + rand() % 900000);
        ifstream checkfile("petugas.txt");
        string line;
        unique = true;
        while (getline(checkfile, line)) {
            if (line.find("id_petugas = " + id_petugas) != string::npos) { unique = false; break; }
        }
    } while (!unique);

    etmin.id_petugas = id_petugas;

    file << "===== DATA PETUGAS =====\n";
    file << "id_petugas = " << etmin.id_petugas << "\n";
    file << "username = " << etmin.username << "\n";
    file << "nama = " << etmin.nama << "\n";
    file << "email = " << etmin.email << "\n";
    file << "password = " << etmin.password << "\n\n";
    file.close();

    cout << "Registrasi petugas berhasil. ID: " << etmin.id_petugas << endl;
}

// ========== UTIL TANGGAL ==========
bool tahunKabisat(int tahun) {
    return (tahun % 4 == 0 && tahun % 100 != 0) || (tahun % 400 == 0);
}
int jumlahHari(int bulan, int tahun) {
    switch (bulan) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12: return 31;
        case 4: case 6: case 9: case 11: return 30;
        case 2: return tahunKabisat(tahun) ? 29 : 28;
        default: return -1;
    }
}

// ========== REGISTER ANGGOTA ==========
void registerAnggota() {
    Anggota a;
    cout << "\n===== REGISTER ANGGOTA =====\n";
    cout << "Masukkan Username: "; cin >> a.username;
    cout << "Masukkan Nama: "; cin.ignore(); getline(cin, a.nama);

    do { cout << "Masukkan Tahun lahir: "; cin >> a.tahun; } while (a.tahun < 1900 || a.tahun > 2025);
    do { cout << "Masukkan Bulan lahir (1-12): "; cin >> a.bulan; } while (a.bulan < 1 || a.bulan > 12);

    int maxH;
    do { cout << "Masukkan Tanggal lahir: "; cin >> a.tanggal; maxH = jumlahHari(a.bulan, a.tahun); }
    while (a.tanggal < 1 || a.tanggal > maxH);

    do {
        cout << "Masukkan Email: "; cin >> a.email;
        if (!isValidEmail(a.email)) {
            cout << "Email tidak valid. Harus dalam format yang benar (contoh: user@example.com).\n";
        }
    } while (!isValidEmail(a.email));

    cout << "Masukkan Alamat: "; cin.ignore(); getline(cin, a.alamat);
    cout << "Masukkan Password: "; cin >> a.password;
    a.password = simpleHash(a.password); // Hash the password

    // generate unique ID
    string ID;
    bool unik;
    do {
        ID = to_string(100000 + rand() % 900000);
        ifstream f("anggota.txt"); string ln; unik = true;
        while (getline(f, ln)) if (ln.find("ID kamu        : " + ID) != string::npos) { unik = false; break; }
    } while (!unik);
    a.ID = ID;

    // generate kode anggota
    string kode_dasar = to_string(a.tahun) + to_string(a.bulan) + to_string(a.tanggal);
    int max_urut = 0;
    ifstream infile("anggota.txt"); string line;
    while (getline(infile, line)) {
        if (line.find("Kode Anggota   : ") != string::npos) {
            string kode = line.substr(17);
            if (kode.substr(0, kode_dasar.size()) == kode_dasar) {
                int u = stoi(kode.substr(kode_dasar.size(), 3));
                if (u > max_urut) max_urut = u;
            }
        }
    }
    string urut = to_string(max_urut + 1);
    urut = string(3 - urut.size(), '0') + urut;
    a.kode_anggota = kode_dasar + urut;
    a.status = 0;

    ofstream fout("anggota.txt", ios::app);
    fout << "\n==========================================\n";
    fout << "registrasi berhasil\n";
    fout << "ID kamu        : " << a.ID << "\n";
    fout << "PW             : " << a.password << "\n";
    fout << "Kode Anggota   : " << a.kode_anggota << "\n";
    fout << "Status         : " << a.status << "\n";
    fout.close();

    cout << "Registrasi berhasil! ID Anggota: " << a.ID << endl;
}

// ========== VERIFIKASI ANGGOTA ==========
void verifikasiAnggota() {
    ifstream file("anggota.txt");
    if (!file) { cout << "Belum ada data anggota.\n"; return; }
    vector<string> semua; string line;
    while (getline(file, line)) semua.push_back(line);
    file.close();

    cout << "\n===== ANGGOTA BELUM TERVERIFIKASI =====\n";
    for (int i = 0; i < (int)semua.size(); ++i) {
        if (semua[i].find("Status") != string::npos && semua[i].find("0") != string::npos) {
            int start = max(0, i - 3);
            for (int j = start; j <= i; ++j) {
                if (semua[j].find("PW") == string::npos) cout << semua[j] << endl;
            }
            cout << "-----------------------------\n";
        }
    }

    cout << "Masukkan ID yang ingin diverifikasi: ";
    string target; cin >> target;
    for (int i = 0; i < (int)semua.size(); ++i) {
        if (semua[i].find("ID kamu") != string::npos && semua[i].find(target) != string::npos) {
            int j = i;
            while (j < (int)semua.size() && semua[j].find("Status") == string::npos) ++j;
            if (j < (int)semua.size()) semua[j] = "Status         : 1";
            break;
        }
    }
    ofstream out("anggota.txt");
    for (auto &s : semua) out << s << "\n";
    out.close();
    cout << "Akun berhasil diverifikasi!\n";
}

// ========== BUKU ==========
bool idBukuUnik(const string &id) {
    ifstream file("buku.txt"); string line;
    while (getline(file, line)) if (line.find("ID Buku      : " + id) != string::npos) return false;
    return true;
}
string generateIDBuku() {
    string id;
    do { id = to_string(100000 + rand() % 900000); } while (!idBukuUnik(id));
    return id;
}

void tambahBuku() {
    Buku b;
    cout << "\n===== TAMBAH BUKU =====\n";
    b.id_buku = generateIDBuku();
    cout << "ID Buku: " << b.id_buku << endl;

    do { cout << "Masukkan ISBN (13 digit): "; cin >> b.isbn; } while (b.isbn.length() != 13);
    cin.ignore();
    cout << "Judul Buku: "; getline(cin, b.judul);
    cout << "Pengarang: "; getline(cin, b.pengarang);
    cout << "Penerbit: "; getline(cin, b.penerbit);
    cout << "Tahun Terbit: "; cin >> b.tahun_terbit;
    cout << "Stok: "; cin >> b.stok;

    ofstream f("buku.txt", ios::app);
    f << "\n===== DATA BUKU =====\n";
    f << "ID Buku      : " << b.id_buku << "\n";
    f << "ISBN         : " << b.isbn << "\n";
    f << "Judul Buku   : " << b.judul << "\n";
    f << "Pengarang    : " << b.pengarang << "\n";
    f << "Penerbit     : " << b.penerbit << "\n";
    f << "Tahun Terbit : " << b.tahun_terbit << "\n";
    f << "Stok         : " << b.stok << "\n";
    f.close();

    cout << "Buku berhasil ditambahkan!\n";
}

void hapusBuku() {
    ifstream f("buku.txt"); if (!f) { cout << "Belum ada data buku.\n"; return; }
    vector<string> lines; string line;
    while (getline(f, line)) lines.push_back(line);
    f.close();

    cout << "Masukkan ID Buku yang ingin dihapus: "; string id; cin >> id;
    bool found = false;
    ofstream out("buku.txt");
    for (int i = 0; i < (int)lines.size(); ++i) {
        if (lines[i].find("ID Buku") != string::npos && lines[i].find(id) != string::npos) {
            found = true;
            i += 7; // skip block (ID + 6 lines)
            continue;
        }
        out << lines[i] << "\n";
    }
    out.close();
    cout << (found ? "Buku berhasil dihapus!\n" : "ID Buku tidak ditemukan.\n");
}

void cariBuku() {
    ifstream f("buku.txt"); if (!f) { cout << "Belum ada data buku.\n"; return; }
    cin.ignore();
    cout << "Masukkan judul buku: "; string kw; getline(cin, kw);
    string line; bool found = false;
    cout << "\n===== HASIL PENCARIAN =====\n";
    while (getline(f, line)) {
        if (line.find("Judul Buku") != string::npos && line.substr(line.find(":") + 2).find(kw) != string::npos) {
            found = true; cout << line << endl;
            for (int i = 0; i < 6; ++i) { if (getline(f, line)) cout << line << endl; }
            cout << "-----------------------------\n";
        }
    }
    if (!found) cout << "Buku tidak ditemukan.\n";
    f.close();
}

void menuLihatBuku() {
    ifstream f("buku.txt"); if (!f) { cout << "Belum ada data buku.\n"; return; }
    vector<Buku> list; string line;
    while (getline(f, line)) {
        if (line.find("ID Buku") != string::npos) {
            Buku b;
            b.id_buku = line.substr(line.find(":") + 2);
            if (getline(f, line)) b.isbn = line.substr(line.find(":") + 2);
            if (getline(f, line)) b.judul = line.substr(line.find(":") + 2);
            if (getline(f, line)) b.pengarang = line.substr(line.find(":") + 2);
            if (getline(f, line)) b.penerbit = line.substr(line.find(":") + 2);
            if (getline(f, line)) b.tahun_terbit = stoi(line.substr(line.find(":") + 2));
            if (getline(f, line)) b.stok = stoi(line.substr(line.find(":") + 2));
            list.push_back(b);
        }
    }
    f.close();

    if (list.empty()) { cout << "Belum ada buku.\n"; return; }

    cout << "\n===== DATA BUKU =====\n";
    for (int i = 0; i < (int)list.size(); ++i) cout << i + 1 << ". " << list[i].judul << " (Stok: " << list[i].stok << ")\n";

    cout << "Pilih nomor untuk detail: "; int pilih; cin >> pilih;
    if (pilih < 1 || pilih > (int)list.size()) { cout << "Pilihan tidak valid.\n"; return; }
    Buku b = list[pilih - 1];
    cout << "\n===== DETAIL =====\n";
    cout << "ID Buku      : " << b.id_buku << "\nISBN         : " << b.isbn << "\nJudul Buku   : " << b.judul << "\nPengarang    : " << b.pengarang << "\nPenerbit     : " << b.penerbit << "\nTahun Terbit : " << b.tahun_terbit << "\nStok         : " << b.stok << endl;
}

// ========== New helper: pilih buku dan kembalikan object untuk anggota ==========
Buku pilihBukuDariList(bool &ok) {
    Buku kosong;
    ok = false;

    ifstream file("buku.txt");
    if (!file) return kosong;

    vector<Buku> listBuku;
    string line;

    while (getline(file, line)) {
        if (line.find("ID Buku") != string::npos) {
            Buku b;
            b.id_buku = line.substr(line.find(":") + 2);

            getline(file, line); b.isbn = line.substr(line.find(":") + 2);
            getline(file, line); b.judul = line.substr(line.find(":") + 2);
            getline(file, line); b.pengarang = line.substr(line.find(":") + 2);
            getline(file, line); b.penerbit = line.substr(line.find(":") + 2);
            getline(file, line); b.tahun_terbit = stoi(line.substr(line.find(":") + 2));
            getline(file, line); b.stok = stoi(line.substr(line.find(":") + 2));

            listBuku.push_back(b);
        }
    }
    file.close();

    if (listBuku.empty()) return kosong;

    cout << "\n===== DATA BUKU =====\n";
    for (int i = 0; i < listBuku.size(); i++) {
        cout << i + 1 << ". " << listBuku[i].judul << endl;
    }
    cout << "====================================\n";

    int pilih;
    cout << "masukkan pilihan anda : ";
    cin >> pilih;

    if (pilih < 1 || pilih > listBuku.size()) {
        cout << "Pilihan tidak valid.\n";
        return kosong;
    }

    ok = true;
    return listBuku[pilih - 1];
}

void menuBuku() {
    int p;
    do {
        cout << "\n===== MENU BUKU =====\n1. Tambah Buku\n2. Lihat Buku\n3. Hapus Buku\n4. Cari Buku\n5. Kembali\nPilih: "; cin >> p;
        switch (p) {
            case 1: tambahBuku(); break;
            case 2: menuLihatBuku(); break;
            case 3: hapusBuku(); break;
            case 4: cariBuku(); break;
        }
    } while (p != 5);
}
void menuBukuAnggota() {
    int p;
    do {
        cout << "\n===== MENU BUKU =====\n1. Lihat Buku\n2. Cari Buku\n3. Kembali\nPilih: "; cin >> p;
        switch (p) {
            case 1: menuLihatBuku(); break;
            case 2: cariBuku(); break;
            
        }
    } while (p != 3);
}


// ========== PEMINJAMAN ==========
bool idPinjamUnik(const string &id) {
    ifstream f("peminjaman.txt"); string l;
    while (getline(f, l)) if (l.find("ID Peminjaman : " + id) != string::npos) return false;
    return true;
}
string generateIDPinjam() { string id; do { id = to_string(100000 + rand() % 900000); } while (!idPinjamUnik(id)); return id; }

void updateStok(const string &id_buku, int perubahan) {
    ifstream f("buku.txt"); if (!f) return;
    vector<string> lines; string ln;
    while (getline(f, ln)) lines.push_back(ln);
    f.close();

    for (int i = 0; i < (int)lines.size(); ++i) {
        if (lines[i].find("ID Buku") != string::npos && lines[i].find(id_buku) != string::npos) {
            if (i + 6 < (int)lines.size()) {
                int stok = 0;
                try { stok = stoi(lines[i + 6].substr(lines[i + 6].find(":") + 2)); } catch(...) { stok = 0; }
                stok += perubahan; if (stok < 0) stok = 0;
                lines[i + 6] = "Stok         : " + to_string(stok);
            }
            break;
        }
    }
    ofstream out("buku.txt");
    for (auto &s : lines) out << s << "\n";
    out.close();
}

void pinjamBuku(string id_anggota_prefill = "", string id_buku_prefill = "") {
    Peminjaman p;
    cout << "\n===== PEMINJAMAN =====\n";
    if (id_anggota_prefill.empty()) {
        cout << "Masukkan ID Anggota: "; cin >> p.id_anggota;
    } else {
        p.id_anggota = id_anggota_prefill;
    }
    if (id_buku_prefill.empty()) {
        // Allow member to select book from list
        bool ok;
        Buku selectedBook = pilihBukuDariList(ok);
        if (!ok) return; // Selection failed
        p.id_buku = selectedBook.id_buku;
    } else {
        p.id_buku = id_buku_prefill;
    }

    // cek stok dulu
    // baca buku, cari stok
    ifstream f("buku.txt"); bool found=false; string ln; int stok=0;
    while (getline(f, ln)) {
        if (ln.find("ID Buku") != string::npos && ln.find(p.id_buku) != string::npos) {
            // stok pada i+6, so read next 6 lines to reach stok
            for (int i=0;i<6;i++) getline(f, ln);
            if (ln.find("Stok")!=string::npos) {
                try { stok = stoi(ln.substr(ln.find(":")+2)); } catch(...) { stok=0; }
                found = true;
            }
            break;
        }
    }
    f.close();
    if (!found) { cout << "ID buku tidak ditemukan.\n"; return; }
    if (stok <= 0) { cout << "Stok habis. Tidak bisa meminjam.\n"; return; }

    p.id_pinjam = generateIDPinjam();
    time_t now = time(0); tm *ltm = localtime(&now);
    p.tgl_pinjam = ltm->tm_mday; p.bln_pinjam = 1 + ltm->tm_mon; p.thn_pinjam = 1900 + ltm->tm_year;

    // calculate expected return date: 7 days later
    tm expected = *ltm;
    expected.tm_mday += 7;
    int max_days = jumlahHari(expected.tm_mon + 1, expected.tm_year + 1900);
    if (expected.tm_mday > max_days) {
        expected.tm_mday -= max_days;
        expected.tm_mon++;
        if (expected.tm_mon > 11) {
            expected.tm_mon = 0;
            expected.tm_year++;
        }
    }
    p.tgl_kembali = expected.tm_mday; p.bln_kembali = expected.tm_mon + 1; p.thn_kembali = expected.tm_year + 1900;

    cout << "Tanggal Pinjam: " << p.tgl_pinjam << "-" << p.bln_pinjam << "-" << p.thn_pinjam << endl;
    cout << "Tanggal Kembali Expected: " << p.tgl_kembali << "-" << p.bln_kembali << "-" << p.thn_kembali << endl;

    p.status = 2; p.denda = 0;

    ofstream out("peminjaman.txt", ios::app);
    out << "\n===== DATA PEMINJAMAN =====\n";
    out << "ID Peminjaman : " << p.id_pinjam << "\n";
    out << "ID Anggota    : " << p.id_anggota << "\n";
    out << "ID Buku       : " << p.id_buku << "\n";
    out << "Tanggal Pinjam: " << p.tgl_pinjam << "-" << p.bln_pinjam << "-" << p.thn_pinjam << "\n";
    out << "Tanggal Kembali Expected: " << p.tgl_kembali << "-" << p.bln_kembali << "-" << p.thn_kembali << "\n";
    out << "Tanggal Kembali Actual: 0-0-0\n";
    out << "Status        : 2\n";
    out << "Denda         : 0\n";
    out.close();

    // Do not update stock yet, pending approval
    cout << "Permintaan peminjaman berhasil diajukan! ID Peminjaman: " << p.id_pinjam << " (Menunggu persetujuan petugas)\n";
}

void kembalikanBuku() {
    ifstream f("peminjaman.txt"); if (!f) { cout << "Belum ada data peminjaman.\n"; return; }
    vector<string> lines; string ln;
    while (getline(f, ln)) lines.push_back(ln);
    f.close();

    cout << "Masukkan ID Peminjaman: "; string id; cin >> id;
    int idx = -1;
    for (int i=0;i<(int)lines.size();++i) if (lines[i].find("ID Peminjaman")!=string::npos && lines[i].find(id)!=string::npos) { idx=i; break; }
    if (idx == -1) { cout << "ID tidak ditemukan.\n"; return; }

    string id_buku = (idx+2 < (int)lines.size()) ? lines[idx+2].substr(lines[idx+2].find(":")+2) : "";
    string status = (idx+6 < (int)lines.size()) ? lines[idx+6].substr(lines[idx+6].find(":")+2) : "1";
    if (status == "0") { cout << "Buku sudah dikembalikan.\n"; return; }
    if (status == "2") { cout << "Peminjaman belum disetujui petugas.\n"; return; }

    // parse tanggal kembali expected
    string tgl_expected = (idx+4 < (int)lines.size()) ? lines[idx+4].substr(lines[idx+4].find(":")+2) : "";
    int d_exp=0, m_exp=0, y_exp=0;
    if (sscanf(tgl_expected.c_str(), "%d-%d-%d", &d_exp, &m_exp, &y_exp) != 3) { cout << "Format tanggal expected tidak valid.\n"; return; }

    tm exp = {}; exp.tm_mday = d_exp; exp.tm_mon = m_exp-1; exp.tm_year = y_exp-1900; exp.tm_hour=0; exp.tm_min=0; exp.tm_sec=0;
    time_t exp_time = mktime(&exp);
    if (exp_time == (time_t)-1) { cout << "Gagal memproses tanggal expected.\n"; return; }

    time_t now = time(0);
    double diff = difftime(now, exp_time);
    int diff_days = (int)(diff / (24*60*60));
    int denda = (diff_days > 0) ? diff_days * 1000 : 0;

    tm *ltm = localtime(&now);
    string tgl_kembali_actual = to_string(ltm->tm_mday) + "-" + to_string(1 + ltm->tm_mon) + "-" + to_string(1900 + ltm->tm_year);

    if (idx+5 < (int)lines.size()) lines[idx+5] = "Tanggal Kembali Actual: " + tgl_kembali_actual;
    if (idx+6 < (int)lines.size()) lines[idx+6] = "Status        : 0";
    if (idx+7 < (int)lines.size()) lines[idx+7] = "Denda         : " + to_string(denda);

    ofstream out("peminjaman.txt");
    for (auto &s : lines) out << s << "\n";
    out.close();

    if (!id_buku.empty()) updateStok(id_buku, +1);
    cout << "Pengembalian berhasil! Denda: Rp. " << denda << endl;
}

// ========== APPROVE PEMINJAMAN ==========
void approvePeminjaman() {
    ifstream f("peminjaman.txt"); if (!f) { cout << "Belum ada data peminjaman.\n"; return; }
    vector<string> lines; string ln;
    while (getline(f, ln)) lines.push_back(ln);
    f.close();

    cout << "\n===== PEMINJAMAN PENDING APPROVAL =====\n";
    vector<int> pendingIndices;
    for (int i = 0; i < (int)lines.size(); ++i) {
        if (lines[i].find("Status        : 2") != string::npos) {
            pendingIndices.push_back(i);
            int start = max(0, i - 6);
            for (int j = start; j <= i; ++j) cout << lines[j] << endl;
            cout << "-----------------------------\n";
        }
    }

    if (pendingIndices.empty()) { cout << "Tidak ada peminjaman yang menunggu approval.\n"; return; }

    cout << "Masukkan ID Peminjaman yang ingin disetujui: ";
    string id; cin >> id;
    int idx = -1;
    for (int i : pendingIndices) {
        if (lines[i - 6].find("ID Peminjaman : " + id) != string::npos) { idx = i; break; }
    }
    if (idx == -1) { cout << "ID tidak ditemukan atau tidak pending.\n"; return; }

    // Change status to 1
    lines[idx] = "Status        : 1";

    // Update stock
    string id_buku = lines[idx - 4].substr(lines[idx - 4].find(":") + 2);
    updateStok(id_buku, -1);

    ofstream out("peminjaman.txt");
    for (auto &s : lines) out << s << "\n";
    out.close();

    cout << "Peminjaman berhasil disetujui!\n";
}

// ========== DETAIL PEMINJAMAN ==========
void detailPeminjaman(string id_anggota) {
    ifstream f("peminjaman.txt"); if (!f) { cout << "Belum ada data peminjaman.\n"; return; }
    vector<string> lines; string ln;
    while (getline(f, ln)) lines.push_back(ln);
    f.close();

    cout << "\n=============================\n";
    cout << "Masukkan ID peminjaman : "; string id; cin >> id;

    bool found = false;
    for (int i = 0; i < (int)lines.size(); ++i) {
        if (lines[i].find("ID Peminjaman : " + id) != string::npos) {
            // Check if ID Anggota matches
            if (i + 1 < (int)lines.size() && lines[i + 1].find("ID Anggota    : " + id_anggota) != string::npos) {
                found = true;
                cout << "\n===== DATA PEMINJAMAN =====\n";
                for (int j = i; j < i + 8 && j < (int)lines.size(); ++j) {
                    cout << lines[j] << endl;
                }
                break;
            }
        }
    }
    if (!found) {
        cout << "ID peminjaman tidak ditemukan atau bukan milik Anda.\n";
    }
}

// ========== PERPANJANG PEMINJAMAN ==========
void perpanjangPeminjaman(string id_anggota) {
    ifstream f("peminjaman.txt"); if (!f) { cout << "Belum ada data peminjaman.\n"; return; }
    vector<string> lines; string ln;
    while (getline(f, ln)) lines.push_back(ln);
    f.close();

    cout << "\n=============================\n";
    cout << "Masukkan ID peminjaman : "; string id; cin >> id;

    int idx = -1;
    for (int i = 0; i < (int)lines.size(); ++i) {
        if (lines[i].find("ID Peminjaman : " + id) != string::npos) {
            // Check if ID Anggota matches and status is 1 (active)
            if (i + 1 < (int)lines.size() && lines[i + 1].find("ID Anggota    : " + id_anggota) != string::npos &&
                i + 6 < (int)lines.size() && lines[i + 6].find("Status        : 1") != string::npos) {
                idx = i + 4; // Tanggal Kembali Expected line
                break;
            }
        }
    }
    if (idx == -1) {
        cout << "ID peminjaman tidak ditemukan, bukan milik Anda, atau tidak aktif.\n";
        return;
    }

    // Parse current expected return date
    string tgl_expected = lines[idx].substr(lines[idx].find(":") + 2);
    int d, m, y;
    if (sscanf(tgl_expected.c_str(), "%d-%d-%d", &d, &m, &y) != 3) {
        cout << "Format tanggal tidak valid.\n";
        return;
    }

    // Extend by 7 days
    tm exp = {}; exp.tm_mday = d; exp.tm_mon = m - 1; exp.tm_year = y - 1900;
    exp.tm_mday += 7;
    int max_days = jumlahHari(exp.tm_mon + 1, exp.tm_year + 1900);
    if (exp.tm_mday > max_days) {
        exp.tm_mday -= max_days;
        exp.tm_mon++;
        if (exp.tm_mon > 11) {
            exp.tm_mon = 0;
            exp.tm_year++;
        }
    }

    string new_date = to_string(exp.tm_mday) + "-" + to_string(exp.tm_mon + 1) + "-" + to_string(exp.tm_year + 1900);
    lines[idx] = "Tanggal Kembali Expected: " + new_date;

    ofstream out("peminjaman.txt");
    for (auto &s : lines) out << s << "\n";
    out.close();

    cout << "Peminjaman berhasil diperpanjang! Tanggal kembali baru: " << new_date << endl;
}

// ========== LOGIN & MENU ==========

void menuPetugas(); // forward
void menuAnggotaInteractive(string id_anggota); // forward

void loginPetugas() {
    vector<Petugas> list = loadPetugas();
    if (list.empty()) { cout << "Belum ada petugas terdaftar. Silakan register petugas.\n"; return; }

    string id, email, pw;
    cout << "\n===== LOGIN PETUGAS =====\n";
    cout << "Masukkan id: "; cin >> id;
    cout << "Masukkan email: "; cin >> email;
    cout << "Masukkan password: "; cin >> pw;

    bool ok=false; Petugas cur;
    for (auto &p : list) if (p.id_petugas==id && p.email==email && p.password==pw) { ok=true; cur=p; break; }
    if (!ok) { cout << "id/email/password salah.\n"; return; }
    cout << "Login berhasil! Selamat datang, " << cur.nama << endl;
    menuPetugas();
}

void loginAnggota() {
    Anggota a;
    cout << "\n===== LOGIN ANGGOTA =====\n";
    cin.ignore();
    cout << "Masukkan Nama: "; getline(cin, a.nama);
    cout << "Masukkan ID: "; cin >> a.ID;
    cout << "Masukkan Password: "; cin >> a.password;
    a.password = simpleHash(a.password); // Hash the input password

    ifstream f("anggota.txt"); if (!f) { cout << "Belum ada data anggota.\n"; return; }
    string ln; bool found=false;
    while (getline(f, ln)) {
        if (ln.find("ID kamu") != string::npos) {
            string id_file = ln.substr(ln.find(":")+2);
            if (!getline(f, ln)) break; string pw_file = ln.substr(ln.find(":")+2);
            if (!getline(f, ln)) break; // kode anggota
            if (!getline(f, ln)) break; string status_file = ln.substr(ln.find(":")+2);
            if (id_file == a.ID) {
                found = true;
                if (a.password != pw_file) { cout << "Password salah!\n"; f.close(); return; }
                if (status_file == "0") { cout << "Akun belum diverifikasi petugas.\n"; f.close(); return; }
                cout << "Login anggota berhasil. Selamat datang, " << a.nama << endl;
                menuAnggotaInteractive(a.ID);
                f.close();
                return;
            }
        }
    }
    f.close();
    if (!found) cout << "ID anggota tidak ditemukan.\n";
}

void menuPetugas() {
    int p;
    do {
        cout << "\n===== MENU PETUGAS =====\n1. Verifikasi Anggota\n2. Kelola Buku\n3. Lihat Peminjaman\n4. Lihat Anggota\n5. Approve Peminjaman\n6. Logout\nPilih: "; cin >> p;
        switch (p) {
            case 1: verifikasiAnggota(); break;
            case 2: menuBuku(); break;
            case 3: { ifstream f("peminjaman.txt"); string l; if (!f) cout<<"Belum ada data peminjaman.\n"; else while (getline(f,l)) cout<<l<<endl; break; }
            case 4: { ifstream f("anggota.txt"); string l; if (!f) cout<<"Belum ada data anggota.\n"; else while (getline(f,l)) cout<<l<<endl; break; }
            case 5: approvePeminjaman(); break;
        }
    } while (p != 6);
}

void menuAnggotaInteractive(string id_anggota) {
    int a;
    do {
        cout << "\n===== MENU ANGGOTA =====\n1. Melihat Buku\n2. Menu Peminjaman\n3. Kembalikan Buku\n4. Logout\nPilih: "; cin >> a;
        switch (a) {
            case 1: menuBukuAnggota(); break;
            case 2: {
                int sub;
                do {
                    cout << "\n===== MENU PEMINJAMAN =====\n1. Pinjam Buku\n2. Detail Peminjaman\n3. Perpanjang Peminjaman\n4. Kembali\nPilih: "; cin >> sub;
                    switch (sub) {
                        case 1: pinjamBuku(id_anggota); break;
                        case 2: detailPeminjaman(id_anggota); break;
                        case 3: perpanjangPeminjaman(id_anggota); break;
                        case 4: break;
                        default: cout << "Pilihan tidak valid.\n";
                    }
                } while (sub != 4);
                break;
            }
            case 3: kembalikanBuku(); break;
        }
    } while (a != 4);
}


// ========== HALAMAN PILIH ==========
void halamanPilihLogin() {
    int pilih;
    cout << "\n===== PILIH LOGIN =====\n";
    cout << "1. Login sebagai Petugas\n";
    cout << "2. Login sebagai Anggota\n";
    cout << "3. Kembali\n";
    cout << "Pilih: "; cin >> pilih;
    switch (pilih) {
        case 1: loginPetugas(); break;
        case 2: loginAnggota(); break;
        case 3: break;
        default: cout << "Pilihan tidak valid.\n";
    }
}

void halamanPilihRegister() {
    int pilih;
    cout << "\n===== PILIH REGISTER =====\n";
    cout << "1. Register sebagai Petugas\n";
    cout << "2. Register sebagai Anggota\n";
    cout << "3. Kembali\n";
    cout << "Pilih: "; cin >> pilih;
    switch (pilih) {
        case 1: registerPetugas(); break;
        case 2: registerAnggota(); break;
        case 3: break;
        default: cout << "Pilihan tidak valid.\n";
    }
}

// ========== MAIN ==========
int main() {
    srand(time(NULL));
    int menu;

    do {
        cout << "\n=========== SISTEM PERPUSTAKAAN ==========" << endl;
        cout << "1. Login" << endl;
        cout << "2. Register" << endl;
        cout << "3. Keluar" << endl;
        cout << "==========================================" << endl;
        cout << "Pilih menu: ";
        cin >> menu;

        switch (menu) {
            case 1: halamanPilihLogin(); break;
            case 2: halamanPilihRegister(); break;
            case 3:
                cout << "Keluar dari sistem...\n";
                break;
            default:
                cout << "Pilihan tidak valid.\n";
        }

    } while (menu != 3);

    return 0;
}

// khanza nyoba git