#pragma once
namespace HL
{
	namespace System
	{
		namespace GC
		{
			struct GCObject
			{
				typedef void(*DestructorT)(void*);
				bool Reachable;
				void* ObjectPtr;
				DestructorT Destructor;
				template<class U>
				static void DestructFunction(void*Ptr) {
					delete static_cast<U*>(Ptr);
				}
			};

			struct GCHandle
			{
				GCObject* Reference;
				std::atomic<bool> IsOnReferenece;
			};

			struct Generation
			{
				Collection::Generic::List<GCObject*> ObjectTable;
				Threading::Lock::CriticalSection Lock;
				void Collect()
				{
					Collection::Generic::List<GCObject*> survivied_objects;
					for (GCObject*& var : ObjectTable)
					{
						if (var->Reachable)
						{
							survivied_objects.Add(var);
							var->Reachable = false;
						}
						else
						{
							var->Destructor(var->ObjectPtr);
							Memory::DefaultAllocator::Instance()->Collect(var);
						}
					}
					ObjectTable = std::move(survivied_objects);
				}
				GCObject* AllocateObject() {
					GCObject* ret = (GCObject*)Memory::DefaultAllocator::Instance()->Alloc(sizeof(GCObject));
					ret->Reachable = true;
					Lock.Lock();
					ObjectTable.Add(ret);
					Lock.Unlock();
					return ret;
				}
			};

			enum class GCStatus
			{
				Idle,
				OnRegularCollecting,
				OnOnceCollecting,
				Exiting
			};

			class GarbageCollector
			{
				std::atomic<bool> m_changed;
				Generation m_generation;
				Collection::Generic::List<GCHandle*> m_handle_table;
				Threading::Lock::SpinLock m_handle_table_lock;
				std::atomic<bool> m_final_singal;

				Collection::Generic::List<Pointer::ptr<Threading::Thread>> m_managed_threads;

				std::atomic<GCStatus> m_status;
				Threading::Thread m_collect_thread;
				size_t m_time_span_cnt;
				Threading::Span m_time_pause;

				void RegularCollect()
				{
					for (;;)
					{
						size_t cnt = 0;
						while (cnt < m_time_span_cnt)
						{
							if (m_final_singal.load(std::memory_order_acquire))
								return;
							Threading::Thread::SleepFor(m_time_pause);
							cnt++;
						}
						if (m_status.load(std::memory_order_acquire) == GCStatus::Idle
							&&m_changed.load(std::memory_order_acquire))
							CollectInternal(GCStatus::OnRegularCollecting);
					}
				}
				void OnceCollect()
				{
					if (m_status.load(std::memory_order_acquire) == GCStatus::Idle
						&&m_changed.load(std::memory_order_acquire))
						CollectInternal(GCStatus::OnOnceCollecting);
				}
				void CollectInternal(GCStatus Status)
				{
					m_status.store(Status, std::memory_order_release);
					m_handle_table_lock.Lock();
					m_generation.Lock.Lock();
					for (auto&&thread : m_managed_threads)
						thread->Suspend();
					CollectHandle();
					m_generation.Collect();
					for (auto&&thread : m_managed_threads)
						thread->Resume();
					m_handle_table_lock.Unlock();
					m_generation.Lock.Unlock();
					m_changed.store(false, std::memory_order_release);
					m_status.store(GCStatus::Idle, std::memory_order_release);
				}
				void CollectHandle()
				{
					Collection::Generic::List<GCHandle*> survivied_handles;
					for (GCHandle*& handle : m_handle_table)
					{
						if (handle->IsOnReferenece.load(std::memory_order_acquire))
						{
							//TODO
							if (handle->Reference != nullptr)
								handle->Reference->Reachable = true;
							survivied_handles.Add(handle);
						}
						else
							Memory::DefaultAllocator::Instance()->Collect(handle);
					}
					m_handle_table = std::move(survivied_handles);
				}
			public:
				GarbageCollector()
					:m_collect_thread([this]() {this->RegularCollect(); }),
					m_final_singal(false),
					m_changed(false),
					m_time_span_cnt(200),
					m_time_pause(50),
					m_status(GCStatus::Idle),
					m_handle_table(1u << 4) {
					m_collect_thread.Go();
				}
				GCHandle* AllocateHandle(GCObject* Target)
				{
					GCHandle* handle = (GCHandle*)Memory::DefaultAllocator::Instance()->Alloc(sizeof(GCHandle));
					handle->IsOnReferenece.store(true, std::memory_order_release);
					handle->Reference = Target;
					m_handle_table_lock.Lock();
					m_handle_table.Add(handle);
					m_handle_table_lock.Unlock();
					m_changed.store(true, std::memory_order_release);
					return handle;
				}
				template<class U>
				GCObject* AddObject(void* ObjectPtr)
				{
					GCObject* ret = m_generation.AllocateObject();
					ret->ObjectPtr = ObjectPtr;
					ret->Destructor = &GCObject::DestructFunction<U>;
					m_changed.store(true, std::memory_order_release);
					return ret;
				}
				inline void Collect()
				{
					OnceCollect();
				}
				~GarbageCollector()
				{
					if (m_final_singal.load(std::memory_order_acquire))
						return;
					m_final_singal.store(true, std::memory_order_release);
					m_handle_table_lock.Lock();
					m_generation.Lock.Lock();
					m_collect_thread.Join();

					for (auto&&var : m_handle_table)
						Memory::DefaultAllocator::Instance()->Collect(var);
					for (auto&&var : m_generation.ObjectTable)
					{
						var->Destructor(var->ObjectPtr);
						Memory::DefaultAllocator::Instance()->Collect(var);
					}
					m_generation.Lock.Unlock();
					m_handle_table_lock.Unlock();
				}
			};

			static GarbageCollector& GCInstance()
			{
				static GarbageCollector GC;
				return GC;
			}
			
			static void* GCRelease(void* Target) {
				if (Target == nullptr)
					return nullptr;
				GCHandle* ptr = static_cast<GCHandle*>(Target);
				ptr->IsOnReferenece.store(false, std::memory_order_release);
				return nullptr;
			}
			static void* GCUpdate(void* Target) {
				GCHandle* ptr = static_cast<GCHandle*>(Target);
				if (ptr == nullptr || !ptr->IsOnReferenece.load(std::memory_order_acquire))
					return nullptr;
				GCHandle* another = GCInstance().AllocateHandle(ptr->Reference);
				return another;
			}
			static bool GCQuery(void* Target) {
				GCHandle* ptr = static_cast<GCHandle*>(Target);
				if (ptr == nullptr)
					return false;
				return ptr->IsOnReferenece.load(std::memory_order_acquire);
			}
			static Pointer::ResourceActions* GCActions() {
				static Pointer::ResourceActions actions = { &GCRelease,&GCUpdate,&GCQuery,nullptr };
				return &actions;
			}

			template<class U, class...Args>
			Pointer::ptr<U> gcnew(Args&& ...args) {
				U* ptr = new U(std::forward<Args>(args)...);
				GCObject* object = GCInstance().AddObject<U>(ptr);
				GCHandle* handle = GCInstance().AllocateHandle(object);
				object->Reachable = false;
				return Pointer::ptr<U>(handle, ptr, GCActions());
			}
		}
	}
}