#include "stdafx.h"
#include "String.h"
#include "Float.h"
#include "Boolean.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "Array.h"
#include "Debugger.h"

using namespace Beer;


StringPool String::gPool = StringPool(); // TODO: get rid of


void BEER_CALL String::init(Thread* thread, StackRef<String> receiver, StackRef<String> ret)
{
	ret = receiver;
}

void BEER_CALL String::operatorString(Thread* thread, StackRef<Object> receiver, StackRef<String> ret)
{
	ret = receiver.staticCast<String>();
}

void BEER_CALL String::getLength(Thread* thread, StackRef<String> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->size());
}

void BEER_CALL String::operatorGet(Thread* thread, StackRef<String> receiver, StackRef<Integer> index, StackRef<Character> ret)
{
	Integer::IntegerData indexdata = index.get()->getData();
	BOUNDS_ASSERT(indexdata, receiver->size());
	ret = Character::makeInlineValue(receiver->c_str()[indexdata]);
}

void BEER_CALL String::operatorAddString(Thread* thread, StackRef<String> receiver, StackRef<String> arg, StackRef<String> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> length(frame, frame->stackPush());
	thread->createInteger(length, receiver->size() + arg->size());

	thread->createString(length, ret); // does not pop length

	ret->copyData(0, receiver->size(), receiver->c_str());
	ret->copyData(receiver->size(), arg->size(), arg->c_str());

	frame->stackPop(); // pop length
}

void BEER_CALL String::operatorAddInteger(Thread* thread, StackRef<String> receiver, StackRef<Integer> arg, StackRef<String> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	// TODO
	stringstream ss;
	ss << arg->getData();
	string argStr = ss.str();

	StackRef<Integer> length(frame, frame->stackPush());
	thread->createInteger(length, receiver->size() + argStr.size());

	thread->createString(length, ret); // does not pop length

	ret->copyData(0, receiver->size(), receiver->c_str());
	ret->copyData(receiver->size(), argStr.size(), argStr.c_str());

	frame->stackPop(); // pop length

}

void BEER_CALL String::operatorAddFloat(Thread* thread, StackRef<String> receiver, StackRef<Float> arg, StackRef<String> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	// TODO
	stringstream ss;
	ss << std::setprecision(8) << std::fixed << arg->getData();
	string argStr = ss.str();

	StackRef<Integer> length(frame, frame->stackPush());
	thread->createInteger(length, receiver->size() + argStr.size());

	thread->createString(length, ret); // does not pop length

	ret->copyData(0, receiver->size(), receiver->c_str());
	ret->copyData(receiver->size(), argStr.size(), argStr.c_str());

	frame->stackPop(); // pop length
}

void BEER_CALL String::operatorAddBoolean(Thread* thread, StackRef<String> receiver, StackRef<Boolean> arg, StackRef<String> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	// TODO
	string str;
	if(arg->getData()) str = BEER_WIDEN("true");
	else str = BEER_WIDEN("false");

	StackRef<Integer> length(frame, frame->stackPush());
	thread->createInteger(length, receiver->size() + str.size());

	thread->createString(length, ret); // does not pop length

	ret->copyData(0, receiver->size(), receiver->c_str());
	ret->copyData(receiver->size(), str.size(), str.c_str());

	frame->stackPop(); // pop length
}

void BEER_CALL String::operatorAddCharacter(Thread* thread, StackRef<String> receiver, StackRef<Character> arg, StackRef<String> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> length(frame, frame->stackPush());
	thread->createInteger(length, receiver->size() + 1);

	thread->createString(length, ret); // does not pop length

	ret->copyData(0, receiver->size(), receiver->c_str());
	Character::CharacterData c = arg->getData();
	ret->copyData(receiver->size(), 1, &c);

	frame->stackPop(); // pop length
}

void BEER_CALL String::operatorAddArray(Thread* thread, StackRef<String> receiver, StackRef<Array> arg, StackRef<String> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<String> serialisedArray(frame, frame->stackPush());
	Array::operatorString(thread, arg, serialisedArray);

	String::operatorAddString(thread, receiver, serialisedArray, ret);

	frame->stackPop(); // pop serialisedArray
}

void BEER_CALL String::operatorEqual(Thread* thread, StackRef<String> receiver, StackRef<String> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->compare(arg.get()) == 0);
}

void BEER_CALL String::operatorNotEqual(Thread* thread, StackRef<String> receiver, StackRef<String> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->compare(arg.get()) != 0);
}

void BEER_CALL String::operatorSmaller(Thread* thread, StackRef<String> receiver, StackRef<String> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->compare(arg.get()) < 0);
}

void BEER_CALL String::operatorSmallerEqual(Thread* thread, StackRef<String> receiver, StackRef<String> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->compare(arg.get()) <= 0);
}

void BEER_CALL String::operatorGreater(Thread* thread, StackRef<String> receiver, StackRef<String> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->compare(arg.get()) > 0);
}

void BEER_CALL String::operatorGreaterEqual(Thread* thread, StackRef<String> receiver, StackRef<String> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->compare(arg.get()) >= 0);
}

void BEER_CALL String::createInstance(Thread* thread, StackRef<Class> receiver, StackRef<String> ret)
{
	Frame* frame = thread->getFrame();
	// TODO: probably not working
	StackRef<Integer> length = StackRef<Integer>(frame, -2);
	thread->createString(length, ret); // does not pop length

	frame->stackPop(); // pop length
}

Class* StringClassInitializer::createClass(Thread* thread, ClassLoader* loader, String* name)
{
	throw Exception(BEER_WIDEN("Implemented elsewehere"), __WFILE__, __LINE__);
}

void StringClassInitializer::initClass(Thread* thread, ClassLoader* loader, Class* klass)
{
	klass->setSuperClass(thread->getVM()->getObjectClass());
	klass->markSealed();
	//klass->markAsValueType();
	
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("String"), BEER_WIDEN("String::String()"), &String::init, 1, 0);

	loader->addVirtualMethod(thread, klass, BEER_WIDEN("<"), BEER_WIDEN("String::<(String)"), &String::operatorSmaller, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("<="), BEER_WIDEN("String::<=(String)"), &String::operatorSmallerEqual, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN(">"), BEER_WIDEN("String::>(String)"), &String::operatorGreater, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN(">="), BEER_WIDEN("String::>=()"), &String::operatorGreaterEqual, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("=="), BEER_WIDEN("String::==(String)"), &String::operatorEqual, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("!="), BEER_WIDEN("String::!=(String)"), &String::operatorNotEqual, 1, 1);

	loader->addVirtualMethod(thread, klass, BEER_WIDEN("+"), BEER_WIDEN("String::+(String)"), &String::operatorAddString, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("+"), BEER_WIDEN("String::+(Integer)"), &String::operatorAddInteger, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("+"), BEER_WIDEN("String::+(Float)"), &String::operatorAddFloat, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("+"), BEER_WIDEN("String::+(Boolean)"), &String::operatorAddBoolean, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("+"), BEER_WIDEN("String::+(Character)"), &String::operatorAddCharacter, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("+"), BEER_WIDEN("String::+(Array)"), &String::operatorAddArray, 1, 1);
	
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("getLength"), BEER_WIDEN("String::getLength()"), &String::getLength, 1, 0);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("get"), BEER_WIDEN("String::get(Integer)"), &String::operatorGet, 1, 1); // TODO: String::[](Integer)

	loader->addOverrideMethod(thread, klass, BEER_WIDEN("String"), BEER_WIDEN("Object::String()"), &String::operatorString, 1, 0);
	loader->addOverrideMethod(thread, klass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &String::createInstance, 1, 0);
}

// string pool

Reference<String> StringPool::translate(Thread* thread, const char16* str)
{
	StringMap::iterator it = mStrings.find(str);
	if(it == mStrings.end())
	{
		Frame* frame = thread->getFrame();
		BEER_STACK_CHECK();

		uint32 length = strlen(str);

		StackRef<String> strOnStack(frame, frame->stackPush());
		thread->createString(strOnStack, length);

		strOnStack->copyData(str, length);

		Reference<String> result(thread->getGC(), *strOnStack);
		frame->stackPop(); // pop string from stack

		return result;
	}

	return it->second;
}