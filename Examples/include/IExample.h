#pragma once

namespace ce
{
	namespace examples
	{
		class IExample
		{
		public:
			virtual ~IExample() = default;
			
			virtual void RunExample() = 0;
		};
	}
}