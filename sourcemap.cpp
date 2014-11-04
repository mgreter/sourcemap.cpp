// include library

// our own header
#include "sourcemap.h"

#define json_value(json_node)            \
  (!json_node ? "null" : string(         \
      json_node->tag == JSON_STRING ?    \
        json_node->string_ :             \
          json_stringify(json_node, "")  \
  ))

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
		throw(string("base 64 integer out of bound"));
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
		throw(string("base 64 char out of bound"));
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

	Row::Row() {}
	SrcMap::SrcMap() {}
	Mapping::Mapping() {}

	const size_t Row::getLength() { return entries.size(); }
	const size_t Entry::getLength() { return values.size(); }
	const size_t Mapping::getLength() { return rows.size(); }

	Entry* Row::getEntry(size_t idx) {
		if (idx >= 0 && idx < entries.size()) {
			return entries[idx];
		}
		return NULL;
	}

	Row* Mapping::getRow(size_t idx) {
		if (idx >= 0 && idx < rows.size()) {
			return rows[idx];
		}
		return NULL;
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
				Entry* entry = row->getEntry(n);
				if (entry->getLength() == 1) {
					result += encodeVLQ(entry->getCol() - column);
					source = entry->getCol();
				} else if (entry->getLength() == 4) {
					result += encodeVLQ(entry->getCol() - column);
					result += encodeVLQ(entry->getSource() - source);
					result += encodeVLQ(entry->getSrcLine() - in_line);
					result += encodeVLQ(entry->getSrcCol() - in_col);
					column = entry->getCol();
					source = entry->getSource();
					in_line = entry->getSrcLine();
					in_col = entry->getSrcCol();
				} else if (entry->getLength() == 5) {
					result += encodeVLQ(entry->getCol() - column);
					result += encodeVLQ(entry->getSource() - source);
					result += encodeVLQ(entry->getSrcLine() - in_line);
					result += encodeVLQ(entry->getSrcCol() - in_col);
					result += encodeVLQ(entry->getToken() - token);
					column = entry->getCol();
					source = entry->getSource();
					in_line = entry->getSrcLine();
					in_col = entry->getSrcCol();
					token = entry->getToken();
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
					row->addEntry(new Entry(
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
					row->addEntry(new Entry(
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
					row->addEntry(new Entry(
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
					throw(string("invalid source map entry"));
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


	void Row::addEntry(Entry* entry)
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
		{ throw(string("token access out of bound")); }
		return tokens[idx];
	}

	string SrcMap::getSource(size_t idx)
	{
		if (idx < 0 || idx >= sources.size())
		{ throw(string("source access out of bound")); }
		return sources[idx];
	}

	string SrcMap::getContent(size_t idx)
	{
		if (idx < 0 || idx >= contents.size())
		{ throw(string("content access out of bound")); }
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

			file = json_value(json_file);
			root = json_value(json_root);
			version = json_value(json_version);

			// assetion for version (must be defined first in source map!)
			if (json_value(json_version) != "3") {
				// must be defined first in source map actually
				throw(string("only source map version 3 is supported"));
			}

			if (json_tokens && (
					json_tokens->tag == JSON_ARRAY ||
					json_tokens->tag == JSON_OBJECT
			)) {
				JsonNode* json_token(0);
				json_foreach(json_token, json_tokens) {
					tokens.push_back(json_value(json_token));
				}
			}

			if (json_sources && (
					json_sources->tag == JSON_ARRAY ||
					json_sources->tag == JSON_OBJECT
			)) {
				JsonNode* json_source(0);
				json_foreach(json_source, json_sources) {
					sources.push_back(json_value(json_source));
				}
			}

			if (json_contents && (
					json_contents->tag == JSON_ARRAY ||
					json_contents->tag == JSON_OBJECT
			)) {
				JsonNode* json_contents(0);
				json_foreach(json_contents, json_contents) {
					contents.push_back(json_value(json_contents));
				}
			}

			map = new Mapping(json_value(json_mappings));

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

		JsonNode* json_file = json_mkstring(file.c_str());
		json_append_member(json_node, "file", json_file);

		JsonNode* json_sources = json_mkarray();
		for (size_t i = 0; i < sources.size(); ++i) {
			const char* include = sources[i].c_str();
			JsonNode* json_source = json_mkstring(include);
			json_append_element(json_sources, json_source);
		}
		json_append_member(json_node, "sources", json_sources);

		JsonNode* json_contents = json_mkarray();
		for (size_t i = 1; i < contents.size(); ++i) {
			const char* content = contents[i].c_str();
			JsonNode* json_content = json_mkstring(content);
			json_append_element(json_contents, json_content);
		}
		json_append_member(json_node, "sourcesContent", json_contents);

		string mappings = map->serialize();
		JsonNode* json_mappings = json_mkstring(mappings.c_str());
		json_append_member(json_node, "mappings", json_mappings);

		JsonNode* json_tokens = json_mkarray();
		for (size_t i = 1; i < tokens.size(); ++i) {
			const char* token = tokens[i].c_str();
			JsonNode* json_token = json_mkstring(token);
			json_append_element(json_tokens, json_token);
		}
		json_append_member(json_node, "names", json_tokens);

		char* str = json_encode(json_node);

		string result = string(str);
		delete(str);
		json_delete(json_node);
		return result;

	}

}
// EO namespace

// implement for c
extern "C"
{

}
