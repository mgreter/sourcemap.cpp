#ifndef SOURCEMAP_TYPEDEF
#define SOURCEMAP_TYPEDEF

#include <iostream>
#include <exception>
#include <stdexcept>
#include <vector>
#include <memory>

#ifdef __cplusplus

// #define HAS_PANDA
// #define PANDA_REFCNT

#ifdef HAS_PANDA

	#include <panda/def.h>
	#include <panda/cast.h>
	#include <panda/refcnt.h>
	#include <panda/traits.h>

	#ifdef PANDA_REFCNT
		#define PANDA_INHERIT : virtual public panda::RefCounted
	#endif

#endif

#ifndef PANDA_INHERIT
	#define PANDA_INHERIT
#endif

// add namespace for c++
namespace SourceMap
{

	// import some stuff
	using std::cerr;
	using std::endl;
	using std::vector;
	using std::string;
	using std::ostream;
	using std::stringstream;
	using std::istringstream;
	using std::logic_error;
	using std::out_of_range;
	using std::runtime_error;
	using std::invalid_argument;

	#ifdef HAS_PANDA
		// using std::weak_ptr;
		using panda::shared_ptr;
		// implement `make_shared` for panda
		template< class T, class... Args >
		shared_ptr<T> make_shared( Args&&... args )
		{ return shared_ptr<T>(new T(args...)); }
	#else
		// using std::weak_ptr;
		using std::shared_ptr;
		using std::make_shared;
	#endif

	#define foreach(type, variable, array) \
		auto variable = array.begin(); \
		auto end_##variable = array.end(); \
		for(; variable != end_##variable; ++variable)

	#define foreach_ptr(type, variable, array) \
		auto it_##variable = array.begin(); \
		auto end_##variable = array.end(); \
		for(shared_ptr<type> variable; it_##variable != end_##variable && (variable = *it_##variable); ++it_##variable)

	#define const_foreach(type, variable, const_array) \
		auto variable = const_array.begin(); \
		auto end_##variable = const_array.end(); \
		for(; variable != end_##variable; ++variable)

	#define const_foreach_ptr(type, variable, array) \
		auto it_##variable = array.begin(); \
		auto end_##variable = array.end(); \
		for(shared_ptr<type> variable; it_##variable != end_##variable && (variable = *it_##variable); ++it_##variable)

};
// EO namespace


// declare for c
extern "C" {
#endif

// void foobar();

#ifdef __cplusplus
}
#endif

#endif
