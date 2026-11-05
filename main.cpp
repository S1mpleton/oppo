#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

struct Device {
    string type;
    string manufacturer;
    int year;
    string parameter1;
    int parameter2;
};

// Each function returns a new container. The original container is unchanged.
vector<Device> addDevice(const vector<Device>& devices, const Device& device) {
    vector<Device> result = devices;
    result.push_back(device);
    return result;
}

bool matches(const Device& d, const string& field, const string& op,
             const string& value) {
    if (field == "type") return d.type == value;
    if (field == "manufacturer") return d.manufacturer == value;
    int year = stoi(value);
    if (op == "=") return d.year == year;
    if (op == "<") return d.year < year;
    return d.year > year;
}

vector<Device> removeDevices(const vector<Device>& devices,
                             const string& field, const string& op,
                             const string& value) {
    vector<Device> result;
    copy_if(devices.begin(), devices.end(), back_inserter(result),
            [&](const Device& d) { return !matches(d, field, op, value); });
    return result;
}

string deviceToString(const Device& d) {
    string result = d.type + " | " + d.manufacturer + " | " +
                    to_string(d.year) + " | ";
    if (d.type == "smartphone")
        return result + "OS: " + d.parameter1 + ", memory: " + to_string(d.parameter2) + " GB";
    if (d.type == "laptop")
        return result + "CPU: " + d.parameter1 + ", RAM: " + to_string(d.parameter2) + " GB";
    return result + "screen: " + d.parameter1 + " inches, battery: " +
           to_string(d.parameter2) + " mAh";
}

bool validCondition(const string& field, const string& op, const string& value) {
    if (field == "type" || field == "manufacturer")
        return op == "=";
    if (field != "year" || (op != "=" && op != "<" && op != ">"))
        return false;
    try {
        size_t used = 0;
        stoi(value, &used);
        return used == value.size();
    } catch (...) {
        return false;
    }
}

bool validDevice(const Device& d) {
    return (d.type == "smartphone" || d.type == "laptop" || d.type == "tablet") &&
           d.year >= 1900 && d.parameter2 > 0;
}

int main(int argc, char* argv[]) {
    const string filename = argc > 1 ? argv[1] : "commands.txt";
    ifstream file(filename);
    if (!file) {
        cout << "Cannot open file: " << filename << '\n';
        return 1;
    }

    vector<Device> devices;
    string line;
    while (getline(file, line)) {
        istringstream input(line);
        string command;
        if (!(input >> command) || command[0] == '#') continue;

        if (command == "ADD") {
            Device d;
            string extra;
            if (!(input >> d.type >> d.manufacturer >> d.year >> d.parameter1 >> d.parameter2) ||
                (input >> extra) || !validDevice(d)) {
                cout << "Invalid ADD: " << line << '\n';
                continue;
            }
            devices = addDevice(devices, d);
        } else if (command == "REM") {
            string field, op, value, extra;
            if (!(input >> field >> op >> value) || (input >> extra) ||
                !validCondition(field, op, value)) {
                cout << "Invalid REM: " << line << '\n';
                continue;
            }
            const size_t before = devices.size();
            devices = removeDevices(devices, field, op, value);
            cout << "Removed: " << before - devices.size() << '\n';
        } else if (command == "PRINT") {
            cout << "--- Devices ---\n";
            if (devices.empty()) cout << "Storage is empty\n";
            else for_each(devices.begin(), devices.end(), [](const Device& d) {
                cout << deviceToString(d) << '\n';
            });
        } else {
            cout << "Unknown command: " << command << '\n';
        }
    }
    return 0;
}
