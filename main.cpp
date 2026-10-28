#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;


class Device {
public:
    string type;
    string manufacturer;
    int year;
    string parameter1;
    int parameter2;

    void print() const {
        cout << type << " | " << manufacturer << " | " << year << " | ";
        if (type == "smartphone")
            cout << "OS: " << parameter1 << ", memory: " << parameter2 << " GB";
        else if (type == "laptop")
            cout << "CPU: " << parameter1 << ", RAM: " << parameter2 << " GB";
        else
            cout << "screen: " << parameter1 << " inches, battery: " << parameter2 << " mAh";
        cout << '\n';
    }
};

class DeviceStorage {
    vector<Device> devices;

public:
    void add(const Device& device) {
        devices.push_back(device);
    }

    void remove(const string& field, const string& op, const string& value) {
        if (op != "=" && op != ">" && op != "<") {
            cout << "Invalid operator\n";
            return;
        }
        if (field != "year" && ((field != "type" && field != "manufacturer") || op != "=")) {
            cout << "Invalid removal condition\n";
            return;
        }
        int yearValue = 0;
        if (field == "year") {
            istringstream input(value);
            if (!(input >> yearValue) || !(input >> ws).eof()) {
                cout << "Invalid year\n";
                return;
            }
        }
        auto matches = [&](const Device& device) {
            if (field == "type") return device.type == value;
            if (field == "manufacturer") return device.manufacturer == value;
            if (op == "=") return device.year == yearValue;
            if (op == "<") return device.year < yearValue;
            return device.year > yearValue;
        };
        size_t before = devices.size();
        devices.erase(remove_if(devices.begin(), devices.end(), matches), devices.end());
        cout << "Removed: " << before - devices.size() << '\n';
    }

    void print() const {
        if (devices.empty()) {
            cout << "Storage is empty\n";
            return;
        }
        for (const Device& device : devices) device.print();
    }
};

int main(int argc, char* argv[]) {
    string filename = argc > 1 ? argv[1] : "commands.txt";
    ifstream file(filename);
    if (!file) {
        cout << "Cannot open file: " << filename << '\n';
        return 1;
    }

    DeviceStorage storage;
    string line;
    while (getline(file, line)) {
        istringstream input(line);
        string command;
        if (!(input >> command) || command[0] == '#') continue;

        if (command == "ADD") {
            Device device;
            if (!(input >> device.type >> device.manufacturer >> device.year
                        >> device.parameter1 >> device.parameter2) ||
                (device.type != "smartphone" && device.type != "laptop" && device.type != "tablet") ||
                device.year < 1900 || device.parameter2 <= 0) {
                cout << "Invalid ADD: " << line << '\n';
                continue;
            }
            storage.add(device);
        } else if (command == "REM") {
            string field, op, value;
            if (input >> field >> op >> value)
                storage.remove(field, op, value);
            else
                cout << "Invalid REM: " << line << '\n';
        } else if (command == "PRINT") {
            cout << "--- Devices ---\n";
            storage.print();
        } else {
            cout << "Unknown command: " << command << '\n';
        }
    }
    return 0;
}
