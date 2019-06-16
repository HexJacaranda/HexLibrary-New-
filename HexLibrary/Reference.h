#pragma once
namespace HL
{
	namespace System
	{
		namespace Pointer
		{
			namespace Reference
			{
				struct ReferenceResource
				{
					typedef void(*DestructorT)(void*);
					size_t ReferenceCount = 0;
					void* Ptr = nullptr;
					DestructorT Destruct = nullptr;
					template<class T>
					static inline void Destructor(void*Ptr) {
						delete static_cast<T*>(Ptr);
					}
				};
				static void* ReferenceRelease(void*Target) {
					ReferenceResource*ptr = static_cast<ReferenceResource*>(Target);
					if (ptr->ReferenceCount > 1)
					{
						ptr->ReferenceCount--;
						return Target;
					}
					else
					{
						ptr->Destruct(ptr->Ptr);
						delete ptr;
						return nullptr;
					}
				}
				static void* ReferenceUpdate(void*Target) {
					ReferenceResource*ptr = static_cast<ReferenceResource*>(Target);
					if (ptr == nullptr)
						return nullptr;
					ptr->ReferenceCount++;
					return Target;
				}
				static bool ReferenceQuery(void*Target) {
					ReferenceResource*ptr = static_cast<ReferenceResource*>(Target);
					if (ptr == nullptr)
						return false;
					return ptr->ReferenceCount != 0;
				}

				static ResourceActions* RefernceActions() {
					static ResourceActions actions = { &ReferenceRelease,&ReferenceUpdate,&ReferenceQuery,nullptr };
					return &actions;
				}

				template<class U,class...Args>
				static ptr<U> newptr(Args&&...args) {
					U*object_ptr = new U(std::forward<Args>(args)...);
					ReferenceResource* resource = new ReferenceResource;
					resource->ReferenceCount = 1;
					resource->Ptr = object_ptr;
					resource->Destruct = &ReferenceResource::Destructor<U>;
					return { resource,object_ptr,RefernceActions() };
				}

				static inline void* NativeReferenceRelease(void*)noexcept { return nullptr; }
				static inline void* NativeReferenceUpdate(void*)noexcept { return nullptr; }

				static ResourceActions* NativeReferenceActions() {
					static ResourceActions actions = { &NativeReferenceRelease ,&NativeReferenceUpdate ,nullptr,nullptr };
					return &actions;
				}

				template<class U>
				inline static ptr<U> ref(U &reference) {
					return { nullptr,&reference,NativeReferenceActions() };
				}

			}
		}
	}
}