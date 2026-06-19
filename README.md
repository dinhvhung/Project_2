# Cách setup hệ thống trên máy tính
## 1. Setup Game
- Nếu bạn chưa có Unity Hub, tải tại [đây](https://docs.unity.com/en-us/hub)
- Tại Unity Hub, tại mục `Install`, ấn `Install Editor` để cài đặt Editor cho Unity, chọn phiên bản `2022.3.62f3`
- Sau khi cài Editor xong, tạo Project mới, chọn `Universal 2D`
- Sau khi đã tạo xong, xóa `Assets` và `ProjectSettings` đi và thay bằng `Assets` và `ProjectSettings` của game BattleCity

## 2. Setup máy ảo Linux Ubuntu
Mở `Command Prompt` quyền admin, gõ
```shell
wsl --install -d Ubuntu-24.04
```

Cài đặt các thư viện cần thiết bằng cách chạy lần lượt các câu lệnh sau
```shell
sudo apt-get update
sudo apt-get install cmake g++ libboost-all-dev python3-dev python3-pip
pip install pybind11-global numpy
sudo apt-get install build-essential python3-pybind11
```
