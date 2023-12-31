
### 1.你能详细描述一下你使用 Epoll 和线程池实现的 Reactor 高并发模型吗？你在设计和实现中遇到了哪些挑战？
> Reactor是一种**事件驱动**的**设计模式**，通用于处理并发的输入/输出

Reactor主要分为三个部分：
- **事件源**（Event Sources）：一组I/O操作
- **事件处理器**（Event Handlers）：通常是一组回调函数or方法，用来处理特定类型事件
- **Reactor对象**：负责监视所有事件

- 从0开始理解Reactor模型是一个不小的挑战对于我来说

### 2.你的 WEB 服务器支持哪些 HTTP 方法？例如，GET、POST、PUT、DELETE 等。

- POST
- GET

### 3.你是如何使用正则和状态机来解析 HTTP 请求的？能否介绍一下你的实现方法和解析流程？

1. 有REQUEST_LINE、HEADERS、BODY三个主状态
2. 依次解析这三个状态，使用正则匹配解析请求行的请求方法、请求地址、HTTP协议版本
3. 使用正则表达式解析请求头，解析一条刷新一下状态
4. 解析BODY状态

### 4.在处理静态资源请求时，你的服务器如何进行文件管理？例如，文件如何存储和检索？

1. 使用static关键字来管理静态资源路径
2. 访问这个路径下的静态资源


### 5.你的日志系统如何工作？你是如何实现单例模式和阻塞队列的？这样做有什么好处？

- **单例模式**：只有一个 `Log` 类的实例
- **阻塞队列**：用于消费者和生产者之前传递数据
- **异步写入**：日志信息首先写入一个阻塞队列，然后由一个专门的线程将阻塞队列中的日志信息写入文件



### 6.你的日志系统记录哪些类型的服务器状态？如何使用这些信息来优化服务器性能？

- **日志级别**：有4种日志类型：ERROR、WARN、INFO、DEBUG
- 

### 7.你的服务器如何处理超时非活动连接？你是如何使用小根堆实现定时器的？这样做有什么优点？

- 有一个线程一直在tick 最近的一个时间是否超时，如果超时就会调用超时回调函数来断开连接
- 小根堆用来O（logn）的查找出最近超时的连接

### 8.你的服务器支持 HTTPS 安全连接吗？如果支持，你是如何实现的？
- 不支持

### 9.你的服务器在高并发场景下的性能如何？你进行过哪些性能测试？
- 支持QPS 10000+
- 使用webbench测试

### 10.你在这个项目中使用了哪些 C++ 的高级特性？如何利用这些特性提高代码的效率和可读性？

- 智能指针
- atomic
- functional



