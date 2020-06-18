#pragma once
#include "napi.h"

class SharedMemory: public Napi::ObjectWrap<SharedMemory>
{
public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);

public:
	SharedMemory(const Napi::CallbackInfo &info);
	~SharedMemory();
};

