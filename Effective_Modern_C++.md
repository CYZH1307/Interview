## 型别推导

### 1.理解模板型别推导

函数模板：

```cpp
template<typename T>
void f(ParamType param);
```

在编译期，编译器会推导`T`和`ParamType`的类型，因为`ParamType`通常会有修饰词，比如：

```cpp
template<typename T>
void f(const T& param); //ParamType = const T&

//如果按照以下的调用
int x = 0;
f(x); //T 被推导为 int，ParamType 被推导为const int&
```

`ParamType` 有三种形式：

- `ParamType` 具有指针或者引用类型
- `ParamType` 是一个万能引用
- `ParamType` 既不是引用也不是指针

#### `ParamType` 具有指针或者引用类型

性别推导会：

1. 如果`expr`具有引用类型，先将引用部分忽略
2. 对`expr`的型别和`ParamType` 的型别执行模式匹配，决定T的型别

比如：

```cpp
//引用
template<typename T>
void f(T& param); 

int x = 0;
const int cx = 0;
const int& rx = 0;
f(x); // T是int，param是int&
f(cx); // T是const int, param是const int&
f(rx); // T是const int，param是const int&

//指针
template<typename T>
void f(T* param); 

int x = 0;
const int *px = &x;
f(x); // T是int，param是int*
f(px); // T是const int, param是const int*
```

#### `ParamType` 是一个万能引用

性别推导会：

1. 如果参数是一个左值，`T` 和 `ParamType`都会被推导为左值引用，
2. 如果参数是一个右值，应用常规规则

```cpp
template<typename T>
void f(T&& param); 

int x = 0;
const int cx = 0;
const int& rx = 0;
f(x); // T是int&，param是int&
f(cx); // T是const int&, param是const int&
f(rx); // T是const int&，param是const int&
f(0); // T是int，param是 int&&
```

#### `ParamType` 既不是引用也不是指针

性别推导会：

1. 如果参数用引用类型，忽略引用部分
2. 如果参数是`const`，也忽略，是`volatile`，也忽略

```cpp
template<typename T>
void f(T param); 

int x = 0;
const int cx = 0;
const int& rx = 0;
f(x); // T是int，param是int
f(cx); // T是int，param是int
f(rx); // T是int，param是int
```

#### 数组实参

```cpp
const char name[] = "123"; //const char[4]
const char *ptrToName = name;  // cosnt char*

template<typename T>
void f(T param) ;
template<typename T>
void f2(T& param);
f(name); // T被推导为const char*
f2(name); // T被推导为const char[4], param被推导为 const char (&)[4]

```

#### 函数实参

```cpp
void func(int, double); // void(int, double)

template<typename T>
void f1(T param) ;
template<typename T>
void f2(T& param);

f1(func); // param 被推导为 void(*)(int, double)
f1(func); // param 被推导为 void(&)(int, double)
```

### 2.理解`auto`型别推导

- 在一般形式中`auto`和`ParamType`的推导一样
- 当`auto`遇到大括号时，`auto`会假定大括号内部是一个`std::initializer_list`

```cpp
auto a = 20; 	// int
auto b (20);	//int
auto c = {20}; 	// std::initializer_list<int>
auto d {20};  	// std::initializer_list<int>
```

- 如果大括号里面的值型别不一会导致推导失败，比如`auto e = {1, 2, 3.0}`，推导不出来`T`

- auto可以作为函数返回值使用，但是无法推导大括号表达式

  ```cpp
  auto f(int x) {return x + 1;} 	//可以
  auto f(int x) {return {x, x + 1};} 	//不可以
  ```

### 3.理解`decltype`

- `decltype`返回给定名字或表达式的确切类型

```cpp
const int i = 0; 	//decltype(i): const int
vector<int> v; 		//decltype(v): vector<int>
					//decltype(v[0]): int&
```

- `decltype(auto)`

```cpp
template<typename Container, typename Index>
auto f(Container& c, Index i) { 
    // auto 型别推导会忽略其引用，这里推导出的是 int
    return c[i];
}

template<typename Container, typename Index>
decltype(auto) f2(Container& c, Index i) {
    // decltype(auto) auto采用型别推导，使用decltype的规则，不会忽略引用
    return c[i];
}
```

- 仅有名字`decltype`推导出声明型别，如果是一个左值表达式，推导出左值引用

```cpp
int x = 0; // decltype((x)): int&
```


- 如果联合起来的话：

```cpp
decltype(auto) f() {
    int x = 0;
    return (x); //返回局部引用，未定义行为
}
```

### 4.掌握查看型别推导结果的方法

- 定义一个模板类TD，诱导发出错误信息

```cpp
template<typename T>
class TD;

TD<decltype(x)> xType; //ide显示TD<int> xType
TD<decltype(y)> yType; //ide显示TD<const int *> yType
```

- 使用`typeid`和`std::type_info::name`

```cpp
std::cout<< typeid(x).name() << '\n'; // i
std::cout<< typeid(y).name() << '\n'; // PKi
```

- 使用`Boost`的`TypeIndex`库


## auto
### 5.优先选用`auto`，而非显式型别声明
- 在C++14中Lambda表达式的形参都可以用auto
- 在32位Windows上`std::vector<int>::size_type`是32位的，而在64位Windows上`std::vector<int>::size_type`是64位的，可以使用auto来定义，方便
- `std::unordered_map<string, int>`的健值类型是`std::pair<const std::string, int>`

### 6.当`auto`推导的型别不符合要求时，使用带显式型别的初始化物习惯用法
- 理解**代理类**，比如`std::vecotr<bool>`的代理类`std::vector<bool>::reference`，还有`std::shared_ptr`,`std::unique_ptr`等
- 对代理类使用auto不是明智的选择。
- 使用`static_cast<T>`

## 转向现代C++
### 7.在创建对象时注意区分`()`和`{}`
- 大括号初始化禁止内建型别之间进行隐式窄化性别转换(narrowing conversion)，比如`int x = {1.0}`是禁止的，但是`int x = (1.0)`是可以的
- C++规定：**任何能够解析为声明的都要解析为声明**
- 所以使用`Point p();`可能变成声明一个函数，而不是一个对象，使用`Point p{};`没有以上歧义
- 大括号初始化语法的调用语句会强烈优先选用带有`std:initializer_list`的性别形参的重载版本

### 8.优先选用`nullptr`，而非`0`或`NULL`
- `nullptr`实际型别为`std::nullptr_t`，`std::nullptr_t`可以隐式转换为所有的裸指针型别
- `nullptr`无法被识别为任何整型
- 所以表达空指针，使用`nullptr`

### 9.优先选用别名声明，而非typedef
- using在函数指针的型别上更加好理解
```cpp
typedef void (*FP)(int, const std::string&);
using FP = void (*)(int, const std::string&);
```
- using支持模版类，而typedef不支持
- 别名模版可以避免写`::type`后缀

### 10.优先选用限定作用域的枚举型别，而非不限作用域的枚举型别
```cpp
enum Color {black, white, red}; //非限定作用域
enum class Color {black, white, red}; //限定作用域
```

- 限定作用域不会造成**命名空间污染**，使用`Color::red`访问，而非限定作用域使用`red`访问
- 限定作用域不会进行无意间的**隐式类型转换**，只能通过强制转换
- 限定作用域的**默认逻辑底层**是int，不限定作用域没有默认
```cpp
eum class Color std::uint32_t; // 也可以自定义类型
eum Color std::uint32_t; // ok
```
- 限定作用域总是可以**前置声明**，而不限定作用域只有在**指定默认底层型别**的前提下才可以前置声明
- 不限定作用域的好处：
在以下情形中，限定作用域枚举型别的名字会很臭长，可以使用不限定作用域枚举型别
```cpp
// 定义一个包含持有名字，电子邮件，声望值的元组
using UserInfo = std::tuple<std::string, std::string, std::size_t>;
UserInfo uInfo;
auto val = std::get<1>(uInfo); // 

enum UserInfoFields {uiName, uiEmail, uiReputation}; // 使用不限定作用域
auto val = std::get<uiEmail>(uInfo); // 更加清楚取的电子邮箱
```


### 11.优先选用删除函数，而非`private`未定义函数
当我们想要阻止其他程序员调用某些函数是，可以使用删除函数(deleted function)，而非将函数放置到private中然后不去定义它
- 删除函数无法通过任何方法使用，即使是成员和友元函数
- 删除函数会被声明为public，这样做的原因是：当客户代码试图调用某个函数时，c++会先校验可访问性，然后校验删除状态
- 任何函数都能成为删除函数，但是只用成员函数能声明为private
样例：
```cpp
bool isLucky(int number); //非成员函数
bool isLucky(char) = delete; // 拒绝char类型访问
bool isLucky(bool) = delete; // 拒绝float类型访问
bool isLucky(double) = delete; // 拒绝double类型访问
```
> 样例中的代码，如果传入一个float，也会被拒绝掉，因为C++float会优先转化为double，而不是int，然后就被编译阻止了

样例2：
```cpp
template<typename T> 
void processPointer(T* ptr); // 指针类型的模板类Í

template<>
void processPointer<void>(void*) = delete; // 
void processPointer<char>(char*) = delete; //
//还可以删除const，volatile等修饰的void*，char*
```


### 12.为意在改写的函数添加`override`声明
> 尽管改写(override)和重载(overload)很像，但是他们是两个完全不同的概念，改写是多态分一个表现形式，而重载不是多态。

如果要正确编写一个改写，应该满足以下条件：
1. 基类中的函数必须是**虚函数**
2. 基类和派生类中的函数**名字**必须完全相同（析构函数除外）
3. 基类和派生类中的函数型别**参数**必须完全相同
4. 基类和派生类中的函数**常量性**必须完全相同
5. 基类和派生类中的函数**返回值**和**异常规格**必须**兼容**。
6. 基类和派生类的的函数**引用饰词**完全相同(C++11)

样例：
```cpp
class Base {
public:
	virtual void mf1() const;
	virtual void mf2(int x);
	virtual void mf3() &;
	void mf4() const;
};

// 警告，但是不会报错，无法完成改写
class Derived: public Base {
public:
	virtual void mf1(); // 错误，常量性不同
	virtual void mf2(unsigned int x); // 错误，参数型别不同
	virtual void mf3() &&; // 错误，引用饰词不同
	void mf4() const; // 错误，mp4未声明为虚函数
}
```

如果将派生类的函数加上override声明，那么情况就会完全不一样了，编译器会报错和override有关信息
```cpp
// 程序无法运行，会报错
class Derived: public Base {
public:
	virtual void mf1() override; 
	virtual void mf2(unsigned int x) override; 
	virtual void mf3() && override;  
	void mf4() const override; 
}
```

> C++新增的语境关键字(contextual keyword)override和final，final用于虚函数，会阻止它在派生类中被改写，final也可以用于一个类，这个类将无法作为基类。如果一个代码中用了override的这个名字，那么也不要紧，因为override仅仅出现在成员函数声明的末尾时才有保留意义。

### 13.优先选用`const_iterator`，而非`iterator`
> 当需要一个迭代器而不会对其内容修改时，应该使用`const_iterator`，尽管它在C++98不是那么好用，但是在C++11及以后获取和使用`const_iterator`变得容易

样例：
```cpp
// 在第一个targetVal后添加insertVal（通用版本）
template<typename C, typename V>
void findAndInsert(C& container, const V& targetVal, const V& insertVal) {
	using std::cbegin();
	using std::cend();
	// 非成员函数版本的cbegin，cend
	auto it = std::find(cbegin(container), cend(container), targerVal());
	container.insert(it, insertVal);
}
```
-  在写通用化库代码时，考虑到容器或者类似容器的数据结构会以非成员函数的方式提供begin和end，所以选用非成员函数版本
- 可惜的是C++11中只有非成员函数版本的begin和end，C++14添加了cbegin,cend,rbegin,rend,crbegin,crend

实现一个非成员函数版本的cbegin也并非难事：
```cpp
template <class C>
auto cbegin(const C& container) -> decltype(std::begin(conainer)) {
	return std::begin(container);
}
```
> 通过const型别的形参container来访问实参，调用非成员版本的begin函数并传入一个const容器会产生一个const_iterator。这个方法甚至不需要调用cbegin


### 14.只要函数不会发射异常，就为其加上`noexcept`声明
> 当知道一个函数不会发出异常而没用给它加上noexcept声明的话，那就是接口规格的缺陷

不会发出异常的表达方式：
```cpp
int f(int x) throw(); // C++98
int f(int x) noexcept; //  C++11
```
> 在noexcept声明的函数中，优化器不需要在**异常传出函数的前提下**，将执行器保持**可开解状态**，也不需要在**异常逸出的前提下**，保证所有对象以其**被构造的顺序的逆序完成析构**，而以`throw()`声明的函数享受不了这样的优化灵活性。

- `push_back()`提供强异常安全保证(strong exception safety guarantee)，在C++98中是因为旧内存中的所有元素都被成功复制到新内存中后，才会执行析构函数。在C++11中采用了**移动代替复制**，而且必须是noexcept声明的移动
- 标准库中的swap是否带有noexcept声明取决于用户定义的swap是否带有noexcept声明
- 默认的，**内存释放函数**和所有的**析构函数**都隐式的具备noexcept性质
- 事实上，大多数函数是**异常中立**的(exception-neutral)，这类函数本身不会抛出异常，而它调用的函数可能抛出异常，它也不具备noexcept的性质

### 15.只要有可能使用`constexpr`，就使用它
- 所有constexpr对象都是const对象，而所有const对象不一定是constexpr对象
- 在编译期时，constexpr函数所有实参值已知，那么结果也会在编译期间算出来，如果任何一个实参值在编译器未知，那么代码将无法通过编译
- 在调用constexpr函数时，如果有传入的值是编译期间未知的，那么它和普通函数没有区别

样例：
```cpp
class Point {
public:
	constexpr Point(double xVal = 0, double yVal = 0) noexcept 
	: x(xVal), y(yVal) {}

	constexpr double xValue() const noexcept { return x; }
	constexpr double yValue() const noexcept { return y; }

	// C++11中返回值为void无法设置为constexpr属性
	constexpr void setX() noexcept { x = newX; } // C++14
	constexpr void setY() noexcept { y = newY; } // C++14
private:
	double x, y;
}

constexpr Point p1(1.0, 2.0);
constexpr Point p2(5.0, 4.0);
```
> 构造函数和其他成员函数也可能时constexpr函数，传入的实参在编译期已知，这样构造出来的Point对象也是具有constexpr属性的，从而也可以编写以下constexpr函数
```cpp
constexpr Point minPoint(const Point& p1, const Point& p2) noexcept {
	return {(p1.xValue() + p2.xValue)/2, (p1.yValue() + p2.yValue)/2};
}

constexpr auto mid = midpoint(p1, p2); //也具备constexpr属性
```
> 这就意味着很多工作都可以在编译器完成，运行更快，编译变慢（


### 16.保证`const`成员函数的线程安全性
> 为了确保const成员函数安全，我们需要考虑两个线程同时调用这个const成员函数的情况

样例：
```cpp
class Polynomial {
public:
	using RootsType = std::vector<double>;
	RootsType roots() const 
	{
		std::lock_guard<std::mutex> g(m); // 加上互斥锁，否则不是线程安全的
		if(!rootsAreVaild) {
			...
			
			rootAreVaild = true;
		}
		return rootVals;
	} // 解除互斥量
private:
	// 被const函数修改，所以要加上mutable
	mutable std::mutex m; 
	mutable bool rootAreValid {false};
	mutable RootsType rootVals {};
}
```
> 这里如果不加std::mutex，可能有不同的多个线程在没有同步的情况下读写同一块内存，就是数据竞险(data race)

- std::atomic类型具有原子性，且比std::mutex性能好，对于单个要求同步的变量或内存区域，使用std::atomic就行操作，但是如果有两个以及以上的变量或内存区域需要作为一个整体来操作时，需要使用std::mutex


### 17.理解特种成员函数的生成机制
> 特种成员函数是指C++会自行生成的成员函数

- 6种特种函数：
  1. 默认构造函数、析构函数
  2. 复制构造函数、复制赋值运算符
  3. 移动构造函数、移动赋值运算符(C++11)
- 两种**复制函数**是**彼此独立**的：即声明其中一个，并不会阻止编译器生成另外一个
- 两种**移动函数**是**不彼此独立**的：即声明其中一个，会阻止编译器生成另外一个
- 声明了复制操作会影响生成移动操作
- 声明了移动操作，编译器就会废除复制操作(delete)
- **三大律**(Rule of There)：如果声明了复制构造函数、复制赋值运算符、或者析构函数其中一个，那么就得同时声明所有的三个
- 结合以上，**移动操作的生成条件**（如果需要生成）：
  1. 该类未声明任何复制操作
  2. 该类未声明任何移动操作
  3. 该类未声明任何析构函数

如果有一个类：
```cpp
class StringTable {
public:
	StringTable() {}
	... // 实现插入，删除，查找等，但是没有实现任何复制，移动，析构
private:
	std::map<int, std::string> values;
}
```
如果**加入析构**函数，在日志中记录析构事件
```cpp
class StringTable {
public:
	StringTable() {}
	... // 实现插入，删除，查找等，但是没有实现任何复制，移动
	～StringTable() {  // 后来加上的析构函数
		makeLogEntry("Destroy StringTable object");
	}
private:
	std::map<int, std::string> values;
}
```
> 如果这样的话就会引发**性能问题**，因为它阻止了移动操作的生成，但是可以被正常地被编译、执行，并且通过测试。针对移动操作的请求触发的是复制操作


## 智能指针

### 18.使用`std::unique_ptr`管理具备专属所有权的资源
> 在默认情况下std::unique_ptr和裸指针有着相同的尺寸。在执行析构操作时，由非空的std::unique_ptr析构其资源。

举一个例子：
```cpp
template<typename... Ts>
auto makeInvestment(Ts&&... params) {
	auto delInvmt = [](Investment* pInvestment) { //自定义析构器
		makeLogEntry(pInvestment); // 写入日志
		delet pInvestment;
	};
	std::unique_ptr<Investment, decltype(delInvmt)> pInv(nullptr, delInvmt);

	if () { // 应该创建Stock对象
		pInv.reset(new Stock(std::forward<Ts>(params)...));
	}
	else if () { // 应该创建Bond对象
		pInv.reset(new Bond(std::forward<Ts>(params)...));
	}
	else if () { // 应该创建RealEstate对象
		pInv.reset(new RealEstate(std::forward<Ts>(params)...));
	}
	return pInv;
}
```
- 默认的析构通过delete运算符实现，但是析构过程中std::unique_ptr可以被设置为自定义析构器(custom deleter)
- 将一个裸指针赋给std::unique_ptr是不会通过编译的，这涉及了隐式型别转换，所以这里需要使用reset来指定pInv获得new产生的对象的所有权
- 每一次new的调用，都使用std::forward对传递给makeInvestment的实参实施完美转发
- 通过基类指针删除一个派生类对象，为此我们需要具备一个虚析构函数，如：
```cpp
class Investment {
	...
	virtual ~Investment();
	...
}
```


### 19.使用`std::shared_ptr`管理具备共享所有权的资源
- std::shared_ptr的大小是**裸指针的两倍**，不像std::unique_ptr是个不定值
- std::shared_ptr的两个指针分别指向**对象**和**控制块**(引用计数、弱计数、其他数据)
- 引用计数的内存必须动态分配
- 引用计数的递增和递减必须是原子操作
- 同样支持自定义析构器，但是用法与std::unique_ptr不同，自定义析构器也不会改变指针的大小
```cpp
auto loggingDel = [](Widget *pw) {
	makeLogEntry(pw);
	delete pw;
};

std::unique_ptr<Widget, decltype(loggingDel)>  upw(new Widget, loggingDel);
std::shared_ptr<Widget> spw(new Widget, loggingDel); //无需将析构器类型作为指针类型的一部分
```
- 一个对象的控制块由首个涉及该对象的std::shared_ptr来确定
- **不建议**用裸指针来初始化shared_ptr
```cpp
auto pw = new Widget;

std::shared_ptr<Widget> spw1(pw, loggingDel);
std::shared_ptr<Widget> spw2(pw, loggingDel); // 重复控制块！！！
```
> 重复的控制块意味着重复的计数引用，意味着多次析构，如果在spw1存在的基础上要初始化spw2应该用spw1来初始化


### 20.对于类似`std::shared_ptr`但有可能空悬的指针使用`std::weak_ptr`
> 像std::shared_ptr那样运行，但又不影响对象的引用计数，其指向的对象也可能被析构，也能判断指针是否空悬。一般通过std::shared_ptr来初始化std::weak_ptr

样例：
```cpp
auto spw = std::make_shared<Widget>();
std::weak_ptr<Widget> wpw(spw);  // 使用shared_ptr初始化

spw = nullptr; // wpw空悬

if(wpw.expired()) ... // expired 判断失效
```

>  是用expired方法和提领操作之间，如果有另外一个线程可能重新复制或操作会导致未定义行为。因此我们需要一个原子操作来完成std::weak_ptr是否失效的检验。可以通过使用std::weak_ptr来创建std::shared_ptr来实现。实现方式有两种：
```cpp
// 如果wpw失效，spw1 为空
auto spw1 = wpw.lock();
// 如果wpw失效，抛出std::bad_weak_ptr型别异常
std::shared_ptr<Widget> spw2(wpw);
```

std::weak_ptr用途：
1. 缓存
2. 观察者列表
3. 避免std::shared_ptr环路

### 21.优先选用`std::make_unique`和`std::make_shared`，而非直接使用`new`
#### 使用make系列函数的原因：
1. **打字少**：配合auto使用可以少写一次对象类型
2. **异常安全**：
```cpp
// 使用new 
processWidget(std::shared_ptr<Widget>(new Widget), computePriority());
// 使用std::make_shared
processWidget(std::make_shared<Widget>(), computePriority());
```
> 在使用new的语句中，编译器可能按照：实施"new Widget"、执行computePriority、运行std::shared_ptr构造函数的顺序来执行操作。这样在运行期computePriority就会产生异常。unique也是如此

3. **性能优(shared)**：都知道shared_ptr是两个指针，如果使用new那么就会引发两次内存分配：为对象进行一次内存分配，为控制块就行一次内存分配；而使用std::make_shared就只用一次内存分配

#### 不应该使用make系列函数的情景：
1. **自定义析构器**：make系列函数并不支持自定义析构器
2. **大括号初始化**：make系列函数对形参进行完美转发，使其有可以完美转发大括号初始化的能力。但是有一个变通的方法：可以先初始化一个std::initializer_list对象，然后作为参数传递给make_unique做一个隐式转换
```cpp
auto up1 = std::make_unique<std::vector<int>>(10, 20); // 10个20
auto up2 = std::make_unique<std::vector<int>>{10, 20}; // error
  
auto initList = {10, 20}; // std::initializer_list对象
auto up3 = std::make_unique<std::vector<int>>(initList); // 10, 20
```
3. **自定义new和delete(shared)**：make_shared会包含控制块的大小，因此全局的new就不一定适用了
4. **大对象(shared)**：std::make_shared采用的是一次内存分配，只有当shared_ptr和weak_ptr都析构时，才会释放内存块；而new是两次内存分配，当shared_ptr都析构时就会释放**对象内存块**，当weak_ptr都析构时就会释放**控制块内存块**。在这个期间就会占用内存。

当我们既需要异常安全，又需要自定义析构器时：
```cpp
void cusDel(Widget *ptr); // 自定义析构器
// 版本一：并非异常安全
processWidget(
	std::shared_ptr<Widget>(new Widget, cusDel), 
	computePriority()
);

// 版本二：性能并非最优
std::shared_ptr<Widget> spw(new Wiget, cusDel);
prcessWiget(spw, computePriority()); // 传递的是左值

// 版本三：安全 + 效率
std::shared_ptr<Widget> spw(new Wiget, cusDel);
prcessWiget(std::move(spw), computePriority()); // 传递的是右值
```


### 22.使用`Pimpl`习惯用法时，将特殊成员函数的定义放到实现文件中
> Pimpl(pointer to implementation)，即指涉到实现的指针，把某类中的数据成员用一个指向到某实现类(or结构体)的指针替代，然后把主类中的数据成员放到实现类中，通过指针访问这些数据成员
```cpp
class Widget {
public:
	Widget();
	...
private:
	std::string name;
	std::vector<int> data;
	Gadget g1, g2; // 某种用户自定义类型
}

//Pimpl
class Widget {
public:
	Widget();
	~Widget(); // 提前声明析构函数
	...
private:
	struct Impl;
	std::unique_ptr<Impl> pImpl;
}
```
然后在其实现文件(widget.cpp)中定义：
```cpp
struct Widget::Impl {
	std::string name;
	std::vector<int> data;
	Gadget g1, g2;
};
Widget::Widget() : pImpl(std::make_unique<Impl>) {}
Widget::~Widget() {} // 析构函数位置在Widget::Impl 之后
```

## 右值引用、移动语义和完美转发

### 23.理解std::move和std::forward
> “std::move并没有移动任何东西，完美转发也并不完美”，这两者在运行期间不会生成任何可执行代码。std::move和std::forward都仅仅是**强制类型转换函数**。std::move无条件将实参转化为右值，std::forward仅在某个特定条件满足时才执行同一个强制转换

样例：
```cpp
class Annotation {
public:
	explicit Annotation(const std::string text) : value(std::move(text)) {...}
	...
private:
	std::string value;
};
```
> 这里的代码成功通过了编译，可惜text并非是被移动，而是被复制到了value中，text在转换之前时左值const std::string，转换后是右值const std::string。这个右值无法传递给std::string的移动构造函数，因为移动构造函数只能接受非常量std::string。可是这样的一个右值可以传递给复制构造函数，因为指向常量的左值引用允许绑定一个常量右值，所以我们学到两点：
1. 执行移动操作不要声明为常量
2. std::move并不移动任何东西，甚至不保证强制转换后的对象是否具备可以移动能力

#### std::forward
*所有函数形参都是左值*，当我们的函数模版使用了万能引用，然后将其传递给另外一个函数时，传递的是一个左值。使用std::forward就是：当实参是使用右值完成初始化时，才会执行右值型别的强制转换。

### 24.区分万能引用和右值引用
> "T&&"不一定是右值引用，还有可能是万能引用。万能引用不仅要形如“T&&”，而且必须存在型别推倒
```cpp
void f(Widget&& param); // 右值引用

Widget&& var1 = Widget(); // 右值引用

auto&& var2 = var1; // 万能引用

template<typename T>
void f(std::vector<T>&& param); // 右值引用

template<typename T>
void f(T&& param) // 万能引用

// const 也会剥夺万能引用的性质
template<typename T>
void f(const T&& param) // 右值引用
```


### 25.针对右值引用实施std::move，针对万能引用实施std::forward
- 如果对于右值引用实施forward也可以运行，但是易错且不符合习惯；
- 对于万能引用实施move，后果可能是某些左值遭到意外改动。
- 使用std::move和std::forward时机应当是该引用最后一次使用的场合

#### 使用重载相比于万能引用：
- **可以使用const**：引用不能带有const饰词
- **编写与维护更多代码**：得写两个函数，如果n个参数得写2^n个函数，甚至有些函数有无穷个参数，而每个参数都有可能是左值或者右值
- **效率低**：例如代码`w.setName("Adela Novak");`，万能引用版本只用调用一次std::string赋值运算符；重载版本使用一 次std::string构造函数，一次std::string移动运算符，一次std::string析构函数。


#### 返回值优化(return value optimization, RVO)
> 1.局部对象型别和函数返回值型别相同，2.返回的就是局部对象本身。当以上两点满足时，就会触发C++的返回值优化

### 26.避免依万能引用型别进行重载
> 可以兼容性能、效率方面

样例：
```cpp
std::multiset<std::string> names;

void logAndAdd(const std::string& name) {
	auto now = std::chrono::system_clock::now();

	log(now, "logAndAdd");
	names.emplace(name);
}
// 有以下调用
std::string petName("Darla");

logAndAdd(petName); // 左值
logAndAdd(std::string("Persephone")); // 右值
logAndAdd("Patty Dog"); // 字面量
```
 >  这里的右值传递到`logAndAdd`中后name是一个左值，然后传递到到names里面时会有一次复制，产生效率问题

改进后：
```cpp
template<typename T>
void logAndAdd(T&& name) { // 万能引用
	auto now = std::chrono::system_clock::now();
	log(now, "logAndAdd");
	names.emplace(std::forward<T>(name)); // forward配万能引用
}
```
> 这里就可以很好的兼顾效率

如果还要加入下标重载：
```cpp
void logAndAdd(int idx) { // 索引，重载
	auto now = std::chrono::system_clock::now();
	log(now, "logAndAdd");
	names.emplace(nameFromIdx(idx)); // 
}
```
> 加入重载可以传递下标

但是假如运行：
```cpp
short nameIdx;
logAndAdd(nameIdx); // 错误
```
> 这里我们理想使用下标重载，但是实际上万能引用成为了它的选择。万能引用会成为大多数类型的选择，往往比预期的要多。所以**尽可能的不要为完美转发添加重载**


### 27.熟悉依万能引用型别进行重载的替代方案

#### 前提问题：
```cpp
class Person {
public:
	template<typename T>
	explicit Person(T&& n) : name(std::forward<T>(n)) {}

	explicit Person(int idx) ;
	
	Person(const Person& rhs); // 编译器生成
	Person(Person&& rhs); // 编译器生成
private：
	std::string name;
}

Person p("Nancy");
auto cloneOfp(p); // 错误，调用了构造函数，而不是复制构造函数
```
> 设计了万能引用的构造函数，会“吸收”一些本来调用复制构造函数的语句使用了构造函数，从而报错。


#### 舍弃重载(针对logAndAdd样例)
> 放弃重载，写多个函数

- 优点：不出错
- 缺点：更多的代码量，无法解决Person样例

#### 传递`const T&`型别形参
> 使用传递cosst T&来代替 T&&万能引用

- 优点：避免万能引用，不出错
- 缺点：无法高效处理传递右值引用的情况

#### 传值
> 使用std::string替换T&&。然后每次按值传递对象

- 优点：避免万能引用，不出错
- 缺点：反直觉

#### 标签分派
>  




### 28.理解引用折叠
### 29.假定移动不存在、成本高、未使用
### 30.熟悉完美转发的失败情形

## lambda表达式
### 31.避免默认捕获模式
- lambda式和闭包类存在于编译器，闭包存在于运行期
- 要注意引用变量和lambda表达式的生命周期的关系
```c++
using FilterContainer = std::vector<std::function<bool(int)>>;
FilterContainer filter;

void addDivisorFilter(){
	auto calc1 = computeSomeValue1();
	auto calc2 = computeSomeValue2();
	auto divisor = computeDivisor(calc1, calc2);
	
	// 错误，divisor和filter的生命周期不一样
	filter.emplace_back(
		[&](int value) {return value % divisor == 0;}
	);
	// 可以，按值传递
	filter.emplace_back(
		[=](int value) {return value % divisor == 0;}
	);
}
```

- lambda式和类成员变量的关系
```c++
class Widget {
public:
	void addFilter() const;
private:
	int divisor;
}

// 错误，支持捕获到非静态的局部变量
void Widget::addFilter() const {
	filter.emplace_back(
		[=](int value) {return value % divisor == 0;}	
	);
}

// = 实际上捕获的是 this指针的副本
void Widget::addFilter() const {
	auto currentObjectPtr = this;
	filter.emplace_back(
		[currentObjectPtr](int value)
		{return value % currentObjectPtr->divisor == 0;}	
	);
}

// 一种可行方案
void Widget::addFilter() const {
	auto divisorCopy = divisor;
	filter.emplace_back(
		[=](int value) {return value % divisorCopy == 0;}	
	);
}

// 广义lambda
void Widget::addFilter() const {
	filter.emplace_back(
		[divisor = divisor](int value) {return value % divisor == 0;}	
	);
}

```

### 32.使用初始化捕获将对象移入闭包
### 33.对auto&&型别的型参使用decltype，以std::forward之
### 34.优先选用lambda式，而非std::bind

## 并发API
### 35.优先选用基于任务而非基于线程的程序设计
### 36.如果异步是必要的，则指定std::launch::async
### 37.使std::thread型别对象在所有路径皆不可联结
### 38.对变化多端的线程句柄析构函数行为保存关注
### 39.考虑针对一次性事件通信使用以void为模版型别的实参的期值
### 40.对并发使用std::atomic，对特种内存使用volatile

## 微调
### 41.针对可复制形参，在移动成本低并且一定会被复制的前提下，考虑将其按值传递
### 42.考虑置入而非插入