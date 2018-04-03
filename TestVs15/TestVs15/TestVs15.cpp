// TestVs15.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <functional>
#include <vector>
#include <future>
#include <list>

using namespace std;

class ClxBase
{
public:
	ClxBase() {};
	virtual ~ClxBase() { cout << "Output from the destructor of class ClsBase" << endl; };

	virtual void DoSomething()
	{
		cout << "Do something in class ClxBase!" << endl;
	};
public:
	void foo()
	{
		static int iTest = 1;
		cout << "iTest" << iTest << endl;
		cout << "++iTest" << ++iTest << endl;
	}
protected:
	int i_ID{ 0 };
};

class ClxDerived :public ClxBase
{
public:
	ClxDerived() = default;
	~ClxDerived() { cout << "Output from the destructor of class ClxDerived" << endl; }
};


class Test
{
public:
	typedef int(*FunType)(int, int);
	void Add_pointer(FunType fun, int a, int b)
	{
		int sum = fun(a, b);
		std::cout << "sum:" << sum << std::endl;
	}

	void Add_function(std::function<int(int, int)> fun, int a, int b)
	{
		int sum = fun(a, b);
		std::cout << "sum:" << sum << std::endl;
	}
};

class TestAdd
{
public:
	int Add(int a, int b)
	{
		return a + b;
	}

	static int Add_s(int a, int b)
	{
		return a + b;
	}
};

void testFunc(std::vector<int>&& vec)
{
	vec.push_back(10);
}

void testFunc(std::vector<int>& vec)
{
	vec.push_back(12);
}
ClxBase testClxFunc()
{
	ClxBase* pbaset = new ClxBase;
	return *pbaset;
}
void xFunc(ClxBase())
{
	cout << "hello clx base" << endl;
}

promise<int> prom;
void PrintPromise()
{
	future<int> fut = prom.get_future();
	int res = fut.get();
	cout << "promise value: " << res << endl;
}

//quick-sort using async
template<typename T>
std::list<T> parallel_quick_sort(std::list<T> input)
{
	if (input.empty())
	{
		return input;
	}
	std::list<T> result;
	result.splice(result.begin(), input, input.begin());  // splice , move input objects into result
	T const& pivot = *result.begin();

	auto divide_point = std::partition(input.begin(), input.end(),  // std partition
		[&](T const& t) {return t < pivot; });

	std::list<T> lower_part;
	lower_part.splice(lower_part.end(), input, input.begin(),
		divide_point);

	std::future<std::list<T> > new_lower(  
		std::async(&parallel_quick_sort<T>, std::move(lower_part)));

	auto new_higher(
		parallel_quick_sort(std::move(input))); 

	result.splice(result.end(), new_higher); 
	result.splice(result.begin(), new_lower.get()); 
	return result;
}

int main()
{
	int i;
	std::cout << "Hello World" << std::endl;

	int&& j = 25;
	
	Test test;

	TestAdd testAdd;
	test.Add_function(std::bind(&TestAdd::Add, testAdd, std::placeholders::_1, std::placeholders::_2), 1, 2);
	test.Add_pointer(TestAdd::Add_s, 2, 3);

	auto add = [](int a, int b)->int 
	{
		 return a + b;
	};
	test.Add_function(add, 3, 5);

	vector<int> testVec{ 1,2,3,4 };
	testFunc(std::move(testVec));

	cout << testVec.size() << endl;

	xFunc(testClxFunc);

	ClxBase* pBase = new ClxDerived;
	pBase->DoSomething();
	delete pBase;

	ClxDerived* derived = new ClxDerived;
	derived->DoSomething();
	delete derived;

	thread th1(PrintPromise);
	prom.set_value(100);
	th1.join();

	std::list<int> sortList{5,7,3,4,1,9,2,8};

	std::cin >> i;
    return 0;
}

