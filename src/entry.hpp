#ifndef SOURCEMAP_ENTRY
#define SOURCEMAP_ENTRY

#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>

#include "index.hpp"
#include "position.hpp"

// using std::string
using namespace std;

// add namespace for c++
namespace SourceMap
{

	class Entry
	{
		friend class Row;
		friend class Mapping;
		friend class SrcMap;
		public: // ctor
			Entry();
			Entry(size_t col);
			Entry(size_t col, size_t src_idx, size_t src_ln, size_t src_col);
			Entry(size_t col, size_t src_idx, size_t src_ln, size_t src_col, size_t tkn_idx);
		public: // operators
			bool operator== (const Entry &entry) const;
			bool operator!= (const Entry &entry) const;
			friend ostream& operator<<(ostream& os, const Entry& entry);
		public: // setters
			void setCol(size_t col) { this->col = col; };
		public: // getters
			size_t getType() const;
			size_t getCol() const;
			size_t getSource() const;
			size_t getSrcLine() const;
			size_t getSrcCol() const;
			size_t getToken() const;
		public: // variables
			size_t type;
			size_t col;
			size_t src_idx;
			size_t src_line;
			size_t src_col;
			size_t tkn_idx;
	};

}
// EO namespace

#endif
