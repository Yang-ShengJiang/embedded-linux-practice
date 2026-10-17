F4：配置文件解析器（myconfig）

功能：
读取 config.ini 配置文件，解析 key=value 格式，存入哈希表。支持打印所有配置项和按 key 查询。

编译运行：
gcc -o myconfig myconfig.c
./myconfig config.ini

config.ini 示例：
# 服务器配置
server_port = 8080
log_path=/var/log/gateway.log

# 连接配置
max_connections=1000
buffer_size = 4096

技术点：
- 文件IO：fopen、fgets、fclose 逐行读取
- 字符串处理：trim 去除首尾空白（空格、制表符、换行、回车），strchr 查找等号，strdup 复制字符串到堆
- 哈希表改造：value 从 int 改为 char*，增加 count 字段，新增 ht_count 和 ht_print
- 等号分割：找到 '=' 后替换为 '\0'，左边为 key，右边为 value
- 错误处理：文件打开失败用 perror 打印

测试：
config.ini 包含 4 个配置项（含注释行和空行）。
运行后输出：
共打印 4 个配置项
  buffer_size = 4096
  max_connections = 1000
  log_path = /var/log/gateway.log
  server_port = 8080
查询 server_port: 8080
查询 old: (不存在)

说明：哈希表头插法导致打印顺序与文件顺序不同，不影响查询结果。