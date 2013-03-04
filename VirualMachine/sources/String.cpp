#include "stdafx.h"
#include "String.h"
#include "Float.h"
#include "Boolean.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "Array.h"

using namespace Beer;


StringPool String::gPool = StringPool(); // TODO: get rid of


void BEER_CALL String::init(Thread* thread, StackFrame* frame, StackRef<String> receiver, StackRef<String> ret)
{
	ret = receiver;
}

void BEER_CALL String::getLength(Thread* thread, StackFrame* frame, StackRef<String> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->size());
}

void BEER_CALL String::operatorGet(Thread* thread, StackFrame* frame, StackRef<String> receiver, StackRef<Integer> index, StackRef<Character> ret)
{
	Integer::IntegerData indexdata = index.get()->getData();
	BOUNDS_ASSERT(indexdata, receiver->size());
	ret = Character::makeInlineValue(receiver->c_str()[indexdata]);
}

void BEER_CALL String::operatorAddString(Thread* thread, StackFrame* frame, StackRef<String> receiver, StackRef<String> arg, StackRef<String> ret)
{
	StackRef<Integer> length(frame, frame->stackPush());
	thread->createInteger(length, receiver->size() + arg->size());

	thread->createString(length, ret);

	ret->copyData(0, receiver->size(), receiver->c_str());
	ret->copyData(receiver->size(), arg->size(), arg->c_str());
}

void BEER_CALL String::operatorAddInteger(Thread* thread, StackFrame* frame, StackRef<String> receiver, StackRef<Integer> arg, StackRef<String> ret)
{
	// TODO
	stringstream ss;
	ss << arg->getData();
	string argStr = ss.str();

	StackRef<Integer> length(frame, frame->stackPush());
	thread->createInteger(length, receiver->size() + argStr.size());

	thread->createString(length, ret);

	ret->copyData(0, receiver->size(), receiver->c_str());
	ret->copyData(receiver->size(), argStr.size(), argStr.c_str());

}

void BEER_CALL String::operatorAddFloat(Thread* thread, StackFrame* frame, StackRef<String> receiver, StackRef<Float> arg, StackRef<String> ret)
{
	// TODO
	stringstream ss;
	ss << std::setprecision(8) << std::fixed << arg->getData();
	string argStr = ss.str();

	StackRef<Integer> length(frame, frame->stackPush());
	thread->createInteger(length, receiver->size() + argStr.size());

	thread->createString(length, ret);

	ret->copyData(0, receiver->size(), receiver->c_str());
	ret->copyData(receiver->size(), argStr.size(), argStr.c_str());
}

void BEER_CALL String::operatorAddBoolean(Thread* thread, StackFrame* frame, StackRef<String> receiver, StackRef<Boolean> arg, StackRef<String> ret)
{
	// TODO
	string str;
	if(arg->getData()) str = BEER_WIDEN("true");
	else str = BEER_WIDEN("false");

	StackRef<Integer> length(frame, frame->stackPush());
	thread->createInteger(length, receiver->size() + str.size());

	thread->createString(length, ret);

	ret->copyData(0, receiver->size(), receiver->c_str());
	ret->copyData(receiver->size(), str.size(), str.c_str());
}

void BEER_CALL String::operatorAddCharacter(Thread* thread, StackFrame* frame, StackRef<String> receiver, StackRef<Character> arg, StackRef<String> ret)
{
	StackRef<Integer> length(frame, frame->stackPush());
	thread->createInteger(length, receiver->size() + 1);

	thread->createString(length, ret);

	ret->copyData(0, receiver->size(), receiver->c_str());
	Character::CharacterData c = arg->getData();
	ret->copyData(receiver->size(), 1, &c);
}

void BEER_CALL String::operatorAddArray(Thread* thread, StackFrame* frame, StackRef<String> receiver, StackRef<Array> arg, StackRef<String> ret)
{
	// TODO
	string str;
	arg->toString(thread->getVM(), str);

	StackRef<Integer> length(frame, frame->stackPush());
	thread->createInteger(length, receiver->size() + str.size());

	thread->createString(length, ret);

	ret->copyData(0, receiver->size(), receiver->c_str());
	ret->copyData(receiver->size(), str.size(), str.c_str());
}

void BEER_CALL String::operatorEqual(Thread* thread, StackFrame* frame, StackRef<String> receiver, StackRef<String> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->compare(arg.get()) == 0);
}

void BEER_CALL String::operatorNotEqual(Thread* thread, StackFrame* frame, StackRef<String> receiver, StackRef<String> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->compare(arg.get()) != 0);
}

void BEER_CALL String::operatorSmaller(Thread* thread, StackFrame* frame, StackRef<String> receiver, StackRef<String> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->compare(arg.get()) < 0);
}

void BEER_CALL String::operatorSmallerEqual(Thread* thread, StackFrame* frame, StackRef<String> receiver, StackRef<String> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->compare(arg.get()) <= 0);
}

void BEER_CALL String::operatorGreater(Thread* thread, StackFrame* frame, StackRef<String> receiver, StackRef<String> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->compare(arg.get()) > 0);
}

void BEER_CALL String::operatorGreaterEqual(Thread* thread, StackFrame* frame, StackRef<String> receiver, StackRef<String> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->compare(arg.get()) >= 0);
}

void BEER_CALL String::createInstance(Thread* thread, StackFrame* frame, StackRef<Class> receiver, StackRef<String> ret)
{
	// TODO: probably not working
	StackRef<Integer> length = StackRef<Integer>(frame, -2);
	thread->createString(length, ret);
}

Class* StringClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	throw Exception(BEER_WIDEN("Implemented elsewehere"), __WFILE__, __LINE__);
}

void StringClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	Method* method = NULL;
	uint16 methodi = 0;

	klass->extends(0, vm->getObjectClass());

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("String")));
	method->setFunction(&String::init);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("String::String()")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("<")));
	method->setFunction(&String::operatorSmaller);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("String::<(String)")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("<=")));
	method->setFunction(&String::operatorSmallerEqual);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("String::<=(String)")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN(">")));
	method->setFunction(&String::operatorGreater);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("String::>(String)")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN(">=")));
	method->setFunction(&String::operatorGreaterEqual);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("String::>=(String)")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("==")));
	method->setFunction(&String::operatorEqual);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("String::==(String)")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("!=")));
	method->setFunction(&String::operatorNotEqual);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("String::!=(String)")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("+")));
	method->setFunction(&String::operatorAddString);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("String::+(String)")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("+")));
	method->setFunction(&String::operatorAddInteger);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("String::+(Integer)")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("+")));
	method->setFunction(&String::operatorAddFloat);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("String::+(Float)")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("+")));
	method->setFunction(&String::operatorAddBoolean);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("String::+(Boolean)")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("+")));
	method->setFunction(&String::operatorAddCharacter);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("String::+(Character)")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("+")));
	method->setFunction(&String::operatorAddArray);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("String::+(Array)")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("getLength")));
	method->setFunction(&String::getLength);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("String::getLength()")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("get")));
	method->setFunction(&String::operatorGet);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("String::get(Integer)")), method)); // TODO: String::[](Integer)

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("createInstance")));
	method->setFunction(&String::createInstance);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("$Class::createInstance()")), method));
}

// string pool

Reference<String> StringPool::translate(Thread* thread, const char16* str)
{
	StringMap::iterator it = mStrings.find(str);
	if(it == mStrings.end())
	{
		uint32 length = strlen(str);
		CopyGC* heap = static_cast<CopyGC*>(thread->getHeap());
		
		StackFrame* frame = thread->getStackFrame();

		StackRef<String> strOnStack(frame, frame->stackPush());

		StackRef<Integer> lengthOnStack(frame, frame->stackPush());
		thread->createInteger(lengthOnStack, length);

		// TODO: why the cast??
		thread->createString(lengthOnStack, strOnStack); // pops length
		strOnStack->copyData(str, length);

		Reference<String> result(heap, *strOnStack);
		frame->stackMoveTop(-1); // pop string from stack
		return result;
	}

	return it->second;
}