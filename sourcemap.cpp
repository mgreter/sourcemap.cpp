// include library
#include <omp.h>
#include <stdexcept>

// our own header
#include "sourcemap.h"

JsonNode* json_import(const char* str) {
	if (string(str) == "null") { return json_mknull(); }
	else if (string(str) == "true") { return json_mkbool(true); }
	else if (string(str) == "false") { return json_mkbool(false); }
	else { return json_mkstring(str); }
}

#define json_double(json_node)           \
  (!json_node ? -1 :                     \
      json_node->tag == JSON_NUMBER ?    \
        json_node->number_ :             \
      json_node->tag == JSON_STRING ?    \
        atof(json_node->string_) :       \
          -1                             \
  )

#define json_export(json_node)           \
  (!json_node ? "null" : string(         \
      json_node->tag == JSON_STRING ?    \
        json_node->string_ :             \
          json_stringify(json_node, "")  \
  ))

#define foreach(type, variable, array) \
	vector<type>::iterator variable = array.begin(); \
	vector<type>::iterator end_##variable = array.end(); \
	for(; variable != end_##variable; ++variable)



// using std::string
using namespace std;

// add namespace for c++
namespace SourceMap
{


	// A Base64 VLQ digit can represent 5 bits, so it is base-32.
	const int VLQ_BASE_SHIFT = 5;
	const int VLQ_BASE = 1 << VLQ_BASE_SHIFT;

	// A mask of bits for a VLQ digit (11111), 31 decimal.
	const int VLQ_BASE_MASK = VLQ_BASE-1;

	// The continuation bit is the 6th bit.
	const int VLQ_CONTINUATION_BIT = VLQ_BASE;

	/**
	 * Converts from a two-complement value to a value where the sign bit is
	 * is placed in the least significant bit. For example, as decimals:
	 *   1 becomes 2 (10 binary), -1 becomes 3 (11 binary)
	 *   2 becomes 4 (100 binary), -2 becomes 5 (101 binary)
	 */
	int toVLQSigned(int value) {
		if (value < 0) {
			return ((-value) << 1) + 1;
		} else {
			return (value << 1) + 0;
		}
	}

	/**
	 * Converts to a two-complement value from a value where the sign bit is
	 * is placed in the least significant bit. For example, as decimals:
	 *   2 (10 binary) becomes 1, 3 (11 binary) becomes -1
	 *   4 (100 binary) becomes 2, 5 (101 binary) becomes -2
	 */
	int fromVLQSigned(int value) {
		int negate = (value & 1) == 1;
		value = value >> 1;
		return negate ? -value : value;
	}

	/**
	  * Coverts a interger value to base64 char
	*/
	char toBase64(int i) {
		if (i >= 0 && i < 26) { return char(i + 65); }
		else if (i > 25 && i < 52) { return char(i + 71); }
		else if (i > 51 && i < 62) { return char(i - 4); }
		else if (i == 62) { return char(43); }
		else if (i == 63) { return char(47); }
		throw(invalid_argument("base 64 integer out of bound"));
	}

	/**
	  * Coverts a char from base64 to integer value.
	*/
	int fromBase64(char c) {
		if (c > 95 && c < 123) { return c - 71; }
		else if (c > 64 && c < 91) { return c - 65; }
		else if (c > 47 && c < 59) { return c + 4; }
		else if (c == 43) { return 62; }
		else if (c == 47) { return 63; }
		throw(invalid_argument("base 64 char out of bound"));
	}

	/**
	 * Writes a VLQ encoded value to the provide appendable.
	 * @throws IOException
	 */
	string encodeVLQ(int value) {
		string result = "";
		value = toVLQSigned(value);
		do {
			int digit = value & VLQ_BASE_MASK;
			value = (unsigned int)value >> VLQ_BASE_SHIFT;
			if (value > 0) {
				digit |= VLQ_CONTINUATION_BIT;
			}
			result += toBase64(digit);
		} while (value > 0);
		return result;
	}

	/**
	 * Decodes the next VLQValue from the provided iterator.
	 */
	int decodeVLQ(string::iterator& it, string::iterator& end) {
		int result = 0;
		int continuation;
		int shift = 0;
		do {
			char c = *it;
			int digit = fromBase64(c);
			continuation = (digit & VLQ_CONTINUATION_BIT) != 0;
			digit &= VLQ_BASE_MASK;
			result = result + (digit << shift);
			shift = shift + VLQ_BASE_SHIFT;
			if (continuation) ++it;
		} while (continuation && it != end);
		return fromVLQSigned(result);
	}

	Entry::Entry(size_t col)
	{
		values.push_back(col);
	}

	Entry::Entry(size_t col, size_t idx, size_t src_ln, size_t src_col)
	{
		values.push_back(col);
		values.push_back(idx);
		values.push_back(src_ln);
		values.push_back(src_col);
	}

	Entry::Entry(size_t col, size_t idx, size_t src_ln, size_t src_col, size_t token)
	{
		values.push_back(col);
		values.push_back(idx);
		values.push_back(src_ln);
		values.push_back(src_col);
		values.push_back(token);
	}

	Row::Row()
	: entries()
	{}

	SrcMap::SrcMap()
	: size(SrcMapPosError)
	{
		this->map = new Mapping();
	}

	Mapping::Mapping()
	: rows()
	{
		this->addNewLine();
	}

	SrcMapPos::SrcMapPos()
	{
		this->row = -1;
		this->col = -1;
	}

	SrcMapPos::SrcMapPos(size_t row, size_t col)
	{
		this->row = row;
		this->col = col;
	}

	const size_t Row::getLength() { return entries.size(); }
	const size_t Entry::getLength() { return values.size(); }
	const size_t Mapping::getLength() { return rows.size(); }

	Entry SrcMap::getEntry(size_t row_idx, size_t entry_idx) {
		return map->getRow(row_idx)->getEntry(entry_idx);
	}

	Entry Row::getEntry(size_t idx) {
		if (idx >= 0 && idx < entries.size()) {
			return entries[idx];
		}
		throw(invalid_argument("getEntry out of bound"));
	}

	bool Entry::operator== (const Entry &entry){
		return (values[0] == entry.values[0]);
	};
	bool Entry::operator!= (const Entry &entry){
		return (values[0] != entry.values[0]);
	};

	Row* Mapping::getRow(size_t idx) {
		if (idx >= 0 && idx < rows.size()) {
			return rows[idx];
		}
		throw(invalid_argument("getRow out of bound"));
	}

	// ToDo: check for out of bound access
	const size_t Entry::getCol() { return values[0]; }
	const size_t Entry::getSource() { return values[1]; }
	const size_t Entry::getSrcLine() { return values[2]; }
	const size_t Entry::getSrcCol() { return values[3]; }
	const size_t Entry::getToken() { return values[4]; }

	string Mapping::serialize()
	{

		string result = "";

		size_t column = 0;
		size_t source = 0;
		size_t in_line = 0;
		size_t in_col = 0;
		size_t token = 0;

		for(size_t i = 0; i < rows.size(); i++) {
			Row* row = rows[i];
			for(size_t n = 0; n < row->getLength(); n++) {
				Entry entry = row->getEntry(n);
				if (entry.getLength() == 1) {
					result += encodeVLQ(entry.getCol() - column);
					source = entry.getCol();
				} else if (entry.getLength() == 4) {
					result += encodeVLQ(entry.getCol() - column);
					result += encodeVLQ(entry.getSource() - source);
					result += encodeVLQ(entry.getSrcLine() - in_line);
					result += encodeVLQ(entry.getSrcCol() - in_col);
					column = entry.getCol();
					source = entry.getSource();
					in_line = entry.getSrcLine();
					in_col = entry.getSrcCol();
				} else if (entry.getLength() == 5) {
					result += encodeVLQ(entry.getCol() - column);
					result += encodeVLQ(entry.getSource() - source);
					result += encodeVLQ(entry.getSrcLine() - in_line);
					result += encodeVLQ(entry.getSrcCol() - in_col);
					result += encodeVLQ(entry.getToken() - token);
					column = entry.getCol();
					source = entry.getSource();
					in_line = entry.getSrcLine();
					in_col = entry.getSrcCol();
					token = entry.getToken();
				}
				if (n + 1 != row->getLength()) result += ",";
			}
			if (i + 1 != rows.size()) result += ";";

		column = 0; }

		return result;

	}

	void Mapping::init(string str)
	{

		string::iterator it = str.begin();
		string::iterator end = str.end();

		Row* row = new Row();
		rows.push_back(row);

		size_t column = 0;
		size_t source = 0;
		size_t in_line = 0;
		size_t in_col = 0;
		size_t token = 0;

		vector<size_t> values;

		while(true) {

			// check if we reached a delimiter for entries
			if (it == end || *it == ',' || *it == ';') {

				// only create valid entries
				// checks out of bound access
				if (
					values.size() == 1 &&
					int(values[0] + column) >= 0
				) {
					row->addEntry(Entry(
						column += values[0]
					));
				}
				else if (
					values.size() == 4 &&
					int(values[0] + column) >= 0 &&
					int(values[1] + source) >= 0 &&
					int(values[2] + in_line) >= 0 &&
					int(values[3] + in_col) >= 0
				) {
					row->addEntry(Entry(
						column += values[0],
						source += values[1],
						in_line += values[2],
						in_col += values[3]
					));
				}
				else if (
					values.size() == 5 &&
					int(values[0] + column) >= 0 &&
					int(values[1] + source) >= 0 &&
					int(values[2] + in_line) >= 0 &&
					int(values[3] + in_col) >= 0 &&
					int(values[4] + token) >= 0
				) {
					row->addEntry(Entry(
						column += values[0],
						source += values[1],
						in_line += values[2],
						in_col += values[3],
						token += values[4]
					));
				}
				// empty rows are allowed
				else if (values.size() != 0) {
					// everything else is not valid
					throw(runtime_error("invalid source map entry"));
				}

				// create a new row
				if (*it == ';') {
					row = new Row();
					rows.push_back(row);
				column = 0; }

				// clear for next
				values.clear();

				// exit loop if at end
				if (it == end) break;

			} else {

				// parse a vlq number and put to values
				values.push_back(decodeVLQ(it, end));

			}

			// advance
			++it;

		}

	}

	Mapping::Mapping(string VLQ)
	{
		init(VLQ);
	}


	void Row::addEntry(Entry entry)
	{
		entries.push_back(entry);
	}

	void Mapping::addRow(Row* row)
	{
		rows.push_back(row);
	}

	string SrcMap::getFile() { return file; }
	string SrcMap::getRoot() { return root; }
	Mapping* SrcMap::getMap() { return map; }

	string SrcMap::getToken(size_t idx)
	{
		if (idx < 0 || idx >= tokens.size())
		{ throw(runtime_error("token access out of bound")); }
		return tokens[idx];
	}

	string SrcMap::getSource(size_t idx)
	{
		if (idx < 0 || idx >= sources.size())
		{ throw(runtime_error("source access out of bound")); }
		return sources[idx];
	}

	string SrcMap::getContent(size_t idx)
	{
		if (idx < 0 || idx >= contents.size())
		{ throw(runtime_error("content access out of bound")); }
		return contents[idx];
	}

	void SrcMap::init (JsonNode* json_node)
	{

		if (json_node && json_node->tag == JSON_OBJECT) {

			JsonNode* json_file = json_find_member(json_node, "file"); // string
			JsonNode* json_root = json_find_member(json_node, "sourceRoot"); // string
			JsonNode* json_version = json_find_member(json_node, "version"); // string
			JsonNode* json_mappings = json_find_member(json_node, "mappings"); // string
			JsonNode* json_tokens = json_find_member(json_node, "names"); // array
			JsonNode* json_sources = json_find_member(json_node, "sources"); // array
			JsonNode* json_contents = json_find_member(json_node, "sourcesContent"); // array
			JsonNode* json_lastLineLength = json_find_member(json_node, "x_lastLineSize"); // string

			file = json_export(json_file);
			root = json_export(json_root);
			version = json_export(json_version);

			// assetion for version (must be defined first in source map!)
			if (json_export(json_version) != "3") {
				// must be defined first in source map actually
				throw(runtime_error("only source map version 3 is supported"));
			}

			if (json_tokens && (
					json_tokens->tag == JSON_ARRAY ||
					json_tokens->tag == JSON_OBJECT
			)) {
				JsonNode* json_token(0);
				json_foreach(json_token, json_tokens) {
					tokens.push_back(json_export(json_token));
				}
			}

			if (json_sources && (
					json_sources->tag == JSON_ARRAY ||
					json_sources->tag == JSON_OBJECT
			)) {
				JsonNode* json_source(0);
				json_foreach(json_source, json_sources) {
					sources.push_back(json_export(json_source));
				}
			}

			if (json_contents && (
					json_contents->tag == JSON_ARRAY ||
					json_contents->tag == JSON_OBJECT
			)) {
				JsonNode* json_content(0);
				json_foreach(json_content, json_contents) {
					contents.push_back(json_export(json_content));
				}
			}

			map = new Mapping(json_export(json_mappings));

			size.row = map->rows.size();
			size.col = (int) json_double(json_lastLineLength);

		}

	}

	SrcMap::SrcMap (JsonNode* json_node)
	{

		init(json_node);

	}

	SrcMap::SrcMap(string json_str)
	{

		JsonNode* json_node = json_decode(json_str.c_str());

		init(json_node);

	}

	string SrcMap::serialize(bool enc)
	{

		JsonNode* json_node = json_mkobject();

		json_append_member(json_node, "version", json_mknumber(3));

		JsonNode* json_file = json_import(file.c_str());
		json_append_member(json_node, "file", json_file);

		size_t sources_size = sources.size();
		JsonNode* json_sources = json_mkarray();
		for (size_t i = 0; i < sources_size; ++i) {
			const char* include = sources[i].c_str();
			JsonNode* json_source = json_import(include);
			json_append_element(json_sources, json_source);
		}
		json_append_member(json_node, "sources", json_sources);

		size_t contents_size = contents.size();
		JsonNode* json_contents = json_mkarray();
		for (size_t i = 0; i < contents_size; ++i) {
			const char* content = contents[i].c_str();
			JsonNode* json_content = json_import(content);
			json_append_element(json_contents, json_content);
		}
		json_append_member(json_node, "sourcesContent", json_contents);

		string mappings = map->serialize();
		JsonNode* json_mappings = json_import(mappings.c_str());
		json_append_member(json_node, "mappings", json_mappings);

		size_t tokens_size = tokens.size();
		JsonNode* json_tokens = json_mkarray();
		for (size_t i = 0; i < tokens_size; ++i) {
			const char* token = tokens[i].c_str();
			JsonNode* json_token = json_import(token);
			json_append_element(json_tokens, json_token);
		}
		json_append_member(json_node, "names", json_tokens);

		char* str = json_encode(json_node);

		string result = string(str);
		delete(str);
		json_delete(json_node);
		return result;

	}

	// add sources from srcmap to our own
	// update mappings from srcmap to new index
	void SrcMap::mergePrepare(SrcMap* srcmap)
	{

		foreach(string, source, srcmap->sources) {
			sources.push_back(*source);
		}

		size_t offset = sources.size();

		foreach(Row*, row, srcmap->map->rows) {
			foreach(Entry, entry, (*row)->entries) {
				if ((*entry).values.size() > 0)
					(*entry).values[1] += offset;
			}
		}

	}

	SrcMap* SrcMap::remap(SrcMap* srcmap)
	{

		size_t i = 0;

		double start = omp_get_wtime();

		size_t offset = sources.size();

		foreach(string, source, srcmap->sources) {
			sources.push_back(*source);
		}

		foreach(Row*, row, map->rows) {
			foreach(Entry, entry, (*row)->entries) {

				if ((*entry).values[1] != 0) continue;

				// our own source map can only be of one file
				vector<Entry> originals = srcmap->map->getEntryAtPosition(i, (*entry).getCol());

				if (originals.size() > 99) { cerr << "too many mappings for remap\n"; }
				else if (originals.size() == 0) { cerr << "no mappings for remap\n"; }
				else {
					foreach(Entry, original, originals) {
						if ((*entry).values.size() > 0) {
							(*entry).values[0] = (*original).values[0];
						}
						if ((*entry).values.size() > 3) {
							(*entry).values[1] = (*original).values[1] + offset;
							(*entry).values[2] = (*original).values[2];
							(*entry).values[3] = (*original).values[3];
						}
						if ((*entry).values.size() > 4) {
							(*entry).values[4] = (*original).values[4];
						}
					}
				}
			}
			i++;
		}

		double end = omp_get_wtime();

		cerr << "Benchmark: " << (end - start) << endl;

		return this;

	}

	// STL helper
	struct adjust_col
	{
		public:
			adjust_col(int off) : offset(off) {}
			void operator()(Entry &entry)
			{
				entry.values[0] += offset;
			}
		private:
			int offset;
	};

	void SrcMap::setLastLineLength(size_t len)
	{
		size.row = map->rows.size();
		size.col = len;
	}

	// bread and butter function that implements all operations
	void SrcMap::splice(SrcMapPos pos, SrcMapPos del)
	{
		splice(pos, del, NULL);
	}

	// bread and butter function that implements all operations
	void SrcMap::splice(SrcMapPos pos, SrcMap* srcmap)
	{
		splice(pos, SrcMapPos(0, 0), srcmap);
	}

	// bread and butter function that implements all operations
	void SrcMap::splice(SrcMapPos pos, SrcMapPos del, SrcMap* srcmap)
	{

		if (del.col == string::npos) throw(invalid_argument("splice del.col is invalid"));
		if (del.row == string::npos) throw(invalid_argument("splice del.row is invalid"));
		if (pos.col == string::npos) throw(invalid_argument("splice pos.col is invalid"));
		if (pos.row == string::npos) throw(invalid_argument("splice pos.row is invalid"));
		if (size.col == string::npos) throw(invalid_argument("splice size.col is invalid"));
		if (size.row == string::npos) throw(invalid_argument("splice size.row is invalid"));

		if (srcmap != NULL) {
			if (srcmap->map == 0) {
				throw(runtime_error("invalid srcmap"));
			}
			if (srcmap->map->rows.size() == 0) {
				throw(runtime_error("empty srcmap"));
			}
		}
		if (map->rows.size() <= pos.row) {
			throw(out_of_range("access out of bound"));
		}
		if (map->rows.size() <= pos.row + del.row) {
			throw(out_of_range("delete out of bound"));
		}

		size_t ins_col = srcmap ? srcmap->size.col : 0;
		size_t ins_row = srcmap ? srcmap->size.row - 1 : 0;

		if (ins_col == string::npos) throw(invalid_argument("splice ins_col is invalid"));
		if (ins_row == string::npos) throw(invalid_argument("splice ins_row is invalid"));

		// make sure we have enough room (needed for the lookups)
		while (map->rows.size() <= pos.row) { map->addNewLine(); }
		// get entries object from row to work on (by reference)
		vector<Entry> &entries = map->rows[pos.row]->entries;

		// find the position where the insert should be placed
		std::vector<Entry>::iterator entry_it = entries.begin();
		std::vector<Entry>::iterator entry_end = entries.end();
		// loop until we have reached the position
		for(; entry_it != entry_end; ++entry_it)
		{ if (entry_it->values[0] >= pos.col) break; }

		// buffer stores all entries that will be added
		vector<Entry> buffer;

		// get mappings to append later if we have additional line
		if (del.row == 0) {
			buffer.insert(buffer.end(), entry_it, entry_end);
			for_each(buffer.begin(), buffer.end(), adjust_col(-pos.col));
		}

		// delete full rows
		if (del.row > 0) {
			vector<Entry> &del_row = map->rows[pos.row + del.row]->entries;
			std::vector<Entry>::iterator del_entries_it = del_row.begin();
			std::vector<Entry>::iterator del_entries_end = del_row.end();
			buffer.insert(buffer.end(), del_entries_it, del_entries_end);
		}

		// delete columns
		if (del.col > 0) {
			// find the position where the delete should happend
			std::vector<Entry>::iterator buffer_it = buffer.begin();
			std::vector<Entry>::iterator buffer_end = buffer.end();
			// loop until we have reached the position
			for(; buffer_it != buffer_end; ++buffer_it)
			{ if (buffer_it->values[0] >= del.col) break; }
			// remove all entries in the delete range
			buffer.erase(buffer.begin(), buffer_it);
			// account for the deleted offset range
			for_each(buffer.begin(), buffer.end(), adjust_col(-del.col));
		}

		// remove the entries from the mapping row
		entries.erase(entry_it, entry_end);

		// remove full rows from the current mappings
		// the remaining line is used for the mangled entries
		std::vector<Row*>::iterator ins_row_it = map->rows.begin() + pos.row;
		std::vector<Row*>::iterator del_row_it = map->rows.begin() + pos.row;
		if (del.row > 0) {
			map->rows.erase(ins_row_it + 1, del_row_it + 1 + del.row);
		}

		// increase offset of entries on first line to be inserted
		// these entries will be placed after the position col offset
		if (srcmap != NULL) {

			vector<Entry> &ins_first = srcmap->map->rows[0]->entries;

			if (pos.col > 0) {
				std::vector<Entry>::iterator ins_first_it = ins_first.begin();
				std::vector<Entry>::iterator ins_first_end = ins_first.end();
				for_each(ins_first_it, ins_first_end, adjust_col(pos.col));
			}

			entries.insert(entry_it, ins_first.begin(), ins_first.end());

			if (srcmap->map->rows.size() > 0) {
				// srcmap->map->rows.erase(srcmap->map->rows.begin(), srcmap->map->rows.begin() + 1);
			}
			// insert complete rows of new source map (depends of del_row and ins_row)
			if (srcmap->map->rows.size() > 1) {
				map->rows.insert(ins_row_it + 1, srcmap->map->rows.begin() + 1, srcmap->map->rows.end());
			}
			// adjust the offsets by the size of the last line that got inserted
			for_each(buffer.begin(), buffer.end(), adjust_col(ins_col));
		}

		// insert the buffered entries at the end of the last inserted line
		vector<Entry> &final_entries = map->rows[pos.row + ins_row]->entries;
		final_entries.insert(final_entries.end(), buffer.begin(), buffer.end());

		size.row += ins_row - del.row;

	}

	void SrcMap::addSource(string file)
	{
		sources.push_back(file);
	}
	void SrcMap::addToken(string token)
	{
		tokens.push_back(token);
	}

	bool findFromLeft (const Entry &entry) {
		return entry.values[0] == entry.values[0];
	}


	// insert an entry at the given position
	void SrcMap::insert(size_t row, Entry entry, bool after)
	{
		// make sure we have enough room (needed for lookup)
		while (map->rows.size() <= row) { map->addNewLine(); }
		// create position object to pass as argument
		SrcMapPos pos = SrcMapPos(row, entry.getCol() + after);
		// get entries object from row to work on
		vector<Entry> &entries = map->rows[pos.row]->entries;
		// find the position where the insert should be placed
		std::vector<Entry>::iterator entry_it = entries.begin();
		std::vector<Entry>::iterator entry_end = entries.end();
		// loop until we have found the position
		for(; entry_it != entry_end; ++entry_it)
		{ if (entry_it->values[0] >= pos.col) break; }
		// insert at end if no position found
		entries.insert(entry_it, entry);
	}

	void Mapping::addNewLine()
	{
		rows.push_back(new Row());
	}

	// return -1 if the index is out of bound
	// for inserts make sure to create room first
	size_t SrcMap::getIndexAtPosition(SrcMapPos pos)
	{
		if (pos.row >= map->rows.size()) return string::npos;
		return map->rows.at(pos.row)->getIndexAtPosition(pos.col);
	}

	size_t Row::getIndexAtPosition(size_t col)
	{
		Entry search(col);
		return std::find(entries.begin(), entries.end(), search) - entries.begin();
	}


	vector<Entry> Mapping::getEntryAtPosition(size_t row, size_t col)
	{

		vector<Entry> entries;

		Row* qwe = rows[row];

		std::vector<Entry>::iterator entry_it = qwe->entries.begin();
		std::vector<Entry>::iterator entry_end = qwe->entries.end();

		for(; entry_it != entry_end; ++entry_it) {
			Entry& entry = *entry_it;
			if (entry.getCol() == col) {
				entries.push_back(entry);
			}
		}

		return entries;
	}

	void SrcMap::debug()
	{
		size_t count = 0;
		foreach(Row*, row, map->rows) {
			foreach(Entry, entry, (*row)->entries) {
				count ++;
			}
		}
		cerr << "rows: " << size.row << ", entries: " << count << endl;
	}

}
// EO namespace

// implement for c
extern "C"
{

}
