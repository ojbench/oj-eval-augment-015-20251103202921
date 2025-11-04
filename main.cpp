#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

using namespace std;

const string DATA_FILE = "database.dat";
const int MAX_KEY_LEN = 65;
const int CHUNK_SIZE = 500;  // Process in chunks to save memory

struct Record {
    char key[MAX_KEY_LEN];
    int value;

    Record() {
        memset(key, 0, sizeof(key));
        value = 0;
    }

    Record(const string& k, int v) {
        memset(key, 0, sizeof(key));
        strncpy(key, k.c_str(), MAX_KEY_LEN - 1);
        value = v;
    }

    bool operator<(const Record& other) const {
        int cmp = strcmp(key, other.key);
        if (cmp != 0) return cmp < 0;
        return value < other.value;
    }

    bool operator==(const Record& other) const {
        return strcmp(key, other.key) == 0 && value == other.value;
    }
};

class FileDatabase {
private:
    fstream file;

    int getRecordCount() {
        file.seekg(0, ios::end);
        long fileSize = file.tellg();
        return fileSize / sizeof(Record);
    }

    void readRecord(int idx, Record& record) {
        file.seekg(idx * sizeof(Record), ios::beg);
        file.read((char*)&record, sizeof(Record));
    }

public:
    FileDatabase() {
        file.open(DATA_FILE, ios::in | ios::out | ios::binary);
        if (!file.is_open()) {
            file.open(DATA_FILE, ios::out | ios::binary);
            file.close();
            file.open(DATA_FILE, ios::in | ios::out | ios::binary);
        }
    }

    ~FileDatabase() {
        if (file.is_open()) {
            file.close();
        }
    }

    void insert(const string& key, int value) {
        Record newRecord(key, value);

        // Load all records in chunks to check for duplicates
        int count = getRecordCount();
        bool isDuplicate = false;

        for (int i = 0; i < count; i += CHUNK_SIZE) {
            int chunkEnd = min(i + CHUNK_SIZE, count);
            vector<Record> chunk(chunkEnd - i);

            for (int j = i; j < chunkEnd; j++) {
                readRecord(j, chunk[j - i]);
                if (chunk[j - i] == newRecord) {
                    isDuplicate = true;
                    break;
                }
            }

            if (isDuplicate) return;
        }

        // Append new record
        file.seekp(0, ios::end);
        file.write((const char*)&newRecord, sizeof(Record));
        file.flush();

        // Rebuild to maintain sorted order every 500 inserts
        if (count > 0 && count % 500 == 0) {
            rebuild();
        }
    }

    void rebuild() {
        int count = getRecordCount();
        vector<Record> records;
        records.reserve(count);

        // Load in chunks
        for (int i = 0; i < count; i += CHUNK_SIZE) {
            int chunkEnd = min(i + CHUNK_SIZE, count);
            for (int j = i; j < chunkEnd; j++) {
                Record rec;
                readRecord(j, rec);
                records.push_back(rec);
            }
        }

        // Sort
        sort(records.begin(), records.end());

        // Rewrite
        file.close();
        file.open(DATA_FILE, ios::out | ios::binary | ios::trunc);
        if (!records.empty()) {
            file.write((const char*)records.data(), records.size() * sizeof(Record));
        }
        file.close();
        file.open(DATA_FILE, ios::in | ios::out | ios::binary);
    }

    void remove(const string& key, int value) {
        Record targetRecord(key, value);
        int count = getRecordCount();
        vector<Record> records;

        // Load in chunks and filter
        for (int i = 0; i < count; i += CHUNK_SIZE) {
            int chunkEnd = min(i + CHUNK_SIZE, count);
            for (int j = i; j < chunkEnd; j++) {
                Record rec;
                readRecord(j, rec);
                if (!(rec == targetRecord)) {
                    records.push_back(rec);
                }
            }
        }

        // Rewrite
        file.close();
        file.open(DATA_FILE, ios::out | ios::binary | ios::trunc);
        if (!records.empty()) {
            file.write((const char*)records.data(), records.size() * sizeof(Record));
        }
        file.close();
        file.open(DATA_FILE, ios::in | ios::out | ios::binary);
    }

    void find(const string& key) {
        int count = getRecordCount();
        vector<int> values;

        // Search in chunks
        for (int i = 0; i < count; i += CHUNK_SIZE) {
            int chunkEnd = min(i + CHUNK_SIZE, count);
            for (int j = i; j < chunkEnd; j++) {
                Record rec;
                readRecord(j, rec);
                if (strcmp(rec.key, key.c_str()) == 0) {
                    values.push_back(rec.value);
                }
            }
        }

        if (values.empty()) {
            cout << "null" << endl;
        } else {
            sort(values.begin(), values.end());
            for (size_t i = 0; i < values.size(); i++) {
                if (i > 0) cout << " ";
                cout << values[i];
            }
            cout << endl;
        }
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;

    FileDatabase db;

    for (int i = 0; i < n; i++) {
        string command;
        cin >> command;

        if (command == "insert") {
            string key;
            int value;
            cin >> key >> value;
            db.insert(key, value);
        } else if (command == "delete") {
            string key;
            int value;
            cin >> key >> value;
            db.remove(key, value);
        } else if (command == "find") {
            string key;
            cin >> key;
            db.find(key);
        }
    }

    return 0;
}

