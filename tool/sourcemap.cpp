// include library
#include <stack>
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <stdlib.h>

#include "sourcemap.h"

// using std::string
using namespace std;
// our own c++ namespace
using namespace SourceMap;

// main program
int main (int argc, char** argv)
{

	// only option so far
	int debug_map = 0;

	// process command line arguments
	for (int i = 1; i < argc; ++i)
	{
		string arg = string(argv[i]);

		// handle debug mappings option
		if (arg == "-d" || arg == "--debug-maps")
		{
			debug_map = 1;
		}
		// Items for printing output and exit
		else if (arg == "-h" || arg == "--help")
		{
			cout << "sourcemap [options] < file.map" << endl;
			cout << "---------" << endl;
			cout << "-d, --debug-maps   pretty print output" << endl;
			cout << "-h, --help         print this help text" << endl;
			cout << "-v, --version      version information" << endl;
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
	};

	if (debug_map) {

		try {

			SrcMap* srcmap = new SrcMap(data);
			Mapping* map = srcmap->getMap();

			for(size_t i = 0; i < map->getLength(); i++) {
				Row* row = map->getRow(i);
				for(size_t n = 0; n < row->getLength(); n++) {
					Entry* entry = row->getEntry(n);
					if (entry->getLength() == 1) {
						cerr << entry->getCol() << endl;
					} else if (entry->getLength() == 4) {
						cerr << entry->getCol() << ":" << entry->getSource() << ":" << entry->getSrcLine() << ":" << entry->getSrcCol() << ":" << endl;
					} else if (entry->getLength() == 5) {
						cerr << entry->getCol() << ":" << entry->getSource() << ":" << entry->getSrcLine() << ":" << entry->getSrcCol() << ":" << entry->getToken() << endl;
					}
				}
			}

		} catch(string& e) {
			cerr << "parsing error: " << e << endl;
		} catch (...) {
			cerr << "unknown error occurred" << endl;
		}

	} else {

	}

/*




	// simple assertion for valid value
	if (pretty > SASS2SCSS_PRETTIFY_3)
	{ pretty = SASS2SCSS_PRETTIFY_3; }

*/

	return 1;

}
