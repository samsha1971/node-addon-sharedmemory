#pragma once

#include <napi.h>
#include <v8.h>
#include <boost/any.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/map.hpp>
#include "boost/interprocess/containers/string.hpp"
#include <boost/interprocess/allocators/allocator.hpp>

using namespace boost::interprocess;


typedef allocator<char, managed_shared_memory::segment_manager> CharAllocator;
typedef vector<char> Buffer;
typedef vector<char, CharAllocator> ShmemBuffer;
typedef allocator<ShmemBuffer, managed_shared_memory::segment_manager> ShmemBufferAllocator;


class SharedUtils
{
public:
	SharedUtils();
	~SharedUtils();
	static bool serialize(napi_value value, Buffer & result);
	static bool deserialize(ShmemBuffer &value, napi_value & result);

private:
	static napi_value fromV8LocalValue(v8::Local<v8::Value> local);
	static v8::Local<v8::Value> toV8LocalValue(napi_value v);
};
