# Midterm Project – `myls`

**Sinh viên:** Bùi Hà My (24IT165)

**Ngôn ngữ:** C (C11)

`myls` là chương trình dòng lệnh mô phỏng một phần lệnh UNIX `ls(1)` theo manual được cung cấp trong bài tập. Chương trình liệt kê file và thư mục, hỗ trợ hiển thị thông tin chi tiết, sắp xếp và duyệt đệ quy.

## Cách tải và chạy

Cần máy NetBSD 10.1 hoặc Linux có trình biên dịch C và `make`. Tải repository về, mở Terminal tại thư mục dự án rồi chạy:

```sh
git clone https://github.com/Milcah-mybha/BuiHaMy_24IT165_midterm.git
cd BuiHaMy_24IT165_midterm
make
./myls
```

Nếu đã tải dự án dưới dạng ZIP, giải nén rồi mở Terminal trong thư mục chứa `Makefile`; chỉ cần chạy `make` và `./myls`.

## Ví dụ sử dụng

```sh
./myls testdir          # Liệt kê nội dung testdir
./myls -a testdir       # Hiện cả file ẩn
./myls -l testdir       # Hiện thông tin chi tiết
./myls -la testdir      # Kết hợp -l và -a
./myls -R testdir       # Liệt kê cả thư mục con
./myls -S testdir       # Sắp xếp theo kích thước giảm dần
```

Cú pháp: `./myls [tùy_chọn] [đường_dẫn ...]`. Nếu không nhập đường dẫn, chương trình liệt kê thư mục hiện tại. Các tùy chọn được hỗ trợ: `-A -a -c -d -F -f -h -i -k -l -n -q -R -r -S -s -t -u -w`.

## Kiểm tra và dọn file biên dịch

```sh
make test     # Chạy bộ kiểm thử; thành công khi hiện "All smoke tests passed."
make clean    # Xóa chương trình và các file sinh ra khi biên dịch
```

Chương trình đã chạy bộ kiểm thử trên Linux và được kiểm tra cú pháp với header của NetBSD 10.1. Nếu dùng máy ảo NetBSD, hãy chạy `make clean`, `make`, `make test` trong máy ảo để kiểm tra trực tiếp. File thực thi `myls` và các file `.o` được tạo bởi `make`, không cần tải riêng.
