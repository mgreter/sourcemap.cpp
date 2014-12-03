// include library
#include <omp.h>
#include <stdexcept>

#include "entry.hpp"

// using string
using namespace std;

// add namespace for c++
namespace SourceMap
{

	Entry::Entry(size_t col)
	{
		this->type = 1;
		this->col = col;
		this->src_idx = 0;
		this->src_line = 0;
		this->src_col = 0;
		this->tkn_idx = 0;
	}

	Entry::Entry(size_t col, size_t idx, size_t src_ln, size_t src_col)
	{
		this->type = 4;
		this->col = col;
		this->src_idx = idx;
		this->src_line = src_ln;
		this->src_col = src_col;
		this->tkn_idx = 0;
	}

	Entry::Entry(size_t col, size_t idx, size_t src_ln, size_t src_col, size_t token)
	{
		this->type = 5;
		this->col = col;
		this->src_idx = idx;
		this->src_line = src_ln;
		this->src_col = src_col;
		this->tkn_idx = token;
	}

	size_t Entry::getType() const { return this->type; }

	bool Entry::operator== (const Entry &entry) const
	{
		return (col == entry.col);
	}
	bool Entry::operator!= (const Entry &entry) const
	{
		return (col != entry.col);
	}

	// ToDo: check for out of bound access
	size_t Entry::getCol() const { return this->col; }
	size_t Entry::getSource() const { return this->src_idx; }
	size_t Entry::getSrcLine() const { return this->src_line; }
	size_t Entry::getSrcCol() const { return this->src_col; }
	size_t Entry::getToken() const { return this->tkn_idx; }

}
// EO namespace

// implement for c
extern "C"
{

}
