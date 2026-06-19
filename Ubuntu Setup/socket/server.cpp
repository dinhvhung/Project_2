#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib> // Thư viện để chạy lệnh system()
#include <cstdio>
using namespace std;

// --- HÀM MỚI 1: Đếm số dòng trong một file ---
int countLines(const string& filename) {
    ifstream file(filename);
    int count = 0;
    string line;
    while (getline(file, line)) {
        // Chỉ đếm nếu dòng không trống (tùy vào cấu trúc file .agents của bạn)
        if (!line.empty()) {
            count++;
        }
    }
    return count;
}

// --- HÀM MỚI 2: Tạo file JSON config ---
void createJsonConfig(const string& mapPath, const string& agentPath, const string& taskPath) {
    // 1. Đếm số lượng agent
    int teamSize = countLines(agentPath);

    // 2. Tạo nội dung JSON
    ofstream fJson("config.json");
    fJson << "{\n";
    fJson << "    \"mapFile\": \"" << mapPath << "\",\n";
    fJson << "    \"agentFile\": \"" << agentPath << "\",\n";
    fJson << "    \"teamSize\": " << teamSize << ",\n";
    fJson << "    \"taskFile\": \"" << taskPath << "\",\n";
    fJson << "    \"version\": \"2024 LoRR\"\n";
    fJson << "}";

    fJson.close();
    cout << "Da tao file config.json voi teamSize: " << teamSize << endl;
}

// --- HÀM MỚI: Xóa bỏ khoảng trắng và ký tự xuống dòng ở đầu/cuối chuỗi ---
string trim(const string& s) {
    size_t first = s.find_first_not_of(" \n\r\t");
    if (string::npos == first) return "";
    size_t last = s.find_last_not_of(" \n\r\t");
    return s.substr(first, (last - first + 1));
}

// Hàm hỗ trợ tách chuỗi theo dấu phân cách
vector<string> split(const string& s, const string& delimiter) {
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do {
        pos = s.find(delimiter, prev);
        if (pos == string::npos) pos = s.length();
        string token = s.substr(prev, pos - prev);
        tokens.push_back(token);
        prev = pos + delimiter.length();
    } while (pos < s.length() && prev < s.length());
    return tokens;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // 1. Tạo Socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Gắn Socket vào Port 8080
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Lắng nghe ở mọi IP (WSL/VM)
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    std::cout << "Server dang doi ket noi tai port 8080..." << std::endl;

    // 3. Chấp nhận kết nối
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    std::cout << "Da ket noi thanh cong!" << std::endl;

    // 4. Đọc tin nhắn
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int valread = recv(new_socket, buffer, 1024, 0);

        if (valread <= 0) {
                std::cout << "Client da ngat ket noi!" << std::endl;
                break;
        }

        std::cout << "Tin nhan tu Unity: " << buffer << std::endl;

        string receivedData(buffer, valread);
        vector<string> parts = split(receivedData, "---");

        if (parts.size() >= 3) {
            // Ghi file .map
            ofstream fMap("data.map");
            fMap << trim(parts[0]);
            fMap.close();

            // Ghi file .agents
            ofstream fAgents("data.agents");
            fAgents << trim(parts[1]);
            fAgents.close();

            // Ghi file .task
            ofstream fTask("data.task");
            fTask << trim(parts[2]);
            fTask.close();

            // --- GỌI HÀM TẠO CONFIG ---
            createJsonConfig("data.map", "data.agents", "data.task");


            cout << "Da cap nhat du lieu vao file map, agents, va task.\n" << endl;

            // Sửa thành lệnh có timeout 0.5 giây
            // Lệnh: timeout 0.5s [lệnh gốc]
            string command = "./../build/lifelong --inputFile config.json -o test.json";

            cout << "----> Dang khoi chay thuat toan LoRR (gioi han 0.5s)..." << endl;

            int returnCode = system(command.c_str());

            // Kiểm tra mã thoát (exit code)
            if (returnCode == 0) {
                cout << "----> Thuoc toan chay xong trong thoi gian cho phep." << endl;
            } else if (returnCode == 31744) { // 124 là exit code của lệnh timeout khi bị quá hạn
                cerr << "----> LOI: Thuat toan bi dung do qua thoi gian 0.5s (Timeout)!" << endl;
            } else {
                cerr << "----> LOI: Thuat toan gap su co khac. Ma loi: " << returnCode << endl;
            }
            // --- SAU KHI THUẬT TOÁN CHẠY XONG ---

            // 1. Mở và đọc toàn bộ nội dung file test.json
            ifstream inFile("test.json");
            stringstream buffer;
            buffer << inFile.rdbuf();
            string fileContent = buffer.str();
            inFile.close();

            // 2. Tìm vị trí của "actualPaths": [
            string key = "\"actualPaths\": [";
            size_t startPos = fileContent.find(key);

            if (startPos != string::npos) {
                // Dịch chuyển con trỏ qua phần key để bắt đầu từ dấu [
                startPos += key.length();

                // Tìm dấu ] kết thúc mảng actualPaths
                size_t endPos = fileContent.find("]", startPos);

                if (endPos != string::npos) {
                    // Cắt lấy chuỗi nằm giữa [ và ]
                    string resultData = fileContent.substr(startPos, endPos - startPos);

                    // Loại bỏ các khoảng trắng thừa, xuống dòng nếu có
                    resultData = trim(resultData);

                    // --- GỬI DỮ LIỆU VỀ UNITY ---
                    // Thêm dấu hiệu nhận biết (ví dụ: DATA_START|...|DATA_END) nếu cần
                    string messageToUnity = "PATHS:" + resultData;
                    send(new_socket, messageToUnity.c_str(), (int)messageToUnity.length(), 0);

                    cout << "Da gui actualPaths cho Unity: " << resultData << endl;
                } else {
                    cerr << "Khong tim thay ket thuc cua actualPaths" << endl;
                }
            } else {
                cerr << "Khong tim thay actualPaths trong file output." << endl;
                string message = "No Paths yet!";
                send(new_socket, message.c_str(), (int)message.length(),0);
            }
            string message = "Đã nhận Map!";
            send(new_socket, message.c_str(), (int)message.length(),0);
        }
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
