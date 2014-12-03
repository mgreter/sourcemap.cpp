// include library
#include <omp.h>
#include <stdexcept>

#include "row.hpp"

// using string
using namespace std;

// add namespace for c++
namespace SourceMap
{

	Row::Row()
	: entries()
	{}

	size_t Row::getLength() const {
		return entries.size();
	}

	const Entry Row::getEntry(size_t idx) const {
		if (idx >= 0 && idx < entries.size()) {
			return entries[idx];
		}
		throw(invalid_argument("getEntry out of bound"));
	}

	void Row::addEntry(Entry entry)
	{
		entries.push_back(entry);
	}

	const vector<Entry> Row::at(size_t col) const
	{
		vector<Entry> entries;
		vector<Entry>::const_iterator entry_it = this->entries.begin();
		vector<Entry>::const_iterator entry_end = this->entries.end();
		// search from left up to position
		for(; entry_it != entry_end; ++entry_it) {
			// collect entries on position
			if (entry_it->col == col)
			{ entries.push_back(*entry_it); }
			// abort the loop after position
			else if (entry_it->col > col) break;
		}
		return entries;
	}


}
// EO namespace
