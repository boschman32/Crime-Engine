#pragma once
#include "IExample.h"

namespace ce
{
	namespace examples
	{
		class ExampleRender :
			public IExample
		{
		public:
			ExampleRender() = default;
			virtual ~ExampleRender() = default;

			void RunExample() override;

		protected:
		private:
		};
	}
}