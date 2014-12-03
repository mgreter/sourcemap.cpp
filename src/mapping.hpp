#ifndef SOURCEMAP_MAPPING
#define SOURCEMAP_MAPPING

#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>

#include "row.hpp"

// using std::string
using namespace std;

// add namespace for c++
namespace SourceMap
{

	class Mapping
	{
		friend class Entry;
		friend class Row;
		friend class SrcMap;
		public: // ctor
			Mapping(size_t version = 3);
			// we only implement v3 currently
			Mapping(string VLQ, size_t version = 3);
		public: // operators
			friend ostream& operator<<(ostream& os, const Mapping& map);
		public: // setters
			void addNewLine();
		public: // getters
			const Row getRow(size_t idx) const;
			size_t getRowCount() const { return rows.size(); };
			size_t getColCount() const { return last_ln_col; };
			SrcMapPos getSize() const {
				return SrcMapPos(
					getRowCount(),
					getColCount()
				);
			};
		public: // getters
			const string serialize() const;
		public: // route to row
			vector<Entry> at(size_t row, size_t col) const
			{ return getRow(row).at(col); };
			vector<Entry> at(const SrcMapPos& pos) const
			{ return getRow(pos.row).at(pos.col); };
		public: // variables
			// SrcMapPos size;
			vector<Row> rows;
		private: // variables
			size_t version;
			size_t last_ln_col;
			void init(const string& VLQ);
	};

}
// EO namespace

#endif
