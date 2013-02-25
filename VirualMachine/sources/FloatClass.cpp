#include "stdafx.h"
#include "FloatClass.h"
#include "IntegerClass.h"
#include "BooleanClass.h"
#include "MethodReflection.h"
#include "StringClass.h"
#include "VirtualMachine.h"

using namespace Beer;


void BEER_CALL BeerFloat_init(VirtualMachine* vm, StackFrame* frame, StackRef<Float> receiver, StackRef<Float> ret)
{
	ret = receiver;
}

void BEER_CALL BeerFloat_toInteger(VirtualMachine* vm, StackFrame* frame, StackRef<Float> receiver, StackRef<Integer> ret)
{
	ret = vm->createInteger(static_cast<Integer::IntegerData>(receiver->getData()));
}

void BEER_CALL BeerFloat_toString(VirtualMachine* vm, StackFrame* frame, StackRef<Float> receiver, StackRef<String> ret)
{
	stringstream ss;
	ss << std::setprecision(8) << std::fixed << receiver->getData();
	// TODO: check and throw Exception
	ret = vm->createString(ss.str());
}


// unary operator

#define BuildUnaryOperatorFn(Name, Do, Return)																			\
struct UnaryOperator##Name																								\
{																														\
	static void BEER_CALL fn(																							\
		VirtualMachine* vm, 																							\
		StackFrame* frame, 																								\
		StackRef<Float> receiver, 																						\
		StackRef<Return> ret)																							\
	{																													\
		ret = vm->getClass(BEER_WIDEN(#Return))->createInstance<Return>(vm, frame, vm->getHeap());						\
		ret->setData(receiver->getData() Do);																			\
	}																													\
};																														\

#define BuildUnaryOperator(Class, Name, Operator, Return, Do)															\
	BuildUnaryOperatorFn(Name, Do, Return);																				\
	method = loader->createMethod<MethodReflection>(																	\
			1, 																											\
			0																											\
		);																												\
	method->setName(vm->createString(BEER_WIDEN(#Operator)));															\
	/*Name##Method->getReturn(0)->setType(integerClass);*/																\
	method->setFunction((&(UnaryOperator##Name::fn)));																	\
	Class->setMethod(methodi++, vm->createPair(vm->createString((string(BEER_WIDEN("Float::")) + BEER_WIDEN(#Operator) + BEER_WIDEN("()"))), method));\




// binary operator

#define BuildBinaryOperatorFn(Name, Operator, Param, Return)															\
struct BinaryOperator##Name																								\
{																														\
	static void BEER_CALL fn(																							\
		VirtualMachine* vm, 																							\
		StackFrame* frame, 																								\
		StackRef<Float> receiver, 																						\
		StackRef<Param> arg, 																							\
		StackRef<Return> ret)																							\
	{																													\
		ret = vm->getClass(BEER_WIDEN(#Return))->createInstance<Return>(vm, frame, vm->getHeap());						\
		ret->setData(static_cast<Return::Return##Data>(receiver->getData()) Operator arg->getData());					\
	}																													\
};																														\

#define BuildBinaryOperator(Class, Name, Operator, Param, Return, Do)													\
	BuildBinaryOperatorFn(Name, Do, Param, Return);																		\
	method = loader->createMethod<MethodReflection>(																	\
			1, 																											\
			1																											\
		);																												\
	method->setName(vm->createString(BEER_WIDEN(#Operator)));															\
	/*Name##Method->getReturn(0)->setType(integerClass);*/																\
	/*Name##Method->getParam(0)->setType(integerClass);*/																\
	method->setFunction(&(BinaryOperator##Name::fn));																	\
	Class->setMethod(methodi++, vm->createPair(vm->createString((string(BEER_WIDEN("Float::")) + BEER_WIDEN(#Operator) + BEER_WIDEN("(") + BEER_WIDEN(#Param) + BEER_WIDEN(")"))), method));\


// compare operator

#define BuildCompareOperatorFn(Name, Operator, Param)																	\
struct CompareOperator##Name																							\
{																														\
	static void BEER_CALL fn(																							\
		VirtualMachine* vm, 																							\
		StackFrame* frame, 																								\
		StackRef<Float> receiver, 																						\
		StackRef<Param> arg, 																							\
		StackRef<Boolean> ret)																							\
	{																													\
		ret = Boolean::makeInlineValue(receiver->getData() Operator arg->getData());									\
	}																													\
};																														\

#define BuildCompareOperator(Class, Name, Operator, Param, Do)															\
	BuildCompareOperatorFn(Name, Do, Param);																			\
	method = loader->createMethod<MethodReflection>(																	\
			1, 																											\
			1																											\
		);																												\
	method->setName(vm->createString(BEER_WIDEN(#Operator)));															\
	/*Name##Method->getReturn(0)->setType(integerClass);*/																\
	/*Name##Method->getParam(0)->setType(integerClass);*/																\
	method->setFunction(&(CompareOperator##Name::fn));																	\
	Class->setMethod(methodi++, vm->createPair(vm->createString((string(BEER_WIDEN("Float::")) + BEER_WIDEN(#Operator) + BEER_WIDEN("(") + BEER_WIDEN(#Param) + BEER_WIDEN(")"))), method));\






Class* FloatClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<FloatClass>(name, 1, 0, 14);
}

void FloatClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	klass->markAsValueType();
	uint16 methodi = 0;
	klass->extends(0, vm->getObjectClass());
	
	MethodReflection* method = loader->createMethod<MethodReflection>(1, 0);
	method->setName(vm->createString(BEER_WIDEN("Float")));
	method->setFunction(&BeerFloat_init);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Float::Float()")), method));
	
	BuildBinaryOperator(klass, Add, +, Float, Float, +);
	BuildBinaryOperator(klass, Sub, -, Float, Float, -);
	BuildBinaryOperator(klass, Mul, *, Float, Float, *);
	BuildBinaryOperator(klass, Div, /, Float, Float, /);
	
	BuildUnaryOperator(klass, Minus, -, Float, * (-1));
	
	BuildCompareOperator(klass, Smaller, <, Float, <);
	BuildCompareOperator(klass, Greater, >, Float, >);
	BuildCompareOperator(klass, SmallerEqual, <=, Float, <=);
	BuildCompareOperator(klass, GreaterEqual, >=, Float, >=);
	BuildCompareOperator(klass, Equal, ==, Float, ==);
	BuildCompareOperator(klass, NotEqual, !=, Float, !=);
	
	method = loader->createMethod<MethodReflection>(1, 0);
	method->setName(vm->createString(BEER_WIDEN("Integer")));
	method->setFunction(&BeerFloat_toInteger);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Float::Integer()")), method));
	
	method = loader->createMethod<MethodReflection>(1, 0);
	method->setName(vm->createString(BEER_WIDEN("String")));
	method->setFunction(&BeerFloat_toString);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Float::String()")), method));
	
}