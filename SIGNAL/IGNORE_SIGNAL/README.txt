Chương trình có 2 vòng lặp for. Trước vòng lặp đầu tiên, Signal đã bị ignore nên tín hiệu SIGINT gửi đến không thể dừng được chương trình.
Trước khi vòng lặp 2 được khởi động, Signal được đặt về trạng thái mặc định, khi bấm Ctrl+C sẽ ngắt chương trình.
