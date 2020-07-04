#pragma once
namespace HL
{
	namespace System
	{
		namespace Text
		{
			namespace RegularExpression
			{
				enum class Instruction :Byte
				{
					ToMatch,
					Matched,
					Jmp,
					Split,
					Target,
					PushCapture,
					PopCapture,
					Nop
				};
				struct OpCode
				{
					Instruction Ins;
					uintptr_t Target;
				};
				class CodeGenerator
				{
				public:
					typedef Collection::Generic::List<OpCode> CodeStream;
					static inline void ToMatch(CodeStream& codes,Content* Target) {
						codes.Add({ Instruction::ToMatch,reinterpret_cast<uintptr_t>(Target) });
					}
					static inline void Matched(CodeStream& codes) {
						codes.Add({ Instruction::Matched,0 });
					}
					static inline size_t Target(CodeStream& codes) {
						codes.Add({ Instruction::Target,0 });
						return codes.Count() - 1;
					}
					static inline size_t Jump(CodeStream& codes,size_t Index) {
						codes.Add({ Instruction::Jmp,Index });
						return codes.Count() - 1;
					}
					static inline size_t Split(CodeStream& codes,size_t Cnt) {
						codes.Add({ Instruction::Split,Cnt });
						return codes.Count() - 1;
					}
					static inline size_t Nop(CodeStream& codes) {
						codes.Add({ Instruction::Nop,0 });
						return codes.Count() - 1;
					}
					static bool PushCapture(CodeStream& codes, Descriptor* value) {
						if (value->CaptureInfo.CaptureArgument != std::numeric_limits<size_t>::max())
						{
							if (value->CaptureInfo.CaptureName != nullptr)
							{
								codes.Add({ Instruction::PushCapture,0 });
								size_t hash = Hash::GetSequenceHash((void*)value->CaptureInfo.CaptureName, value->CaptureInfo.CaptureArgument * sizeof(wchar_t));
								codes.Add({ Instruction::Target, hash });
							}
							else
							{
								codes.Add({ Instruction::PushCapture,1 });
								codes.Add({ Instruction::Target,value->CaptureInfo.CaptureArgument });
							}
							return true;
						}
						return false;
					}
					static inline void PopCapture(CodeStream& codes) {
						codes.Add({ Instruction::PopCapture,0 });
					}
					static size_t GenerateJoint(CodeStream& codes,Descriptor* value) {
						DestriptorCollection* target = static_cast<DestriptorCollection*>(value);
						size_t entry = codes.Count();
						for (size_t i = 0; i < target->Descriptors.Count(); ++i)
							Generate(codes, target->Descriptors[i]);
						return entry;
					}
					static size_t GenerateParallel(CodeStream& codes, Descriptor* value) {
						DestriptorCollection* target = static_cast<DestriptorCollection*>(value);
						Collection::Generic::Array<size_t> jump_calls(target->Descriptors.Count());
						size_t entry = codes.Count();
						Split(codes, target->Descriptors.Count());
						size_t base =codes.Count();
						for (size_t i = 0; i < target->Descriptors.Count(); ++i)
							Target(codes);
						for (size_t i = 0; i < target->Descriptors.Count(); ++i)
						{
							size_t internal_base = Generate(codes, target->Descriptors[i]);
							codes[base + i].Target = internal_base;
							jump_calls[i] = Jump(codes, 0);
						}
						size_t exit_index = Nop(codes);
						for (size_t i = 0; i < jump_calls.Count(); ++i)
							codes[jump_calls[i]].Target = exit_index;
						return entry;
					}
					static size_t GenerateRepeat(CodeStream& codes, Descriptor* value) {
						RepeatDescriptor* target = static_cast<RepeatDescriptor*>(value);
						size_t entry = codes.Count();
						for (size_t i = 0; i < target->Min; ++i)
							Generate(codes, target->Target);
						if (target->Max == std::numeric_limits<size_t>::max())
						{
							size_t split_ins_index = Split(codes, 2);
							size_t first_index = Target(codes);
							size_t second_index = Target(codes);
							size_t node_entry = Generate(codes, target->Target);
							Jump(codes, split_ins_index);
							size_t exit_index = Nop(codes);
							if (target->Greedy)
							{
								codes[first_index].Target = node_entry;
								codes[second_index].Target = exit_index;
							}
							else
							{
								codes[first_index].Target = exit_index;
								codes[second_index].Target = node_entry;
							}
						}
						else
						{
							for (size_t i = 0; i < target->Max - target->Min; ++i) {
								Split(codes, 2);
								size_t first_index = Target(codes);
								size_t second_index = Target(codes);
								size_t node_entry = Generate(codes, target->Target);
								size_t exit_index = Nop(codes);
								if (target->Greedy)
								{
									codes[first_index].Target = node_entry;
									codes[second_index].Target = exit_index;
								}
								else
								{
									codes[first_index].Target = exit_index;
									codes[second_index].Target = node_entry;
								}
							}
						}
						return entry;
					}
					static size_t GenerateSet(CodeStream& codes, Descriptor* value) {
						ContentDescriptor* target = static_cast<ContentDescriptor*>(value);
						ToMatch(codes, target->ContentPtr);
						return codes.Count() - 1;
					}
					static size_t GenerateSingle(CodeStream& codes, Descriptor* value) {
						ContentDescriptor* target = static_cast<ContentDescriptor*>(value);
						ToMatch(codes, target->ContentPtr);
						return codes.Count() - 1;
					}
					static size_t Generate(CodeStream& codes, Descriptor* value) {
						bool on_capture = PushCapture(codes, value);
						size_t ret = 0;
						switch (value->Type)
						{
						case DescriptorType::Joint:
							ret = GenerateJoint(codes, value); break;
						case DescriptorType::Parallel:
							ret = GenerateParallel(codes, value); break;
						case DescriptorType::Repeat:
							ret = GenerateRepeat(codes, value); break;
						case DescriptorType::Set:
							ret = GenerateSet(codes, value); break;
						case DescriptorType::Single:
							ret = GenerateSingle(codes, value); break;
						}
						if (on_capture)
							PopCapture(codes);
						return ret;
					}
					static Pointer::ptr<CodeStream> GenerateCode(Descriptor* value) {
						CodeStream ret;
						Generate(ret, value);
						Matched(ret);
						return Pointer::Reference::newptr<CodeStream>(std::move(ret));
					}
				};
				class OpCodeFormat
				{
				public:
					static void Print(Collection::Generic::List<OpCode> const& opcodes) {
						for (size_t i = 0; i < opcodes.Count(); ++i) {
#if _LIB_X64
							printf_s("%3llu ", i);
							switch (opcodes[i].Ins)
							{
							case Instruction::Jmp:
								printf_s("jmp %3llu", opcodes[i].Target); break;
							case Instruction::Target:
								printf_s("target: %llu", opcodes[i].Target); break;
							case Instruction::Split:
								printf_s("split"); break;
							case Instruction::ToMatch:
								printf_s("to match address: 0x%llx", opcodes[i].Target); break;
#else
							printf_s("%3u ", i);
							switch (opcodes[i].Ins)
							{
							case Instruction::Jmp:
								printf_s("jmp %3u", opcodes[i].Target); break;
							case Instruction::Target:
								printf_s("target: %u", opcodes[i].Target); break;
							case Instruction::Split:
								printf_s("split"); break;
							case Instruction::ToMatch:
								printf_s("to match address: 0x%x", opcodes[i].Target); break;
#endif				
							case Instruction::Nop:
								printf_s("nop"); break;
							case Instruction::Matched:
								printf_s("matched"); break;
							case Instruction::PushCapture:
								if (opcodes[i].Target == 0)
									printf_s("push capture hash");
								else
									printf_s("push capture index");
								break;
							case Instruction::PopCapture:
								printf_s("pop capture"); break;
							}
							printf_s("\n");
						}
					}
				};
				struct RegexCaputreInfo
				{
					wchar_t const* Base;
					wchar_t const* Top;
					inline void Go(wchar_t const* Sp)noexcept
					{
						if (Sp < Base)
							Base = Top = Sp;
						if (Base == nullptr)
							Base = Sp;
						Top = Sp + 1;
					}
				};
				struct RegexCaptureResult
				{
					Collection::Generic::Dictionary<WString, RegexCaputreInfo> NamedTable;
					Collection::Generic::Array<RegexCaputreInfo> AnonymousTable;

					RegexCaptureResult(Collection::Generic::List<WString> const&Named, size_t Count)
						:AnonymousTable(Count) {
						for (size_t i = 0; i < Named.Count(); ++i)
							NamedTable.Add(Named[i], { nullptr,nullptr });
					}
					void Clear() {
						for (auto&&item : NamedTable)
						{
							item.Value.Base = nullptr;
							item.Value.Top = nullptr;
						}
						for (auto&&item : AnonymousTable)
						{
							item.Base = nullptr;
							item.Top = nullptr;
						}
					}
				};
				struct RegexVMWorkBranch
				{
					OpCode const* Target;
					wchar_t const* String;
				};

				template<class T>
				struct RegexPile
				{
					T* Data;
					size_t Current;
					size_t Max;
					RegexPile(size_t Count) :Current(0), Max(Count) {
						Data = (T*)malloc(sizeof(T)*Count);
					}
					inline void Adjust(size_t Dp) {
						if (Dp + Current > Max)
						{
							void* new_block = realloc(Data, sizeof(T) * 2 * Max);
							if (new_block == nullptr)
							{
								free(Data);
								return;
							}
							else
								Data = (T*)new_block;
							Max *= 2;
						}
					}
					inline T* begin()noexcept {
						return Data;
					}
					inline T* end()noexcept {
						return Data + Current;
					}
					~RegexPile() {
						if (Data != nullptr) {
							free(Data);
							Data = nullptr;
						}
					}
				};

				struct RegexVMStack
				{
					RegexPile<RegexVMWorkBranch> BranchesPile;
					RegexPile<RegexCaputreInfo*> CapturePile;
					RegexVMStack(size_t BranchesCapacity, size_t CaptureStackDepth)
						:BranchesPile(BranchesCapacity), CapturePile(CaptureStackDepth) {
					}
					~RegexVMStack() = default;
				};

				class RegexVM
				{
					inline static void GoCapture(RegexPile<RegexCaputreInfo*>& pile, wchar_t const*sp) {
						for (RegexCaputreInfo*item : pile)
							item->Go(sp);
					}
				public:
					static wchar_t const* Run(OpCode const* base, wchar_t const*string, wchar_t const*top, RegexCaptureResult* capture, RegexVMStack* stack) {
						wchar_t const* sp = string;
						OpCode const* pc = base;
						stack->BranchesPile.Current = 1;
						stack->BranchesPile.Data[0] = { pc,sp };
						while (stack->BranchesPile.Current > 0)
						{
							--stack->BranchesPile.Current;
							pc = stack->BranchesPile.Data[stack->BranchesPile.Current].Target;
							sp = stack->BranchesPile.Data[stack->BranchesPile.Current].String;
							for (; sp != top;)
							{
								switch (pc->Ins)
								{
								case Instruction::ToMatch:
								{
									Content* content = (Content*)pc->Target;
									if (!content->Accept(*sp))
										goto Round;
									else if (capture != nullptr)
										GoCapture(stack->CapturePile, sp);
									sp++;
									pc++;
									break;
								}
								case Instruction::Matched:
									return sp;
								case Instruction::Jmp:
									pc = base + pc->Target;
									break;
								case Instruction::Split:
								{
									OpCode const* target = pc + pc->Target;
									stack->BranchesPile.Adjust(pc->Target);
									for (; target != pc; target--)
										stack->BranchesPile.Data[stack->BranchesPile.Current++] = { base + target->Target,sp };
									pc++;
									pc = base + pc->Target;
									break;
								}
								case Instruction::PushCapture:
									if (capture != nullptr)
									{
										if ((pc++)->Target == 0)
											stack->CapturePile.Data[stack->CapturePile.Current++] = &capture->NamedTable.ByHash(pc->Target);
										else
											stack->CapturePile.Data[stack->CapturePile.Current++] = &capture->AnonymousTable[pc->Target];
									}
									else
										pc++;
									pc++;
									break;
								case Instruction::PopCapture:
									if (capture != nullptr)
										stack->CapturePile.Current--;
									pc++;
									break;
								case Instruction::Nop:
									pc++;
									break;
								}
							}
						Round:
							(void)0;
						}
						return nullptr;
					}
				};
				class RegexMatchResult
				{
					size_t m_index = 0;
					size_t m_length = 0;
					Collection::Generic::Dictionary<WString, WString> m_named_capture;
					Collection::Generic::List<WString> m_anonymous_capture;
				public:
					RegexMatchResult() = default;
					RegexMatchResult(size_t Index, size_t Length, RegexCaptureResult const& Capture)
						:m_index(Index), m_length(Length) {
						for (auto&&var : Capture.NamedTable)
							m_named_capture.Add(var.Key, WString::From(var.Value.Base, var.Value.Top - var.Value.Base));
						for (size_t i = 0; i < Capture.AnonymousTable.Count(); ++i)
							m_anonymous_capture.Add(std::move(WString::From(Capture.AnonymousTable[i].Base, Capture.AnonymousTable[i].Top - Capture.AnonymousTable[i].Base)));
					}
					RegexMatchResult(size_t Index, size_t Length) :m_index(Index), m_length(Length) {}
					RegexMatchResult(RegexMatchResult const&) = default;
					RegexMatchResult(RegexMatchResult&&) = default;
					RegexMatchResult& operator=(RegexMatchResult const&) = default;
					RegexMatchResult& operator=(RegexMatchResult &&) = default;
					~RegexMatchResult() = default;

					inline WString const& operator[](size_t Index)const {
						return m_anonymous_capture[Index];
					}
					inline WString const& operator[](WString const&Key)const {
						return m_named_capture[Key];
					}
					inline WString const& GetAnonymousCapture(size_t Index)const {
						return m_anonymous_capture[Index];
					}
					inline WString const& GetNamedCapture(WString const&Key)const {
						return m_named_capture[Key];
					}
				};
				class Regex
				{
					Pointer::ptr<CodeGenerator::CodeStream> m_code_stream;
					Pointer::ptr<RegexParser> m_parser;
					size_t m_total_capture_depth = 0;
				public:
					constexpr static size_t DefaultStackCapacity = 32;
				public:
					Regex() = default;
					Regex(Regex&&) = default;
					Regex(Regex const&) = default;
					Regex(WString const&source) {
						m_parser = Pointer::Reference::newptr<RegexParser>(source);
						Descriptor* descriptor = m_parser->Parse();
						m_code_stream = CodeGenerator::GenerateCode(descriptor);
						m_total_capture_depth = m_parser->GetAnonymousCaptureCount() + m_parser->GetNamedCaptureList().Count();
					}
					Regex& operator=(Regex const&) = default;
					Regex& operator=(Regex &&) = default;
					~Regex() = default;
					RegexMatchResult Match(const wchar_t* base, const wchar_t*top) {
						RegexCaptureResult capture(m_parser->GetNamedCaptureList(), m_parser->GetAnonymousCaptureCount());
						RegexVMStack stack(DefaultStackCapacity, m_total_capture_depth);
						wchar_t const*iterator = base;
						while (iterator != top)
						{
							wchar_t const* result = RegexVM::Run(m_code_stream->begin(), iterator, top, &capture, &stack);
							if (result != nullptr)
								return RegexMatchResult(iterator - base, result - iterator, capture);
						}
					}
					inline RegexMatchResult Match(Text::WString const&Target) {
						return Match(Target.GetNativePtr(), Target.GetNativePtr() + Target.Length());
					}
					Collection::Generic::List<RegexMatchResult> Matches(const wchar_t* base, const wchar_t*top)
					{
						Collection::Generic::List<RegexMatchResult> matches;
						RegexVMStack stack(DefaultStackCapacity, m_total_capture_depth);
						RegexCaptureResult capture(m_parser->GetNamedCaptureList(), m_parser->GetAnonymousCaptureCount());
						wchar_t const* iterator = base;
						while (iterator != top)
						{
							wchar_t const* result = RegexVM::Run(m_code_stream->begin(), iterator, top, &capture, &stack);
							if (result != nullptr&&result != iterator)
							{
								matches.Add(std::move(RegexMatchResult(iterator - base, result - iterator, capture)));
								iterator = result;
							}
							else
								iterator++;
							capture.Clear();
						}
						return matches;
					}
					inline Collection::Generic::List<RegexMatchResult> Matches(Text::WString const& Target)
					{
						return Matches(Target.GetNativePtr(), Target.GetNativePtr() + Target.Length());
					}
					inline bool IsMatchFirst(Text::WString const&Target) {
						return IsMatchFirst(Target.GetNativePtr(), Target.GetNativePtr() + Target.Length());
					}
					bool IsMatchFirst(const wchar_t* base, const wchar_t*top) {
						wchar_t const*begin = base;
						RegexVMStack stack(DefaultStackCapacity, m_total_capture_depth);
						while (begin != top)
						{
							wchar_t const* result = RegexVM::Run(m_code_stream->begin(), begin, top, nullptr, &stack);
							if (result != nullptr)
								return true;
							begin++;
						}
						return false;
					}
					bool IsMatch(Text::WString const&Target) {
						RegexVMStack stack(DefaultStackCapacity, m_total_capture_depth);
						return RegexVM::Run(m_code_stream->begin(), Target.GetNativePtr(), Target.GetNativePtr() + Target.Length(), nullptr, &stack) != nullptr;
					}
					inline bool IsMatch(Text::WString const&Target, RegexVMStack& stack) {
						return RegexVM::Run(m_code_stream->begin(), Target.GetNativePtr(), Target.GetNativePtr() + Target.Length(), nullptr, &stack) != nullptr;
					}
					bool IsMatch(const wchar_t* base, const wchar_t*top) {
						RegexVMStack stack(DefaultStackCapacity, m_total_capture_depth);
						return RegexVM::Run(m_code_stream->begin(), base, top, nullptr, &stack) != nullptr;
					}
					inline bool IsMatch(const wchar_t* base, const wchar_t*top, RegexVMStack& stack) {
						return RegexVM::Run(m_code_stream->begin(), base, top, nullptr, &stack) != nullptr;
					}
				};
			}
		}
	}
}