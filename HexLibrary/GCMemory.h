#pragma once
namespace HL
{
	namespace System
	{
		namespace GC
		{
			struct Block
			{
				MemorySpan* Domain;
				Block* Next;
			};

			struct MemorySpan
			{
				Block* FreeList;
				size_t FreeCnt;
				Threading::Lock::CriticalSection Lock;
			};

			class GCMemory
			{
				bool m_async = true;
				Collection::Generic::Array<MemorySpan> m_spans;
				size_t m_base_byte = 8;
				inline Block* AllocateBlock(size_t byte)noexcept {
					void* memory = Memory::DefaultAllocator::Instance()->Alloc(sizeof(Block) + byte);
					Block* node = (Block*)memory;
					node->Next = nullptr;
					return node;
				}
				inline void LockSpan(MemorySpan& target)noexcept {
					if (m_async)
						target.Lock.Lock();
				}
				inline void UnlockSpan(MemorySpan& target)noexcept {
					if (m_async)
						target.Lock.Unlock();
				}
			public:
				GCMemory(size_t BaseBytes, size_t SpanCnt, size_t CapacityFactor)
					:m_spans(SpanCnt),
					m_base_byte(BaseBytes)
				{
					size_t byte = BaseBytes;
					size_t cnt = CapacityFactor * (m_base_byte << SpanCnt);
					for (size_t i = 0; i < SpanCnt; ++i) {
						for (size_t var = 0; var < cnt; ++var)
						{
							Block* node = AllocateBlock(byte);
							node->Domain = &m_spans[i];
							node->Next = m_spans[i].FreeList;
							m_spans[i].FreeList = node;
						}
						m_spans[i].FreeCnt = cnt;
						byte <<= 1;
						cnt >>= 1;
					}
				}
				~GCMemory() {
					for (auto&&span : m_spans)
					{
						Block* iterator = span.FreeList;
						Block* next = nullptr;
						while (iterator != nullptr)
						{
							next = iterator->Next;
							Memory::DefaultAllocator::Instance()->Collect(iterator);
							iterator = next;
						}
					}
				}
				void* Allocate(size_t Size) {
					size_t index = 0;
					size_t target_byte = m_base_byte;
					for (; index < m_spans.Count() && target_byte < Size; target_byte <<= 1, index++);
					MemorySpan& target = m_spans[index];
					LockSpan(target);
					if (target.FreeCnt == 0)
					{
						UnlockSpan(target);
						Block* node = AllocateBlock(target_byte);
						node->Domain = &target;
						return (Byte*)node + sizeof(Block);
					}
					else
						target.FreeCnt--;
					Block* node = target.FreeList;
					target.FreeList = node->Next;
					UnlockSpan(target);
					return (Byte*)node + sizeof(Block);
				}
				void Collect(void* Target) {
					if (Target == nullptr)
						return;
					Block* node = (Block*)((Byte*)Target - sizeof(Block));
					LockSpan(*node->Domain);
					node->Domain->FreeCnt++;
					node->Next = node->Domain->FreeList;
					node->Domain->FreeList = node;
					UnlockSpan(*node->Domain);
				}
			};
			
			struct Buffer
			{
				MemorySlot* Slot;
				size_t Offset;
				Buffer* Next;
			};

			struct MemorySlot
			{
				size_t Color;
				size_t Cnt;
				void* Memory;
				Buffer* FreeList;
				Buffer* NodeBlock;
				MemorySlot* Next;
				MemorySlot* Prev;
				static void SetSlot(void* memory, MemorySlot* slot, size_t bytes, size_t cnt, size_t color) {
					Byte* base = (Byte*)memory + color;
					slot->Memory = memory;
					for (size_t i = 0; i < cnt; ++i)
					{
						*(uintptr_t*)base = (uintptr_t)&slot->NodeBlock[i];
						slot->NodeBlock[i].Offset = base - (Byte*)memory + sizeof(void*);
						slot->NodeBlock[i].Slot = slot;
						if (i < cnt - 1)
							slot->NodeBlock[i].Next = &slot->NodeBlock[i + 1];
						base += sizeof(void*) + bytes;
					}
					slot->NodeBlock[cnt - 1].Next = nullptr;
					slot->FreeList = slot->NodeBlock;
					slot->Color = color;
					slot->Cnt = cnt;
					slot->Next = nullptr;
					slot->Prev = nullptr;
				}
				static void PendTail(MemorySlot*& head,MemorySlot*& tail, MemorySlot* target) {
					if (head == nullptr&&tail == nullptr)
						head = tail = target;
					else
					{
						tail->Next = target;
						target->Next = nullptr;
						target->Prev = tail;
						tail = target;
					}
				}
				static void PendHead(MemorySlot*&head, MemorySlot*&tail, MemorySlot* target) {
					if (head == nullptr&&tail == nullptr)
						head = tail = target;
					else
					{
						target->Prev = nullptr;
						target->Next = head;
						head->Prev = target;
						head = target;
					}
				}
				static void Resume(MemorySlot*& head, MemorySlot*& tail, MemorySlot* target) {
					if (head == tail || target == tail)
						return;
					TakeSlot(head, tail, target);
					PendTail(head, tail, target);
				}
				static void TakeSlot(MemorySlot*&head, MemorySlot*&tail, MemorySlot* target) {
					if (target->Prev == nullptr)
						head = target->Next;
					else
					{
						target->Prev->Next = target->Next;
						target->Prev = nullptr;
					}
					if (target->Next == nullptr)
						tail = target->Prev;
					else
					{
						target->Next->Prev = target->Prev;
						target->Next = nullptr;
					}
				}

				void* Allocate() {
					void* ret = (Byte*)Memory + FreeList->Offset;
					FreeList = FreeList->Next;
					Cnt--;
					return ret;
				}
			};

			class MemoryPool
			{
				HANDLE m_heap;

				MemorySlot* m_head = nullptr;
				MemorySlot* m_tail = nullptr;

				MemorySlot* m_full_slot_head = nullptr;
				MemorySlot* m_full_slot_tail = nullptr;

				size_t m_each_size = 0;
				size_t m_slot_size = 0;
				size_t m_cnt = 0;
				size_t m_slot_cnt = 0;
				size_t m_align = 0;
				size_t m_max_align = 0;
				Threading::Lock::CriticalSection m_lock;
				MemorySlot* AllocatePage() {
					size_t color = 0;
					if (m_tail != nullptr)
						color = (m_tail->Color + m_align) % m_max_align;
					void* memory = HeapAlloc(m_heap, 0, m_slot_size);
					MemorySlot* slot = (MemorySlot*)((Byte*)memory + m_slot_size - sizeof(MemorySlot));
					slot->NodeBlock = (Buffer*)malloc(sizeof(Buffer)*m_cnt);
					MemorySlot::SetSlot(memory, slot, m_each_size, m_cnt, color);
					return slot;
				}
			public:
				MemoryPool(size_t Bytes, size_t Align, size_t SlotCnt)
					:m_slot_cnt(SlotCnt),
					m_slot_size(4096),
					m_each_size(Bytes),
					m_align(Align) {

					m_heap = HeapCreate(0, m_slot_size*SlotCnt, 0);
					size_t block_size = sizeof(void*) + Bytes;
					//³¢ÊÔ¼ÆËã×ÅÉ«
					size_t cnt = (m_slot_size - sizeof(MemorySlot)) / block_size;
					size_t available = m_slot_size - cnt * block_size - sizeof(MemorySlot);
					size_t max_color = 0;
					if (available < Align * 8)
					{
						available = Align * 8;
						cnt = (m_slot_size - sizeof(MemorySlot) - available) / block_size;
					}
					else
						available = (available / Align) * Align;
					m_cnt = cnt;
					m_max_align = available;

					for (size_t i = 0; i < SlotCnt; ++i)
						MemorySlot::PendTail(m_head, m_tail, AllocatePage());
				}

				void* Allocate() {
					m_lock.Lock();
					MemorySlot* iterator = m_head;
					while (iterator != nullptr) {
						if (iterator->FreeList != nullptr)
						{
							void* ptr = iterator->Allocate();
							m_lock.Unlock();
							return ptr;
						}
						else
						{
							MemorySlot* target = iterator;
							iterator = iterator->Next;
							MemorySlot::TakeSlot(m_head, m_tail, target);
							MemorySlot::PendTail(m_full_slot_head, m_full_slot_tail, target);
						}
					}
					void* ptr = nullptr;
					MemorySlot* slot = AllocatePage();
					MemorySlot::PendHead(m_head, m_tail, slot);
					ptr = slot->Allocate();
					m_lock.Unlock();
					return ptr;
				}

				void Collect(void* Target) {
					Buffer* target = *(Buffer**)((Byte*)Target - sizeof(void*));
					m_lock.Lock();
					target->Next = target->Slot->FreeList;
					target->Slot->FreeList = target;
					target->Slot->Cnt++;
					if (target->Slot->Cnt == 1)
					{
						MemorySlot::TakeSlot(m_full_slot_head, m_full_slot_tail, target->Slot);
						MemorySlot::PendTail(m_head, m_tail, target->Slot);
					}
					else if (target->Slot->Cnt == m_cnt)
						MemorySlot::Resume(m_head, m_tail, target->Slot);
					m_lock.Unlock();
				}

				void ShrinkPages(size_t ReservedPageCnt) {
					m_lock.Lock();

					m_lock.Unlock();
				}

				~MemoryPool() {
					if (m_heap != nullptr)
					{
						m_lock.Lock();
						MemorySlot* iterator = m_head;
						MemorySlot* next = nullptr;
						while (iterator != nullptr)
						{
							next = iterator->Next;
							Memory::DefaultAllocator::Instance()->Collect(iterator->NodeBlock);
							HeapFree(m_heap, 0, iterator->Memory);
							iterator = next;
						}
						HeapDestroy(m_heap);
						m_head = m_tail = nullptr;
						m_heap = nullptr;
						m_lock.Unlock();
					}
				}
			};
		}
	}
}