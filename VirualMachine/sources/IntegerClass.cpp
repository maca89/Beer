#include "stdafx.h"
#include "IntegerClass.h"
#include "MethodReflection.h"
#include "StringClass.h"
#include "VirtualMachine.h"
#include "ParamReflection.h"
#include "FloatClass.h"
#include "BooleanClass.h"

using namespace Beer;


void BEER_CALL BeerInteger_init(VirtualMachine* vm, StackFrame* frame, StackRef<Integer> receiver, StackRef<Integer> ret1)
{
	ret1 = receiver;
}

void BEER_CALL BeerInteger_toFloat(VirtualMachine* vm, StackFrame* frame, StackRef<Integer> receiver, StackRef<Float> ret)
{
	ret = vm->createFloat(static_cast<Float::FloatData>(receiver->getData()));
}

void BEER_CALL BeerInteger_toString(VirtualMachine* vm, StackFrame* frame, StackRef<Integer> receiver, StackRef<String> ret)
{
	stringstream ss;
	ss << receiver->getData();
	// TODO: check and throw Exception
	ret = vm->createString(ss.str());
}

/*void BEER_CALL BeerInteger_equalInteger(VirtualMachine* vm, StackFrame* frame, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() == arg->getData());
}*/

// unary operator

#define BuildUnaryOperatorFn(Name, Do, Return)																			\
struct UnaryOperator##Name																								\
{																														\
	static void BEER_CALL fn(																							\
		VirtualMachine* vm, 																							\
		StackFrame* frame, 																								\
		StackRef<Integer> receiver, 																					\
		StackRef<Return> ret)																							\
	{																													\
		ret = vm->create##Return(receiver->getData() Do);																\
	}																													\
};																														\

#define BuildUnaryOperator(Class, Name, Operator, Return, Do)															\
	BuildUnaryOperatorFn(Name, Do, Return);																				\
	method = loader->createMethod<MethodReflection>(																	\
			1, 																											\
			0																											\
		);																												\
	/*method->getReturn(0)->setType(integerClass);*/																	\
	method->setName(vm->createString(BEER_WIDEN(#Operator)));															\
	method->setFunction(&(UnaryOperator##Name::fn));																	\
	Class->setMethod(methodi++, vm->createPair(vm->createString((string(BEER_WIDEN("Integer::")) + BEER_WIDEN(#Operator) + BEER_WIDEN("()")).c_str()), method));\




// binary operator

#define BuildBinaryOperatorFn(Name, Operator, Param, Return)															\
struct BinaryOperator##Name																								\
{																														\
	static void BEER_CALL fn(																							\
		VirtualMachine* vm, 																							\
		StackFrame* frame, 																								\
		StackRef<Integer> receiver, 																					\
		StackRef<Param> arg, 																							\
		StackRef<Return> ret)																							\
	{																													\
		ret = vm->create##Return(static_cast<Return::Return##Data>(receiver->getData() Operator arg->getData()));		\
	}																													\
};																														\

#define BuildBinaryOperator(Class, Name, Operator, Param, Return, Do)													\
	BuildBinaryOperatorFn(Name, Do, Param, Return);																		\
	method = loader->createMethod<MethodReflection>(																	\
			1, 																											\
			1																											\
		);																												\
	method->setName(vm->createString(BEER_WIDEN(#Operator)));														\
	/*method->getReturn(0)->setType(integerClass);*/																\
	/*method->getParam(0)->setType(integerClass);*/																\
	method->setFunction(&(BinaryOperator##Name::fn));																\
	Class->setMethod(methodi++, vm->createPair(vm->createString((string(BEER_WIDEN("Integer::")) + BEER_WIDEN(#Operator) + BEER_WIDEN("(") + BEER_WIDEN(#Param) + BEER_WIDEN(")")).c_str()), method));\


// compare operator

#define BuildCompareOperatorFn(Name, Operator, Param)																	\
struct CompareOperator##Name																							\
{																														\
	static void BEER_CALL fn(																							\
		VirtualMachine* vm, 																							\
		StackFrame* frame, 																								\
		StackRef<Integer> receiver, 																					\
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
	/*method->getReturn(0)->setType(integerClass);*/																	\
	/*method->getParam(0)->setType(integerClass);*/																		\
	method->setFunction(&(CompareOperator##Name::fn));																	\
	Class->setMethod(methodi++, vm->createPair(vm->createString((string(BEER_WIDEN("Integer::")) + BEER_WIDEN(#Operator) + BEER_WIDEN("(") + BEER_WIDEN(#Param) + BEER_WIDEN(")")).c_str()), method));	\



Class* IntegerClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<IntegerClass>(name, 1, 0, 15);
}

void IntegerClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	klass->markAsValueType();
	uint16 methodi = 0;
	MethodReflection* method = NULL;

	Class* integerClass = klass;
	Class* objectClass = vm->getObjectClass();
	//ClassReflection* floatClass = vm->getFloatClass();
	//ClassReflection* booleanClass = vm->getBooleanClass();

	klass->extends(methodi++, objectClass);

	//MethodReflection* initMethod = loader->createMethod<MethodReflection>(BEER_WIDEN("Integer"), string(klass->getName()) + BEER_WIDEN("::Integer()"), 1, 0);
	//initMethod->setFunction(&BeerInteger_init);
	//klass->setMethod(methodi++, initMethod);
	
	BuildBinaryOperator(klass, Add, +, Integer, Integer, +);
	BuildBinaryOperator(klass, Sub, -, Integer, Integer, -);
	BuildBinaryOperator(klass, Mul, *, Integer, Integer, *);
	BuildBinaryOperator(klass, Div, /, Integer, Float, /);
	BuildBinaryOperator(klass, Modulo, %, Integer, Integer, %);
	
	BuildUnaryOperator(klass, Minus, -, Integer, * (-1));
	
	BuildCompareOperator(klass, Smaller, <, Integer, <);
	BuildCompareOperator(klass, Greater, >, Integer, >);
	BuildCompareOperator(klass, SmallerEqual, <=, Integer, <=);
	BuildCompareOperator(klass, GreaterEqual, >=, Integer, >=);
	BuildCompareOperator(klass, Equal, ==, Integer, ==);
	BuildCompareOperator(klass, NotEqual, !=, Integer, !=);
	
	method = loader->createMethod<MethodReflection>(1, 0);
	method->setName(vm->createString(BEER_WIDEN("Float")));
	method->setFunction(&BeerInteger_toFloat);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Integer::Float()")), method));
	
	method = loader->createMethod<MethodReflection>(1, 0);
	method->setName(vm->createString(BEER_WIDEN("String")));
	method->setFunction(&BeerInteger_toString);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Integer::String()")), method));
}

