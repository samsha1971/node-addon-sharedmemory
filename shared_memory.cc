#include "shared_memory.h"
#include "shared_map.h"
#include "shared_vector.h"
#include <iostream>


Napi::Object SharedMemory::Init(Napi::Env env, Napi::Object exports)
{
	exports = SharedMap::Init(env, exports);
	exports = SharedVector::Init(env, exports);
	return exports;
}



SharedMemory::SharedMemory(const Napi::CallbackInfo &info)
	: Napi::ObjectWrap<SharedMemory>(info)
{
	
}

SharedMemory::~SharedMemory()
{
	
}

