// Author: Tim Diels <timdiels.m@gmail.com>

#pragma once

#include <boost/filesystem.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/optional.hpp>
#include <deep_blue_genome/common/util.h>
#include <deep_blue_genome/util/serialization/unique_ptr.h> // std::unique_ptr, not supported in all collections, but it's fairly easy to add an implementation by copy pasting in util/serialization
#include <deep_blue_genome/util/serialization/unordered_set.h>
#include <deep_blue_genome/util/serialization/forward_list.h>
#include <deep_blue_genome/util/serialization/list.h>
#include <deep_blue_genome/util/serialization/unordered_map.h>
#include <deep_blue_genome/util/serialization/vector.h>
#include <deep_blue_genome/util/serialization/flat_set.h>
#include <deep_blue_genome/util/serialization/flat_map.h>

namespace DEEP_BLUE_GENOME {

class Serialization
{
public:
	template <class T>
	static void load_from_binary(std::string path, T& object) {
		using namespace std;
		using namespace boost::iostreams;
		stream_buffer<file_source> buffer(path, ios::binary); // Note: this turned out to be strangely slightly faster than mapped_file
		boost::archive::binary_iarchive ar(buffer);
		ar >> object;
	}

	template <class T>
	static void save_to_binary(std::string path, const T& object) {
		using namespace std;
		using namespace boost::iostreams;
		using namespace boost::filesystem;
		create_directories(boost::filesystem::path(path).remove_filename());
		stream_buffer<file_sink> out(path, ios::binary);
		boost::archive::binary_oarchive ar(out);
		try {
			ar << object;
		}
		catch (const exception& e) {
			throw runtime_error("Error while writing to " + path + ": " + exception_what(e));
		}
	}
};


} // end namespace



