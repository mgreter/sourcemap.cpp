#ifndef SOURCEMAP_FORMAT_V3
#define SOURCEMAP_FORMAT_V3

#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>

#include "../mapping.hpp"

// using std::string
using namespace std;

// add namespace for c++
namespace SourceMap
{
	namespace Format
	{
		namespace V3
		{
			const string serialize(const Mapping& map);
			void unserialize(Mapping& map, const string& data);
		}
	}

}
// EO namespace

#endif
