// include library
#include <omp.h>
#include <stdexcept>

#include "mapping.hpp"
#include "format/v3.hpp"

// using string
using namespace std;

// add namespace for c++
namespace SourceMap
{

	Mapping::Mapping(size_t version)
	{
		this->version = version;
		this->addNewLine();
		this->last_ln_col = 0;
	}

	Mapping::Mapping(string VLQ, size_t version)
	{
		this->version = version;
		this->addNewLine();
		this->last_ln_col = 0;
		this->init(VLQ);
	}

	const Row Mapping::getRow(size_t idx) const {
		if (idx >= 0 && idx < rows.size()) return rows[idx];
		else throw(invalid_argument("out of bound"));
	}

	const string Mapping::serialize() const
	{
		if (this->version == 3) {
			return SourceMap::Format::V3::serialize(*this);
		} else {
			throw(runtime_error("unknown format"));
		}
	}

	void Mapping::init(const string& str)
	{
		if (this->version == 3) {
			SourceMap::Format::V3::unserialize(*this, str);
		} else {
			throw(runtime_error("unknown format"));
		}
	}

	void Mapping::addNewLine()
	{
		rows.push_back(Row());
	}


}
// EO namespace

// implement for c
extern "C"
{

}
