```cpp
#pragma once

#include <climits>
#include <cstddef>

/*
*	这个是内存分配器或者说是内存池
*	主要用于模拟原生的new和delete运算符
*	
*	因为如果对于某一类型的对象进行大量的内存操作, 比如说销毁和创建, 
*	如果只使用原生的, 那么会容易造成内存碎片的问题, 从而降低程序性能
*	甚至会造成程序崩溃.
*	
*	而解决策略就是创建一个内存池, 这个内存池向系统申请一大块内存
*	然后我们如果要申请内存操作对象时, 就向内存池申请内存
*	然后我们不再使用这个对象, 就把这个对象的内存归还给内存池, 以备之后再利用
*	如果内存池已满, 就再申请一大块内存.
*	以空间换时间的策略,来避免内存碎片的问题, 也大大提高内存分配效率
* 
*	总之, 内存池就是个管家一样的存在, 我们不直接接触系统申请内存, 而是让内存池代理这个操作.
* 
*		
*	这个内存分配器只支持单个对象的内存分配, 不支持对象数组内存分配.
*	当然我们也可以把这个对象包装一层, 来支持这个数组内存分配.
*	
*	这个分配器的思路是首先申请一大片内存
*	然后根据目标类型分块
*	注意此时的各个分块是未被初始化, 或者说构造的
*	我们内存池将内存分配跟对象构造分离开来, 以支持给更加细粒度的操作
* 
*	当我们内存池满了后, 就开辟出新的内存大块, 而这个内存大块与之前的内存大块其实也是用链表管理的, 新的内存大块被放在表头
*	这个内存块的开头第一个分块是用作指针, 以指向下一个内存块
*	当我们向内存池new一个对象时, 内存池就取出第二个分块, 分配给这个对象, 然后再调用定点构造器, 原地构造出指定对象
*	
*	而我们释放这个对象时, 并没有真正将这个对象所占有的内存还回给系统, 而是还回给内存池, 而内存池将这个归还回来的内存分块连接到一个链表的表头
*	当我们再次向内存池new一个对象时, 就查看这个链表到底空不空, 如果不空则取出这个链表的第一个分块构造返回给用户, 否则就向这个大内存块取出分块构造.
* 
*	当然, 我们释放这个对象也是分步进行的, 第一步就是调用destroy, 触发这个对象自己的析构函数, 用来释放这个对象自己所管理的内存,
*	然后再调用deallocate, 归还这个分块.
*	当然如果这个对象并没有管理资源, 也可以不用调用这个对象的析构函数, 直接归还分块
*	
*	值得注意的是, 我们所维护的节点是个共用体, 既可以储存对象数据, 也可以用作指针指向下一个分块
*	为什么不将这个节点用包含数据和指针两个成员的结构表示节点, 而用共用体呢?
*	因为, 共用体节省内存大小, 并且我们发现, 当我们未使用分块节点或者被放在回收链表时, 其实里面的数据是无用的, 所以干脆就可以把这个节点当作指针来用
*	而当我们要使用这个节点是, 里面的数据被构造出来, 此时就被当作数据本身.
*	数据和指针的使用时机本身是错开的, 因此用共用体就再合适不过了.
*/


template <typename T, size_t Size = 1024>
class Allocator
{
public:
	// 重定义类型
	using value_type = T;
	using pointer = T*;
	using reference = T&;
	using const_pointer = const T*;
	using const_reference = const T&;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	
	// 自指
	using self_ = Allocator<T, Size>;
	using self_reference = Allocator<T, Size>&;

private:
	// 节点单元
	// 是个共用体, 在未使用时, 当作指针, 当被构造完成时, 则为数据单元
	union _Node
	{
		value_type data;
		_Node* next;
	};

	// 私有重定义类型
	using raw_pointer = char*;  // 未使用具体化时的内存块单位是char
	using node_type = _Node;
	using node_pointer = _Node*;

	//私有成员 都是_Node指针
	node_pointer _currentBlock;
	node_pointer _currentNode;
	node_pointer _lastNode;
	node_pointer _freeNode;
	
	// 申请内存块的函数
	void allocateBlock() noexcept;

public:

	// 内存块的大小必须要至少是节点大小的两倍
	static_assert(Size >= 2 * sizeof(node_type), "Block size is too small.");


	static constexpr size_type Num = Size / sizeof(node_type) - 1;  // 每个内存块所能储存的节点的个数
	static constexpr size_type MaxNum = Num * size_type(-1);   // 这个分配器所能够储存的最大节点数

	Allocator() noexcept = default; //采用默认构造函数, 四个私有成员的值都是nullptr
	Allocator(Allocator&& other_alloc) noexcept; // 允许移动构造函数

	// 禁止复制构造函数和赋值运算符函数
	Allocator(const Allocator&) = delete;
	Allocator& operator=(const Allocator&) = delete;
	Allocator& operator=(Allocator&&) = delete;

	//析构函数, 要用来释放这个内存分配器的内存
	~Allocator() noexcept;

public:

	// 下面是暴露出来的公有接口

	//取地址函数
	pointer address(reference _ref) const noexcept;
	const_pointer address(const_reference _cref) const noexcept;

	//分配和回收节点
	pointer allocate() noexcept;
	void deallocate(pointer _ptr);

	//在指定节点构造对象
	template<typename... Args>
	pointer construct(pointer _ptr, const Args &... args);
	// 在指定地点释放这个节点对象所管理的内存, 
	// 如果这个对象并没有管理堆内存, 其实也可以不用这个函数, 直接调用deallocate回收节点就行了
	void destroy(pointer _ptr);

	// 一步到位模拟new运算符
	template<typename... Args>
	pointer newObject(const Args & ...args);

	// 一步到位模拟delete运算符
	template<typename... Args>
	void deleteObject(pointer _ptr);
};

// 获取内存大块
template<typename T, size_t Size>
inline void Allocator<T, Size>::allocateBlock() noexcept
{
	raw_pointer rawBlock = reinterpret_cast<raw_pointer> (operator new(Size)); // 原始态的内存大块
	reinterpret_cast<node_pointer>(rawBlock)->next = _currentBlock; // 将原始大块转换为共用体节点的数组, 而这个数组的第一个节点被当作指针以指向下一个内存大块
	_currentBlock = reinterpret_cast<node_pointer>(rawBlock);	// 当前内存块就是这个原始态的内存大块
	_currentNode = _currentBlock + 1;	// 当前非指针节点就是开头指针节点的下一个
	_lastNode = _currentNode + Num;  // 确定当前内存块的边界
	// 这个当前内存块可能并未能被节点完全占据
}

// 获得另一个内存池的内容的所有权
template<typename T, size_t Size>
inline Allocator<T, Size>::Allocator(Allocator&& other_alloc) noexcept
{
	_currentBlock = other_alloc._currentBlock;
	_currentNode = other_alloc._currentNode;
	_lastNode = other_alloc._lastNode;
	_freeNode = other_alloc._freeNode;

	//不要忘记将源内存池的成员设为null
	other_alloc._currentBlock = nullptr;
	other_alloc._currentNode = nullptr;
	other_alloc._lastNode = nullptr;
	other_alloc._freeNode = nullptr;
}

// 析构函数 释放内存池所管理的内存
template<typename T, size_t Size>
inline Allocator<T, Size>::~Allocator() noexcept
{
	// 一个个内存大块节点接个释放
	node_pointer cur = _currentBlock;
	while (cur)
	{
		node_pointer tmp = cur->next;
		operator delete(reinterpret_cast<void*>(cur));
		cur = tmp;
	}
}


template<typename T, size_t Size>
inline typename Allocator<T, Size>::pointer 
Allocator<T, Size>::address(reference _ref) const noexcept
{
	return &_ref;
}

template<typename T, size_t Size>
inline typename Allocator<T, Size>::const_pointer
Allocator<T, Size>::address(const_reference _ref) const noexcept
{
	return &_ref;
}

// 只负责分配内存, 不负责构造, 所分配的内存的内容是未定义的
template<typename T, size_t Size>
inline typename Allocator<T, Size>::pointer
Allocator<T, Size>::allocate() noexcept
{
	// 如果回收分块链表未空, 就取出这个链表的表头节点
	if (_freeNode)
	{
		pointer result = reinterpret_cast<pointer>(_freeNode); // 注意这个操作, 使用强制转换将这个节点转化为目标类型的格式
		_freeNode = _freeNode->next;
		return result;
	}
	// 否则就从内存大块申请内存
	else
	{
		if (_currentNode >= _lastNode) // 如果内存池满了或者为空, 就调用addBlock()获取内存大块, 然后继续分配
			allocateBlock();
		return reinterpret_cast<pointer>(_currentNode++); // 返回当前节点并向后移动一位
	}
}

//回收内存分块
template<typename T, size_t Size>
inline void Allocator<T, Size>::deallocate(pointer _ptr)
{
	if (_ptr)
	{
		// 使用强制转换, 将节点转化为指针, 并放到回收链表的表头
		reinterpret_cast<node_pointer>(_ptr)->next = _freeNode; 
		_freeNode = reinterpret_cast<node_pointer>(_ptr);
	}
}

//释放位于指定分块的对象所管理的资源
template<typename T, size_t Size>
inline void Allocator<T, Size>::destroy(pointer _ptr)
{
	_ptr->~T();
}

//在指定的位置构造, 而这个位置就在用内存池分配出来的分块内存这里
template<typename T, size_t Size>
template<typename ...Args>
inline typename Allocator<T, Size>::pointer
Allocator<T, Size>::construct(pointer _ptr, const Args & ...args)
{
	return new(_ptr) T((args)...); // 调用目标对象的构造函数
}

// 模拟new运算符
template<typename T, size_t Size>
template<typename ...Args>
inline typename Allocator<T, Size>::pointer
Allocator<T, Size>::newObject(const Args & ...args)
{
	pointer result = allocate();
	return construct(result, args...);
}

// 模拟delete运算符
template<typename T, size_t Size>
template<typename ...Args>
inline void Allocator<T, Size>::deleteObject(pointer _ptr)
{
	if (_ptr)
	{
		destroy(_ptr);
		deallocate(_ptr);
	}
}

```
