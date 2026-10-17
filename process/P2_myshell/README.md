P2：简易 shell（myshell）

功能：
模拟 Linux shell 基础功能。循环打印提示符，接收用户输入，支持带参数命令，使用 fork + execvp 执行。支持内置命令（cd、exit）与外部命令的区分执行。

编译运行：
gcc -o myshell myshell.c
./myshell

技术点：
- fork + execvp + waitpid：创建子进程执行外部命令，父进程阻塞等待，避免僵尸进程。
- 内置命令处理：cd 和 exit 不能通过 fork 执行，必须由 shell 进程调用 chdir() 或直接退出。
- 字符串解析：fgets 读取整行，strtok 按空格和换行切割参数，参数数组以 NULL 结尾。
- getenv：通过环境变量获取 HOME 目录，实现无参数 cd。
- 缓冲刷新：printf 提示符后需调用 fflush(stdout) 确保立即显示。

测试：
- 输入 ls -l、pwd、cat 等命令正常执行。
- 输入 cd 切换目录，cd /tmp 切换指定目录，无参数 cd 返回 HOME。
- 输入不存在的命令时打印 execvp 错误，shell 不退出。
- 输入空行时重新显示提示符。
- 输入 exit 正常退出。