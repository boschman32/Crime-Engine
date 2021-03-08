#pragma once
#include "IExample.h"

namespace ce
{
	class IEvent;
	
	namespace examples
	{
		class TestEvent;

		class ExampleEvents : public IExample
		{
		public:
			void RunExample() override;

			void OnTestEvent(TestEvent& a_event) const;
		};
	}
}
