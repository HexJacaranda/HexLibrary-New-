#include <iostream>
#include <map>
#include "Include.h"

using namespace HL::System::Collection::Generic;
using namespace HL::System::Collection;
using namespace HL::System::Text;
using namespace HL::System::Text::RegularExpression;
using namespace HL::System::Pointer;
using namespace HL::System::Pointer::Reference;
using namespace HL::System;
using namespace HL::System::Threading::Tasks;
using namespace HL::System::GC;

void Main();
int main() {
	Diagnosis::memory::enable_memory_leak_report();
	std::wcout.imbue(std::locale("chs"));
	Main();
	system("pause");
	return 0;
}

void lock_test() {
	int cnt = 0;
	size_t thread_cnt = 8;
	size_t sum_cnt = 10000;
	std::atomic<long long> time;
	time.store(0, std::memory_order_relaxed);
	Threading::Lock::Interlock interlock;
	Threading::Lock::CriticalSection section;
	Collection::Generic::Array<ptr<Task<void_t>>> tasks(thread_cnt);
	for (size_t i = 0; i < thread_cnt; ++i)
		tasks[i] = Threading::Tasks::Async([&]() {
		time.fetch_add(Diagnosis::time::measure_time<std::chrono::milliseconds>(
			[&]() {
			for (size_t var = 0; var < sum_cnt; ++var)
			{
				interlock.Lock();
				cnt++;
				interlock.Unlock();
			}
		}, 1), std::memory_order_acq_rel);
	});
	for (auto&&task : tasks)
		task->Await();
	std::wcout << L"interlock result: " << cnt << std::endl;
	std::wcout << L"interlock time: " << time.load(std::memory_order_acquire) / thread_cnt << L" ms" << std::endl;

	time.store(0, std::memory_order_relaxed);
	for (size_t i = 0; i < thread_cnt; ++i)
		tasks[i] = Threading::Tasks::Async([&]() {
		time.fetch_add(Diagnosis::time::measure_time<std::chrono::milliseconds>(
			[&]() {
			for (size_t var = 0; var < sum_cnt; ++var)
			{
				section.Lock();
				cnt++;
				section.Unlock();
			}
		}, 1), std::memory_order_acq_rel);
	});
	for (auto&&task : tasks)
		task->Await();
	std::wcout << L"critical section result: " << cnt << std::endl;
	std::wcout << L"critical section time: " << time.load(std::memory_order_acquire) / thread_cnt << L" ms" << std::endl;
}

void gc_memory_test() {
	Array<size_t> distribution(8);
	for (auto&&var : distribution)
		var = 10000;
	GC::GCMemory memory(8, 8, 10000);
	constexpr int size = 10000;
	Generic::Array<void*> gc_pool(size);
	Generic::Array<void*> pool(size);

	Generic::Array<int> thunk(size);
	for (auto&&var : thunk)
		var = std::rand() % 256;

	std::wcout << L"GC Allocate: " << Diagnosis::time::measure_time<std::chrono::microseconds>([&]() {
		for (int i = 0; i < size; ++i)
			gc_pool[i] = memory.Allocate(thunk[i]);
	}, 1) << L"¦Ìs" << std::endl;

	std::wcout << L"System Allocate: " << Diagnosis::time::measure_time<std::chrono::microseconds>([&]() {
		for (int i = 0; i < size; ++i)
			pool[i] = new char[thunk[i]];
	}, 1) << L"¦Ìs" << std::endl;

	for (auto&&var : gc_pool)
		memory.Collect(var);
	for (auto&&var : pool)
		delete[] var;
}

void heap_memory_test() {
	constexpr size_t cnt = 1 << 8;
	constexpr size_t each = 128;
	void* malloc_memory[cnt];

	HANDLE heap = GetProcessHeap();
	std::wcout << L"heap alloc time: " << Diagnosis::time::measure_time<std::chrono::milliseconds>([&]() {
		for (size_t i = 0; i < cnt; i++)
			malloc_memory[i] = HeapAlloc(heap, 0, each);
		}, 1) << L"ms" << std::endl;
	for (size_t i = 0; i < cnt; i++)
		HeapFree(heap, 0, malloc_memory[i]);

	std::wcout << L"malloc time: " << Diagnosis::time::measure_time<std::chrono::milliseconds>([&]() {
		for (size_t i = 0; i < cnt; i++)
			malloc_memory[i] = malloc(each);
		}, 1) << L"ms" << std::endl;
	for (int i = 0; i < cnt; i++)
		Memory::DefaultAllocator::Instance()->Collect(malloc_memory[i]);
}

void gc_test() {
	constexpr size_t cnt = 8;
	constexpr size_t thunk_cnt = 10000;
	Generic::Array<ptr<Threading::Tasks::Task<void_t>>> tasks(cnt);
	std::atomic<long long> time;

	for (int k = 0; k < 10; ++k)
	{
		time.store(0, std::memory_order_release);
		for (int i = 0; i < cnt; ++i) {
			tasks[i] = Async([&, i]() {
				time.fetch_add(Diagnosis::time::measure_time<std::chrono::milliseconds>([&, i]() {
					for (size_t var = 0; var < thunk_cnt; ++var)
						GC::gcnew<char>();
				}, 1), std::memory_order_acq_rel);
			});
		}
		for (auto&&var : tasks)
			var->Await();
		std::wcout << L"µÚ" << k + 1 << L"´Î" << L"ºÄÊ±: " <<
			time.load(std::memory_order_acquire) / cnt << L"ms" << std::endl;

		GC::GCInstance().Collect();
		Threading::Thread::SleepFor(500);
	}
}

void concurrent_stack_test() {
	Stack<size_t> stack;
	Threading::Lock::Interlock lock;
	HL::System::Concurrency::ConcurrentStack<int> con_stack;

	size_t thread_cnt = 8;
	size_t sum_cnt = 10000;
	std::atomic<long long> time;
	time.store(0, std::memory_order_relaxed);
	Collection::Generic::Array<ptr<Task<void_t>>> tasks(thread_cnt);
	for (size_t i = 0; i < thread_cnt; ++i)
		tasks[i] = Threading::Tasks::Async([&]() {
		time.fetch_add(Diagnosis::time::measure_time<std::chrono::milliseconds>(
			[&]() {
			for (size_t var = 0; var < sum_cnt; ++var)
			{
				lock.Lock();
				stack.Push(var);
				lock.Unlock();
			}
		}, 1), std::memory_order_acq_rel);
	});
	for (auto&&task : tasks)
		task->Await();
	std::wcout << L"stack + interlock push time: " << time.load(std::memory_order_acquire) / thread_cnt << L" ms" << std::endl;

	time.store(0, std::memory_order_relaxed);
	for (size_t i = 0; i < thread_cnt; ++i)
		tasks[i] = Threading::Tasks::Async([&]() {
		time.fetch_add(Diagnosis::time::measure_time<std::chrono::milliseconds>(
			[&]() {
			for (size_t var = 0; var < sum_cnt; ++var)
				con_stack.Push(var);
		}, 1), std::memory_order_acq_rel);
	});
	for (auto&&task : tasks)
		task->Await();
	std::wcout << L"concurrent stack push time: " << time.load(std::memory_order_acquire) / thread_cnt << L" ms" << std::endl;

}

void Main()
{
	IO::FileHandle handle(L"D:\\JsonSampleData\\data5.txt", IO::FileMode::Read, IO::Encoding::Unicode);
	auto json = IO::File::Read(handle);
	Json::JsonParser parser(json);
	ptr<Json::JsonArray> array;
	std::wcout << Diagnosis::time::measure_time<std::chrono::microseconds>([&]() {
		array = std::move(parser.ParseArray());
		}, 1) << L" ¦Ìs" << std::endl;
	
}