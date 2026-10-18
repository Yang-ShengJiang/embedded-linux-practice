I3：POSIX 消息队列

功能：
父子进程通过 POSIX 消息队列（mqueue）进行带优先级的数据通信。子进程先发送低优先级消息，后发送高优先级消息；父进程接收时，验证内核是否自动按优先级从高到低出队。

技术点：
- mq_open：创建/打开消息队列，传入 struct mq_attr 指针设置队列容量和单条消息大小。
- mq_send：发送消息，最后一个参数为优先级（数字越大优先级越高）。
- mq_receive：接收消息，内核自动将当前队列中优先级最高的消息排在前面。
- 清理机制：mq_close 关闭描述符，mq_unlink 从系统中彻底删除队列。
- 编译链接：需链接实时库 gcc -lrt。

编译运行：
gcc -o msgqueue msgqueue.c -lrt
./msgqueue