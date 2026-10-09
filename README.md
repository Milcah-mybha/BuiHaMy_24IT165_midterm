# Midterm Project – `myls`

**Sinh viên:** Bùi Hà My (24IT165)

**Ngôn ngữ:** C (C11)

**Môi trường chạy:** NetBSD 10.1

`myls` là chương trình dòng lệnh mô phỏng một phần lệnh UNIX `ls(1)` theo manual của bài tập. Chương trình liệt kê file và thư mục, hỗ trợ xem thông tin chi tiết, sắp xếp và duyệt đệ quy.

## Tải và chạy trên NetBSD

Máy cần có `cc`, `make` và kết nối Internet. **Không cần cài Git**: mở Terminal trong NetBSD và chạy lần lượt:

```sh
ftp -o midterm.tar.gz https://github.com/Milcah-mybha/BuiHaMy_24IT165_midterm/archive/refs/heads/main.tar.gz
tar -xzf midterm.tar.gz
cd BuiHaMy_24IT165_midterm-main
make
./myls -la testdir
```

Sau `make`, file thực thi `myls` được tạo ngay trong thư mục dự án. Lệnh cuối liệt kê các file mẫu trong `testdir`, gồm cả file ẩn và thông tin chi tiết. Chạy `./myls` không kèm đường dẫn để liệt kê thư mục hiện tại.

Nếu máy đã có Git, có thể thay ba lệnh tải và giải nén ở trên bằng:

```sh
git clone https://github.com/Milcah-mybha/BuiHaMy_24IT165_midterm.git
cd BuiHaMy_24IT165_midterm
```

## Ví dụ sử dụng

```sh
./myls testdir        # Liệt kê nội dung testdir
./myls -a testdir     # Hiện cả file ẩn
./myls -l testdir     # Hiện thông tin chi tiết
./myls -R testdir     # Liệt kê cả thư mục con
./myls -S testdir     # Sắp xếp theo kích thước giảm dần
```

Cú pháp: `./myls [tùy_chọn] [đường_dẫn ...]`. Các tùy chọn được hỗ trợ: `-A -a -c -d -F -f -h -i -k -l -n -q -R -r -S -s -t -u -w`.

## Kiểm thử và dọn file biên dịch

Trong thư mục dự án, chạy:

```sh
make test
```

Kết quả thành công là `All smoke tests passed.` Chương trình đã được biên dịch và kiểm thử trên NetBSD 10.1. Dùng `make clean` để xóa file thực thi và các file `.o`; sau đó chạy `make` nếu muốn biên dịch lại.
