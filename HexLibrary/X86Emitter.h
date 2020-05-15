#pragma once
namespace HL::System::Runtime::JIT::Emit
{
	namespace X86
	{
		class Prefix
		{
		public:
			static constexpr Int8 Register16bit = 0x66;
			static constexpr Int8 Register32bit = 0x67;
			static constexpr Int8 Register64bit = 0x48;
		};

		class Register
		{
		public:
			static constexpr Int8 AX = 0b000;
			static constexpr Int8 CX = 0b001;
			static constexpr Int8 DX = 0b010;
			static constexpr Int8 BX = 0b011;
			static constexpr Int8 SP = 0b100;
			static constexpr Int8 BP = 0b101;
			static constexpr Int8 SI = 0b110;
			static constexpr Int8 DI = 0b111;
			static constexpr Int8 XMM[] = {
				0b000,0b001,0b010,0b011,0b100,0b101,0b110,0b111
			};
			static constexpr Int8 Shift = 3;
			static constexpr Int8 Empty = 0b000;
		};
		
		class AddressingMode
		{
		public:
			static constexpr Int8 RegisterAddressing = 0b00;
			static constexpr Int8 RegisterAddressingWithOffsetOn8bit = 0b01;
			static constexpr Int8 RegisterAddressingWithOffsetOn16Or32bit = 0b10;
			static constexpr Int8 Register = 0b11;
			static constexpr Int8 Shift = 6;
		};

		class Context 
		{
		public:
			static constexpr Int32 PlatformMask = 0b10;
			static void OperandSlice(EmitContext* context, SlotType To) {
				context->EmitState |= (Int8)To;
			}
			static void OperandPromote(EmitContext* context, SlotType To) {
				context->EmitState |= (Int8)To;
			}
			static bool X86(EmitContext* context) {
				return context->CommonFlags & PlatformMask;
			}
		};

		class RegisterOrMemory :public Register
		{
		public:
			//Enables only when it's not AddressingMode::Register
			static constexpr Int8 SIBAddressing = 0b100;
			//Enables only when it's AddressingMode::RegisterAddressing
			static constexpr Int8 Offset32bit = 0b101;
		};

		class Redirect
		{
		public:
			static constexpr Int8 Near = 0;
			static constexpr Int8 Far = 1;		
		};

		static inline constexpr Int8 ModRM(Int8 Mod, Int8 Reg, Int8 RM) {
			return (Mod << AddressingMode::Shift) | (Reg << Register::Shift) | RM;
		}
		static inline constexpr Int8 SIB(Int8 Scale, Int8 Base, Int8 Index) {
			return (Scale << 6) | (Base << 3) | Index;
		}

		class X86OpCodes
		{
			//Imm = immediate
			//R = register
			//V = via
			//M = memory
			//REX = REX prefix
			//4 = for
			//W = with
		public:
			static constexpr Int8 REX[] = { 0,Prefix::Register16bit,0,Prefix::Register64bit };

			static constexpr Int8 MovImmToR[] = { (Int8)0xB0,(Int8)0xB8,(Int8)0xB8,(Int8)0xB8 };

			static constexpr Int8 MovImmToM[] = { (Int8)0xC6,(Int8)0xC7,(Int8)0xC7,(Int8)0xC7 };

			static constexpr Int8 MovRToR[] = { (Int8)0x88,(Int8)0x89,(Int8)0x89,(Int8)0x89 };

			static constexpr Int8 MovRToMVR[] = { (Int8)0x88,(Int8)0x89,(Int8)0x89,(Int8)0x89 };

			static constexpr Int8 MovMToRVImm[] = { (Int8)0xA0,(Int8)0xA1,(Int8)0xA1,(Int8)0xA1 };

			static constexpr Int8 MovRToMVImm[] = { (Int8)0xA2,(Int8)0xA3,(Int8)0xA3,(Int8)0xA3 };

			static constexpr Int8 MovMToRVR[] = { (Int8)0x8A,(Int8)0x8B,(Int8)0x8B,(Int8)0x8B };
			
			static constexpr Int8 AddRToR[] = { (Int8)0x01,(Int8)0x01,(Int8)0x01,(Int8)0x01 };

			static constexpr Int8 AddImmToR[] = { (Int8)0x81,(Int8)0x82,(Int8)0x82,(Int8)0x82 };

			static constexpr Int8 AddImmToR4AX[] = { (Int8)0x04,(Int8)0x05,(Int8)0x05,(Int8)0x05 };

			static constexpr Int8 SubRToR[] = { (Int8)0x28,(Int8)0x29,(Int8)0x29,(Int8)0x29 };

			static constexpr Int8 SubImmToR[] = { (Int8)0x80,(Int8)0x81,(Int8)0x81,(Int8)0x81 };

			static constexpr Int8 SubImmToR4AX[] = { (Int8)0x2C,(Int8)0x2D,(Int8)0x2D,(Int8)0x2D };

			static constexpr Int8 MulRToR4AX[] = { (Int8)0xF6,(Int8)0xF7,(Int8)0xF7,(Int8)0xF7 };

			static constexpr Int16 MulRToR[] = { (Int16)0x0FAF,(Int16)0x0FAF,(Int16)0x0FAF,(Int16)0x0FAF };

			static constexpr Int8 MulImmToR[] = { (Int8)0x69,(Int8)0x69,(Int8)0x69,(Int8)0x69 };

			static constexpr Int8 DivRMToR[] = { (Int8)0xF6,(Int8)0xF7,(Int8)0xF7,(Int8)0xF7 };

			static constexpr Int8 CDQ = (Int8)0x99;

			static constexpr Int8 CmpImmWR4AX[] = { (Int8)0x3C,(Int8)0x3D,(Int8)0x3D,(Int8)0x3D };

			static constexpr Int8 CmpImmWR[] = { (Int8)0x80,(Int8)0x81,(Int8)0x81,(Int8)0x81 };

			static constexpr Int8 CmpMRWR[] = { (Int8)0x3A,(Int8)0x3B,(Int8)0x3B,(Int8)0x3B };

			static constexpr Int8 PopToM[] = { (Int8)0x8F,(Int8)0x8F,(Int8)0x8F,(Int8)0x8F };

			static constexpr Int8 PopToR[] = { (Int8)0x58,(Int8)0x58,(Int8)0x58,(Int8)0x58 };

			static constexpr Int8 PushM[] = { (Int8)0xFF,(Int8)0xFF,(Int8)0xFF,(Int8)0xFF };

			static constexpr Int8 PushR[] = { (Int8)0x50,(Int8)0x50,(Int8)0x50,(Int8)0x50 };

			static constexpr Int8 PushImm[] = { (Int8)0x6A,(Int8)0x68,(Int8)0x68,(Int8)0x68 };

			static constexpr Int8 Ret[] = { (Int8)0xC3,(Int8)0xCB };

			static constexpr Int8 JmpImmRel[] = { (Int8)0xEB,(Int8)0xE9,(Int8)0xE9 ,(Int8)0xEB };

			static constexpr Int8 JmpImmAbs[] = { (Int8)0xEA,(Int8)0xEA,(Int8)0xEA ,(Int8)0xEA };

			static constexpr Int8 JmpRMAbs[] = { (Int8)0xFF,(Int8)0xFF,(Int8)0xFF ,(Int8)0xFF };

			static constexpr Int16 JccImmRel[] = { (Int16)0x0F84,(Int16)0x0F85,(Int16)0x0F8F,
												   (Int16)0x0F8C,(Int16)0x0F8D,(Int16)0x0F8E };

			static constexpr Int8 CallImmRel[] = { (Int8)0xE8,(Int8)0xE8,(Int8)0xE8 ,(Int8)0xE8 };

			static constexpr Int8 CallRMAbs[] = { (Int8)0xFF,(Int8)0xFF,(Int8)0xFF ,(Int8)0xFF };

			static constexpr Int8 CallRMRel[] = { (Int8)0xE8,(Int8)0xE8,(Int8)0xE8 ,(Int8)0xE8 };

			static constexpr Int8 Pause = (Int8)0xCC;

			static constexpr Int8 LShiftBImm[] = { (Int8)0xC0,(Int8)0xC1, (Int8)0xC1, (Int8)0xC1};

			static constexpr Int8 LShiftB1[] = { (Int8)0xD0,(Int8)0xD1, (Int8)0xD1, (Int8)0xD1};

			static constexpr Int8 LShiftBR[] = { (Int8)0xD2,(Int8)0xD3, (Int8)0xD3, (Int8)0xD3 };

			static constexpr Int8 RShiftBImm[] = { (Int8)0xC0,(Int8)0xC1, (Int8)0xC1, (Int8)0xC1 };

			static constexpr Int8 RShiftB1[] = { (Int8)0xD0,(Int8)0xD1, (Int8)0xD1, (Int8)0xD1 };

			static constexpr Int8 RShiftBR[] = { (Int8)0xD2,(Int8)0xD3, (Int8)0xD3, (Int8)0xD3 };

			static constexpr Int8 OrR2R[] = { (Int8)0x08,(Int8)0x09, (Int8)0x09, (Int8)0x09 };

			static constexpr Int8 OrImm2R4AX[] = { (Int8)0x0C,(Int8)0x0D, (Int8)0x0D, (Int8)0x0D };

			static constexpr Int8 OrImm2R[] = { (Int8)0x80,(Int8)0x81, (Int8)0x81, (Int8)0x81 };

			static constexpr Int8 AndR2R[] = { (Int8)0x20,(Int8)0x21, (Int8)0x21, (Int8)0x21 };

			static constexpr Int8 AndImm2R4AX[] = { (Int8)0x24,(Int8)0x25, (Int8)0x25, (Int8)0x25 };

			static constexpr Int8 AndImm2R[] = { (Int8)0x80,(Int8)0x81, (Int8)0x81, (Int8)0x81 };

			static constexpr Int8 XorR2R[] = { (Int8)0x30,(Int8)0x31, (Int8)0x31, (Int8)0x31 };

			static constexpr Int8 XorImm2R4AX[] = { (Int8)0x34,(Int8)0x35, (Int8)0x35, (Int8)0x35 };

			static constexpr Int8 XorImm2R[] = { (Int8)0x80,(Int8)0x81, (Int8)0x81, (Int8)0x81 };

			static constexpr Int8 Not[] = { (Int8)0xF6,(Int8)0xF7, (Int8)0xF7, (Int8)0xF7 };
		};

		enum class AddressFixUpType :Int8
		{
			AbsoluteToRelative,
			RelativeToAbsolute
		};

		struct AddressFixUp
		{
			Int32 Index;
			SlotType Slot;
			AddressFixUpType Type;
			//To indicate the offset from operand to end of the opcode
			Int16 Offset;
		};


		class X86Emitter :public INativeEmitter
		{		
			Collection::Generic::List<AddressFixUp> m_fixups;

			template<class IntT>
			void Write(IntT value) {
				Binary::BinaryHelper::WriteIntByBigEndianess(
					GetExecutablePage()->PrepareWrite(sizeof(IntT)), value);
				GetExecutablePage()->CommitWrite(sizeof(IntT));
			}
			template<class IntT>
			void WriteLittleEndian(IntT value) {
				Binary::BinaryHelper::WriteIntByLittleEndianess(
					GetExecutablePage()->PrepareWrite(sizeof(IntT)), value);
				GetExecutablePage()->CommitWrite(sizeof(IntT));
			}
			void WriteImmediate(SlotType Slot, Int64 Imm)
			{
				switch (Slot)
				{
				case SlotType::Int8:Write((Int8)Imm); break;
				case SlotType::Int16:Write((Int16)Imm); break;
				case SlotType::Int32:
				case SlotType::Float:
					Write((Int32)Imm); break;
				case SlotType::Int64:
				case SlotType::Double:
					Write((Int64)Imm); break;
				}
			}
			static void WriteImmediate(Int8* Target,SlotType Slot, Int64 Imm)
			{
				switch (Slot)
				{
				case SlotType::Int8:Binary::BinaryHelper::WriteIntByBigEndianess(
					Target, (Int8)Imm); break;
				case SlotType::Int16:Binary::BinaryHelper::WriteIntByBigEndianess(
					Target, (Int16)Imm); break;
				case SlotType::Int32:
				case SlotType::Float:
					Binary::BinaryHelper::WriteIntByBigEndianess(
						Target, (Int32)Imm); break;
				case SlotType::Int64:
				case SlotType::Double:
					Binary::BinaryHelper::WriteIntByBigEndianess(
						Target, (Int64)Imm); break;
				}
			}
			static Int64 ReadAsImmediate(Int8* Target, SlotType Slot) {
				switch (Slot)
				{
				case SlotType::Int8:return *Target;
				case SlotType::Int16:return *(Int16*)Target;
				case SlotType::Int32:
				case SlotType::Float:
					return *(Int32*)Target;
				case SlotType::Int64:
				case SlotType::Double:
					return *(Int64*)Target;					
				}
				Exception::Throw<EmitException>(L"Invalid slot type when reading immediate!");
				return *Target;
			}
			void WriteREX(SlotType Slot)
			{
				Int8 REX = X86OpCodes::REX[(int)Slot];
				if (REX != 0)
					Write(REX);
			}

			inline void NeedFixup(SlotType Slot, AddressFixUpType Type,Int16 Offset = 0) {
				m_fixups.Add({ GetExecutablePage()->Current(),Slot,Type,Offset });
			}
			inline void NeedFixup(SlotType Slot, Int32 Index, AddressFixUpType Type, Int16 Offset = 0) {
				m_fixups.Add({ Index,Slot,Type,Offset });
			}
		public:
			virtual void StartEmitting()
			{

			}
			virtual void CommitEmitting()
			{
				Int8* base = GetExecutablePage()->GetRawPage();
				//Fix up work
				{									
					for (auto&& fixup : m_fixups)
					{
						Int64 value = ReadAsImmediate(base + fixup.Index, fixup.Slot);
						if (fixup.Type == AddressFixUpType::AbsoluteToRelative)
							value = value - ((Int64)base + (Int64)fixup.Index + (Int64)fixup.Offset);
						else if (fixup.Type == AddressFixUpType::RelativeToAbsolute)
							value = value + ((Int64)base + (Int64)fixup.Index + (Int64)fixup.Offset);
						WriteImmediate(base + fixup.Index, fixup.Slot, value);
					}
				}
				m_fixups.Clear();
				//Fill the remain blank with int3
				{
					Int32 index = GetExecutablePage()->Current();
					Int32 length = GetExecutablePage()->Length();
					for (; index < length; ++index)
						base[index] = X86OpCodes::Pause;
				}
				
			}
			//Memory/Register operation

			virtual void EmitStoreImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot)
			{
				WriteREX(Slot);
				Write((Int8)(X86OpCodes::MovImmToR[(int)Slot] + Register));
				WriteImmediate(Slot, Imm);
			}
			virtual void EmitStoreImmediateToMemoryViaRegister(Int8 Register, Int64 Imm, SlotType Slot)
			{
				WriteREX(Slot);
				Write(X86OpCodes::MovImmToM[(int)Slot]);
				Write(ModRM(AddressingMode::RegisterAddressing, 0b000, Register));
				WriteImmediate(Slot, Imm);
			}
			virtual void EmitStoreImmediateToMemoryViaImmediate(Int64 AddressImm, Int64 Imm, SlotType Slot)
			{
				WriteREX(Slot);
				Write(X86OpCodes::MovImmToM[(int)Slot]);
				Write(ModRM(AddressingMode::RegisterAddressing, 0b000, RegisterOrMemory::Offset32bit));
				WriteImmediate(Slot, AddressImm);
				if (Slot == SlotType::Int64)
				{
					Slot = SlotType::Int32;
					Context::OperandSlice(GetEmitContext(), Slot);				
				}
				WriteImmediate(Slot, Imm);
			}
			virtual void EmitStoreRegisterToMemoryViaImmediate(Int8 Register, Int64 AddressImm, SlotType Slot)
			{
				if (Register != Register::AX)
					EmitLoadRegisterToRegister(Register::AX, Register, Slot);
				Write(X86OpCodes::MovRToMVImm[(int)Slot]);
				WriteImmediate(Slot, AddressImm);
			}
			virtual void EmitStoreRegisterToMemoryViaRegister(Int8 SourceRegister, Int8 AddressRegister, SlotType Slot)
			{
				WriteREX(Slot);
				Write(X86OpCodes::MovRToMVR[(int)Slot]);
				Write(ModRM(AddressingMode::RegisterAddressing, SourceRegister, AddressRegister));
			}

			virtual void EmitLoadRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot)
			{
				WriteREX(Slot);
				Write(X86OpCodes::MovRToR[(int)Slot]);
				Write(ModRM(AddressingMode::Register, SourceRegister, DestinationRegister));
			}
			virtual void EmitLoadMemoryToRegisterViaImmediate(Int8 Register, Int64 AddressImm, SlotType Slot)
			{
				Write(X86OpCodes::MovMToRVImm[(int)Slot]);
				WriteImmediate(Slot, AddressImm);
				if (Register != Register::AX)
					EmitLoadRegisterToRegister(Register, Register::AX, Slot);
			}
			virtual void EmitLoadMemoryToRegisterViaRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot)
			{
				WriteREX(Slot);
				Write(X86OpCodes::MovMToRVR[(int)Slot]);
				Write(ModRM(AddressingMode::RegisterAddressing, DestinationRegister, SourceRegister));
			}

			//Arithmetic operation

			virtual void EmitAddRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot, ArithmeticType)
			{
				WriteREX(Slot);
				Write(X86OpCodes::AddRToR[(int)Slot]);
				Write(ModRM(AddressingMode::Register, SourceRegister, DestinationRegister));
			}
			virtual void EmitAddImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot, ArithmeticType)
			{
				WriteREX(Slot);
				if (Register == Register::AX)
					Write(X86OpCodes::AddImmToR4AX[(int)Slot]);
				else
				{
					Write(X86OpCodes::AddImmToR[(int)Slot]);
					Write(ModRM(AddressingMode::Register, 0b000, Register));
				}
				WriteImmediate(Slot, Imm);
			}

			virtual void EmitSubRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot, ArithmeticType)
			{
				WriteREX(Slot);
				Write(X86OpCodes::SubRToR[(int)Slot]);
				Write(ModRM(AddressingMode::Register, SourceRegister, DestinationRegister));
			}
			virtual void EmitSubImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot, ArithmeticType)
			{
				WriteREX(Slot);
				if (Register == Register::AX)
					Write(X86OpCodes::SubImmToR4AX[(int)Slot]);
				else
				{
					Write(X86OpCodes::SubImmToR[(int)Slot]);
					Write(ModRM(AddressingMode::Register, 0b101, Register));
				}
				WriteImmediate(Slot, Imm);
			}

			virtual void EmitMulRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot, ArithmeticType)
			{
				WriteREX(Slot);
				if (DestinationRegister == Register::AX)
				{
					Write(X86OpCodes::MulRToR4AX[(int)Slot]);
					Write(ModRM(AddressingMode::Register, 0b101, SourceRegister));
				}
				else
				{
					Write(X86OpCodes::MulRToR[(int)Slot]);
					Write(ModRM(AddressingMode::Register, DestinationRegister, SourceRegister));
				}
			}
			virtual void EmitMulImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot, ArithmeticType)
			{
				WriteREX(Slot);
				Write(X86OpCodes::MulImmToR[(int)Slot]);
				Write(ModRM(AddressingMode::Register, Register, Register));
				if (Slot == SlotType::Int64)
					WriteImmediate(SlotType::Int32, Imm);
				else if (Slot == SlotType::Int8)
					WriteImmediate(SlotType::Int16, Imm);
				else
					WriteImmediate(Slot, Imm);
			}

			virtual void EmitDivRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot, ArithmeticType Type)
			{
				GetEmitContext()->SetRegisterOccupied(Register::AX, DestinationRegister);

				if (DestinationRegister != Register::AX)
				{
					if (SourceRegister == Register::AX)
					{
						Int8 ephemeral_register = GetEmitContext()->AvailableRegister();
						if (ephemeral_register == -1)
							Exception::Throw<EmitException>(L"All registers unavailable");
						EmitLoadRegisterToRegister(ephemeral_register, Register::AX, Slot);
						SourceRegister = ephemeral_register;
					}
					EmitLoadRegisterToRegister(Register::AX, DestinationRegister, Slot);
				}

				WriteREX(Slot);
				Write(X86OpCodes::CDQ);
				WriteREX(Slot);
				Write(X86OpCodes::DivRMToR[(int)Slot]);
				if (Type == ArithmeticType::Signed)
					Write(ModRM(AddressingMode::Register, 0b111, SourceRegister));
				else if (Type == ArithmeticType::Unsigned)
					Write(ModRM(AddressingMode::Register, 0b110, SourceRegister));
				if (DestinationRegister != Register::AX)
					EmitLoadRegisterToRegister(DestinationRegister, Register::AX, Slot);

				GetEmitContext()->SetRegisterAvailable(Register::AX,DestinationRegister);
			}
			virtual void EmitDivImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot, ArithmeticType Type)
			{
				GetEmitContext()->SetRegisterOccupied(Register::AX);
				GetEmitContext()->SetRegisterOccupied(Register);

				Int8 ephemeral_register = GetEmitContext()->AvailableRegister();
				if (ephemeral_register == -1)
					Exception::Throw<EmitException>(L"All registers unavailable");
				EmitStoreImmediateToRegister(ephemeral_register, Imm, Slot);

				if (Register != Register::AX)
					EmitLoadRegisterToRegister(Register::AX, Register, Slot);
				WriteREX(Slot);
				Write(X86OpCodes::CDQ);
				WriteREX(Slot);
				Write(X86OpCodes::DivRMToR[(int)Slot]);
				if (Type == ArithmeticType::Signed)
					Write(ModRM(AddressingMode::Register, 0b111, ephemeral_register));
				else if (Type == ArithmeticType::Unsigned)
					Write(ModRM(AddressingMode::Register, 0b110, ephemeral_register));

				if (Register != Register::AX)
					EmitLoadRegisterToRegister(Register, Register::AX, Slot);

				GetEmitContext()->SetRegisterAvailable(Register::AX);
				GetEmitContext()->SetRegisterAvailable(Register);
			}

			virtual void EmitCompareRegisterWithMemoryViaImmediate(Int8 Register, Int64 Address, SlotType Slot)
			{
				WriteREX(Slot);
				Write(X86OpCodes::CmpMRWR[(int)Slot]);
				Write(ModRM(AddressingMode::RegisterAddressing, Register, RegisterOrMemory::Offset32bit));
				WriteImmediate(Slot, Address);
			}
			virtual void EmitCompareRegisterWithMemoryViaRegister(Int8 Register, Int8 AddressRegister, SlotType Slot)
			{
				WriteREX(Slot);
				Write(X86OpCodes::CmpMRWR[(int)Slot]);
				Write(ModRM(AddressingMode::RegisterAddressing, Register, AddressRegister));
			}
			virtual void EmitCompareRegisterWithRegister(Int8 LeftRegister, Int8 RightRegister, SlotType Slot)
			{
				WriteREX(Slot);
				Write(X86OpCodes::CmpMRWR[(int)Slot]);
				Write(ModRM(AddressingMode::Register, LeftRegister, RightRegister));
			}
			virtual void EmitCompareRegisterWithImmediate(Int8 Register, Int64 Imm, SlotType Slot)
			{
				WriteREX(Slot);
				if (Register == Register::AX)
					Write(X86OpCodes::CmpImmWR4AX[(int)Slot]);
				else
				{
					Write(X86OpCodes::CmpImmWR[(int)Slot]);
					Write(ModRM(AddressingMode::Register, 0b111, Register));
				}
				if (Slot == SlotType::Int64)
					Slot = SlotType::Int32;
				WriteImmediate(Slot, Imm);
			}

			//Stack operation

			virtual void EmitPopToRegister(Int8 Register, SlotType Slot)
			{
				if (Slot == SlotType::Int8)
					Slot = SlotType::Int16;
				Write((Int8)(X86OpCodes::PopToR[(int)Slot] + Register));
			}
			virtual void EmitPopToMemoryViaRegister(Int8 Register, SlotType Slot)
			{
				if (Slot == SlotType::Int8)
					Slot = SlotType::Int16;
				Write(X86OpCodes::PopToM[(int)Slot]);
				Write(ModRM(AddressingMode::RegisterAddressing, 0b000, Register));
			}
			virtual void EmitPopToMemoryViaImmediate(Int64 Imm, SlotType Slot)
			{
				if (Slot == SlotType::Int8)
					Slot = SlotType::Int16;
				Write(X86OpCodes::PopToM[(int)Slot]);
				Write(ModRM(AddressingMode::RegisterAddressing, 0b000, RegisterOrMemory::Offset32bit));
				WriteImmediate(Slot, Imm);
			}

			virtual void EmitPushRegister(Int8 Register, SlotType Slot)
			{
				if (Slot == SlotType::Int8)
				{
					Slot = SlotType::Int16;
					Context::OperandPromote(GetEmitContext(), Slot);
				}
				if (Slot == SlotType::Int16)
					WriteREX(Slot);
				Write((Int8)(X86OpCodes::PushR[(int)Slot] + Register));
			}
			virtual void EmitPushMemoryViaRegister(Int8 Register, SlotType Slot)
			{
				if (Slot == SlotType::Int8)
				{
					Slot = SlotType::Int16;
					Context::OperandPromote(GetEmitContext(), Slot);
				}
				if (Slot == SlotType::Int16)
					WriteREX(Slot);
				Write(X86OpCodes::PushM[(int)Slot]);
				Write(ModRM(AddressingMode::RegisterAddressing, 0b110, Register));
			}
			virtual void EmitPushMemoryViaImmediate(Int64 Imm, SlotType Slot)
			{
				if (Slot == SlotType::Int8)
				{
					Slot = SlotType::Int16;
					Context::OperandPromote(GetEmitContext(), Slot);
				}
				if (Slot == SlotType::Int16)
					WriteREX(Slot);
				Write(X86OpCodes::PushM[(int)Slot]);
				Write(ModRM(AddressingMode::RegisterAddressing, 0b110, RegisterOrMemory::Offset32bit));
				WriteImmediate(Slot, Imm);
			}
			virtual void EmitPushImmediate(Int64 Imm, SlotType Slot)
			{
				if (Slot == SlotType::Int64)
				{
					Slot = SlotType::Int32;
					Context::OperandSlice(GetEmitContext(), Slot);
				}
				else if (Slot == SlotType::Int16)
				{
					Slot = SlotType::Int32;
					Context::OperandPromote(GetEmitContext(), Slot);
				}
				Write(X86OpCodes::PushImm[(int)Slot]);
				WriteImmediate(Slot, Imm);
			}

			//Flow control

			virtual void EmitReturn()
			{
				Write(X86OpCodes::Ret[0]);
			}

			virtual void EmitJmpViaImmediate(Int64 Imm, SlotType Slot, RedirectSemantic Sem)
			{
				if (Sem == RedirectSemantic::Absolute && Slot == SlotType::Int8)
				{
					Slot = SlotType::Int16;
					Context::OperandPromote(GetEmitContext(), Slot);
				}
				else if (Slot == SlotType::Int64)
				{
					Slot = SlotType::Int32;
					Context::OperandSlice(GetEmitContext(), Slot);
				}
				const Int8* OpCodes = 
					Sem == RedirectSemantic::Relative ? X86OpCodes::JmpImmRel : X86OpCodes::JmpImmAbs;
				Write(OpCodes[(int)Slot]);
				WriteImmediate(Slot, Imm);
			}
			virtual void EmitJmpViaRegister(Int8 Register, SlotType Slot, RedirectSemantic Sem)
			{
				if (Sem == RedirectSemantic::Relative)
				{
					EmitAddImmediateToRegister(Register, 0, Slot, ArithmeticType::Signed);
					NeedFixup(Slot, GetExecutablePage()->Current() - SlotTypeToSize(Slot), AddressFixUpType::RelativeToAbsolute);
				}
				if (Slot == SlotType::Int8)
				{
					Slot = SlotType::Int16;
					Context::OperandPromote(GetEmitContext(), Slot);
				}
				Write(X86OpCodes::JmpRMAbs[(int)Slot]);
				Write(ModRM(AddressingMode::Register, 0b100, Register));
			}

			virtual void EmitJcc(Condition Cond, Int64 Imm, SlotType Slot, RedirectSemantic Sem)
			{
				if (Slot == SlotType::Int8 || Slot == SlotType::Int16)
					Context::OperandPromote(GetEmitContext(), SlotType::Int32);
				else if (Slot == SlotType::Int64)
					Context::OperandSlice(GetEmitContext(), SlotType::Int32);
				Write(X86OpCodes::JccImmRel[(int)Cond]);
				if (Sem == RedirectSemantic::Absolute)
					NeedFixup(SlotType::Int32, AddressFixUpType::AbsoluteToRelative);
				WriteImmediate(SlotType::Int32, Imm);
			}

			virtual void EmitCallViaRegister(Int8 Register, SlotType Slot, RedirectSemantic Sem)
			{
				if (Sem == RedirectSemantic::Relative && Slot == SlotType::Int64)
				{
					Context::OperandSlice(GetEmitContext(), SlotType::Int32);
					Slot = SlotType::Int32;
				}
				else if (Slot == SlotType::Int8)
				{
					Context::OperandPromote(GetEmitContext(), SlotType::Int16);
					Slot = SlotType::Int16;
				}
				const Int8* OpCodes =
					Sem == RedirectSemantic::Relative ? X86OpCodes::CallRMRel : X86OpCodes::CallRMAbs;
				Write(OpCodes[(int)Slot] );
				Write(ModRM(AddressingMode::Register, 0b010, Register));
			}

			virtual void EmitCallViaImmediate(Int64 Imm, SlotType Slot, RedirectSemantic Sem)
			{
				if (Sem == RedirectSemantic::Relative && Slot == SlotType::Int64)
				{
					Context::OperandSlice(GetEmitContext(), SlotType::Int32);
					Slot = SlotType::Int32;
				}
				else if (Slot == SlotType::Int8)
				{
					Context::OperandPromote(GetEmitContext(), SlotType::Int16);
					Slot = SlotType::Int16;
				}
				if (Sem == RedirectSemantic::Absolute)
				{
					Write(X86OpCodes::CallRMAbs[(int)Slot]);
					Write(ModRM(AddressingMode::RegisterAddressing, 0b010, RegisterOrMemory::Offset32bit));	
				}
				else
					Write(X86OpCodes::CallImmRel[(int)Slot]);
				WriteImmediate(Slot, Imm);
			}

			//Bit operation

			virtual void EmitLShiftRegisterByImmediateTimes(Int8 Register, Int64 Imm, SlotType Slot, ArithmeticType)
			{
				WriteREX(Slot);
				if (Imm == 1ll)
					Write(X86OpCodes::LShiftB1[(int)Slot]);
				else
					Write(X86OpCodes::LShiftBImm[(int)Slot]);
				Write(ModRM(AddressingMode::Register, 0b100, Register));
				if (Slot != SlotType::Int8)
					Context::OperandSlice(GetEmitContext(), SlotType::Int8);
				if (Imm > 1ll)
					WriteImmediate(SlotType::Int8, (Int8)Imm);
			}
			virtual void EmitLShiftRegisterByRegisterTimes(Int8 Register, Int8 SourceRegister, SlotType Slot, ArithmeticType)
			{
				GetEmitContext()->SetRegisterOccupied(Register::CX);
				if (SourceRegister != Register::CX)
					EmitLoadRegisterToRegister(Register::CX, SourceRegister, SlotType::Int8);
				WriteREX(Slot);
				Write(X86OpCodes::LShiftBR[(int)Slot]);
				Write(ModRM(AddressingMode::Register, 0b100, Register));
				GetEmitContext()->SetRegisterAvailable(Register::CX);
			}

			virtual void EmitRShiftRegisterByImmediateTimes(Int8 Register, Int64 Imm, SlotType Slot, ArithmeticType)
			{
				WriteREX(Slot);
				if (Imm == 1ll)
					Write(X86OpCodes::RShiftB1[(int)Slot]);
				else
					Write(X86OpCodes::RShiftBImm[(int)Slot]);
				Write(ModRM(AddressingMode::Register, 0b101, Register));
				if (Slot != SlotType::Int8)
					Context::OperandSlice(GetEmitContext(), SlotType::Int8);
				if (Imm > 1ll)
					WriteImmediate(SlotType::Int8, (Int8)Imm);
			}
			virtual void EmitRShiftRegisterByRegisterTimes(Int8 Register, Int8 SourceRegister, SlotType Slot, ArithmeticType)
			{
				GetEmitContext()->SetRegisterOccupied(Register::CX);
				if (SourceRegister != Register::CX)
					EmitLoadRegisterToRegister(Register::CX, SourceRegister, SlotType::Int8);
				WriteREX(Slot);
				Write(X86OpCodes::RShiftBR[(int)Slot]);
				Write(ModRM(AddressingMode::Register, 0b101, Register));
				GetEmitContext()->SetRegisterAvailable(Register::CX);
			}

			virtual void EmitOrRegisterToRegister(Int8 Register, Int8 SourceRegister, SlotType Slot)
			{
				WriteREX(Slot);
				Write(X86OpCodes::OrR2R[(int)Slot]);
				Write(ModRM(AddressingMode::Register, SourceRegister, Register));
			}
			virtual void EmitOrImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot)
			{
				WriteREX(Slot);
				if (Slot == SlotType::Int64)
				{
					Slot = SlotType::Int32;
					Context::OperandSlice(GetEmitContext(), SlotType::Int32);
				}			
				if (Register == Register::AX)
					Write(X86OpCodes::OrImm2R4AX[(int)Slot]);
				else
					Write(X86OpCodes::OrImm2R[(int)Slot]);
				Write(ModRM(AddressingMode::Register, 0b001, Register));
				WriteImmediate(Slot, Imm);
			}

			virtual void EmitAndRegisterToRegister(Int8 Register, Int8 SourceRegister, SlotType Slot)
			{
				WriteREX(Slot);
				Write(X86OpCodes::AndR2R[(int)Slot]);
				Write(ModRM(AddressingMode::Register, SourceRegister, Register));
			}
			virtual void EmitAndImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot)
			{
				WriteREX(Slot);
				if (Slot == SlotType::Int64)
				{
					Slot = SlotType::Int32;
					Context::OperandSlice(GetEmitContext(), SlotType::Int32);
				}
				if (Register == Register::AX)
					Write(X86OpCodes::AndImm2R4AX[(int)Slot]);
				else
					Write(X86OpCodes::AndImm2R[(int)Slot]);
				Write(ModRM(AddressingMode::Register, 0b100, Register));
				WriteImmediate(Slot, Imm);
			}

			virtual void EmitXorRegisterToRegister(Int8 Register, Int8 SourceRegister, SlotType Slot)
			{
				WriteREX(Slot);
				Write(X86OpCodes::XorR2R[(int)Slot]);
				Write(ModRM(AddressingMode::Register, SourceRegister, Register));
			}
			virtual void EmitXorImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot)
			{
				WriteREX(Slot);
				if (Slot == SlotType::Int64)
				{
					Slot = SlotType::Int32;
					Context::OperandSlice(GetEmitContext(), SlotType::Int32);
				}
				if (Register == Register::AX)
					Write(X86OpCodes::XorImm2R4AX[(int)Slot]);
				else
					Write(X86OpCodes::XorImm2R[(int)Slot]);
				Write(ModRM(AddressingMode::Register, 0b110, Register));
				WriteImmediate(Slot, Imm);
			}

			virtual void EmitNotRegister(Int8 Register, SlotType Slot)
			{
				WriteREX(Slot);
				Write(X86OpCodes::Not[(int)Slot]);
				Write(ModRM(AddressingMode::Register, 0b010, Register));
			}
			virtual void EmitNotMemoryViaImmediate(Int64 Imm, SlotType Slot)
			{
				WriteREX(Slot);
				Write(X86OpCodes::Not[(int)Slot]);
				Write(ModRM(AddressingMode::RegisterAddressing, 0b010, RegisterOrMemory::Offset32bit));
				WriteImmediate(Slot, Imm);
			}
			virtual void EmitNotMemoryViaRegister(Int8 Register, SlotType Slot)
			{
				WriteREX(Slot);
				Write(X86OpCodes::Not[(int)Slot]);
				Write(ModRM(AddressingMode::RegisterAddressing, 0b010, Register));
			}
		};
	}
}