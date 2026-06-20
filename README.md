# Cách setup hệ thống trên máy tính
## 1. Setup Game
- Nếu bạn chưa có Unity Hub, tải tại [đây](https://docs.unity.com/en-us/hub)
- Tại Unity Hub, tại mục `Install`, ấn `Install Editor` để cài đặt Editor cho Unity, chọn phiên bản `2022.3.62f3`
- Sau khi cài Editor xong, tạo Project mới, chọn `Universal 2D`
- Sau khi đã tạo xong, xóa `Assets` và `ProjectSettings` đi và thay bằng `Assets` và `ProjectSettings` của game BattleCity

## 2. Setup máy ảo Linux Ubuntu
### Cài Ubuntu
Mở `Command Prompt` quyền admin, gõ
```shell
wsl --install -d Ubuntu-24.04
```

### Cài đặt các thư viện cần thiết bằng cách chạy lần lượt các câu lệnh sau
```shell
sudo apt-get update
sudo apt-get install cmake g++ libboost-all-dev python3-dev python3-pip
pip install pybind11-global numpy
sudo apt-get install build-essential python3-pybind11
```

### Kéo code Ubuntu về
```shell
git clone --filter=blob:none --no-checkout https://github.com/dinhvhung/Project_2.git
cd myrepo

git sparse-checkout init --cone
git sparse-checkout set 'Ubuntu Setup'

git checkout main

cd Project_2/'Ubuntu Setup'
```

### Tạo file build để chạy code
Chạy lần lượt các lệnh sau
```
mkdir build
cmake -B build ./ -DCMAKE_BUILD_TYPE=Release
make -C build -j1
```
(Bạn có thể thay `j1` = `j2`, `j3`, `j4` nếu RAM đủ mạnh để tiết kiệm thời gian tải)

#### Lệnh mẫu để chạy thử
```
./build/lifelong --inputFile ./example_problems/random.domain/random_32_32_20_100.json -o test.json
```
Nếu hiện ra như sau tức là đã chạy được thuật toán
```
[2026-06-20 11:06:08.229345] [0x00007476232a9740] [info]    [timestep=0] Preprocessing success
[2026-06-20 11:06:08.229421] [0x00007476232a9740] [info]    Task 1 is revealed
[2026-06-20 11:06:08.229431] [0x00007476232a9740] [info]    Task 2 is revealed
[2026-06-20 11:06:08.229438] [0x00007476232a9740] [info]    Task 3 is revealed
[2026-06-20 11:06:08.229445] [0x00007476232a9740] [info]    Task 4 is revealed
[2026-06-20 11:06:08.229451] [0x00007476232a9740] [info]    Task 5 is revealed
...
```
### Chạy Server
- Tại Ubuntu, chạy lần lượt
```
cd socket
g++ server.cpp -o server
./server
```
- Lúc này, server sẽ đợi kết nối từ Unity. Tại Unity, hãy kiểm tra chắc chắn rằng script `SocketMap.cs` đã được gắn vào 1 `GameObject` bất kỳ, ấn nút khởi động Game để Unity bắt đầu gửi thông tin đến Ubuntu
