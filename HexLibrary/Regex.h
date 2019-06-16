#pragma once
namespace HL
{
	namespace System
	{
		namespace Text
		{
			namespace RegularExpression
			{
				namespace Abandoned
				{
#if FALSE
					//����Regex�����ڴ������
					template<class T>
					class Pool
					{
						Collection::Generic::Dictionary<intptr_t, T*> m_objects;
					public:
						Pool() {}
						Pool(Pool &&) = default;
						Pool(Pool const&) = delete;
						Pool& operator=(Pool const&rhs) = delete;
						Pool& operator=(Pool&&) = default;
						T* Add(T*Target) {
							m_objects.Add((intptr_t)Target, Target);
							return Target;
						}
						void Remove(T*Target) {
							m_objects.Remove((intptr_t)Target);
							delete Target;
						}
						template<class...Args>
						inline T* Create(Args &&...args) {
							T*ret = new T(std::forward<Args>(args)...);
							m_objects.Add((intptr_t)ret, ret);
							return ret;
						}
						template<class U, class...Args>
						inline U* CreateWith(Args const&...args) {
							U*ret = new U(args...);
							m_objects.Add((intptr_t)ret, ret);
							return ret;
						}
						Collection::Generic::Dictionary<intptr_t, T*>&GetContainer() {
							return this->m_objects;
						}
						Collection::Generic::Dictionary<intptr_t, T*> const&GetContainer()const {
							return this->m_objects;
						}
						inline void Clear() {
							this->m_objects.Clear();
						}
						~Pool() {
							for (auto&&item : m_objects)
								delete item.Value;
						}
					};
					//������Ϣ
					struct CaptureInfo
					{
						wchar_t const* Base;
						size_t Size;
					};
					//�����
					struct CapturePackage
					{
					public:
						const wchar_t* Source = nullptr;
						System::Collection::Generic::Dictionary<WString, CaptureInfo> NamedCapture;
						System::Collection::Generic::Array<CaptureInfo> AnonymousCapture;

						CapturePackage(const wchar_t* Target, size_t AnonymousCaptureLength, Collection::Generic::List<WString> const&NamedCaptureTable)
							:AnonymousCapture(AnonymousCaptureLength), Source(Target) {
							for (size_t i = 0; i < NamedCaptureTable.Count(); ++i)
								NamedCapture.Add(NamedCaptureTable[i], CaptureInfo{ nullptr,0 });
						}
						void SetToZero() {
							for (auto&&var : NamedCapture)
								var.Value.Size = 0;
							for (size_t i = 0; i < AnonymousCapture.Count(); ++i)
								AnonymousCapture[i].Size = 0;
						}
					};
					//����
					class Capture
					{
						WString m_name;
						size_t m_anonymous_index = std::numeric_limits<size_t>::max();
					public:
						Capture(size_t AnonymousIndex) :m_anonymous_index(AnonymousIndex) {}
						Capture(WString const&Name) :m_name(Name) {}
						bool Go(CapturePackage*Package, wchar_t const* Current) {
							if (Package == nullptr)
								return false;
							CaptureInfo*info = nullptr;
							if (m_anonymous_index != std::numeric_limits<size_t>::max())
								info = &Package->AnonymousCapture[m_anonymous_index];
							else
								info = &Package->NamedCapture[m_name];
							if (info->Size == 0)
								info->Base = Current;
							info->Size++;
							return true;
						}
						void WithDraw(CapturePackage*Package) {
							if (Package == nullptr)
								return;
							CaptureInfo*info = nullptr;
							if (m_anonymous_index != std::numeric_limits<size_t>::max())
								info = &Package->AnonymousCapture[m_anonymous_index];
							else
								info = &Package->NamedCapture[m_name];
							info->Size--;
						}
						~Capture() {}
					};
					//��
					class Edge
					{
					public:
						Edge() {}
						Edge(Content*Target) :Content(Target) {}
						//��ʼ״̬
						Status*From = nullptr;
						//Ŀ��״̬
						Status*To = nullptr;
						//ƥ������
						Content*Content = nullptr;
						//�Ƿ���Ч
						bool Valid = true;
						//�Ƿ�Ϊ�Ի���
						bool LoopEdge = false;
						//����
						Collection::Generic::List<Capture*> Actions;
					};
					//״̬
					class Status
					{
					public:
						//����״̬�ı�
						Collection::Generic::List<Edge*> InEdges;
						//��ȥ�ı�
						Collection::Generic::List<Edge*> OutEdges;
						//�Ƿ��ǽ���״̬
						bool Final = false;
						//�Ƿ���Ч
						bool Valid = false;
						//�Ƿ���ʹ�,����Debug
						bool Visited = false;
						//��Ч����
						bool EquivalentFinal = false;
					};
					//��Դ
					class NFAResource
					{
					public:
						Pool<NFA> NFAPool;
						Pool<Content> ContentPool;
						Pool<Edge> EdgePool;
						Pool<Status> StatusPool;
						Pool<Capture> CapturePool;
						void Clear() {
							NFAPool.Clear();
							ContentPool.Clear();
							EdgePool.Clear();
							StatusPool.Clear();
							CapturePool.Clear();
						}
					};
					//NFA״̬��
					class NFA
					{
						NFAResource*Resources;
					public:
						NFA(NFAResource*Origin) :Resources(Origin), Head(nullptr), Tail(nullptr) {}
						NFA() :Head(nullptr), Tail(nullptr), Resources(nullptr) {}
						Status*Head;
						Status*Tail;
						//����ڵ�
						void BuildNode(Content*Content)
						{
							Head = Resources->StatusPool.Create();
							Tail = Resources->StatusPool.Create();
							if (Content)
							{
								Edge* edge = Resources->EdgePool.Create();
								edge->Content = Content;
								Connect(edge, this->Head, this->Tail);
							}
						}
						//��This,R��������
						void Joint(NFA*Right) {
							Connect(Resources->EdgePool.Create(), this->Tail, Right->Head);
						}
						//��Target��������
						void Parallel(NFA*Target) {
							Connect(Resources->EdgePool.Create(), this->Head, Target->Head);
							Connect(Resources->EdgePool.Create(), Target->Tail, this->Tail);
						}
						//��Target��������
						void Parallel(Edge*Target) {
							Connect(Target, this->Head, this->Tail);
						}
						//�ñ߽�����״̬��������
						static void Connect(Edge*ConnectEdge, Status*From, Status*To) {
							ConnectEdge->From = From;
							ConnectEdge->To = To;
							From->OutEdges.Add(ConnectEdge);
							To->InEdges.Add(ConnectEdge);
						}
					};
					//�հ�
					struct Closure
					{
						Status*Target;
						Edge*ValidEdge;
					};
					//NFA��
					class NFASimplify
					{
					public:
						//���������Ч״̬
						static void MarkValidStatus(Pool<Status> &Target) {
							for (auto&&var : Target.GetContainer())
							{
								for (size_t i = 0; i < var.Value->InEdges.Count(); ++i)
								{
									if (var.Value->InEdges[i]->Content != nullptr
										|| var.Value->InEdges[i]->LoopEdge)
									{
										var.Value->Valid = true;
										break;
									}
								}
							}
						}
						//���״̬������Depth����1�ıհ�(��Ҫָ�����еİ���Depth����Ϊ-1)
						static void GetClosure(Collection::Generic::List<Closure>&Out, Status*Where, size_t Depth = 0) {
							for (size_t i = 0; i < Where->OutEdges.Count(); i++) {
								if ((Where->OutEdges[i]->Content != nullptr
									|| Where->OutEdges[i]->To->Final))
								{
									if (Depth < 1)
										continue;
									Closure closure;
									closure.Target = Where->OutEdges[i]->To;
									closure.ValidEdge = Where->OutEdges[i];
									Out.Add(closure);
								}
								else
								{
									Where->OutEdges[i]->Valid = false;
									GetClosure(Out, Where->OutEdges[i]->To, Depth + 1);
								}
							}
						}
						//��ǵ�Ч����״̬
						static void MarkEquivalentEndStatus(Pool<Status> &Target) {
							for (auto&&var : Target.GetContainer())
							{
								if (!var.Value->Valid)
									continue;
								for (size_t i = 0; i < var.Value->OutEdges.Count(); ++i)
								{
									if (var.Value->OutEdges[i]->To->Final)
									{
										var.Value->EquivalentFinal = true;
										break;
									}
								}
							}
						}
						//�Ƴ���Ч����״̬
						static void RemoveInvalidStatusAndEdges(NFAResource*Re) {
							for (auto&&var : Re->StatusPool.GetContainer())
							{
								if (var.Value->Valid)
								{
									size_t offset = 0;
									for (size_t i = 0; i < var.Value->OutEdges.Count(); ++i) {
										if (!var.Value->OutEdges[i - offset]->Valid)
										{
											var.Value->OutEdges.RemoveAt(i - offset);
											offset++;
										}
									}
								}
							}
						}
						//��NFA
						static NFA* SimplifyNFA(NFAResource*Pool, NFA*Target) {
							MarkValidStatus(Pool->StatusPool);
							for (auto&&var : Pool->StatusPool.GetContainer())
							{
								if (!var.Value->Valid)
									continue;
								Collection::Generic::List<Closure> closures;
								GetClosure(closures, var.Value);
								for (size_t i = 0; i < closures.Count(); ++i) {
									size_t index = closures[i].Target->InEdges.IndexOf(closures[i].ValidEdge);
									if (index != std::numeric_limits<size_t>::max())
										closures[i].Target->InEdges[index]->Valid = false;
									Edge*edge = Pool->EdgePool.Create();
									edge->Actions = closures[i].ValidEdge->Actions;//��������
									edge->Content = closures[i].ValidEdge->Content;//���ݿ���
									if (closures[i].Target == var.Value)//��¼Loop��
										edge->LoopEdge = true;
									NFA::Connect(edge, var.Value, closures[i].Target);
								}
							}
							RemoveInvalidStatusAndEdges(Pool);
							MarkEquivalentEndStatus(Pool->StatusPool);
							return Target;
						}
					};
					//NFA������
					class NFAGenerator
					{
						Collection::Generic::Stack<Capture*> m_capture_stack;
						NFAResource* m_resource;
						inline Edge* NewEdge() {
							return m_resource->EdgePool.Create();
						}
						inline Edge* NewEdge(Content*Content) {
							return m_resource->EdgePool.Create(Content);
						}
						inline Status* NewStatus() {
							return m_resource->StatusPool.Create();
						}
						inline NFA* NewNFA() {
							return m_resource->NFAPool.Create(m_resource);
						}
						template<class...Args>
						inline Capture* NewCapture(Args&&...args) {
							return this->m_resource->CapturePool.Add(new Capture(std::forward<Args>(args)...));
						}
						bool PushCapture(Descriptor*Target) {
							if (Target->CaptureInfo.CaptureArgument != std::numeric_limits<size_t>::max())
							{
								if (Target->CaptureInfo.CaptureName != nullptr)
								{
									WString name;
									name.Append(Target->CaptureInfo.CaptureName, Target->CaptureInfo.CaptureArgument);
									m_capture_stack.Push(NewCapture(std::move(name)));
								}
								else
									m_capture_stack.Push(NewCapture(Target->CaptureInfo.CaptureArgument));
								return true;
							}
							return false;
						}
						inline void PopCapture() {
							m_capture_stack.Pop();
						}
						Edge* SetCapture(Edge*Target) {
							for (auto&&item : m_capture_stack)
								Target->Actions.Add(item);
							return Target;
						}
					public:
						NFAGenerator(NFAResource*Resource) :m_resource(Resource) {}
						~NFAGenerator() {}
						NFA* Generate(Descriptor*value)
						{
							bool on_capture = PushCapture(value);
							NFA* target = nullptr;
							switch (value->Type)
							{
							case DescriptorType::Joint:
								target = GenerateJoint(value); break;
							case DescriptorType::Parallel:
								target = GenerateParallel(value); break;
							case DescriptorType::Repeat:
								target = GenerateRepeat(value); break;
							case DescriptorType::Set:
								target = GenerateSet(value); break;
							case DescriptorType::Single:
								target = GenerateSingle(value); break;
							default:
								return nullptr;
							}
							if (on_capture)
								PopCapture();
							return target;
						}
						NFA* GenerateSingle(Descriptor* value)
						{
							ContentDescriptor* target = static_cast<ContentDescriptor*>(value);
							NFA* ret = NewNFA();
							ret->BuildNode(nullptr);
							ret->Parallel(SetCapture(NewEdge(target->ContentPtr)));
							return ret;
						}
						NFA* GenerateJoint(Descriptor* value)
						{
							DestriptorCollection* target = static_cast<DestriptorCollection*>(value);
							NFA* ret = NewNFA();
							ret->BuildNode(nullptr);
							NFA* prev = nullptr;
							NFA* head = Generate(target->Descriptors[0]);
							NFA::Connect(NewEdge(), ret->Head, head->Head);
							prev = head;
							for (size_t i = 1; i < target->Descriptors.Count() - 1; ++i)
							{
								NFA* mid = Generate(target->Descriptors[i]);
								NFA::Connect(NewEdge(), prev->Tail, mid->Head);
								prev = mid;
							}

							NFA* tail = nullptr;
							if (target->Descriptors.Count() > 1)
							{
								tail = Generate(target->Descriptors[target->Descriptors.Count() - 1]);
								NFA::Connect(NewEdge(), prev->Tail, tail->Head);
							}
							else
								tail = prev;
							NFA::Connect(NewEdge(), tail->Tail, ret->Tail);
							return ret;
						}
						NFA* GenerateParallel(Descriptor* value)
						{
							DestriptorCollection* target = static_cast<DestriptorCollection*>(value);
							NFA* ret = NewNFA();
							ret->BuildNode(nullptr);
							for (size_t i = 0; i < target->Descriptors.Count(); ++i)
								ret->Parallel(Generate(target->Descriptors[i]));
							return ret;
						}
						NFA* GenerateRepeat(Descriptor* value)
						{
							RepeatDescriptor* target = static_cast<RepeatDescriptor*>(value);
							NFA*ret = NewNFA();
							ret->BuildNode(nullptr);
							NFA*tail = nullptr;

							if (target->Min == 1)
							{
								NFA* node = Generate(target->Target);
								NFA::Connect(NewEdge(), ret->Head, node->Head);
								tail = node;
							}
							else if (target->Min > 1)
							{
								//��ͷ
								NFA* node = Generate(target->Target);
								NFA::Connect(NewEdge(), ret->Head, node->Head);
								NFA*prev = node;
								for (size_t i = target->Min - 2; i > 0; i--)//�м�ڵ�
								{
									NFA*mid = Generate(target->Target);
									prev->Joint(mid);
									prev = mid;
								}
								//��β
								tail = Generate(target->Target);
								prev->Joint(tail);
							}

							if (target->Max - target->Min == 1)
							{
								NFA*node = Generate(target->Target);
								NFA*greedy_wrapper = NewNFA();
								greedy_wrapper->BuildNode(nullptr);
								//��ǰһ���ڵ���йҽ�
								if (tail == nullptr)
									NFA::Connect(NewEdge(), ret->Head, greedy_wrapper->Head);
								else
									tail->Joint(greedy_wrapper);
								//��β����
								NFA::Connect(NewEdge(), greedy_wrapper->Tail, ret->Tail);

								if (target->Greedy)//̰��ʱ˳�򽻻�
								{
									greedy_wrapper->Parallel(node);//���߽ڵ�
									NFA::Connect(NewEdge(), greedy_wrapper->Head, ret->Tail);//�ٿ�����
								}
								else
								{
									NFA::Connect(NewEdge(), greedy_wrapper->Head, ret->Tail);//�ȿ�����
									greedy_wrapper->Parallel(node);//���߽ڵ�
								}
							}
							else if (target->Max == std::numeric_limits<size_t>::max())
							{
								NFA*node = Generate(target->Target);
								Status*hollow = NewStatus();//�սڵ㣬����NFA�������ι���ͷ��
								if (target->Greedy)//̰��
								{
									NFA::Connect(NewEdge(), hollow, node->Head);//���߽ڵ�
									NFA::Connect(NewEdge(), hollow, ret->Tail);//��������
								}
								else
								{
									NFA::Connect(NewEdge(), hollow, ret->Tail);//��������
									NFA::Connect(NewEdge(), hollow, node->Head);//�ٴ����߽ڵ�
								}
								NFA::Connect(NewEdge(), node->Tail, hollow);
								if (tail == nullptr)
									NFA::Connect(NewEdge(), ret->Head, hollow);
								else
									NFA::Connect(NewEdge(), tail->Tail, hollow);

							}
							else if (target->Max - target->Min > 1)
							{
								NFA*node = Generate(target->Target);
								NFA* head_greedy_wrapper = NewNFA();
								head_greedy_wrapper->BuildNode(nullptr);
								//�ҽ�ǰ��ڵ�
								if (tail == nullptr)
									NFA::Connect(NewEdge(), ret->Head, head_greedy_wrapper->Head);
								else
									tail->Joint(head_greedy_wrapper);
								if (target->Greedy)//̰������
								{
									head_greedy_wrapper->Parallel(node);
									NFA::Connect(NewEdge(), node->Head, ret->Tail);//������
								}
								else
								{
									NFA::Connect(NewEdge(), node->Head, ret->Tail);//������
									head_greedy_wrapper->Parallel(node);
								}
								NFA*prev = head_greedy_wrapper;
								for (size_t i = target->Max - target->Min - 2; i > 0; i--)//�м�ڵ�
								{
									NFA*mid = Generate(target->Target);
									NFA*mid_greedy_wrapper = NewNFA();
									mid_greedy_wrapper->BuildNode(nullptr);
									prev->Joint(mid);
									if (target->Greedy)//̰�����򽻻�
									{
										mid_greedy_wrapper->Parallel(mid);
										NFA::Connect(NewEdge(), mid->Head, ret->Tail);//������
									}
									else
									{
										NFA::Connect(NewEdge(), mid->Head, ret->Tail);//������
										mid_greedy_wrapper->Parallel(mid);
									}
									prev = mid_greedy_wrapper;
								}
								tail = Generate(target->Target);
								NFA*tail_greedy_wrapper = NewNFA();
								tail_greedy_wrapper->BuildNode(nullptr);
								prev->Joint(tail_greedy_wrapper);
								if (target->Greedy)
								{
									tail_greedy_wrapper->Parallel(tail);
									NFA::Connect(NewEdge(), tail_greedy_wrapper->Head, ret->Tail);//������
								}
								else
								{
									NFA::Connect(NewEdge(), tail_greedy_wrapper->Head, ret->Tail);//������
									tail_greedy_wrapper->Parallel(tail);
								}
								NFA::Connect(NewEdge(), tail_greedy_wrapper->Tail, ret->Tail);//��β����
							}
							else if (target->Max - target->Min == 0)
							{
								NFA::Connect(NewEdge(), tail->Tail, ret->Tail);
							}
							return ret;
						}
						NFA* GenerateSet(Descriptor* value)
						{
							ContentDescriptor* target = static_cast<ContentDescriptor*>(value);
							NFA* ret = NewNFA();
							ret->BuildNode(nullptr);
							ret->Parallel(SetCapture(NewEdge(target->ContentPtr)));
							return ret;
						}
						NFA* GenrateAndSimplify(Descriptor* value)
						{
							NFA* raw = Generate(value);
							raw->Head->Valid = true;
							raw->Tail->Valid = true;
							raw->Tail->Final = true;
							return NFASimplify::SimplifyNFA(this->m_resource, raw);
						}
					};
					//ƥ����
					class MatchResult
					{
						size_t m_index;
						size_t m_length;
						Collection::Generic::Dictionary<WString, WString> m_named_capture;
						Collection::Generic::List<WString> m_anonymous_capture;
					public:
						MatchResult(size_t Index, size_t Length, CapturePackage const& Capture) :m_index(Index), m_length(Length) {
							for (auto&&var : Capture.NamedCapture)
								m_named_capture.Add(var.Key, WString::From(var.Value.Base, var.Value.Size));
							for (size_t i = 0; i < Capture.AnonymousCapture.Count(); ++i)
								m_anonymous_capture.Add(std::move(WString::From(Capture.AnonymousCapture[i].Base, Capture.AnonymousCapture[i].Size)));
						}
						WString const& operator[](size_t Index)const {
							return m_anonymous_capture[Index];
						}
						WString const& operator[](WString const&Key)const {
							return m_named_capture[Key];
						}
						WString const& GetAnonymousCapture(size_t Index)const {
							return m_anonymous_capture[Index];
						}
						WString const& GetNamedCapture(WString const&Key)const {
							return m_named_capture[Key];
						}
					};

					//NFAƥ��
					class NFAMatch
					{
					public:
						static void CaptureGo(Edge*Target, const wchar_t*Current, CapturePackage*Package) {
							for (size_t i = 0; i < Target->Actions.Count(); ++i)
								Target->Actions[i]->Go(Package, Current);
						}
						static void CaptureWithdraw(Edge*Target, CapturePackage*Package) {
							for (size_t i = 0; i < Target->Actions.Count(); ++i)
								Target->Actions[i]->WithDraw(Package);
						}
						static const wchar_t* Match(const wchar_t*Target, const wchar_t*Top, Status*Where, CapturePackage*Package)
						{
							if (Target == Top)
							{
								if (Where->Final || Where->EquivalentFinal)
									return Target;
								else
									return nullptr;
							}
							if (Where->Final)
								return Target;
							for (size_t i = 0; i < Where->OutEdges.Count(); ++i) {
								if (Where->OutEdges[i]->Content == nullptr)
								{
									const wchar_t*result = Match(Target, Top, Where->OutEdges[i]->To, Package);
									if (result != nullptr)
										return result;
								}
								else if (Where->OutEdges[i]->Content->Accept(*Target))
								{
									if (Package != nullptr)
										CaptureGo(Where->OutEdges[i], Target, Package);
									const wchar_t*result = Match(Target + 1, Top, Where->OutEdges[i]->To, Package);
									if (result != nullptr)
										return result;
									else
										CaptureWithdraw(Where->OutEdges[i], Package);
								}
							}
							return nullptr;
						}
						static Pointer::ptr<MatchResult> MatchFirst(const wchar_t*Target, const wchar_t*Top, Status*Where, size_t AnonymousLength, Collection::Generic::List<WString> const&NamedTable)
						{
							const wchar_t*iter = Target;
							CapturePackage package(Target, AnonymousLength, NamedTable);
							while (iter != Top)
							{
								const wchar_t* ret = Match(iter, Top, Where, &package);
								if (ret != nullptr)
									return Pointer::Reference::newptr<MatchResult>(iter - Target, ret - Target, package);
								iter++;
							}
							return nullptr;
						}
						static bool IsMatchFirst(const wchar_t*Target, const wchar_t*Top, Status*Where)
						{
							const wchar_t*iter = Target;
							while (iter != Top)
							{
								if (Match(iter, Top, Where, nullptr) != nullptr)
									return true;
								iter++;
							}
							return false;
						}
						static inline bool IsMatch(const wchar_t*Target, const wchar_t*Top, Status*Where)
						{
							return Match(Target, Top, Where, nullptr) != nullptr;
						}
						static Pointer::ptr<Collection::Generic::List<Pointer::ptr<MatchResult>>> Matches(const wchar_t*Target, const wchar_t*Top, Status*Where, size_t AnonymousLength, Collection::Generic::List<WString> const&NamedTable) {
							const wchar_t*iter = Target;
							CapturePackage package(Target, AnonymousLength, NamedTable);
							Pointer::ptr<Collection::Generic::List<Pointer::ptr<MatchResult>>> ret = Pointer::Reference::newptr<Collection::Generic::List<Pointer::ptr<MatchResult>>>();
							while (iter != Top)
							{
								const wchar_t* sub_ret = Match(iter, Top, Where, &package);
								if (sub_ret != nullptr)
								{
									ret->Add(Pointer::Reference::newptr<MatchResult>(iter - Target, sub_ret - Target, package));
									iter = sub_ret;
									package.SetToZero();
								}
								else
									iter++;
							}
							return ret;
						}
					};
					//������ʽ
					class Regex
					{
						Pointer::ptr<RegexParser> m_parser;
						Pointer::ptr<NFAGenerator> m_generator;
						Pointer::ptr<NFAResource> m_resource;
						NFA* m_enter;
					public:
						Regex(WString const&Pattern) {
							m_parser = Pointer::Reference::newptr<RegexParser>(Pattern);
							m_resource = Pointer::Reference::newptr<NFAResource>();
							m_generator = Pointer::Reference::newptr<NFAGenerator>(m_resource.GetObjectPtr());
							m_enter = m_generator->GenrateAndSimplify(m_parser->Parse());
						}
						Regex(Regex&&) = default;
						Regex(Regex const&) = default;
						Regex&operator=(Regex const&) = default;
						Regex&operator=(Regex&&) = default;
						~Regex() = default;

						//�Ƿ�ƥ��Ŀ���ַ���(����������ַ��������Լӿ��ٶ�)
						inline bool IsMatch(WString const&Target)const {
							return NFAMatch::IsMatch(Target.GetNativePtr(), Target.GetNativePtr() + Target.Length(), m_enter->Head);
						}
						//�Ƿ�ƥ��Ŀ���ַ���(����������ַ��������Լӿ��ٶ�)
						inline bool IsMatchFirst(WString const&Target)const {
							return NFAMatch::IsMatchFirst(Target.GetNativePtr(), Target.GetNativePtr() + Target.Length(), m_enter->Head);
						}
						//ƥ���һ���ַ���
						inline Pointer::ptr<MatchResult> MatchFirst(WString const&Target)const {
							return NFAMatch::MatchFirst(Target.GetNativePtr(), Target.GetNativePtr() + Target.Length(), m_enter->Head, m_parser->GetAnonymousCaptureCount(), m_parser->GetNamedCaptureList());
						}
						inline Pointer::ptr<Collection::Generic::List<Pointer::ptr<MatchResult>>> Matches(WString const&Target)const {
							return NFAMatch::Matches(Target.GetNativePtr(), Target.GetNativePtr() + Target.Length(), m_enter->Head, m_parser->GetAnonymousCaptureCount(), m_parser->GetNamedCaptureList());
						}
					};
#endif
				}
			}
		}
	}
}
