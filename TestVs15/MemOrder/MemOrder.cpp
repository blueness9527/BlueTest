// MemOrder.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <thread>
#include <atomic>
#include <iostream>

#define MEM_ORDER std::memory_order_relaxed
//#define MEM_ORDER std::memory_order_seq_cst

std::atomic<int> x(0), y(0), z(0);  // 1
std::atomic<bool> go(false);  // 2

unsigned const loop_count = 10;

struct read_values
{
	int x, y, z;
};

read_values values1[loop_count];
read_values values2[loop_count];
read_values values3[loop_count];
read_values values4[loop_count];
read_values values5[loop_count];

void increment(std::atomic<int>* var_to_inc, read_values* values)
{
	while (!go)
		std::this_thread::yield();  // 3 �������ȴ��ź�
	for (unsigned i = 0; i < loop_count; ++i)
	{
		values[i].x = x.load(MEM_ORDER);
		values[i].y = y.load(MEM_ORDER);
		values[i].z = z.load(MEM_ORDER);
		var_to_inc->store(i + 1, MEM_ORDER);  // 4
		std::this_thread::yield();
	}
}

void read_vals(read_values* values)
{
	while (!go)
		std::this_thread::yield(); // 5 �������ȴ��ź�
	for (unsigned i = 0; i < loop_count; ++i)
	{
		values[i].x = x.load(MEM_ORDER);
		values[i].y = y.load(MEM_ORDER);
		values[i].z = z.load(MEM_ORDER);
		std::this_thread::yield();
	}
}

void print(read_values* v)
{
	for (unsigned i = 0; i < loop_count; ++i)
	{
		if (i)
			std::cout << ",";
		std::cout << "(" << v[i].x << "," << v[i].y << "," << v[i].z << ")";
	}
	std::cout << std::endl;
}

int main()
{
	std::thread t1(increment, &x, values1);
	std::thread t2(increment, &y, values2);
	std::thread t3(increment, &z, values3);
	std::thread t4(read_vals, values4);
	std::thread t5(read_vals, values5);

	go = true;  // 6 ��ʼִ����ѭ�����ź�

	t5.join();
	t4.join();
	t3.join();
	t2.join();
	t1.join();

	print(values1);  // 7 ��ӡ���ս��
	print(values2);
	print(values3);
	print(values4);
	print(values5);

	int i = 0;
	std::cin >> i;
}