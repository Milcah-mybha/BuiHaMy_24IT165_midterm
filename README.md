# Midterm Project: Cài đặt lệnh `ls(1)`

**Sinh viên:** Bùi Hà My · **Mã sinh viên:** 24IT165

**Ngôn ngữ:** C (chuẩn C11)

**Repository:** [BuiHaMy_24IT165_midterm](https://github.com/Milcah-mybha/BuiHaMy_24IT165_midterm)

## Giới thiệu

`myls` là chương trình dòng lệnh mô phỏng một phần lệnh UNIX `ls`, dựa trên trang manual NetBSD `ls(1)` được cung cấp trong đề bài. Chương trình có thể liệt kê file, thư mục và thông tin của chúng. Nếu không nhập đường dẫn, `myls` liệt kê thư mục hiện tại.

## Cài đặt và chạy

**Yêu cầu:** Linux, trình biên dịch C và `make`. Sau khi tải repository về, mở Terminal trong thư mục chứa `Makefile` rồi chạy:

```sh
make
./myls
```

Ví dụ khác:

```sh
./myls testdir          # Liệt kê các file trong testdir
./myls -la testdir      # Hiện cả file ẩn và thông tin chi tiết
./myls -R testdir       # Liệt kê cả thư mục con
./myls -S testdir       # Sắp xếp theo kích thước giảm dần
```

Cú pháp chung là `./myls [tùy_chọn] [đường_dẫn ...]`. Khi có nhiều đường dẫn, chương trình in các file trước, rồi đến nội dung các thư mục. Có thể dùng `--` trước tên file bắt đầu bằng dấu `-`, ví dụ `./myls -- -tenfile`.

Chạy `make clean` để xóa chương trình và các file sinh ra khi biên dịch; sau đó chạy `make` để biên dịch lại.

## Các tùy chọn đã cài đặt

| Tùy chọn | Chức năng |
| --- | --- |
| `-a`, `-A` | Hiện file ẩn; `-A` không hiện `.` và `..`. |
| `-c`, `-u` | Dùng thời điểm thay đổi trạng thái hoặc truy cập khi sắp xếp theo thời gian và khi in dạng dài. |
| `-d` | In thông tin của chính thư mục thay vì liệt kê bên trong. |
| `-F` | Thêm ký hiệu cho thư mục `/`, file thực thi `*`, liên kết tượng trưng `@`, socket `=` và FIFO `|`. |
| `-f` | Không sắp xếp; giữ thứ tự đọc từ thư mục. |
| `-h`, `-k` | Hiển thị kích thước dễ đọc hoặc số block theo đơn vị 1024 byte. |
| `-i` | In số inode. |
| `-l`, `-n` | In thông tin chi tiết; `-n` dùng UID và GID dạng số. |
| `-q`, `-w` | Thay ký tự không in được bằng `?` hoặc in nguyên dạng. |
| `-R` | Liệt kê đệ quy các thư mục con. |
| `-r` | Đảo thứ tự sắp xếp. |
| `-S` | Sắp xếp theo kích thước giảm dần. |
| `-s` | In số block đã cấp phát cho file. |
| `-t` | Sắp xếp theo thời gian, mới nhất trước. |

Nếu dùng cả hai tùy chọn trong một cặp như `-l`/`-n` hoặc `-d`/`-R`, tùy chọn xuất hiện **sau** có hiệu lực. `BLOCKSIZE` có thể đổi đơn vị cho `-s` (ví dụ `BLOCKSIZE=1K ./myls -s testdir`); `-h` và `-k` ghi đè giá trị này. `TZ` ảnh hưởng đến thời gian hiển thị thông qua thư viện C.

## Cấu trúc mã nguồn

| File | Nhiệm vụ |
| --- | --- |
| `main.c` | Khởi động chương trình. |
| `options.c`, `options.h` | Đọc các tùy chọn dòng lệnh. |
| `listing.c`, `listing.h` | Đọc thư mục, xử lý đường dẫn và duyệt đệ quy. |
| `sort.c`, `sort.h` | Sắp xếp các mục cần in. |
| `format.c`, `format.h` | Định dạng tên file và các thông tin chi tiết. |
| `tests/smoke.sh` | Kiểm thử các chức năng chính. |
| `Makefile` | Biên dịch, kiểm thử và dọn file biên dịch. |

## Kiểm thử

Chạy:

```sh
make test
```

Nếu thành công, Terminal in `All smoke tests passed.` Bộ kiểm thử tạo dữ liệu tạm để kiểm tra file ẩn, sắp xếp, định dạng dài, liên kết tượng trưng, đệ quy, đơn vị block, ký tự không in được và cách xử lý đường dẫn không tồn tại. Khi gặp lỗi, chương trình in thông báo ra `stderr` và trả mã thoát khác `0`.

## Giới hạn và nộp bài

Chương trình được xây dựng và kiểm thử trên Linux. Các loại file riêng của NetBSD như *whiteout* và *archive state* không có trên hệ thống file Linux thông thường, nên chưa thể kiểm thử dấu nhận diện tương ứng. Bộ kiểm thử bao phủ các trường hợp chính, không bao phủ mọi tổ hợp tùy chọn.

Repository chỉ chứa mã nguồn, `Makefile`, `README.md`, `.gitignore`, thư mục `tests/` và dữ liệu mẫu `testdir/`. File thực thi `myls`, file `.o` và `.d` được loại khỏi Git bằng `.gitignore`. Để kiểm tra bản nộp, tải repository về một thư mục mới rồi chạy `make`, `./myls` và `make test`.
