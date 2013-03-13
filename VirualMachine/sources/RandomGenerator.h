#pragma once
#include "prereq.h"
#include "Object.h"
#include "Frame.h"
#include "MerseneTwisterRandom.h"
#include "ClassLoader.h"

namespace Beer
{
	class Integer;
	class Boolean;
	class Float;
	class Character;

	class RandomGenerator : public Object
	{
	protected:
		MerseneTwisterRandom mGenerator;

	public:
		
		static void BEER_CALL createInstance(Thread* thread, StackRef<Class> receiver, StackRef<RandomGenerator> ret);
		
		static void BEER_CALL init(Thread* thread, StackRef<RandomGenerator> receiver, StackRef<RandomGenerator> ret);
		static void BEER_CALL initInteger(Thread* thread, StackRef<RandomGenerator> receiver, StackRef<Integer> seed, StackRef<RandomGenerator> ret);

		static void BEER_CALL operatorInteger(Thread* thread, StackRef<RandomGenerator> receiver, StackRef<Integer> ret);
		static void BEER_CALL operatorBoolean(Thread* thread, StackRef<RandomGenerator> receiver, StackRef<Boolean> ret);
		static void BEER_CALL operatorFloat(Thread* thread, StackRef<RandomGenerator> receiver, StackRef<Float> ret);
		static void BEER_CALL operatorCharacter(Thread* thread, StackRef<RandomGenerator> receiver, StackRef<Character> ret);

		static void BEER_CALL toss(Thread* thread, StackRef<RandomGenerator> receiver, StackRef<Float> p, StackRef<Boolean> ret);
	};

	class RandomGeneratorClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};