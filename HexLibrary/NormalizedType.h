#pragma once
namespace HL::System::Runtime::Core
{
    enum class ElementType
    {
        End = 0x00,
        Void = 0x01,
        Boolean = 0x02,
        Char = 0x03,
        Int8 = 0x04,
        UInt8 = 0x05,
        Int16 = 0x06,
        UInt16 = 0x07,
        Int32 = 0x08,
        UInt32 = 0x09,
        Int64 = 0x0A,
        UInt64 = 0x0B,
        Float = 0x0C,
        Double = 0x0D,
        String = 0x0E,

        Pointer = 0x0F,
        ByReference = 0x10,

        ValueType = 0x11,
        Class = 0x12,
        Variable = 0x13,
        Array = 0x14,
        Generic = 0x15,
        TypedByReference = 0x16,

        NativeInt = 0x18,
        NativeUInt = 0x19,
        FunctionPointer = 0x1B,
        Object = 0x1C,// System.Object
        SZArray = 0x1D
	};
}