# cpp小项目



## 题目要求

请实现一个单写者多读者的循环队列。每个写者和读者都用一个进程实现，进程间通信请使用共享内存的方式。请注意读写数据时的同步互斥问题。（代码中要有加锁解锁的体现）
示例：写者写入: 8, 9, 4, 1, 2, 6 所有读者: 均能读出8, 9, 4, 1, 2, 6。

## 测试环境 
Linux VM-12-10-centos 4.18.0-348.7.1.el8_5.x86_64 #1 SMP Wed Dec 22 13:25:12 UTC 2021 x86_64 x86_64 x86_64 GNU/Linux


## 编译命令

 g++ -std=c++11 -pthread process.cpp -lrt -o process.exe 
 
 
## 测试结果

Writer: wrote 8 to queue
Reader 1: read 8 from queue
Writer: wrote 9 to queue
Reader 3: read 9 from queue
Writer: wrote 4 to queue
Reader 2: read 4 from queue
Writer: wrote 1 to queue
Reader 1: read 1 from queue
Writer: wrote 2 to queue
Reader 1: read 2 from queue
Writer: wrote 6 to queue
Reader 3: read 6 from queue


