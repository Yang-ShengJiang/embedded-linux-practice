F3：文件单词计数（mywc）

功能：
模拟 Linux wc 命令，统计文件的行数、单词数、字符数。行数为换行符个数，单词为连续非空白字符序列，字符数为文件总字节数。

编译运行：
gcc -o mywc mywc.c
./mywc test.txt

技术点：

文件IO：open、read、close

循环读取文件，每次读入一个缓冲区

状态机：用 in_word 标记当前是否在单词内部

缓冲区边界处理：单词可能跨两次 read，状态变量在循环外维护

空白字符统一判断：空格、换行、制表符

文件末尾无换行时，循环结束后补计最后一个单词

测试：

输入	            行数	单词数	字符数
空文件	            0	    0	    0
hello	            0	    1	    5
hello\n	            1	    1	    6
hello world	        0	    2	    11
hello world	        0	    2	    13
hello\nworld\n	    2	    2	    12
\n\n\n	            3	    0	    3
三个空格	         0	     0	     3
与系统 wc 结果一致。