## 系统级I/O
### Unix I/O
Linux shell创建每个进程开始时都会打开三个文件：
1. 标准输入(描述符为0)
2. 标准输出(描述符为1)
3. 标准错误(描述符为2)
分别在头文件<unistd.h>中定义了常量STDIN_FILENO,STDOUT_FLIENO,STDERR_FILENO。对于每一个打开的文件，内核都会维护一个文件位置k，来表示文件开头起始的字节偏移量。

### 文件
> 每个Linux文件都有一个类型来表明

- 普通文件(regular file)：包含任意数据，应用程序常常区分文本文件(text file)和二进制文件(binary file)，文本文件只包含ASCII和Unicode字符，对于内核来说这两者没有区别
- 目录(directory)：一组包含链接的文件，每一个链接(link)都将一个文件名映射到一个文件，这个文件也可能是一个目录
- 套接字(socket)：用来与另一个进程进行跨网络通信的文件

### 打开和关闭文件
```cpp
// 打开文件
int open(char* filename, int flags, mode_t mode);
// 关闭文件
int close(int fd);
```
打开文件的一些flags：
- O_RDONLY：只读
- O_WRONLY：只写
- O_RDWR：读写
- O_CERAT：如果不存在，就创建一个截断的(truncated)空文件
- O_TRUNC：如果存在，就截断它
- O_APPEND：设置文件位置到文件结尾处

### 读和写文件
```cpp
// 读文件
ssize_t read(int fd, void *buf, size_t n);
// 写文件
ssize_t write(int fd, const void *buf, size_t n)
```
> 在x86-64系统中：size_t被定义为 unsigned long，而ssize_t被定义为long。当read和write传送的字节比应用程序要求的要少时，可能有以下原因
- 读遇到EOF：
- 从终端读文本行：
- 读写网络套接字：

### 用RIO包健壮地读写
### 读取文件元数据
### 读取目录内容
### 共享文件
### I/O重定向
### 标准I/O
### 综合
### 小结

## 网络编程

### 客户端-服务端编程模型
### 网络
### 全球IP因特网
### 套接字接口
### Web服务器
### 综合
### 小结

## 并发编程

### 基于进程的并发编程
### 基于I/O多路复用的并发
### 基于线程的并发编程
### 多线程程序中的共享变量
### 用信号量同步线程

### 使用线程提高并行性
### 其他并发问题
### 小结