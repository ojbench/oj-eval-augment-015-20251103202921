#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <map>

using namespace std;

const string DATA_FILE = "database.dat";
const int MAX_KEY_LEN = 65;

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
    
    void loadAllRecords(vector<Record>& records) {
        file.seekg(0, ios::end);
        long fileSize = file.tellg();
        if (fileSize == 0) return;
        
        file.seekg(0, ios::beg);
        int count = fileSize / sizeof(Record);
        records.resize(count);
        file.read((char*)records.data(), fileSize);
    }
    
    void saveAllRecords(const vector<Record>& records) {
        file.close();
        file.open(DATA_FILE, ios::out | ios::binary | ios::trunc);
        if (!records.empty()) {
            file.write((const char*)records.data(), records.size() * sizeof(Record));
        }
        file.close();
        file.open(DATA_FILE, ios::in | ios::out | ios::binary);
    }
    
public:
    FileDatabase() {
        // Try to open existing file
        file.open(DATA_FILE, ios::in | ios::out | ios::binary);
        if (!file.is_open()) {
            // Create new file if it doesn't exist
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
        vector<Record> records;
        loadAllRecords(records);

        Record newRecord(key, value);

        // Check if record already exists
        auto it = std::find(records.begin(), records.end(), newRecord);
        if (it != records.end()) {
            return; // Duplicate, don't insert
        }

        records.push_back(newRecord);
        sort(records.begin(), records.end());

        saveAllRecords(records);
    }

    void remove(const string& key, int value) {
        vector<Record> records;
        loadAllRecords(records);

        Record targetRecord(key, value);

        auto it = std::find(records.begin(), records.end(), targetRecord);
        if (it != records.end()) {
            records.erase(it);
            saveAllRecords(records);
        }
    }
    
    void find(const string& key) {
        vector<Record> records;
        loadAllRecords(records);
        
        vector<int> values;
        for (const auto& record : records) {
            if (strcmp(record.key, key.c_str()) == 0) {
                values.push_back(record.value);
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

