#include "stdafx.h"
#include "StringClass.h"
#include "FloatClass.h"
#include "BooleanClass.h"
#include "MethodReflection.h"
#include "VirtualMachine.h"
#include "ArrayClass.h"

using namespace Beer;


// compare operator

#define BuildCompareStringOperatorFn(Name, Operation)																	\
struct CompareStringOperator##Name																						\
{																														\
	static void BEER_CALL fn(																							\
		VirtualMachine* vm, 																							\
		StackFrame* frame, 																								\
		StackRef<String> receiver, 																						\
		StackRef<String> arg, 																							\
		StackRef<Boolean> ret)																							\
	{																													\
		ret = vm->createBoolean(receiver->compare(arg.get()) Operation);												\
	}																													\
};																														\

#define BuildCompareStringOperator(Name, Operator, Operation)															\
	BuildCompareStringOperatorFn(Name, Operation);																		\
	method = loader->createMethod<MethodReflection>(																	\
			BEER_WIDEN(#Operator), 																						\
			string(BEER_WIDEN("String::")) + BEER_WIDEN(#Operator) + BEER_WIDEN("(String)"), 								\
			1, 																											\
			1																											\
		);																												\
	/*Name##Method->getReturn(0)->setType(integerClass);*/																\
	/*Name##Method->getParam(0)->setType(integerClass);*/																\
	method->setFunction(&(CompareStringOperator##Name::fn));															\
	klass->setMethod(methodi++, method);																				\




void BEER_CALL BeerString_init(VirtualMachine* vm, StackFrame* frame, StackRef<String> receiver, StackRef<String> ret)
{
	ret = receiver;
}

void BEER_CALL BeerString_getLength(VirtualMachine* vm, StackFrame* frame, StackRef<String> receiver, StackRef<Integer> ret)
{
	ret = vm->createInteger(receiver->size());
}

void BEER_CALL BeerString_concatString(VirtualMachine* vm, StackFrame* frame, StackRef<String> receiver, StackRef<String> arg, StackRef<String> ret)
{
	ret = vm->createString(receiver->size() + arg->size());
	ret->copyData(0, receiver->size(), receiver->c_str());
	ret->copyData(receiver->size(), arg->size(), arg->c_str());
}

void BEER_CALL BeerString_concatInteger(VirtualMachine* vm, StackFrame* frame, StackRef<String> receiver, StackRef<Integer> arg, StackRef<String> ret)
{
	stringstream ss;
	ss << arg->getData();
	string argStr = ss.str();

	ret = vm->createString(receiver->size() + argStr.size());
	ret->copyData(0, receiver->size(), receiver->c_str());
	ret->copyData(receiver->size(), argStr.size(), argStr.c_str());

}

void BEER_CALL BeerString_concatFloat(VirtualMachine* vm, StackFrame* frame, StackRef<String> receiver, StackRef<Float> arg, StackRef<String> ret)
{
	stringstream ss;
	ss << std::setprecision(8) << std::fixed << arg->getData();
	string argStr = ss.str();

	ret = vm->createString(receiver->size() + argStr.size());
	ret->copyData(0, receiver->size(), receiver->c_str());
	ret->copyData(receiver->size(), argStr.size(), argStr.c_str());
}

void BEER_CALL BeerString_concatBoolean(VirtualMachine* vm, StackFrame* frame, StackRef<String> receiver, StackRef<Boolean> arg, StackRef<String> ret)
{
	string str;
	if(arg->getData()) str = BEER_WIDEN("true");
	else str = BEER_WIDEN("false");

	ret = vm->createString(receiver->size() + str.size());
	ret->copyData(0, receiver->size(), receiver->c_str());
	ret->copyData(receiver->size(), str.size(), str.c_str());
}

void BEER_CALL BeerString_concatCharacter(VirtualMachine* vm, StackFrame* frame, StackRef<String> receiver, StackRef<Character> arg, StackRef<String> ret)
{
	ret = vm->createString(receiver->size() + 1);
	ret->copyData(0, receiver->size(), receiver->c_str());
	Character::CharacterData c = arg->getData();
	ret->copyData(receiver->size(), 1, &c);
}

void BEER_CALL BeerString_concatArray(VirtualMachine* vm, StackFrame* frame, StackRef<String> receiver, StackRef<Array> arg, StackRef<String> ret)
{
	string str;
	arg->toString(vm, str);

	ret = vm->createString(receiver->size() + str.size());
	ret->copyData(0, receiver->size(), receiver->c_str());
	ret->copyData(receiver->size(), str.size(), str.c_str());
}


String* StringClass::createInstance(StackFrame* frame, GarbageCollector* gc)
{
	Integer::IntegerData length = frame->stackTop<Integer>(frame->stackTopIndex())->getData();
	frame->stackPop(); // pop size
	String* str = gc->alloc<String>(sizeof(String) + sizeof(String::CharacterData) * (length + 1), /*String::CHILDREN_COUNT +*/ getPropertiesCount()); // +1 for \0
	str->size(length);
	//str->copyData();
	str->setClass(this);
	return str;
}

ClassReflection* StringClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, string name)
{
	return loader->createClass<StringClass>(name, 1, 0, 14);
}

void StringClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass)
{
	MethodReflection* method = NULL;
	uint16 methodi = 0;

	klass->extends(0, vm->getObjectClass());

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("String"), BEER_WIDEN("String::String()"), 1, 0);
	method->setFunction(&BeerString_init);
	klass->setMethod(methodi++, method);

	BuildCompareStringOperator(Lower, <, < 0);
	BuildCompareStringOperator(LowerEqual, <=, <= 0);
	BuildCompareStringOperator(Greater, >, > 0);
	BuildCompareStringOperator(GreaterEqual, >=, >= 0);
	BuildCompareStringOperator(Equal, ==, == 0);
	BuildCompareStringOperator(NotEqual, !=, != 0);

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("String"), BEER_WIDEN("String::+(String)"), 1, 1);
	method->setFunction(&BeerString_concatString);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("String"), BEER_WIDEN("String::+(Integer)"), 1, 1);
	method->setFunction(&BeerString_concatInteger);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("String"), BEER_WIDEN("String::+(Float)"), 1, 1);
	method->setFunction(&BeerString_concatFloat);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("String"), BEER_WIDEN("String::+(Boolean)"), 1, 1);
	method->setFunction(&BeerString_concatBoolean);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("String"), BEER_WIDEN("String::+(Character)"), 1, 1);
	method->setFunction(&BeerString_concatCharacter);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("String"), BEER_WIDEN("String::+(Array)"), 1, 1);
	method->setFunction(&BeerString_concatArray);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("Integer"), BEER_WIDEN("String::getLength()"), 1, 0);
	method->setFunction(&BeerString_getLength);
	klass->setMethod(methodi++, method);
}

// string pool

Reference<String> StringPool::translate(VirtualMachine* vm, const char16* str)
{
	StringMap::iterator it = mStrings.find(str);
	if(it == mStrings.end())
	{
		uint32 length = strlen(str);
		StackFrame* frame = vm->getStackFrame();
		CopyGC* heap = static_cast<CopyGC*>(vm->getHeap());

		frame->stackPush(vm->createInteger(length));
		String* result = vm->getStringClass()->createInstance<String>(frame, heap);
		result->copyData(str, length);

		return Reference<String>(heap, result);
	}

	return it->second;
}