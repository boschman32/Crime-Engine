#pragma once
#include "IExample.h"
namespace ce
{
	namespace examples
	{
		class ExampleAnimation :
			public IExample
		{
		public:
			ExampleAnimation() = default;
			virtual ~ExampleAnimation() = default;

			void RunExample() override;
		};
	}
}