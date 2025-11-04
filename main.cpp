#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <map>
#include <set>

using namespace std;

const string DATA_FILE = "database.dat";

class FileDatabase {
private:
    map<string, set<int>> data;

    void loadFromFile() {
        ifstream file(DATA_FILE, ios::binary);
        if (!file.is_open()) return;

        data.clear();
        int numKeys;
        if (!(file.read((char*)&numKeys, sizeof(int)))) return;

        for (int i = 0; i < numKeys; i++) {
            int keyLen;
            file.read((char*)&keyLen, sizeof(int));

            string key(keyLen, '\0');
            file.read(&key[0], keyLen);

            int numValues;
            file.read((char*)&numValues, sizeof(int));

            set<int> values;
            for (int j = 0; j < numValues; j++) {
                int value;
                file.read((char*)&value, sizeof(int));
                values.insert(value);
            }

            data[key] = values;
        }
        file.close();
    }

    void saveToFile() {
        ofstream file(DATA_FILE, ios::binary | ios::trunc);

        int numKeys = data.size();
        file.write((const char*)&numKeys, sizeof(int));

        for (const auto& pair : data) {
            int keyLen = pair.first.length();
            file.write((const char*)&keyLen, sizeof(int));
            file.write(pair.first.c_str(), keyLen);

            int numValues = pair.second.size();
            file.write((const char*)&numValues, sizeof(int));

            for (int value : pair.second) {
                file.write((const char*)&value, sizeof(int));
            }
        }

        file.close();
    }

public:
    FileDatabase() {
        loadFromFile();
    }

    ~FileDatabase() {
        saveToFile();
    }

    void insert(const string& key, int value) {
        data[key].insert(value);
    }

    void remove(const string& key, int value) {
        if (data.count(key)) {
            data[key].erase(value);
            if (data[key].empty()) {
                data.erase(key);
            }
        }
    }

    void find(const string& key) {
        if (data.count(key) == 0 || data[key].empty()) {
            cout << "null" << endl;
        } else {
            bool first = true;
            for (int value : data[key]) {
                if (!first) cout << " ";
                cout << value;
                first = false;
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

