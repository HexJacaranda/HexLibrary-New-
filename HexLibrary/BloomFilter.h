#pragma once
#include <cmath>
namespace HL
{
	namespace System
	{
		namespace Collection
		{
			namespace BloomFilter
			{
				inline size_t EstimateMinimalBitCnt(size_t ElementCnt, double MaxFalsePosibility)noexcept {
					return (size_t)(ElementCnt * std::log2(1 / MaxFalsePosibility));
				}

				template<class LeftT,class RightT>
				class HashCombinator
				{
					LeftT m_left;
					RightT m_right;
				public:
					template<typename = std::enable_if_t<
						std::conjunction_v<
						std::is_copy_constructible<LeftT>,
						std::is_copy_constructible<RightT>>, int>>
						HashCombinator(LeftT const&left, RightT const&right) noexcept(std::conjunction_v<
							std::is_nothrow_copy_constructible<LeftT>,
							std::is_nothrow_copy_constructible<RightT>>)
						:m_left(left), m_right(right) {}
					template<typename = std::enable_if_t<
						std::conjunction_v<
						std::is_move_constructible<LeftT>,
						std::is_move_constructible<RightT>>, int>>
						HashCombinator(LeftT &&left, RightT &&right) noexcept(std::conjunction_v<
							std::is_nothrow_move_constructible<LeftT>,
							std::is_nothrow_move_constructible<RightT>>)
						:m_left(std::move(left)), m_right(std::move(right)) {}
					HashCombinator(HashCombinator const&) = default;
					HashCombinator(HashCombinator&&) = default;
					HashCombinator&operator=(HashCombinator const&) = default;
					HashCombinator&operator=(HashCombinator&&) = default;
					~HashCombinator() = default;
					template<class T>
					inline auto operator()(size_t index, T&& input)const
						->decltype(m_left(std::forward<T>(input)) + std::declval<size_t>()*m_right(std::forward<T>(input))) {
						return m_left(std::forward<T>(input)) + index * m_right(std::forward<T>(input));
					}
					template<class T>
					inline auto operator()(size_t index, T&& input)
						->decltype(m_left(std::forward<T>(input)) + std::declval<size_t>()*m_right(std::forward<T>(input))) {
						return m_left(std::forward<T>(input)) + index * m_right(std::forward<T>(input));
					}
				};
				
				template<class LeftT,class RightT>
				inline HashCombinator<LeftT, RightT> Combine(LeftT&& left, RightT&& right) {
					return { std::forward<LeftT>(left),std::forward<RightT>(right) };
				}

				template<class CombinatorT>
				class StandardBloomFilter
				{
					CombinatorT m_combinator;
					size_t m_bitmap_cnt = 0;
					size_t m_hash_function_cnt = 0;
					size_t m_element_cnt = 0;
					char* m_bitmap = nullptr;
					inline void SetBit(size_t index) {
						size_t bit = index % 8;
						index >>= 3;
						m_bitmap[index] |= 1 << bit;
					}
					inline bool ReadBit(size_t index)const {
						size_t bit = index % 8;
						index >>= 3;
						return (m_bitmap[index] >> bit) & 1;
					}
					void Construct(size_t ElementCnt, size_t BitCnt) {
						m_bitmap_cnt = BitCnt;
						m_element_cnt = ElementCnt;
						m_bitmap = (char*)Memory::DefaultAllocator::Instance()->Alloc(sizeof(bool)*((m_bitmap_cnt >> 3) + 1));
						Memory::ZeroMemory(m_bitmap, (m_bitmap_cnt >> 3) + 1);
						m_hash_function_cnt = (m_bitmap_cnt * std::log(2)) / m_element_cnt;
						if (m_hash_function_cnt == 0)
							m_hash_function_cnt++;
					}
				public:
					template<typename = std::enable_if_t<
						std::is_copy_constructible_v<CombinatorT>, int>>
						StandardBloomFilter(CombinatorT const& combinator,size_t cnt,size_t bitcnt)
						noexcept(std::is_nothrow_copy_constructible_v<CombinatorT>)
						:m_combinator(combinator) {
						Construct(cnt, bitcnt);
					}
					template<typename = std::enable_if_t<
						std::is_copy_constructible_v<CombinatorT>, int>>
						StandardBloomFilter(CombinatorT && combinator,size_t cnt,size_t bitcnt)
						noexcept(std::is_nothrow_copy_constructible_v<CombinatorT>)
						:m_combinator(std::move(combinator)) {
						Construct(cnt, bitcnt);
					}
					StandardBloomFilter(StandardBloomFilter&&lhs) :
						m_combinator(std::move(lhs.m_combinator)),
						m_bitmap_cnt(lhs.m_bitmap_cnt),
						m_element_cnt(lhs.m_element_cnt),
						m_hash_function_cnt(lhs.m_hash_function_cnt),
						m_bitmap(lhs.m_bitmap) {
						lhs.m_bitmap = nullptr;
					}
					StandardBloomFilter(StandardBloomFilter const&rhs) :
						m_combinator(rhs.m_combinator),
						m_bitmap_cnt(rhs.m_bitmap_cnt),
						m_element_cnt(rhs.m_element_cnt),
						m_hash_function_cnt(rhs.m_hash_function_cnt)
					{
						if (rhs.m_bitmap != nullptr)
						{
							m_bitmap = (char*)Memory::DefaultAllocator::Instance()->Alloc(sizeof(bool)*((m_bitmap_cnt >> 3) + 1));
							Memory::MemoryCopy(m_bitmap, rhs.m_bitmap, sizeof(bool)*((m_bitmap_cnt >> 3) + 1));
						}
					}
					StandardBloomFilter& operator=(StandardBloomFilter const&rhs) {
						m_combinator = rhs.m_combinator;
						m_bitmap_cnt = rhs.m_bitmap_cnt;
						m_element_cnt = rhs.m_element_cnt;
						m_hash_function_cnt = rhs.m_hash_function_cnt;
						Memory::DefaultAllocator::Instance()->Collect(m_bitmap);
						m_bitmap = nullptr;
						if (rhs.m_bitmap != nullptr)
						{
							m_bitmap = (char*)Memory::DefaultAllocator::Instance()->Alloc(sizeof(bool)*((m_bitmap_cnt >> 3) + 1));
							Memory::MemoryCopy(m_bitmap, rhs.m_bitmap, sizeof(bool)*((m_bitmap_cnt >> 3) + 1));
						}
						return *this;
					}
					StandardBloomFilter& operator=(StandardBloomFilter &&lhs) {
						m_combinator = lhs.m_combinator;
						m_bitmap_cnt = lhs.m_bitmap_cnt;
						m_element_cnt = lhs.m_element_cnt;
						m_hash_function_cnt = lhs.m_hash_function_cnt;
						Memory::DefaultAllocator::Instance()->Collect(m_bitmap);
						m_bitmap = lhs.m_bitmap;
						lhs.m_bitmap = nullptr;
						return *this;
					}
					~StandardBloomFilter() {
						if (m_bitmap != nullptr) {
							Memory::DefaultAllocator::Instance()->Collect(m_bitmap);
							m_bitmap = nullptr;
						}
					}
					template<class T>
					inline void Put(T&& object) {
						for (size_t i = 0; i < m_hash_function_cnt; ++i)
							SetBit(m_combinator(i, std::forward<T>(object)) % m_bitmap_cnt);
					}
					template<class T>
					inline bool Exists(T&& object)const {
						for (size_t i = 0; i < m_hash_function_cnt; ++i)
							if (!ReadBit(m_combinator(i, std::forward<T>(object)) % m_bitmap_cnt))
								return false;
						return true;
					}
				};

				template<class CombinatorT>
				inline StandardBloomFilter<CombinatorT> SBF(CombinatorT && combinator, size_t ElementCnt, size_t BitCnt) {
					return { std::forward<CombinatorT>(combinator),ElementCnt,BitCnt };
				}

				struct Unit {
					unsigned int First : 4;
					unsigned int Second : 4;
				};

				struct CntPair
				{
					unsigned int First : 4;
					unsigned int Second : 4;
					unsigned int Alignment : (alignof(Unit) - 1) * 8;
				};

				template<class CombinatorT>
				class CountBloomFilter
				{
					CombinatorT m_combinator;
					void* m_bitmap;
					size_t m_bitmap_cnt = 0;
					size_t m_hash_function_cnt = 0;
					size_t m_element_cnt = 0;
					void Construct(size_t ElementCnt, size_t BitCnt) {
						m_bitmap_cnt = BitCnt;
						m_element_cnt = ElementCnt;
						m_bitmap = Memory::DefaultAllocator::Instance()->Alloc((m_bitmap_cnt >> 1) + 1);
						Memory::ZeroMemory(m_bitmap, (m_bitmap_cnt >> 1) + 1);
						m_hash_function_cnt = (m_bitmap_cnt * std::log(2)) / m_element_cnt;
						if (m_hash_function_cnt == 0)
							m_hash_function_cnt++;
					}
					inline bool ReadBit(size_t index)const {
						CntPair* target = (CntPair*)((Byte*)m_bitmap + (index >> 1));
						return index & 1u ? target->Second : target->First;
					}
					inline void SetBit(size_t index) {
						CntPair* target = (CntPair*)((Byte*)m_bitmap + (index >> 1));
						index & 1u ? target->Second++ : target->First++;
					}
					inline void CleanBit(size_t index) {
						CntPair* target = (CntPair*)((Byte*)m_bitmap + (index >> 1));
						index & 1u ? target->Second-- : target->First--;
					}
				public:
					template<typename = std::enable_if_t<
						std::is_copy_constructible_v<CombinatorT>, int>>
						CountBloomFilter(CombinatorT const& combinator, size_t cnt, size_t bitcnt)
						noexcept(std::is_nothrow_copy_constructible_v<CombinatorT>)
						:m_combinator(combinator) {
						Construct(cnt, bitcnt);
					}
					template<typename = std::enable_if_t<
						std::is_copy_constructible_v<CombinatorT>, int>>
						CountBloomFilter(CombinatorT && combinator, size_t cnt, size_t bitcnt)
						noexcept(std::is_nothrow_copy_constructible_v<CombinatorT>)
						:m_combinator(std::move(combinator)) {
						Construct(cnt, bitcnt);
					}
					CountBloomFilter(CountBloomFilter &&lhs) :
						m_combinator(std::move(lhs.m_combinator)),
						m_bitmap_cnt(lhs.m_bitmap_cnt),
						m_element_cnt(lhs.m_element_cnt),
						m_hash_function_cnt(lhs.m_hash_function_cnt),
						m_bitmap(lhs.m_bitmap) {
						lhs.m_bitmap = nullptr;
					}
					CountBloomFilter(CountBloomFilter const&rhs) :
						m_combinator(rhs.m_combinator),
						m_bitmap_cnt(rhs.m_bitmap_cnt),
						m_element_cnt(rhs.m_element_cnt),
						m_hash_function_cnt(rhs.m_hash_function_cnt)
					{
						if (rhs.m_bitmap != nullptr)
						{
							m_bitmap = Memory::DefaultAllocator::Instance()->Alloc(sizeof(bool)*((m_bitmap_cnt >> 1) + 1));
							Memory::MemoryCopy(m_bitmap, rhs.m_bitmap, sizeof(bool)*((m_bitmap_cnt >> 1) + 1));
						}
					}
					CountBloomFilter& operator=(CountBloomFilter const&rhs) {
						m_combinator = rhs.m_combinator;
						m_bitmap_cnt = rhs.m_bitmap_cnt;
						m_element_cnt = rhs.m_element_cnt;
						m_hash_function_cnt = rhs.m_hash_function_cnt;
						Memory::DefaultAllocator::Instance()->Collect(m_bitmap);
						m_bitmap = nullptr;
						if (rhs.m_bitmap != nullptr)
						{
							m_bitmap = Memory::DefaultAllocator::Instance()->Alloc(sizeof(bool)*((m_bitmap_cnt >> 1) + 1));
							Memory::MemoryCopy(m_bitmap, rhs.m_bitmap, sizeof(bool)*((m_bitmap_cnt >> 1) + 1));
						}
						return *this;
					}
					CountBloomFilter& operator=(CountBloomFilter &&lhs) {
						m_combinator = lhs.m_combinator;
						m_bitmap_cnt = lhs.m_bitmap_cnt;
						m_element_cnt = lhs.m_element_cnt;
						m_hash_function_cnt = lhs.m_hash_function_cnt;
						Memory::DefaultAllocator::Instance()->Collect(m_bitmap);
						m_bitmap = lhs.m_bitmap;
						lhs.m_bitmap = nullptr;
						return *this;
					}
					~CountBloomFilter() {
						if (m_bitmap != nullptr) {
							Memory::DefaultAllocator::Instance()->Collect(m_bitmap);
							m_bitmap = nullptr;
						}
					}

					template<class T>
					inline void Put(T&& object) {
						for (size_t i = 0; i < m_hash_function_cnt; ++i)
							SetBit(m_combinator(i, std::forward<T>(object)) % m_bitmap_cnt);
					}
					template<class T>
					inline void Erase(T&& object) {
						for (size_t i = 0; i < m_hash_function_cnt; ++i)
							CleanBit(m_combinator(i, std::forward<T>(object)) % m_bitmap_cnt);
					}
					template<class T>
					inline bool Exists(T&& object)const {
						for (size_t i = 0; i < m_hash_function_cnt; ++i)
							if (!ReadBit(m_combinator(i, std::forward<T>(object)) % m_bitmap_cnt))
								return false;
						return true;
					}
				};

				template<class CombinatorT>
				inline CountBloomFilter<CombinatorT> CBF(CombinatorT && combinator, size_t ElementCnt, size_t BitCnt) {
					return { std::forward<CombinatorT>(combinator),ElementCnt,BitCnt };
				}

				struct HashingHelper
				{
					inline static size_t RandomPermutation(size_t mod, size_t value) {
						return (3 * value) % mod;
					}
					inline constexpr static void SetBit(size_t& value, size_t origin, size_t index) {
						value |= ((size_t)1 << index) & origin;
					}
					inline static size_t DthHash(size_t hash, size_t wide, size_t index) {
						size_t ret = 0;
						hash >>= sizeof(size_t) * 8 - (index + 1)*wide;
						for (size_t i = 0; i < wide; ++i)
							SetBit(ret, hash, i);
						return ret;
					}
					static constexpr size_t CounterBitWidth = 4;
				};

				struct Cell
				{
					unsigned int Cnt : HashingHelper::CounterBitWidth;
				};

				struct Bucket
				{
					void* Bit;
					size_t CellCnt;
					size_t TotalCnt;
				};

				struct Table
				{
					Bucket* Buckets;
					size_t BucketCnt;
				};

				template<class HashFunctionT>
				class DLeftHashingBloomFilter
				{
					Table* m_tables = nullptr;
					size_t m_table_cnt = 0;
					size_t m_bits_per_bucket = 0;
					size_t m_index_bit_width = 0;
					size_t m_fingerprint_bit_width = 0;
					size_t m_cell_byte = 0;
					HashFunctionT m_hash_function;
				public:
					DLeftHashingBloomFilter() {
						constexpr size_t total_bits = sizeof(size_t) * 8;
						m_index_bit_width = total_bits / (m_table_cnt + 1);
						m_fingerprint_bit_width = total_bits - m_index_bit_width * m_table_cnt;
					}
					template<class T>
					void Put(T&& target) {
						Bucket* target_bucket = nullptr;
						size_t origin_hash = m_hash_function(std::forward<T>(target));
						for (size_t i = 0; i < m_table_cnt; ++i) {
							size_t hash = HashingHelper::DthHash(origin_hash, m_index_bit_width, i);
							hash = HashingHelper::RandomPermutation(1 >> m_index_bit_width, hash);
							size_t bucket_index = hash % m_tables[i].BucketCnt;
							if (target_bucket == nullptr ||
								m_tables[i].Buckets[bucket_index].TotalCnt < target_bucket->TotalCnt)
								target_bucket = &m_tables[i].Buckets[bucket_index];
						}
						for (size_t i = 0; i < target_bucket->CellCnt; ++i) 
						{
							
						}
					}
				};
			}
		}
	}
}