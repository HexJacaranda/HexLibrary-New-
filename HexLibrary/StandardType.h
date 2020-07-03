#pragma once
#include "RuntimeAlias.h"
namespace RTC
{
    enum class CoreType :UInt8
    {
        //Primitive Type

        None = 0x00,
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

        //Composite Type

        Pointer = 0x0F,
        ByReference = 0x10,

        //General Type

        ValueType = 0x11,
        ReferenceType = 0x12,
        FunctionPointer = 0x13,


        //Short cuts for common internal types
        
        Object = 0x14,
        //Generalized Array
        Array = 0x15,
        //Single dimension, zero-based Array
        SZArray = 0x16
    };
}