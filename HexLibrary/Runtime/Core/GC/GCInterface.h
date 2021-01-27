#pragma once
#include "..\..\RuntimeAlias.h"
#include "..\Object\Object.h"
namespace RTGC
{
	RTO::ObjectRef Allocate(UInt32 TotalSize);
	RTO::ObjectRef AllocateOnLOH(UInt32 TotalSize);
}