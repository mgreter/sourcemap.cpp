#ifdef __cplusplus

#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>

#include "json.hpp"
#include "mapping.hpp"

#ifndef VERSION
#define VERSION "[NA]"
#endif

// using std::string
using namespace std;

// add namespace for c++
namespace SourceMap
{

	// define version from arguments
	// compile with g++ -DVERSION="\"vX.X.X\""
	const string SOURCEMAP_VERSION = VERSION;

	/****************************************************************************/
	/****************************************************************************/


	/****************************************************************************/
	/****************************************************************************/


	/****************************************************************************/
	/****************************************************************************/


	class SrcMap
	{
		friend class Entry;
		friend class Row;
		friend class Mapping;

		public: // ctor
			SrcMap() {};
			SrcMap(const string& json_str);
			SrcMap(const JsonNode& json_node);

		public: // setters
			// append to the vectors
			void addToken(string token);
			void addSource(string file);
			// get index or add to vector
			size_t pushToken(string token);
			size_t pushSource(string file);

		public: // getters
			const string getFile() const;
			const string getRoot() const;
			const Mapping getMap() const;
			// access the vectors
			const string getToken(size_t idx) const;
			const string getSource(size_t idx) const;
			const string getContent(size_t idx) const;
			// get size of the vectors
			size_t getRowSize() const { return map.rows.size(); };
			size_t getTokenSize() const { return tokens.size(); };
			size_t getSourceSize() const { return sources.size(); };
			// use enc to disable map encoding
			char* serialize(bool enc = true) const;

		public: // methods
			// truncate or increase size
			// call before adding entries
			void truncate(SrcMapPos size);

			// insert the map at the given position
			// ToDo: means size needs to go on to map
			// sourcemaps first needs to adapt mappings
			void insert(SrcMapPos pos, const Mapping& map);
			void insert(SrcMapPos pos, const SrcMap& srcmap);

			// delete the given size from position
			// will truncate the size accordingly
			// only possible because we now the number
			// of columns to be be removed from caller
			void remove(SrcMapPos pos, SrcMapPos del);

			// main manipulation method (delete/insert)
			// ToDo: means size needs to go on to map
			// sourcemaps first needs to adapt mappings
			void splice(SrcMapPos pos, SrcMapPos del, const Mapping& map);
			void splice(SrcMapPos pos, SrcMapPos del, const SrcMap& srcmap);

			// remap is a specific operation to remap/merge an
			// intermediate source-map to its original content
			void remap(SrcMap srcmap);


			// main manipulation method (delete/insert)


			void insert(size_t row, Entry entry, bool after = false);

			void mergePrepare(SrcMap srcmap);
			void setLastLineLength(size_t col);

		public: // route to other functions
			const Entry getEntry(size_t row, size_t idx) const;
			const Entry getEntry(const SrcMapIdx& idx) const;
			const vector<Entry> at(size_t row, size_t col) const;
			const vector<Entry> at(const SrcMapPos& pos) const;

		public: // variables
			string file;
			string root;
			Mapping map;
			string version;
			vector<string> tokens;
			vector<string> sources;
			vector<string> contents;
			void init(const JsonNode& json_node);
	};

}
// EO namespace

// declare for c
extern "C" {
#endif

// void foobar();

#ifdef __cplusplus
}
#endif
