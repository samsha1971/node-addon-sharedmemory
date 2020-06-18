#include "shared_vector.h"
#include <iostream>

Napi::FunctionReference SharedVector::constructor;
Napi::Object SharedVector::Init(Napi::Env env, Napi::Object exports)
{
	Napi::HandleScope scope(env);

	Napi::Function func = DefineClass(env,
		"Vector",
		{
			InstanceMethod("push_back", &SharedVector::push_back),
			InstanceMethod("at", &SharedVector::at),
			InstanceMethod("erase", &SharedVector::erase),
			InstanceMethod("empty", &SharedVector::empty),
			InstanceMethod("clear", &SharedVector::clear),
			//InstanceMethod("getValue", &SharedVector::getValue),
			InstanceAccessor("name", &SharedVector::getName, &SharedVector::setName),
			InstanceAccessor("value", &SharedVector::getValue, NULL)
		}
	);

	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();

	exports.Set("Vector", func);
	return exports;
}

ShmemString SharedVector::toShare(std::string str)
{
	CharAllocator charAlloc(pSegment->get_segment_manager());
	ShmemString sstr(charAlloc);
	sstr = str.c_str();
	return sstr;
}

std::string SharedVector::fromShare(ShmemString sstr)
{
	std::string str = sstr.c_str();
	return str;
}


Napi::Value SharedVector::getName(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);
	return Napi::String::New(env, this->name);
}
void SharedVector::setName(const Napi::CallbackInfo &info, const Napi::Value &value)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);
	this->name = value.As<Napi::String>();
}

SharedVector::SharedVector(const Napi::CallbackInfo &info)
	: Napi::ObjectWrap<SharedVector>(info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	size_t length = info.Length();

	if (length <= 0 || !info[0].IsString())
	{
		Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
		return;
	}

	Napi::String value = info[0].As<Napi::String>();
	this->name = value.Utf8Value();

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

		pVector = pSegment->find<ShmemVector>("ShmemVector").first;
		if (pVector == NULL)
		{
			const StringAllocator stringAlloc(pSegment->get_segment_manager());
			pVector = pSegment->construct<ShmemVector>("ShmemVector")(stringAlloc);
		}

		pObj = pSegment->find<ShmemObject>("ShmemVector_ShmemObject").first;
		if (pObj == NULL)
		{
			pObj = pSegment->construct<ShmemObject>("ShmemVector_ShmemObject")();
		}
	}
	catch (std::exception& e)
	{
		Napi::Error::New(env, e.what() ).ThrowAsJavaScriptException();
		return;
	}
}

SharedVector::~SharedVector()
{
	delete pSegment;
}


void SharedVector::push_back(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);


	scoped_lock<interprocess_mutex> lock(pObj->mutex);

	size_t length = info.Length();

	if (length <= 0 || !info[0].IsString())
	{
		Napi::TypeError::New(env, "Invalid Parameters, String expected").ThrowAsJavaScriptException();
		return;
	}

	std::string value = info[0].As<Napi::String>();
	ShmemString svalue = toShare(value);
	pVector->push_back(svalue);
	pSegment->flush();

}

Napi::Value SharedVector::at(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);
	Napi::String r = Napi::String::New(env, "");
	size_t length = info.Length();
	if (length <= 0 || !info[0].IsNumber())
	{
		Napi::TypeError::New(env, "Invalid Parameters, Number expected").ThrowAsJavaScriptException();
		return  r;
	}
	int32_t pos = info[0].As<Napi::Number>().Int32Value();
	if (pos >= pVector->size())
	{
		Napi::Error::New(env, "Invalid Parameter, Out of range").ThrowAsJavaScriptException();
		return  r;
	}
	ShmemString svalue = pVector->at(pos);
	r = Napi::String::New(env, svalue.c_str());
	return r;
}

void SharedVector::erase(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);
	size_t length = info.Length();
	if (length <= 0 || !info[0].IsNumber())
	{
		Napi::TypeError::New(env, "Invalid Parameters, Number expected").ThrowAsJavaScriptException();
		return;
	}
	scoped_lock<interprocess_mutex> lock(pObj->mutex);
	ShmemVector::iterator pos = pVector->begin();
	int32_t num = info[0].As<Napi::Number>().ToNumber().Int32Value();
	pos += num;
	pVector->erase(pos);
	pSegment->flush();

}

Napi::Value SharedVector::empty(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);
	return Napi::Boolean::New(env, pVector->empty());
}

void SharedVector::clear(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);
	scoped_lock<interprocess_mutex> lock(pObj->mutex);
	pVector->clear();
	pSegment->flush();
};

Napi::Value SharedVector::getValue(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);
	Napi::Array r = Napi::Array::New(env);

	for (int i = 0; i < pVector->size(); i++)
	{
		r[i] = pVector->at(i).c_str();
	}
	return r;
}
