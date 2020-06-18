#include "shared_map.h"
#include <iostream>

Napi::FunctionReference SharedMap::constructor;
Napi::Object SharedMap::Init(Napi::Env env, Napi::Object exports)
{
	Napi::HandleScope scope(env);

	Napi::Function func = DefineClass(env,
		"Map",
		{
			InstanceMethod("insert", &SharedMap::insert),
			InstanceMethod("at", &SharedMap::at),
			InstanceMethod("erase", &SharedMap::erase),
			InstanceMethod("empty", &SharedMap::empty),
			InstanceMethod("clear", &SharedMap::clear),
			//InstanceMethod("getValue", &SharedMap::getValue),
			InstanceAccessor("name", &SharedMap::getName, &SharedMap::setName),
			InstanceAccessor("value", &SharedMap::getValue, NULL)
		}
	);

	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();

	exports.Set("Map", func);
	return exports;
}

ShmemString SharedMap::toShare(std::string str)
{
	CharAllocator charAlloc(pSegment->get_segment_manager());
	ShmemString sstr(charAlloc);
	sstr = str.c_str();
	return sstr;
}

std::string SharedMap::fromShare(ShmemString sstr)
{
	std::string str = sstr.c_str();
	return str;
}

Napi::Value SharedMap::getName(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);
	return Napi::String::New(env, this->name);
}

void SharedMap::setName(const Napi::CallbackInfo &info, const Napi::Value &value)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);
	this->name = value.As<Napi::String>();
}

SharedMap::SharedMap(const Napi::CallbackInfo &info)
	: Napi::ObjectWrap<SharedMap>(info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	size_t length = info.Length();

	if (length <= 0 || !info[0].IsString())
	{
		Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
		return;
	}

	Napi::String arg0 = info[0].As<Napi::String>();
	this->name = arg0.Utf8Value();

	Napi::Number arg1 = info[1].As<Napi::Number>();
	int32_t memorySize = 64 * 1024;
	if (arg1.IsNumber()) {
		memorySize = arg1.ToNumber().Int32Value();
	}
	if (memorySize < 1024)
		memorySize = 1024;

	try
	{
		pSegment = new managed_mapped_file(open_or_create, this->name.c_str(), memorySize);

		pMap = pSegment->find<ShmemMap>("ShmemMap").first;
		if (pMap == NULL)
		{
			const PairAllocator pairAlloc(pSegment->get_segment_manager());
			pMap = pSegment->construct<ShmemMap>("ShmemMap")(pairAlloc);
		}

		pObj = pSegment->find<ShmemObject>("ShmemMap_ShmemObject").first;
		if (pObj == NULL)
		{
			pObj = pSegment->construct<ShmemObject>("ShmemMap_ShmemObject")();
		}
	}
	catch (std::exception& e)
	{
		Napi::Error::New(env, e.what() ).ThrowAsJavaScriptException();
		return;
	}
}

SharedMap::~SharedMap()
{
	delete pSegment;
}

void SharedMap::insert(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	size_t length = info.Length();
	if (length <= 1 || !info[0].IsString() || !info[1].IsString())
	{
		Napi::TypeError::New(env, "Invalid Parameters, map pair (key,value) expected").ThrowAsJavaScriptException();
		return;
	}

	scoped_lock<interprocess_mutex> lock(pObj->mutex);

	CharAllocator charAlloc(pSegment->get_segment_manager());
	std::string key = info[0].As<Napi::String>();
	ShmemString skey = toShare(key);
	std::string value = info[1].As<Napi::String>();
	ShmemString svalue = toShare(value);
	pMap->insert(PairType(skey, svalue));
	pSegment->flush();
}

Napi::Value SharedMap::at(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);
	Napi::Object r = Napi::Object::New(env);
	size_t length = info.Length();
	if (length <= 0 || !info[0].IsString())
	{
		Napi::TypeError::New(env, "Invalid Parameters, String expected").ThrowAsJavaScriptException();
		return r;
	}

	std::string key = info[0].As<Napi::String>();
	ShmemString skey = toShare(key);

	ShmemMap::iterator pos = pMap->find(skey);

	if (pos == pMap->end())
	{
		Napi::Error::New(env, "Invalid Parameter, Out of Range").ThrowAsJavaScriptException();
		return r;
	}
	ShmemString svalue = pMap->at(skey);
	r.Set(key, svalue.c_str());

	return r;
}

void SharedMap::erase(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);
	size_t length = info.Length();
	if (length <= 0 || !info[0].IsString())
	{
		Napi::TypeError::New(env, "Invalid Parameters, String expected").ThrowAsJavaScriptException();
		return;
	}

	scoped_lock<interprocess_mutex> lock(pObj->mutex);
	std::string key = info[0].As<Napi::String>();
	ShmemString skey = toShare(key);
	pMap->erase(skey);
	pSegment->flush();
}

Napi::Value SharedMap::empty(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);
	return Napi::Boolean::New(env, pMap->empty());
}

void SharedMap::clear(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);
	scoped_lock<interprocess_mutex> lock(pObj->mutex);
	pMap->clear();
	pSegment->flush();
};

Napi::Value SharedMap::getValue(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);
	Napi::Object r = Napi::Object::New(env);
	for (auto &kv : *pMap) {
		std::string key = kv.first.c_str();
		std::string value = kv.second.c_str();
		r.Set(key, value);
	}
	return r;
}
