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
	std::stringstream ss;
	ss << receiver->getData();
	// TODO: check and throw Exception
	ret = vm->createString(ss.str());
}

/*void BEER_CALL BeerInteger_operatorIncrement(VirtualMachine* vm, StackFrame* frame, StackRef<Integer> receiver, StackRef<Integer> ret)
{
	Integer::IntegerData data = receiver->getData();
	receiver = vm->createInteger(data + 1);
	ret = vm->createInteger(data);
}

void BEER_CALL BeerInteger_operatorDecrement(VirtualMachine* vm, StackFrame* frame, StackRef<Integer> receiver, StackRef<Integer> ret)
{
	Integer::IntegerData data = receiver->getData();
	receiver = vm->createInteger(data - 1);
	ret = vm->createInteger(data);
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
	MethodReflection* Name##Method																						\
		= loader->createMethod<MethodReflection>(																		\
			#Operator, 																									\
			std::string(Class->getName()) + "::" + #Operator + "()", 													\
			1, 																											\
			0																											\
		);																												\
	/*Name##Method->getReturn(0)->setType(integerClass);*/																\
	Name##Method->setFunction(&(UnaryOperator##Name::fn));																\
	Class->setMethod(methodi++, Name##Method);																			\




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
		ret = vm->create##Return(receiver->getData() Operator arg->getData());											\
	}																													\
};																														\

#define BuildBinaryOperator(Class, Name, Operator, Param, Return, Do)													\
	BuildBinaryOperatorFn(Name, Do, Param, Return);																		\
	MethodReflection* Name##Method																						\
		= loader->createMethod<MethodReflection>(																		\
			#Operator, 																									\
			std::string(Class->getName()) + "::" + #Operator + "(" + #Param + ")", 										\
			1, 																											\
			1																											\
		);																												\
	/*Name##Method->getReturn(0)->setType(integerClass);*/																\
	/*Name##Method->getParam(0)->setType(integerClass);*/																\
	Name##Method->setFunction(&(BinaryOperator##Name::fn));																\
	Class->setMethod(methodi++, Name##Method);																			\


// compare operator

#define BuildCompareOperatorFn(Name, Operator, Param, Return)															\
struct CompareOperator##Name																							\
{																														\
	static void BEER_CALL fn(																							\
		VirtualMachine* vm, 																							\
		StackFrame* frame, 																								\
		StackRef<Integer> receiver, 																					\
		StackRef<Param> arg, 																							\
		StackRef<Return> ret)																							\
	{																													\
		ret = vm->getClass(#Return)->createInstance<Return>(frame, vm->getHeap());										\
		ret->setData(static_cast<Return::Return##Data>(receiver->getData() Operator arg->getData()));					\
	}																													\
};																														\

#define BuildCompareOperator(Class, Name, Operator, Param, Return, Do)													\
	BuildCompareOperatorFn(Name, Do, Param, Return);																	\
	MethodReflection* Name##Method																						\
		= loader->createMethod<MethodReflection>(																		\
			#Operator, 																									\
			std::string(Class->getName()) + "::" + #Operator + "(" + #Param + ")", 										\
			1, 																											\
			1																											\
		);																												\
	/*Name##Method->getReturn(0)->setType(integerClass);*/																\
	/*Name##Method->getParam(0)->setType(integerClass);*/																\
	Name##Method->setFunction(&(CompareOperator##Name::fn));															\
	Class->setMethod(methodi++, Name##Method);																			\



ClassReflection* IntegerClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, std::string name)
{
	return loader->createClass<IntegerClass>(name, 1, 0, 15);
}

void IntegerClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass)
{
	klass->markAsValueType();
	uint16 methodi = 0;
	MethodReflection* method = NULL;

	ClassReflection* integerClass = klass;
	ClassReflection* objectClass = vm->getClass("Object");
	ClassReflection* floatClass = vm->getClass("Float");
	ClassReflection* booleantClass = vm->getClass("Boolean");

	klass->extends(methodi++, objectClass);

	//MethodReflection* initMethod = loader->createMethod<MethodReflection>("Integer", std::string(klass->getName()) + "::Integer()", 1, 0);
	//initMethod->setFunction(&BeerInteger_init);
	//klass->setMethod(methodi++, initMethod);
	
	BuildBinaryOperator(klass, Add, +, Integer, Integer, +);
	BuildBinaryOperator(klass, Sub, -, Integer, Integer, -);
	BuildBinaryOperator(klass, Mul, *, Integer, Integer, *);
	BuildBinaryOperator(klass, Div, /, Integer, Float, /);
	BuildBinaryOperator(klass, Modulo, %, Integer, Integer, %);
	
	//BuildUnaryOperator(klass, Increment, ++, Integer, ++);
	//BuildUnaryOperator(klass, Decrement, --, Integer, --);
	BuildUnaryOperator(klass, Minus, -, Integer, * (-1));
	
	BuildCompareOperator(klass, Smaller, <, Integer, Boolean, <);
	BuildCompareOperator(klass, Greater, >, Integer, Boolean, >);
	BuildCompareOperator(klass, SmallerEqual, <=, Integer, Boolean, <=);
	BuildCompareOperator(klass, GreaterEqual, >=, Integer, Boolean, >=);
	BuildCompareOperator(klass, Equal, ==, Integer, Boolean, ==);
	BuildCompareOperator(klass, NotEqual, !=, Integer, Boolean, !=);
	
	method = loader->createMethod<MethodReflection>("Float", "Integer::Float()", 1, 0);
	method->setFunction(&BeerInteger_toFloat);
	klass->setMethod(methodi++, method);
	
	method = loader->createMethod<MethodReflection>("String", "Integer::String()", 1, 0);
	method->setFunction(&BeerInteger_toString);
	klass->setMethod(methodi++, method);
	
	/*method = loader->createMethod<MethodReflection>("Integer", "Integer::++()", 1, 0);
	method->setFunction(&BeerInteger_operatorIncrement);
	klass->setMethod(methodi++, method);
	
	method = loader->createMethod<MethodReflection>("Integer", "Integer::--()", 1, 0);
	method->setFunction(&BeerInteger_operatorDecrement);
	klass->setMethod(methodi++, method);*/
}

