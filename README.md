# Midterm Project – Cài đặt lệnh `ls(1)`

**Sinh viên:** Bùi Hà My  
**Mã sinh viên:** 24IT165  
**Ngôn ngữ:** C (chuẩn C11)  
**Tài liệu tham chiếu:** trang manual NetBSD `ls(1)` được cung cấp trong đề bài.

## 1. Giới thiệu

`myls` là phiên bản đơn giản của lệnh UNIX `ls`. Chương trình liệt kê nội dung thư mục hoặc thông tin các đường dẫn được truyền vào, mỗi mục trên một dòng. Nếu không truyền đường dẫn, chương trình liệt kê thư mục hiện tại. Khi có nhiều đường dẫn, các file được in trước, sau đó đến các thư mục. Lỗi được in ra `stderr`; mã thoát bằng `0` khi thành công và khác `0` khi có lỗi.

## 2. Yêu cầu môi trường và cách chạy

Cần máy Linux có trình biên dịch C và `make` (ví dụ `gcc` và GNU Make). Sau khi tải hoặc clone toàn bộ repository, mở Terminal **tại thư mục chứa Makefile** rồi chạy:

```sh
make
./myls
```

Một số ví dụ:

```sh
./myls testdir             # Liệt kê testdir
./myls -la testdir         # Hiện cả file ẩn và thông tin chi tiết
./myls -R testdir          # Liệt kê đệ quy
./myls -S testdir          # Sắp xếp theo kích thước giảm dần
./myls -i -s -l testdir    # In inode, số block và định dạng dài
```

Cú pháp chung: `./myls [tùy_chọn] [đường_dẫn ...]`. Dùng `--` trước đường dẫn bắt đầu bằng dấu `-`, ví dụ `./myls -- -tenfile`.

Biên dịch lại từ đầu bằng `make clean` rồi `make`. Các file `*.o`, `*.d` và chương trình `myls` được tạo khi biên dịch, không cần đưa lên GitHub.

## 3. Chức năng đã cài đặt

| Tùy chọn | Chức năng |
| --- | --- |
| `-A` | Hiện file ẩn, trừ `.` và `..`. |
| `-a` | Hiện cả các mục bắt đầu bằng dấu chấm. |
| `-c`, `-u` | Dùng thời điểm thay đổi trạng thái hoặc truy cập cho `-t` và `-l`. |
| `-d` | In chính thư mục được chỉ định thay vì liệt kê bên trong. |
| `-F` | Thêm dấu nhận diện loại file: `/`, `*`, `@`, `=`, `|`. |
| `-f` | Giữ thứ tự đọc từ thư mục, không sắp xếp. |
| `-h` | Hiển thị kích thước dễ đọc khi dùng `-l` hoặc `-s`. |
| `-i` | In số inode. |
| `-k` | Dùng đơn vị 1024 byte cho `-s`. |
| `-l` | In định dạng dài: quyền, số liên kết, chủ sở hữu, nhóm, kích thước, thời gian và tên. |
| `-n` | Như `-l`, nhưng in UID/GID dưới dạng số. |
| `-q`, `-w` | Thay ký tự không in được bằng `?`, hoặc in nguyên dạng. |
| `-R` | Liệt kê đệ quy các thư mục con. |
| `-r` | Đảo thứ tự sắp xếp. |
| `-S` | Sắp xếp theo kích thước giảm dần. |
| `-s` | In số block đã cấp phát cho file. |
| `-t` | Sắp xếp theo thời gian, mới nhất trước. |

Tùy chọn xuất hiện sau sẽ ghi đè tùy chọn trước trong các cặp `-a`/`-A`, `-c`/`-u`, `-d`/`-R`, `-h`/`-k`, `-l`/`-n`, `-q`/`-w`. Khi chạy dưới tài khoản root, chế độ mặc định tương đương `-A`. Biến môi trường `BLOCKSIZE` điều chỉnh đơn vị của `-s` nếu không có `-h` hoặc `-k`; chương trình nhận số byte dương kèm hậu tố tùy chọn `B`, `K`, `M`, `G`, `T`, `P`, `E`. Biến `TZ` được thư viện C dùng để hiển thị thời gian.

## 4. Cấu trúc dự án

| File | Vai trò |
| --- | --- |
| `main.c` | Điểm bắt đầu chương trình. |
| `options.c`, `options.h` | Đọc và lưu các tùy chọn dòng lệnh. |
| `listing.c`, `listing.h` | Đọc thư mục, xử lý đường dẫn và duyệt đệ quy. |
| `sort.c`, `sort.h` | Sắp xếp danh sách mục. |
| `format.c`, `format.h` | Định dạng tên, quyền, thời gian, kích thước và block. |
| `tests/smoke.sh` | Các phép kiểm thử tích hợp với dữ liệu tạm. |
| `Makefile` | Biên dịch, kiểm thử và dọn file sinh ra. |
| `.gitignore` | Loại file biên dịch khỏi repository. |

## 5. Kiểm thử

Chạy:

```sh
make test
```

Kết quả mong đợi: `All smoke tests passed.` Bộ kiểm thử tạo thư mục tạm và kiểm tra file ẩn, sắp xếp, định dạng dài, liên kết tượng trưng, đệ quy, `BLOCKSIZE`, ký tự không in được và mã thoát khi đường dẫn không tồn tại. Có thể kiểm tra thủ công bằng các ví dụ ở mục 2.

## 6. Giới hạn

Chương trình được xây dựng và kiểm thử trên Linux. Các loại file riêng của NetBSD như whiteout và archive state không có trên hệ thống file Linux thông thường, nên không tạo được dấu nhận diện tương ứng trong môi trường này. Bộ kiểm thử tự động bao phủ các trường hợp chính, không chứng minh mọi tổ hợp tùy chọn đều đúng.

## 7. Repository và nộp bài

**GitHub repository:** https://github.com/Milcah-mybha/BuiHaMy_24IT165_midterm

Repository cần chứa `Makefile`, `README.md`, `.gitignore`, các file `.c`, `.h`, thư mục `tests/` và dữ liệu mẫu `testdir/`. Không commit `myls`, file `.o` hoặc `.d`. Sau khi đẩy mã nguồn lên GitHub, tải repository về một thư mục mới, chạy `make`, `./myls` và `make test` để kiểm tra đúng bản thầy sẽ nhận. Nộp báo cáo qua e-learning theo hướng dẫn của giảng viên.
