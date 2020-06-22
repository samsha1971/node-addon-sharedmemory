#pragma once

#include "shared_utils.h"

typedef basic_string<char, std::char_traits<char>, CharAllocator> ShmemString;
typedef std::pair<const ShmemBuffer, ShmemBuffer> PairType;
typedef allocator<PairType, managed_shared_memory::segment_manager> PairAllocator;
typedef map<ShmemBuffer, ShmemBuffer, std::less<ShmemBuffer>, PairAllocator> ShmemMap;


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

};

