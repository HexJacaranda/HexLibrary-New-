#pragma once
namespace HL
{
	namespace System
	{
		namespace Threading
		{
			struct ThreadPack
			{
				Delegation::Delegate<void> delegate;
				Pointer::ptr<Thread> me;
				template<class Fun>
				ThreadPack(Fun&&target) :delegate(std::forward<Fun>(target)) {}
			};

			class Thread: public Pointer::me<Thread>
			{
				NativeThread::Handle m_handle = nullptr;
				ThreadPack m_pack;
			private:
				static void ThreadAgent(void* pack_ptr) {
					ThreadPack* ptr = static_cast<ThreadPack*>(pack_ptr);
					ptr->delegate();
					ptr->me.SetToNull();
				}
			public:
				Thread(Thread&&)noexcept = default;
				Thread(Thread const&) = delete;
				Thread& operator=(Thread&&) = default;
				Thread& operator=(Thread const&) = delete;
				template<class StarterT>
				Thread(StarterT&& target) :m_pack(std::forward<StarterT>(target)) {}
				~Thread() = default;
				inline void Go()noexcept {
					m_pack.me = This.Lock();
					m_handle = NativeThread::StartNativeThread(ThreadAgent, 0, &m_pack);
				}
				inline void Join()const noexcept {
					NativeThread::WaitFor(m_handle, NativeThread::Infinite);
				}
				inline void WaitFor(Span Miliseconds)const noexcept {
					NativeThread::WaitFor(m_handle, Miliseconds);
				}
				inline void Suspend()const noexcept {
					NativeThread::Suspend(m_handle);
				}
				inline void Resume()const noexcept {
					NativeThread::Resume(m_handle);
				}
				static void SleepFor(Span Miliseconds)noexcept {
					NativeThread::SleepFor(Miliseconds);
				}
			};
		}
	}
}