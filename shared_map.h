#pragma once

#include <napi.h>
#include <boost/any.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/containers/map.hpp>
#include "boost/interprocess/containers/string.hpp"
#include <boost/interprocess/allocators/allocator.hpp>
#include <string>
#include <cstdlib>

using namespace boost::interprocess;

typedef allocator<char, managed_shared_memory::segment_manager> CharAllocator;
typedef basic_string<char, std::char_traits<char>, CharAllocator> ShmemString;
typedef std::pair<const ShmemString, ShmemString> PairType;
typedef allocator<PairType, managed_shared_memory::segment_manager> PairAllocator;
typedef map<ShmemString, ShmemString, std::less<ShmemString>, PairAllocator> ShmemMap;


class SharedMap : public Napi::ObjectWrap<SharedMap>
{
	class ShmemObject
	{
	public:
		ShmemObject() {}
		boost::interprocess::interprocess_mutex mutex;
	};
public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);

public:
	SharedMap(const Napi::CallbackInfo &info);
	~SharedMap();

private:
	//methods
	void insert(const Napi::CallbackInfo &info);
	Napi::Value at(const Napi::CallbackInfo &info);
	void erase(const Napi::CallbackInfo &info);
	Napi::Value empty(const Napi::CallbackInfo &info);
	void clear(const Napi::CallbackInfo &info);
	

private:
	// variables
	static Napi::FunctionReference constructor;
	managed_mapped_file *pSegment;
	ShmemMap *pMap;
	ShmemObject *pObj;

	// properties
	std::string name;
	Napi::Value getName(const Napi::CallbackInfo &info);
	void setName(const Napi::CallbackInfo &info, const Napi::Value &value);

	Napi::Value getValue(const Napi::CallbackInfo &info);
	
	// utils
	ShmemString toShare(std::string);
	std::string fromShare(ShmemString);

};

