//some_test.cpp

#include "../UnitTest++/src/UnitTest++.h"
#include "../UnitTest++/src/ReportAssert.h"

TEST(Demonstration)
{
	CHECK(true);
}

int main()
{
	return UnitTest::RunAllTests();
}