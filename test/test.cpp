	//some_test.cpp

#include <iostream>
#include <stdexcept>
#include "../sourcemap.h"
#include "../UnitTest++/src/UnitTest++.h"
#include "../UnitTest++/src/ReportAssert.h"

using namespace SourceMap;

SUITE(Basic)
{

	SrcMap* srcmap = new SrcMap();

	TEST(SourceSize)
	{
		srcmap->addSource("src.css");
		CHECK_EQUAL(1, srcmap->getSourceSize());
	}

	TEST(TokenSize)
	{
		srcmap->addToken("token");
		CHECK_EQUAL(1, srcmap->getTokenSize());
	}

	TEST(InsertEntry)
	{
		srcmap->insert(0, Entry(0, 0, 0, 1, 0));
		Entry entry = srcmap->getEntry(0, 0);
		srcmap->setLastLineLength(0);
		CHECK_EQUAL(0, entry.getCol());
		CHECK_EQUAL(0, entry.getToken());
		CHECK_EQUAL(0, entry.getSrcLine());
		CHECK_EQUAL(1, entry.getSrcCol());
		CHECK_EQUAL(0, entry.getToken());
	}

	TEST(SpliceInsert01)
	{

		SrcMap* insert = new SrcMap();
		insert->addSource("src.css");
		insert->addToken("token");
		insert->insert(0, Entry(1, 0, 0, 2, 0));
		insert->setLastLineLength(2);
		srcmap->splice(SrcMapPos(0, 0), insert);

		CHECK_EQUAL(1, srcmap->map->getLength());
		CHECK_EQUAL(2, srcmap->map->getRow(0)->getLength());

		Entry entry1 = srcmap->getEntry(0, 0);
		CHECK_EQUAL(1, entry1.getCol());
		CHECK_EQUAL(0, entry1.getToken());
		CHECK_EQUAL(0, entry1.getSrcLine());
		CHECK_EQUAL(2, entry1.getSrcCol());
		CHECK_EQUAL(0, entry1.getToken());

		Entry entry2 = srcmap->getEntry(0, 1);
		CHECK_EQUAL(2, entry2.getCol());
		CHECK_EQUAL(0, entry2.getToken());
		CHECK_EQUAL(0, entry2.getSrcLine());
		CHECK_EQUAL(1, entry2.getSrcCol());
		CHECK_EQUAL(0, entry2.getToken());

	}

	TEST(SpliceInsert02)
	{

		SrcMap* insert = new SrcMap();
		insert->addSource("src.css");
		insert->addToken("token");
		insert->insert(0, Entry(0, 0, 0, 3, 0));
		insert->setLastLineLength(3);
		srcmap->splice(SrcMapPos(0, 1), insert);

		CHECK_EQUAL(1, srcmap->map->getLength());
		CHECK_EQUAL(3, srcmap->map->getRow(0)->getLength());

		Entry entry3 = srcmap->getEntry(0, 0);
		CHECK_EQUAL(1, entry3.getCol());
		CHECK_EQUAL(0, entry3.getToken());
		CHECK_EQUAL(0, entry3.getSrcLine());
		CHECK_EQUAL(3, entry3.getSrcCol());
		CHECK_EQUAL(0, entry3.getToken());

		Entry entry4 = srcmap->getEntry(0, 1);
		CHECK_EQUAL(3, entry4.getCol());
		CHECK_EQUAL(0, entry4.getToken());
		CHECK_EQUAL(0, entry4.getSrcLine());
		CHECK_EQUAL(2, entry4.getSrcCol());
		CHECK_EQUAL(0, entry4.getToken());

		Entry entry5 = srcmap->getEntry(0, 2);
		CHECK_EQUAL(4, entry5.getCol());
		CHECK_EQUAL(0, entry5.getToken());
		CHECK_EQUAL(0, entry5.getSrcLine());
		CHECK_EQUAL(1, entry5.getSrcCol());
		CHECK_EQUAL(0, entry5.getToken());

	}


	TEST(SpliceInsert03)
	{

		SrcMap* insert = new SrcMap();
		insert->addSource("src.css");
		insert->addToken("token");
		insert->map->addNewLine();
		insert->setLastLineLength(1);
		srcmap->splice(SrcMapPos(0, 2), insert);

		CHECK_EQUAL(2, srcmap->map->getLength());
		CHECK_EQUAL(1, srcmap->map->getRow(0)->getLength());
		CHECK_EQUAL(2, srcmap->map->getRow(1)->getLength());

		Entry entry3 = srcmap->getEntry(0, 0);
		CHECK_EQUAL(1, entry3.getCol());
		CHECK_EQUAL(0, entry3.getToken());
		CHECK_EQUAL(0, entry3.getSrcLine());
		CHECK_EQUAL(3, entry3.getSrcCol());
		CHECK_EQUAL(0, entry3.getToken());

		Entry entry4 = srcmap->getEntry(1, 0);
		CHECK_EQUAL(2, entry4.getCol());
		CHECK_EQUAL(0, entry4.getToken());
		CHECK_EQUAL(0, entry4.getSrcLine());
		CHECK_EQUAL(2, entry4.getSrcCol());
		CHECK_EQUAL(0, entry4.getToken());

		Entry entry5 = srcmap->getEntry(1, 1);
		CHECK_EQUAL(3, entry5.getCol());
		CHECK_EQUAL(0, entry5.getToken());
		CHECK_EQUAL(0, entry5.getSrcLine());
		CHECK_EQUAL(1, entry5.getSrcCol());
		CHECK_EQUAL(0, entry5.getToken());

	}

	TEST(SpliceDelete02)
	{

		SrcMap* insert = new SrcMap();
		insert->setLastLineLength(0);
		CHECK_EQUAL(2, srcmap->map->getLength());
		srcmap->splice(SrcMapPos(0, 0), SrcMapPos(1, 3));
		delete insert;
		CHECK_EQUAL(1, srcmap->map->getLength());
		CHECK_EQUAL(1, srcmap->map->getRow(0)->getLength());

		Entry entry4 = srcmap->getEntry(0, 0);
		CHECK_EQUAL(0, entry4.getCol());
		CHECK_EQUAL(0, entry4.getToken());
		CHECK_EQUAL(0, entry4.getSrcLine());
		CHECK_EQUAL(1, entry4.getSrcCol());
		CHECK_EQUAL(0, entry4.getToken());

	}

	TEST(ThrowErrors)
	{
		CHECK_THROW(srcmap->splice(SrcMapPos(9, 9), SrcMapPos(0, 0)), out_of_range);
		CHECK_THROW(srcmap->splice(SrcMapPos(0, 0), SrcMapPos(9, 9)), out_of_range);
		CHECK_THROW(srcmap->splice(SrcMapPos(-1, 0), SrcMapPos(0, 0)), invalid_argument);
		CHECK_THROW(srcmap->splice(SrcMapPos(0, -1), SrcMapPos(0, 0)), invalid_argument);
		CHECK_THROW(srcmap->splice(SrcMapPos(0, 0), SrcMapPos(-1, 0)), invalid_argument);
		CHECK_THROW(srcmap->splice(SrcMapPos(0, 0), SrcMapPos(0, -1)), invalid_argument);
	}

}

int main()
{
	return UnitTest::RunAllTests();
}