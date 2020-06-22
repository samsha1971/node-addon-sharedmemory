#include "shared_map.h"


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
		Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
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
	if (length <= 1)
	{
		Napi::TypeError::New(env, "Invalid Parameters, map pair (key,value) expected").ThrowAsJavaScriptException();
		return;
	}

	scoped_lock<interprocess_mutex> lock(pObj->mutex);

	CharAllocator charAlloc(pSegment->get_segment_manager());

	Buffer buffer0, buffer1;
	bool b0 = SharedUtils::serialize(info[0], buffer0);
	bool b1 = SharedUtils::serialize(info[1], buffer1);
	if (b0 && b1)
	{
		ShmemBuffer sKey(charAlloc);
		SharedUtils::copy(buffer0, sKey);
		ShmemBuffer sValue(charAlloc);
		SharedUtils::copy(buffer1, sValue);
		pMap->insert(PairType(sKey, sValue));
		pSegment->flush();
	}
	else
	{
		Napi::Error::New(env, "Invalid Parameters").ThrowAsJavaScriptException();
	}

}

Napi::Value SharedMap::at(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);
	// Napi::Object r = Napi::Object::New(env);
	Napi::Value r;
	size_t length = info.Length();
	if (length <= 0)
	{
		Napi::TypeError::New(env, "Invalid Parameters, String expected").ThrowAsJavaScriptException();
		return r;
	}

	CharAllocator charAlloc(pSegment->get_segment_manager());
	napi_value key = info[0];
	ShmemBuffer sKey(charAlloc);
	Buffer buffer0;
	bool b0 = SharedUtils::serialize(key, buffer0);
	if (b0)
	{
		ShmemBuffer sKey(charAlloc);
		SharedUtils::copy(buffer0, sKey);

		ShmemMap::iterator pos = pMap->find(sKey);
		if (pos == pMap->end())
		{
			Napi::RangeError::New(env, "Invalid Parameter, Out of Range").ThrowAsJavaScriptException();
			return r;
		}
		ShmemBuffer sValue = pMap->at(sKey);
		napi_value value;
		bool b = SharedUtils::deserialize(sValue, value);
		if (b) {
			return Napi::Value(env, value);
		}
		else
		{
			Napi::Error::New(env, "deserialize").ThrowAsJavaScriptException();
			return r;
		}
	}
	else
	{
		Napi::Error::New(env, "serialize").ThrowAsJavaScriptException();
		return r;
	}

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
	CharAllocator charAlloc(pSegment->get_segment_manager());
	ShmemBuffer sKey(charAlloc);
	Buffer buffer0;
	bool b0 = SharedUtils::serialize(info[0], buffer0);
	if (b0)
	{
		ShmemBuffer sKey(charAlloc);
		SharedUtils::copy(buffer0, sKey);
		pMap->erase(sKey);
		pSegment->flush();
	}
	else
	{
		Napi::Error::New(env, "serialize").ThrowAsJavaScriptException();
	}
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
		ShmemBuffer sKey = kv.first;
		ShmemBuffer sValue = kv.second;
		napi_value key, value;
		bool b0 = SharedUtils::deserialize(sKey, key);
		bool b1 = SharedUtils::deserialize(sValue, value);
		if (b0 && b1) {
			r.Set(key, value);
		}
		else
		{
			Napi::Error::New(env, "Invalid Parameter, Out of range").ThrowAsJavaScriptException();
			return r;
		}

	}
	return r;
}
