#pragma once
namespace HL::System::Runtime::Descriptor
{
	enum class MethodDescriptorType
	{
		IL = 0x00,
		Native = 0x01,
		Interface = 0x10
	};

	class MethodDescriptor
	{
		Int8* m_entrance;
		Int16 m_index;
		Int32 m_flag;
		static constexpr Int32 MDTypeMask      = 0x0000000F;
		static constexpr Int32 MethodTokenMask = 0x000FFFF0;
		static constexpr Int32 TypeIndexMask   = 0x0FF00000;
		static constexpr Int32 GenericMask     = 0xF0000000;
	public:
		MethodDescriptor(MethodDescriptorType type,
							Int16 index,
							Int8* entrance,
							Int16 methodToken,
							Int8 typeIndex,
							bool generic) {
			m_entrance = type == MethodDescriptorType::Interface ? nullptr : entrance;
			m_index = index;
			m_flag = (Int8)type | (methodToken << 4) | (typeIndex << 20) | ((Int8)generic << 28);
		}
		inline Int8* GetEntrance()const {
			return m_entrance;
		}
		inline Int16 GetIndexInMethodTable()const {
			return m_index;
		}
		inline Int16 MethodToReferToken()const {
			return (Int16)((m_flag & MethodTokenMask) >> 4);
		}
		inline Int8 TypeToReferIndex()const {
			return (Int8)((m_flag & TypeIndexMask) >> 20);
		}
		inline bool IsGeneric()const {
			return !!m_flag & GenericMask;
		}
	};
}