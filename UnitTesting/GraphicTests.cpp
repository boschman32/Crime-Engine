#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GraphicsTests
{
	TEST_CLASS(DummyClass)
	{
	public:

		TEST_METHOD(DummyTest)
		{
			Assert::IsTrue(true);
		}
	};
}
