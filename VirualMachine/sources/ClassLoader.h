#pragma once
#include "prereq.h"


namespace Beer
{
	class VirtualMachine;
	struct GarbageCollector;
	class ClassLoader;
	class ClassReflection;
	class MethodReflection;
	class ParamReflection;

	class ClassInitializer
	{
	public:
		virtual ~ClassInitializer() {}

		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, string name) = 0;
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass) = 0;
	};

	class ClassLoader // TODO: interface
	{
	protected:
		typedef std::map<string, ClassInitializer*> ClassInitializerMap;
		typedef std::list<string> StringList;

		VirtualMachine* mVM;
		GarbageCollector* mClassHeap;
		ClassInitializerMap mClassInitializers;
		StringList mBeingLoaded;

	public:
		ClassLoader(VirtualMachine* vm, GarbageCollector* heap);
		virtual ~ClassLoader();

		void addClassInitializer(string classname, ClassInitializer* initializer);
		void removeClassInitializer(string classname);
		ClassInitializer* getClassInitializer(string classname);
		bool hasClassInitializer(string name) const;

		//virtual void loadInternalClasses(); // lazy loaded

		void loadClass(string name);
		bool canLoadClass(string name);
		//void unloadClass(string name) // TODO
		
		template <typename T>
		INLINE T* createClass(string name, uint16 parents, uint16 properties, uint16 methods)
		{
			T* klass = /*reinterpret_cast*/static_cast<T*>(createClass(name, sizeof(T), /*T::CHILDREN_COUNT*/0, parents, properties, methods));
			new(klass) T(); // init __vtable
			//mVM->addClass(klass); // TODO: move somewhere else

			uint32 addr = *reinterpret_cast<uint32*>(klass);
			NULL_ASSERT(addr);

			return klass;
		}

		template <typename T>
		INLINE T* createMethod(string name, string selector, uint16 returns, uint16 params)
		{
			T* method = static_cast<T*>(createMethod(name, selector, sizeof(T), /*T::CHILDREN_COUNT*/0, returns, params));
			new(method) T(); // init __vtable
			return method;
		}

		template <typename T>
		INLINE T* createParam(string name)
		{
			T* param = static_cast<T*>(createParam(name, sizeof(T), /*T::CHILDREN_COUNT*/0));
			new(param) T(); // init __vtable
			return param;
		}

	protected:
		// classes
		ClassReflection* createClass(string name, uint32 staticSize, uint16 childrenCount, uint16 parents, uint16 properties, uint16 methods);
		void initClass(ClassReflection* klass, string name, uint32 staticSize, uint16 parents, uint16 properties, uint16 methods);
		uint32 countClassDynamicSize(string name, uint16 parents, uint16 properties, uint16 methods);

		// methods
		MethodReflection* createMethod(string name, string selector, uint32 staticSize, uint16 childrenCount, uint16 returns, uint16 params);
		void initMethod(MethodReflection* method, string name, string selector, uint32 staticSize, uint16 returns, uint16 params);
		uint32 countMethodDynamicSize(string name, string selector, uint16 returns, uint16 params);

		// params
		ParamReflection* createParam(string name, uint32 staticSize, uint16 childrenCount);
		void initParam(ParamReflection* param, string name, uint32 staticSize);
		uint32 countParamDynamicSize(string name);
	};
};