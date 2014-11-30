#ifdef __cplusplus

#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>

#include "json.hpp"

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

	class SrcMapPos
	{
		public:
			SrcMapPos();
			SrcMapPos(size_t row, size_t col);
		public:
			size_t row;
			size_t col;
	};

	const SrcMapPos SrcMapPosStart(0, 0);
	const SrcMapPos SrcMapPosError(-1, -1);

	class Entry
	{
		friend class Row;
		friend class Mapping;
		friend class SrcMap;
		public:
			Entry(); // ctor
			Entry(size_t col); // ctor
			Entry(size_t col, size_t src_idx, size_t src_ln, size_t src_col); // ctor
			Entry(size_t col, size_t src_idx, size_t src_ln, size_t src_col, size_t token); // ctor
			const size_t getLength();
			const size_t getCol();
			const size_t getSource();
			const size_t getSrcLine();
			const size_t getSrcCol();
			const size_t getToken();
		public: // protected:
			vector<size_t> values;

		public:
		bool operator== (const Entry &entry);
		bool operator!= (const Entry &entry);

	};

	class Row
	{
		friend class Entry;
		friend class Mapping;
		friend class SrcMap;
		public:
			Row(); // ctor
			const size_t getLength();
			Entry getEntry(size_t idx);
			void addEntry(Entry entry);
			size_t getIndexAtPosition(size_t col);
			vector<Entry> getEntryAtPosition(size_t col);
		public: // protected:
			vector<Entry> entries;
	};

	class Mapping
	{
		friend class Entry;
		friend class Row;
		friend class SrcMap;
		public:
			Mapping(); // ctor
			Mapping(string VLQ); // ctor
			const size_t getLength();
			Row* getRow(size_t idx);
			void addRow(Row* row);
			void addNewLine();
			string serialize();
			vector<Entry> getEntryAtPosition(size_t row, size_t col);
		public: // protected:
			vector<Row*> rows;
			void init(string VLQ);
	};

	class SrcMap
	{
		friend class Entry;
		friend class Row;
		friend class Mapping;
		public:
			SrcMap(); // ctor
			SrcMap(string json_str); // ctor
			SrcMap(JsonNode* json_node); // ctor
			string getFile();
			string getRoot();
			Mapping* getMap();
			string getToken(size_t idx);
			string getSource(size_t idx);
			string getContent(size_t idx);

			size_t getTokenSize() { return tokens.size(); };
			size_t getSourceSize() { return sources.size(); };

			// use enc to disable map encoding
			string serialize(bool enc = true);

			void debug();
			void addToken(string token);
			void addSource(string file);
			void setLastLineLength(size_t len);

			Entry getEntry(size_t row_idx, size_t entry_idx);

			void insert(size_t row, Entry entry, bool after = false);
			size_t getIndexAtPosition(SrcMapPos pos);

			// change the original mappings
			SrcMap* remap(SrcMap* srcmap);
			void mergePrepare(SrcMap* srcmap);
			void splice(SrcMapPos pos, SrcMapPos del);
			void splice(SrcMapPos pos, SrcMap* srcmap);
			void splice(SrcMapPos pos, SrcMapPos del, SrcMap* srcmap);

		public: // protected:
			string file;
			string root;
			Mapping* map;
			string version;
			SrcMapPos size;
			vector<string> tokens;
			vector<string> sources;
			vector<string> contents;
			void init(JsonNode* json_node);
	};

}
// EO namespace

// declare for c
extern "C" {
#endif

	void testme();

#ifdef __cplusplus
}
#endif
