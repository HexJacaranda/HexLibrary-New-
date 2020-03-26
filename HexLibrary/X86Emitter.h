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
			static void OperandSlice(EmitContext* context, SlotType To) {
				context->EmitState |= (Int8)To;
			}
			static void OperandPromote(EmitContext* context, SlotType To) {
				context->EmitState |= (Int8)To;
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

			static constexpr Int8 SubRToR[] = { (Int8)0x01,(Int8)0x01,(Int8)0x01,(Int8)0x01 };

			static constexpr Int8 SubImmToR[] = { (Int8)0x80,(Int8)0x81,(Int8)0x81,(Int8)0x81 };

			static constexpr Int8 SubImmToR4AX[] = { (Int8)0x2C,(Int8)0x2D,(Int8)0x2D,(Int8)0x2D };

			static constexpr Int8 MulRToR4AX[] = { (Int8)0xF6,(Int8)0xF7,(Int8)0xF7,(Int8)0xF7 };

			static constexpr Int16 MulRToR[] = { (Int16)0x0FAF,(Int16)0x0FAF,(Int16)0x0FAF,(Int16)0x0FAF };

			static constexpr Int8 MulImmToR[] = { (Int8)0x69,(Int8)0x69,(Int8)0x69,(Int8)0x69 };

			static constexpr Int8 DivRMToR[] = { (Int8)0xF6,(Int8)0xF7,(Int8)0xF7,(Int8)0xF7 };

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

			static constexpr Int8 CallImmAbs[] = { (Int8)0x9A,(Int8)0x9A,(Int8)0x9A ,(Int8)0x9A };

			static constexpr Int8 CallRMAbs[] = { (Int8)0xFF,(Int8)0xFF,(Int8)0xFF ,(Int8)0xFF };
		};




		class X86Emitter :public INativeEmitter
		{		
			template<class IntT>
			void Write(IntT value) {
				Binary::BinaryWriter::WriteIntByBigEndianess(
					GetExecutablePage()->PrepareWrite(sizeof(IntT)), value);
				GetExecutablePage()->CommitWrite(sizeof(IntT));
			}
			template<class IntT>
			void WriteLittleEndian(IntT value) {
				Binary::BinaryWriter::WriteIntByLittleEndianess(
					GetExecutablePage()->PrepareWrite(sizeof(IntT)), value);
				GetExecutablePage()->CommitWrite(sizeof(IntT));
			}
			void WriteImmediate(SlotType Slot, Int64 Imm)
			{
				switch (Slot)
				{
				case SlotType::Int8:WriteLittleEndian((Int8)Imm); break;
				case SlotType::Int16:WriteLittleEndian((Int16)Imm); break;
				case SlotType::Int32:
				case SlotType::Float:
					WriteLittleEndian((Int32)Imm); break;
				case SlotType::Int64:
				case SlotType::Double:
					WriteLittleEndian((Int64)Imm); break;
				}
			}
			void WriteREX(SlotType Slot)
			{
				Int8 REX = X86OpCodes::REX[(int)Slot];
				if (REX != 0)
					Write(REX);
			}
		public:
			virtual void EmitStoreImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot)
			{
				WriteREX(Slot);
				Write(X86OpCodes::MovImmToR[(int)Slot] + Register);
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
				Write(X86OpCodes::AddRToR[(int)Slot]);
				Write(ModRM(AddressingMode::Register, SourceRegister, DestinationRegister));
			}
			virtual void EmitSubImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot, ArithmeticType)
			{
				WriteREX(Slot);
				if (Register == Register::AX)
					Write(X86OpCodes::AddImmToR4AX[(int)Slot]);
				else
				{
					Write(X86OpCodes::AddImmToR[(int)Slot]);
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
				if (DestinationRegister != Register::AX)
					EmitLoadRegisterToRegister(Register::AX, DestinationRegister, Slot);
				WriteREX(Slot);
				Write(X86OpCodes::DivRMToR[(int)Slot]);
				if (Type == ArithmeticType::Signed)
					Write(ModRM(AddressingMode::Register, 0b111, SourceRegister));
				else if (Type == ArithmeticType::Unsigned)
					Write(ModRM(AddressingMode::Register, 0b110, SourceRegister));
				if (DestinationRegister != Register::AX)
					EmitLoadRegisterToRegister(DestinationRegister, Register::AX, Slot);
			}
			virtual void EmitDivImmediateToRegister(Int8 Register, SlotType, Int64 Imm, SlotType Slot, ArithmeticType Type)
			{
				if (Register != Register::AX)
					EmitLoadRegisterToRegister(Register::AX, Register, Slot);
				WriteREX(Slot);
				Write(X86OpCodes::DivRMToR[(int)Slot]);
				Int8 ephemeral_register = GetEmitContext()->AvailableRegister();
				if (ephemeral_register == -1)
					Exception::Throw<EmitException>(L"All registers unavailable");
				if (Type == ArithmeticType::Signed)
					Write(ModRM(AddressingMode::Register, 0b111, ephemeral_register));
				else if (Type == ArithmeticType::Unsigned)
					Write(ModRM(AddressingMode::Register, 0b110, ephemeral_register));
				if (Register != Register::AX)
					EmitLoadRegisterToRegister(Register, Register::AX, Slot);
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

			virtual void EmitPopToRegister(Int8 Register, SlotType Slot)
			{
				if (Slot == SlotType::Int8)
					Slot = SlotType::Int16;
				Write(X86OpCodes::PopToR[(int)Slot] + Register);
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
				Write(X86OpCodes::PushR[(int)Slot] + Register);
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

			virtual void EmitReturn()
			{
				
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
					//
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
				if (Sem == RedirectSemantic::Absolute)
				{

				}
				if (Slot == SlotType::Int8 || Slot == SlotType::Int16)
					Context::OperandPromote(GetEmitContext(), SlotType::Int32);
				else if(Slot == SlotType::Int64)
					Context::OperandSlice(GetEmitContext(), SlotType::Int32);

				Write(X86OpCodes::JccImmRel[(int)Cond]);
				WriteImmediate(SlotType::Int32, Imm);
			}

			virtual void EmitCallViaRegister(Int8 Register, SlotType Slot, RedirectSemantic Sem)
			{
				if (Sem == RedirectSemantic::Relative)
				{
					//
				}
				if (Slot == SlotType::Int8)
				{
					Slot = SlotType::Int16;
					Context::OperandPromote(GetEmitContext(), Slot);
				}
				Write(X86OpCodes::CallRMAbs[(int)Slot] );
				Write(ModRM(AddressingMode::Register, 0b010, Register));
			}

			virtual void EmitCallViaImmediate(Int64 Imm, SlotType Slot, RedirectSemantic Sem)
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
					Sem == RedirectSemantic::Relative ? X86OpCodes::CallImmRel : X86OpCodes::CallImmAbs;
				Write(OpCodes[(int)Slot]);
				WriteImmediate(Slot, Imm);
			}
		};
	}
}