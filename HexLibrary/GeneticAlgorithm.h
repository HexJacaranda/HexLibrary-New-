#pragma once
namespace HL
{
	namespace Experimental
	{
		namespace GeneticAlgorithm
		{
			template<class DataT,class SelectorT,class EvaluatorT,class CrossOverT>
			class Engine
			{
				DataT m_data;
				SelectorT m_selector;
				EvaluatorT m_evaluator;
				
			public:

			};
		}
	}
}