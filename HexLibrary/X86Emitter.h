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

		class RegisterOrMemory :public Register
		{
		public:
			//Enables only when it's not AddressingMode::Register
			static constexpr int SIBAddressing = 0b100;
			//Enables only when it's AddressingMode::RegisterAddressing
			static constexpr int Offset32bit = 0b101;
		};

		static inline constexpr Int8 ModMR(Int8 Mod, Int8 Reg, Int8 RM) {
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
		};

		class X86Emitter :public INativeEmitter
		{
			void Write(Int8 value) {

			}
			void Write(Int16 value) {

			}
			void Write(Int32 value) {

			}
			void Write(Int64 value) {

			}
			void WriteLittleEndian(Int8 value) {

			}
			void WriteLittleEndian(Int16 value) {

			}
			void WriteLittleEndian(Int32 value) {

			}
			void WriteLittleEndian(Int64 value) {

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
			virtual void EmitStoreImmediateToRegister(Int8 Register, SlotType Slot, Int64 Imm)
			{
				WriteREX(Slot);
				Write(X86OpCodes::MovImmToR[(int)Slot] + Register);
				WriteImmediate(Slot, Imm);
			}	
			virtual void EmitStoreImmediateToMemoryViaRegister(Int8 Register, SlotType Slot, Int64 Imm)
			{
				WriteREX(Slot);
				Write(X86OpCodes::MovImmToM[(int)Slot]);
				Write(ModMR(AddressingMode::RegisterAddressing, 0b000, Register));
				WriteImmediate(Slot, Imm);
			}
			virtual void EmitStoreImmediateToMemoryViaImmediate(Int64 AddressImm, SlotType Slot, Int64 Imm) 
			{
				WriteREX(Slot);
				Write(X86OpCodes::MovImmToM[(int)Slot]);
				Write(ModMR(AddressingMode::RegisterAddressing, 0b000, RegisterOrMemory::Offset32bit));
				WriteImmediate(Slot, AddressImm);
				if (Slot == SlotType::Int64)
					WriteImmediate(SlotType::Int32, Imm);
				else
					WriteImmediate(Slot, Imm);
			}
			virtual void EmitStoreRegisterToMemoryViaImmediate(Int8 Register, SlotType Slot, Int64 AddressImm) 
			{
				if (Register != Register::AX)
					EmitLoadRegisterToRegister(Register::AX, Register, Slot);
				Write(X86OpCodes::MovRToMVImm[(int)Slot]);
				WriteImmediate(Slot, AddressImm);
			}
			virtual void EmitStoreRegisterToMemoryViaRegister(Int8 SourceRegister, SlotType Slot, Int8 AddressRegister)
			{
				WriteREX(Slot);
				Write(X86OpCodes::MovRToMVR[(int)Slot]);
				Write(ModMR(AddressingMode::RegisterAddressing, SourceRegister, AddressRegister));
			}

			virtual void EmitLoadRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot) 
			{
				WriteREX(Slot);
				Write(X86OpCodes::MovRToR[(int)Slot]);
				Write(ModMR(AddressingMode::Register, SourceRegister, DestinationRegister));
			}
			virtual void EmitLoadMemoryToRegisterViaImmediate(Int8 Register, SlotType Slot, Int64 AddressImm) 
			{
				Write(X86OpCodes::MovMToRVImm[(int)Slot]);
				WriteImmediate(Slot, AddressImm);
				if (Register != Register::AX)
					EmitLoadRegisterToRegister(Register, Register::AX, Slot);
			}
			virtual void EmitLoadMemoryToRegisterViaRegister(Int8 DestinationRegister, SlotType Slot, Int8 SourceRegister)
			{
				WriteREX(Slot);
				Write(X86OpCodes::MovMToRVR[(int)Slot]);
				Write(ModMR(AddressingMode::RegisterAddressing, DestinationRegister, SourceRegister));
			}

			virtual void EmitAddRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot)
			{
				WriteREX(Slot);
				Write(X86OpCodes::AddRToR[(int)Slot]);
				Write(ModMR(AddressingMode::Register, SourceRegister, DestinationRegister));
			}
			virtual void EmitAddImmediateToRegister(Int8 Register, SlotType Slot, Int64 Imm)
			{
				WriteREX(Slot);
				if (Register == Register::AX)
					Write(X86OpCodes::AddImmToR4AX[(int)Slot]);
				else
				{
					Write(X86OpCodes::AddImmToR[(int)Slot]);
					Write(ModMR(AddressingMode::Register, 0b000, Register));
				}
				WriteImmediate(Slot, Imm);
			}

			virtual void EmitSubRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot)
			{
				WriteREX(Slot);
				Write(X86OpCodes::AddRToR[(int)Slot]);
				Write(ModMR(AddressingMode::Register, SourceRegister, DestinationRegister));
			}
			virtual void EmitSubImmediateToRegister(Int8 Register, SlotType Slot, Int64 Imm)
			{
				WriteREX(Slot);
				if (Register == Register::AX)
					Write(X86OpCodes::AddImmToR4AX[(int)Slot]);
				else
				{
					Write(X86OpCodes::AddImmToR[(int)Slot]);
					Write(ModMR(AddressingMode::Register, 0b101, Register));
				}
				WriteImmediate(Slot, Imm);
			}

			virtual void EmitMulRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType) {}
			virtual void EmitMulImmediateToRegister(Int8 Register, SlotType, Int64 Imm, SlotType) {}

			virtual void EmitDivRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType) {}
			virtual void EmitDivImmediateToRegister(Int8 Register, SlotType, Int64 Imm, SlotType) {}

			virtual void EmitCompareRegisterWithMemory(Int8 Register, Int64 Address, SlotType) {}
			virtual void EmitCompareRegisterWithRegister(Int8 LeftRegister, Int8 RightRegister) {}
			virtual void EmitCompareRegisterWithImmediate(Int8 Register, Int64 Imm, SlotType) {}

			virtual void EmitPop() {}
			virtual void EmitReturn() {}
			virtual void EmitJmpViaImmediate(Int64 Imm, SlotType) {}
			virtual void EmitJmpViaRegister(Int8 Register, SlotType) {}
			virtual void EmitJcc(Condition, Int64 Imm, SlotType) {}
		};
	}
}