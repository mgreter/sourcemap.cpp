//some_test.cpp

#include <iostream>
#include <stdexcept>
#include "document.hpp"
#include "UnitTest++.h"
#include "ReportAssert.h"

using namespace SourceMap;

SUITE(Basic)
{

	SrcMapDocSP srcmap = make_shared<SrcMapDoc>();

	TEST(SrcMapPosInit)
	{
		SrcMapPos pos;
		pos = SrcMapPos("");
		CHECK_EQUAL(size_t(0), pos.row);
		CHECK_EQUAL(size_t(0), pos.col);
		pos = SrcMapPos("\n");
		CHECK_EQUAL(size_t(1), pos.row);
		CHECK_EQUAL(size_t(0), pos.col);
		pos = SrcMapPos("foobar");
		CHECK_EQUAL(size_t(0), pos.row);
		CHECK_EQUAL(size_t(6), pos.col);
		pos = SrcMapPos("foo\nbar");
		CHECK_EQUAL(size_t(1), pos.row);
		CHECK_EQUAL(size_t(3), pos.col);
		pos = SrcMapPos("foo\nbar\n");
		CHECK_EQUAL(size_t(2), pos.row);
		CHECK_EQUAL(size_t(0), pos.col);
	}

	TEST(SrcMapPosPlus)
	{
		SrcMapPos pos;
		SrcMapPos pos1(0, 0);
		SrcMapPos pos2(1, 0);
		SrcMapPos pos3(0, 6);
		SrcMapPos pos4(1, 3);
		SrcMapPos pos5(2, 0);

		pos = pos1 + pos2;
		CHECK_EQUAL(size_t(1), pos.row);
		CHECK_EQUAL(size_t(0), pos.col);
		pos = pos1 + pos3;
		CHECK_EQUAL(size_t(0), pos.row);
		CHECK_EQUAL(size_t(6), pos.col);
		pos = pos1 + pos4;
		CHECK_EQUAL(size_t(1), pos.row);
		CHECK_EQUAL(size_t(3), pos.col);
		pos = pos1 + pos5;
		CHECK_EQUAL(size_t(2), pos.row);
		CHECK_EQUAL(size_t(0), pos.col);

		pos = pos2 + pos1;
		CHECK_EQUAL(size_t(1), pos.row);
		CHECK_EQUAL(size_t(0), pos.col);
		pos = pos2 + pos3;
		CHECK_EQUAL(size_t(1), pos.row);
		CHECK_EQUAL(size_t(6), pos.col);
		pos = pos2 + pos4;
		CHECK_EQUAL(size_t(2), pos.row);
		CHECK_EQUAL(size_t(3), pos.col);
		pos = pos2 + pos5;
		CHECK_EQUAL(size_t(3), pos.row);
		CHECK_EQUAL(size_t(0), pos.col);

		pos = pos3 + pos1;
		CHECK_EQUAL(size_t(0), pos.row);
		CHECK_EQUAL(size_t(6), pos.col);
		pos = pos3 + pos2;
		CHECK_EQUAL(size_t(1), pos.row);
		CHECK_EQUAL(size_t(0), pos.col);
		pos = pos3 + pos4;
		CHECK_EQUAL(size_t(1), pos.row);
		CHECK_EQUAL(size_t(3), pos.col);
		pos = pos3 + pos5;
		CHECK_EQUAL(size_t(2), pos.row);
		CHECK_EQUAL(size_t(0), pos.col);

		pos = pos4 + pos1;
		CHECK_EQUAL(size_t(1), pos.row);
		CHECK_EQUAL(size_t(3), pos.col);
		pos = pos4 + pos2;
		CHECK_EQUAL(size_t(2), pos.row);
		CHECK_EQUAL(size_t(0), pos.col);
		pos = pos4 + pos3;
		CHECK_EQUAL(size_t(1), pos.row);
		CHECK_EQUAL(size_t(9), pos.col);
		pos = pos4 + pos5;
		CHECK_EQUAL(size_t(3), pos.row);
		CHECK_EQUAL(size_t(0), pos.col);

		pos = pos5 + pos1;
		CHECK_EQUAL(size_t(2), pos.row);
		CHECK_EQUAL(size_t(0), pos.col);
		pos = pos5 + pos2;
		CHECK_EQUAL(size_t(3), pos.row);
		CHECK_EQUAL(size_t(0), pos.col);
		pos = pos5 + pos3;
		CHECK_EQUAL(size_t(2), pos.row);
		CHECK_EQUAL(size_t(6), pos.col);
		pos = pos5 + pos4;
		CHECK_EQUAL(size_t(3), pos.row);
		CHECK_EQUAL(size_t(3), pos.col);

	}

	TEST(SourceSize)
	{
		srcmap->addSource("src.css");
		CHECK_EQUAL(size_t(1), srcmap->getSourceSize());
	}

	TEST(TokenSize)
	{
		srcmap->addToken("token");
		CHECK_EQUAL(size_t(1), srcmap->getTokenSize());
	}

	void testMe(MappingsSP map)
	{
			LineMapSP rowA = map->getLineMap(0);
	}

	TEST(SomeTest)
	{
		MappingsSP map = make_shared<Mappings>();
		map->addNewLine(1);
		{
			LineMapSP rowA = map->getLineMap(0);
			rowA = map->getLineMap(0);
			std::cerr << "#########\n";
			rowA = map->getLineMap(0);
		}
		LineMapSP rowB = map->getLineMap(0);
	}

	TEST(InsertEntry)
	{
		srcmap->insert(0, make_shared<ColMap>(0, 0, 0, 1, 0));
		ColMapSP entry = srcmap->getColMap(0, 0);
		srcmap->setLastLineLength(0);
		CHECK_EQUAL(size_t(0), entry->getCol());
		CHECK_EQUAL(size_t(0), entry->getToken());
		CHECK_EQUAL(size_t(0), entry->getSrcLine());
		CHECK_EQUAL(size_t(1), entry->getSrcCol());
		CHECK_EQUAL(size_t(0), entry->getToken());
	}

	TEST(SpliceInsert01)
	{

		SrcMapDocSP insert = make_shared<SrcMapDoc>();
		insert->addSource("src.css");
		insert->addToken("token");
		insert->insert(0, make_shared<ColMap>(1, 0, 0, 2, 0));
		insert->setLastLineLength(2);

		srcmap->insert(SrcMapPos(0, 0), *insert);

		CHECK_EQUAL(size_t(1), srcmap->map->getRowCount());
		CHECK_EQUAL(size_t(2), srcmap->map->getLineMap(0)->getLength());

		ColMapSP entry1 = srcmap->getColMap(0, 0);
		CHECK_EQUAL(size_t(1), entry1->getCol());
		CHECK_EQUAL(size_t(0), entry1->getToken());
		CHECK_EQUAL(size_t(0), entry1->getSrcLine());
		CHECK_EQUAL(size_t(2), entry1->getSrcCol());
		CHECK_EQUAL(size_t(0), entry1->getToken());

		ColMapSP entry2 = srcmap->getColMap(0, 1);
		CHECK_EQUAL(size_t(2), entry2->getCol());
		CHECK_EQUAL(size_t(0), entry2->getToken());
		CHECK_EQUAL(size_t(0), entry2->getSrcLine());
		CHECK_EQUAL(size_t(1), entry2->getSrcCol());
		CHECK_EQUAL(size_t(0), entry2->getToken());

	}

	TEST(SpliceInsert02)
	{

		SrcMapDocSP insert = make_shared<SrcMapDoc>();
		insert->addSource("src.css");
		insert->addToken("token");
		insert->insert(0, make_shared<ColMap>(0, 0, 0, 3, 0));
		insert->setLastLineLength(3);
		srcmap->insert(SrcMapPos(0, 1), *insert);

		CHECK_EQUAL(size_t(1), srcmap->map->getRowCount());
		CHECK_EQUAL(size_t(3), srcmap->map->getLineMap(0)->getLength());

		// check that we did not modify insert
		ColMapSP entry1 = insert->getColMap(0, 0);
		CHECK_EQUAL(size_t(1), entry1->getCol());
		CHECK_EQUAL(size_t(0), entry1->getToken());
		CHECK_EQUAL(size_t(0), entry1->getSrcLine());
		CHECK_EQUAL(size_t(3), entry1->getSrcCol());
		CHECK_EQUAL(size_t(0), entry1->getToken());

		ColMapSP entry3 = srcmap->getColMap(0, 0);
		CHECK_EQUAL(size_t(1), entry3->getCol());
		CHECK_EQUAL(size_t(0), entry3->getToken());
		CHECK_EQUAL(size_t(0), entry3->getSrcLine());
		CHECK_EQUAL(size_t(3), entry3->getSrcCol());
		CHECK_EQUAL(size_t(0), entry3->getToken());

		ColMapSP entry4 = srcmap->getColMap(0, 1);
		CHECK_EQUAL(size_t(4), entry4->getCol());
		CHECK_EQUAL(size_t(0), entry4->getToken());
		CHECK_EQUAL(size_t(0), entry4->getSrcLine());
		CHECK_EQUAL(size_t(2), entry4->getSrcCol());
		CHECK_EQUAL(size_t(0), entry4->getToken());

		ColMapSP entry5 = srcmap->getColMap(0, 2);
		CHECK_EQUAL(size_t(5), entry5->getCol());
		CHECK_EQUAL(size_t(0), entry5->getToken());
		CHECK_EQUAL(size_t(0), entry5->getSrcLine());
		CHECK_EQUAL(size_t(1), entry5->getSrcCol());
		CHECK_EQUAL(size_t(0), entry5->getToken());

		// check that we did not modify insert
		ColMapSP entry2 = insert->getColMap(0, 0);
		CHECK_EQUAL(size_t(1), entry2->getCol());
		CHECK_EQUAL(size_t(0), entry2->getToken());
		CHECK_EQUAL(size_t(0), entry2->getSrcLine());
		CHECK_EQUAL(size_t(3), entry2->getSrcCol());
		CHECK_EQUAL(size_t(0), entry2->getToken());

	}

	TEST(SpliceInsert03)
	{

		SrcMapDocSP insert = make_shared<SrcMapDoc>();
		insert->addSource("src.css");
		insert->addToken("token");
		insert->map->addNewLine();
		insert->setLastLineLength(1);
		srcmap->insert(SrcMapPos(0, 2), *insert);

		CHECK_EQUAL(size_t(2), srcmap->map->getRowCount());
		CHECK_EQUAL(size_t(1), srcmap->map->getLineMap(0)->getLength());
		CHECK_EQUAL(size_t(2), srcmap->map->getLineMap(1)->getLength());

		ColMapSP entry3 = srcmap->getColMap(0, 0);
		CHECK_EQUAL(size_t(1), entry3->getCol());
		CHECK_EQUAL(size_t(0), entry3->getToken());
		CHECK_EQUAL(size_t(0), entry3->getSrcLine());
		CHECK_EQUAL(size_t(3), entry3->getSrcCol());
		CHECK_EQUAL(size_t(0), entry3->getToken());

		ColMapSP entry4 = srcmap->getColMap(1, 0);
		CHECK_EQUAL(size_t(3), entry4->getCol());
		CHECK_EQUAL(size_t(0), entry4->getToken());
		CHECK_EQUAL(size_t(0), entry4->getSrcLine());
		CHECK_EQUAL(size_t(2), entry4->getSrcCol());
		CHECK_EQUAL(size_t(0), entry4->getToken());

		ColMapSP entry5 = srcmap->getColMap(1, 1);
		CHECK_EQUAL(size_t(4), entry5->getCol());
		CHECK_EQUAL(size_t(0), entry5->getToken());
		CHECK_EQUAL(size_t(0), entry5->getSrcLine());
		CHECK_EQUAL(size_t(1), entry5->getSrcCol());
		CHECK_EQUAL(size_t(0), entry5->getToken());

	}

	TEST(SpliceDelete01)
	{

		CHECK_EQUAL(size_t(2), srcmap->map->getRowCount());
		srcmap->remove(SrcMapPos(0, 0), SrcMapPos(0, 1));
		CHECK_EQUAL(size_t(2), srcmap->map->getRowCount());
		CHECK_EQUAL(size_t(1), srcmap->map->getLineMap(0)->getLength());
		CHECK_EQUAL(size_t(2), srcmap->map->getLineMap(1)->getLength());

		ColMapSP entry3 = srcmap->getColMap(0, 0);
		CHECK_EQUAL(size_t(0), entry3->getCol());
		CHECK_EQUAL(size_t(0), entry3->getToken());
		CHECK_EQUAL(size_t(0), entry3->getSrcLine());
		CHECK_EQUAL(size_t(3), entry3->getSrcCol());
		CHECK_EQUAL(size_t(0), entry3->getToken());

	}

	TEST(SpliceDelete02)
	{

		CHECK_EQUAL(size_t(2), srcmap->map->getRowCount());
		srcmap->remove(SrcMapPos(0, 0), SrcMapPos(0, 1));
		CHECK_EQUAL(size_t(2), srcmap->map->getRowCount());
		CHECK_EQUAL(size_t(0), srcmap->map->getLineMap(0)->getLength());
		CHECK_EQUAL(size_t(2), srcmap->map->getLineMap(1)->getLength());

		ColMapSP entry4 = srcmap->getColMap(1, 0);
		CHECK_EQUAL(size_t(3), entry4->getCol());
		CHECK_EQUAL(size_t(0), entry4->getToken());
		CHECK_EQUAL(size_t(0), entry4->getSrcLine());
		CHECK_EQUAL(size_t(2), entry4->getSrcCol());
		CHECK_EQUAL(size_t(0), entry4->getToken());

		ColMapSP entry5 = srcmap->getColMap(1, 1);
		CHECK_EQUAL(size_t(4), entry5->getCol());
		CHECK_EQUAL(size_t(0), entry5->getToken());
		CHECK_EQUAL(size_t(0), entry5->getSrcLine());
		CHECK_EQUAL(size_t(1), entry5->getSrcCol());
		CHECK_EQUAL(size_t(0), entry5->getToken());

	}

	TEST(SpliceDelete03)
	{

		CHECK_EQUAL(size_t(2), srcmap->map->getRowCount());
		srcmap->remove(SrcMapPos(1, 3), SrcMapPos(0, 1));
		CHECK_EQUAL(size_t(2), srcmap->map->getRowCount());
		CHECK_EQUAL(size_t(0), srcmap->map->getLineMap(0)->getLength());
		CHECK_EQUAL(size_t(1), srcmap->map->getLineMap(1)->getLength());

		ColMapSP entry4 = srcmap->getColMap(1, 0);
		CHECK_EQUAL(size_t(3), entry4->getCol());
		CHECK_EQUAL(size_t(0), entry4->getToken());
		CHECK_EQUAL(size_t(0), entry4->getSrcLine());
		CHECK_EQUAL(size_t(1), entry4->getSrcCol());
		CHECK_EQUAL(size_t(0), entry4->getToken());

	}

	TEST(SpliceDelete04)
	{

		CHECK_EQUAL(size_t(2), srcmap->map->getRowCount());
		srcmap->remove(SrcMapPos(0, 1), SrcMapPos(1, 2));
		CHECK_EQUAL(size_t(1), srcmap->map->getRowCount());
		CHECK_EQUAL(size_t(1), srcmap->map->getLineMap(0)->getLength());

		ColMapSP entry4 = srcmap->getColMap(0, 0);
		CHECK_EQUAL(size_t(1), entry4->getCol());
		CHECK_EQUAL(size_t(0), entry4->getToken());
		CHECK_EQUAL(size_t(0), entry4->getSrcLine());
		CHECK_EQUAL(size_t(1), entry4->getSrcCol());
		CHECK_EQUAL(size_t(0), entry4->getToken());

	}

	TEST(ThrowErrors)
	{
		CHECK_THROW(srcmap->remove(SrcMapPos(0, 0), SrcMapPos(9, 9)), out_of_range);
		CHECK_THROW(srcmap->remove(SrcMapPos(0, 0), SrcMapPos(-1, 0)), invalid_argument);
		CHECK_THROW(srcmap->remove(SrcMapPos(0, 0), SrcMapPos(0, -1)), invalid_argument);

	}

}

int main()
{
	return UnitTest::RunAllTests();
}