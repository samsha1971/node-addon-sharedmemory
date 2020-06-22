#pragma once

#include "shared_utils.h"

typedef vector<ShmemBuffer, ShmemBufferAllocator> ShmemVector;

class SharedVector : public Napi::ObjectWrap<SharedVector>
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
	SharedVector(const Napi::CallbackInfo &info);
	~SharedVector();

private:
	//methods
	void push_back(const Napi::CallbackInfo &info);
	Napi::Value at(const Napi::CallbackInfo &info);
	void erase(const Napi::CallbackInfo &info);
	Napi::Value empty(const Napi::CallbackInfo &info);
	void clear(const Napi::CallbackInfo &info);

private:
	// variables
	static Napi::FunctionReference constructor;
	managed_mapped_file *pSegment;
	ShmemVector *pVector;
	ShmemObject *pObj;

	// properties
	std::string name;
	Napi::Value getName(const Napi::CallbackInfo &info);
	void setName(const Napi::CallbackInfo &info, const Napi::Value &value);

	Napi::Value getValue(const Napi::CallbackInfo &info);

	// utils
};

