// include library
#include <stack>
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <stdlib.h>

#include "../sourcemap.h"

// using std::string
using namespace std;
// our own c++ namespace
using namespace SourceMap;

bool json_cmp_property (JsonNode* json_a, JsonNode* json_b, const char* key)
{

	JsonNode* member_a = json_find_member(json_a, key);
	JsonNode* member_b = json_find_member(json_b, key);

	return (member_a == NULL && member_b == NULL) ||
	       (member_a == NULL && member_b != NULL) ||
	       (member_a != NULL && member_b == NULL) ||
	         string(json_stringify(member_a, "")) ==
	         string(json_stringify(member_b, ""));
}

// main program
int main (int argc, char** argv)
{

	// execute option
	int test_map = 0;
	int debug_map = 0;
	int remap_map = 0;

	// process command line arguments
	for (int i = 1; i < argc; ++i)
	{
		string arg = string(argv[i]);

		// handle code coverage test option
		if (arg == "-t" || arg == "--test")
		{
			test_map = 1;
		}
		else if (arg == "-r" || arg == "--remap")
		{
			remap_map = 1;
		}
		// handle debug mappings option
		else if (arg == "-d" || arg == "--debug-maps")
		{
			debug_map = 1;
		}
		// Items for printing output and exit
		else if (arg == "-h" || arg == "--help")
		{
			cout << "sourcemap [options] < file.map" << endl;
			cout << "------------------------------" << endl;
			cout << "-d, --debug-maps   pretty print output" << endl;
			cout << "-h, --help         print this help text" << endl;
			cout << "-t, --test         code coverage test" << endl;
			cout << "-v, --version      version information" << endl;
			cout << "-r, --remap        remap" << endl;
			exit(0);
		}
		else if (arg == "-v" || arg == "--version")
		{
			// check current hash and if it matches any of the tags, use tag
			cout << SourceMap::SOURCEMAP_VERSION << endl;
			exit(0);
		}
		// error out with a generic error message (keep it simple for now)
		else
		{
			cerr << "Unknown command line option " << argv[i] << endl;
			exit(EXIT_FAILURE);
		}

	}
	// EO each arg

	// declare variable
	string line;

	// accumulate for test
	string data = "";

	// read from stdin
	while (cin)
	{
		// read a line
		getline(cin, line);
		// process the line
		data += line + "\n";
		// break if at end of file
		if (cin.eof()) break;
	}
	// EO while cin

	try
	{

		// print map debug
		if (debug_map)
		{

			SrcMap* srcmap = new SrcMap(data);
			Mapping* map = srcmap->getMap();

			cout << "file: " << srcmap->getFile() << endl;
			cout << "root: " << srcmap->getRoot() << endl;

			for(size_t i = 0; i < map->getLength(); i++) {
				Row* row = map->getRow(i);
				for(size_t n = 0; n < row->getLength(); n++) {
					Entry entry = row->getEntry(n);
					if (entry.getLength() == 1) {
						cout << entry.getCol() << endl;
					} else if (entry.getLength() == 4) {
						cout << entry.getCol() << ":" << entry.getSource() << "=" << srcmap->getSource(entry.getSource()) << "|" << entry.getSource() << ":" << entry.getSrcLine() << ":" << entry.getSrcCol() << ":" << endl;
					} else if (entry.getLength() == 5) {
						cout << entry.getCol() << ":" << entry.getSource() << "=" << srcmap->getSource(entry.getSource()) << ":" << entry.getSrcLine() << ":" << entry.getSrcCol() << ":" << entry.getToken() << "=" << srcmap->getToken(entry.getToken()) << endl;
					}
				}
			}

		}
		// EO debug_map

		// run internal code coverage test
		// we will compare data and output TAP
		// tests if encode(decode(data)) == data
		if (test_map)
		{

			JsonNode* json_a = json_decode(data.c_str());
			string json = (new SrcMap(data))->serialize();
			JsonNode* json_b = json_decode(json.c_str());

			// check if we have the same information
			bool pass = json_cmp_property(json_a, json_b, "file") &&
			            json_cmp_property(json_a, json_b, "names") &&
			            json_cmp_property(json_a, json_b, "version") &&
			            json_cmp_property(json_a, json_b, "mappings") &&
			            json_cmp_property(json_a, json_b, "lineCount") &&
			            json_cmp_property(json_a, json_b, "sourceRoot") &&
			            json_cmp_property(json_a, json_b, "sourcesContent");


			// output TAP test compatible string
			cout << (pass ? "ok" : "not ok") << endl;

			// indicate failure on test
			if (!pass) return EXIT_FAILURE;

		}
		// EO test_map

	}
	catch(string& e)
	{
		cerr << "parsing error: " << e << endl;
		return EXIT_FAILURE;
	}
	catch (...)
	{
		cerr << "unknown error occurred" << endl;
		return EXIT_FAILURE;
	}

	// success
	return 0;

}
// EO int main