F1：简易文件拷贝（mycp）

功能：
模拟 Linux cp 命令，将源文件内容完整拷贝到目标文件。目标文件不存在则创建，存在则覆盖，拷贝完成后打印实际字节数。

编译运行：
gcc -o mycp mycp.c
./mycp src.txt dst.txt

技术点：

文件IO系统调用：open、read、write、close

源文件以 O_RDONLY 打开，目标文件以 O_WRONLY|O_CREAT|O_TRUNC 打开

循环读取，每次读一个缓冲区，直到 read 返回 0

write 可能写不全，用内层循环确保全部写入

错误处理：使用 errno 和 strerror 打印错误原因

参数检查：main 接收 argc/argv，校验参数个数

显式调用 umask(0022)，避免文件权限受外部环境影响

测试：
拷贝 1MB 文件，diff 对比无差异。
源文件不存在、目标路径无写权限、参数个数错误时，均打印明确错误并退出。