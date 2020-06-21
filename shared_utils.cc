#include "shared_utils.h"
#include <iostream>
#include <v8.h>

typedef struct {
	const uint8_t* data;
	size_t size;
} serialized_value;


SharedUtils::SharedUtils(){
	
}

SharedUtils::~SharedUtils()
{

}

bool SharedUtils::serialize(napi_value value, Buffer & result)
{
	
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8::Local<v8::Context> context = isolate->GetCurrentContext();
	v8::ValueSerializer serializer(isolate);
	serializer.WriteHeader();
	
	
	v8::Local<v8::Value> v8_value = toV8LocalValue(value);
	if (serializer.WriteValue(context, v8_value).FromMaybe(false)) {
		auto data = serializer.Release();
		serialized_value sv = { data.first, data.second };
		for (int i = 0; i < sv.size; i++)
			result.push_back(sv.data[i]);
		return true;
	}
	return false;
}

bool SharedUtils::deserialize(ShmemBuffer & buf, napi_value & result)
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8::Local<v8::Context> context = isolate->GetCurrentContext();
	
	v8::ValueDeserializer deserializer(isolate, (const uint8_t*)buf.data(), buf.size());
	if (deserializer.ReadHeader(context).FromMaybe(false)) {

		v8::MaybeLocal<v8::Value> value = deserializer.ReadValue(context);
		if (!value.IsEmpty()) {
			result = fromV8LocalValue(value.ToLocalChecked());
		}
		return true;
	}
	else
	{
		return false;
	}
}

napi_value SharedUtils::fromV8LocalValue(v8::Local<v8::Value> local) {
	return reinterpret_cast<napi_value>(*local);
}

v8::Local<v8::Value> SharedUtils::toV8LocalValue(napi_value v) {
	v8::Local<v8::Value> local;
	memcpy(&local, &v, sizeof(v));
	return local;
}

