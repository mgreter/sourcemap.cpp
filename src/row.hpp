#ifndef SOURCEMAP_ROW
#define SOURCEMAP_ROW

#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>

#include "entry.hpp"

// using std::string
using namespace std;

// add namespace for c++
namespace SourceMap
{

	class Row
	{
		friend class Entry;
		friend class Mapping;
		friend class SrcMap;
		public: // ctor
			Row();
		public: // setters
			void addEntry(Entry entry);
		public: // getters
			size_t getLength() const;
			size_t getEntryCount() const { return getLength(); };
			const Entry getEntry(size_t idx) const;
			const vector<Entry> at(size_t col) const;
		public: // variables
			vector<Entry> entries;
	};

}
// EO namespace

#endif
