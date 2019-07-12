#pragma once
namespace HL
{
	namespace System
	{
		namespace Threading
		{
			namespace Tasks
			{
				class WaitTimeOutException :public Exception::IException
				{
				public:
					WaitTimeOutException() :Exception::IException(L"WaitTimeOut") {}
				};

				template<class U>
				class AsyncResult
				{
					std::atomic<bool> m_ready;
					std::atomic<bool> m_set_once;
					ConditionVariable m_waiter;
					Lock::CriticalSection m_lock;
					U m_result;
				public:
					template<typename = std::enable_if_t<std::is_copy_constructible_v<U>,int>>
					AsyncResult(U const&default_value) noexcept(std::is_nothrow_copy_constructible_v<U>)
						:m_result(default_value) {
						m_ready.store(false, std::memory_order_relaxed);
						m_set_once.store(false, std::memory_order_relaxed);
					}
					template<typename = std::enable_if_t<std::is_move_constructible_v<U>, int>>
					AsyncResult(U &&default_value) noexcept(std::is_nothrow_move_constructible_v<U>)
						:m_result(default_value) {
						m_ready.store(false, std::memory_order_relaxed);
						m_set_once.store(false, std::memory_order_relaxed);
					}
					template<class...Args , typename = std::enable_if_t<std::is_constructible_v<U, Args...>, int >>
					AsyncResult(Args&&...args) noexcept(std::is_nothrow_constructible_v<U, Args...>)
						:m_result(std::forward<Args>(args)...) {
						m_ready.store(false, std::memory_order_relaxed);
						m_set_once.store(false, std::memory_order_relaxed);
					}
					template<typename = std::enable_if_t<std::is_default_constructible_v<U>,int>>
					AsyncResult() noexcept(std::is_nothrow_default_constructible_v<U>)
						:m_result() {
						m_ready.store(false, std::memory_order_relaxed);
						m_set_once.store(false, std::memory_order_relaxed);
					}
					AsyncResult(AsyncResult&&) = delete;
					AsyncResult(AsyncResult const&) = delete;
					AsyncResult& operator=(AsyncResult&&) = delete;
					AsyncResult& operator=(AsyncResult const&) = delete;
					~AsyncResult() = default;				
					inline U& Await()noexcept {
						if (!IsReady())
							m_waiter.Wait(m_lock);
						return m_result;
					}
					inline U& Await(Span miliseconds) {
						if (!IsReady())
						{
							m_waiter.Wait(m_lock, miliseconds);
							if (!IsReady())
								Exception::Throw<WaitTimeOutException>();
						}
						return m_result;
					}
					inline bool IsReady()const noexcept {
						return m_ready.load(std::memory_order_acquire);
					}
					void SetValue(U const&result)
					{
						if (!m_set_once.load(std::memory_order_acquire))
							m_set_once.store(true, std::memory_order_release);
						else
							return;
						m_result = result;
						m_ready.store(true, std::memory_order_release);
						m_waiter.WakeUpAll();
					}
					void SetValue(U &&result)
					{
						if (!m_set_once.load(std::memory_order_acquire))
							m_set_once.store(true, std::memory_order_release);
						else
							return;
						m_result = std::move(result);
						m_ready.store(true, std::memory_order_release);
						m_waiter.WakeUpAll();
					}
				};

				template<class T>
				class Task
				{
					Pointer::ptr<Thread> m_thread;
					AsyncResult<T> m_result;
					template<class U,class TaskT,class...Args>
					inline std::enable_if_t<!std::is_same_v<U, void_t>,void> Execute(TaskT&&task, Args&&...args) {
						m_result.SetValue(std::move(std::forward<TaskT>(task)(std::forward<Args>(args)...)));
					}
					template<class U,class TaskT, class...Args>
					inline std::enable_if_t<std::is_same_v<U, void_t>> Execute(TaskT&&task, Args&&...args) {
						std::forward<TaskT>(task)(std::forward<Args>(args)...);
						m_result.SetValue(void_t());
					}
				public:
					Task() = default;
					Task(Task const&) = delete;
					Task(Task&&) = delete;
					Task& operator=(Task const&) = delete;
					Task& operator=(Task&&) = delete;
					~Task() {
						m_thread->Join();
					}
					template<class TaskT,class...Args>
					void Run(TaskT&&task, Args&&...args)
					{
						m_thread = GC::gcnew<Thread>([=]() {Execute<T>(task, args...); });
						m_thread->Go();
					}
					inline T& Await() {
						return m_result.Await();
					}
					template<class TaskT,class...Args>
					Pointer::ptr<Task<std::conditional_t<std::is_same_v<void, Utility::callable_return_t<TaskT, Args...>>, void_t, Utility::callable_return_t<TaskT, Args...>>>>
						Then(TaskT&& task, Args&& ...args) {
						typedef Task<std::conditional_t<std::is_same_v<void, Utility::callable_return_t<TaskT, Args...>>, void_t, Utility::callable_return_t<TaskT, Args...>>> task_t;
						Await();
						Pointer::ptr<task_t> ret = std::move(Pointer::Reference::newptr<task_t>());
						ret->Run(std::forward<TaskT>(task), std::forward<Args>(args)...);
						return std::move(ret);
					}
				};

				template<class T>
				class Task<T&>
				{
					Pointer::ptr<Thread> m_thread;
					AsyncResult<T*> m_result;
				public:
					Task(Task const&) = delete;
					Task(Task&&) = delete;
					Task& operator=(Task const&) = delete;
					Task& operator=(Task&&) = delete;
					~Task() {
						m_thread->Join();
					}
					template<class TaskT, class...Args>
					void Run(TaskT&&task, Args&&...args)
					{
						m_thread = GC::gcnew<Thread>([=]() {
							m_result.SetValue(&task(args...));
						});
						m_thread->Go();
					}
					inline T& Await() {
						return *m_result.Await();
					}
					template<class TaskT, class...Args>
					Pointer::ptr<Task<std::conditional_t<std::is_same_v<void, Utility::callable_return_t<TaskT, Args...>>, void_t, Utility::callable_return_t<TaskT, Args...>>>>
						Then(TaskT&& task, Args&& ...args) {
						typedef Task<std::conditional_t<std::is_same_v<void, Utility::callable_return_t<TaskT, Args...>>, void_t, Utility::callable_return_t<TaskT, Args...>>> task_t;
						Await();
						Pointer::ptr<task_t> ret = std::move(Pointer::Reference::newptr<task_t>());
						ret->Run(std::forward<TaskT>(task), std::forward<Args>(args)...);
						return std::move(ret);
					}
				}; 

				template<class TaskT,class...Args>
				Pointer::ptr<Task<std::conditional_t<std::is_same_v<void,Utility::callable_return_t<TaskT,Args...>>,void_t, Utility::callable_return_t<TaskT, Args...>>>>
					Async(TaskT&&task, Args&&...args) {
					typedef Task<std::conditional_t<std::is_same_v<void, Utility::callable_return_t<TaskT, Args...>>, void_t, Utility::callable_return_t<TaskT, Args...>>> task_t;
					Pointer::ptr<task_t> ret = std::move(Pointer::Reference::newptr<task_t>());
					ret->Run(std::forward<TaskT>(task), std::forward<Args>(args)...);
					return std::move(ret);
				}
				template<class EnumerableT>
				Pointer::ptr<Task<void_t>> WhenAll(EnumerableT&& range) {
					Pointer::ptr<Task<void_t>> ret = Pointer::Reference::newptr<Task<void_t>>();
					ret->Run([=]() {for (auto&& task : range) task.Await(); });
					return ret;
				}
				template<class IteratorT>
				Pointer::ptr<Task<void_t>> WhenAll(IteratorT&& begin, IteratorT&& end) {
					Pointer::ptr<Task<void_t>> ret = Pointer::Reference::newptr<Task<void_t>>();
					ret->Run([=]() {for (IteratorT iter = begin; iter != end; ++iter)(*iter).Await(); });
					return ret;
				}
			} 
		}
	}
}