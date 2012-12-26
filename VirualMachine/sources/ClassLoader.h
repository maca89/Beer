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

		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, std::string name) = 0;
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass) = 0;
	};

	class ClassLoader // TODO: interface
	{
	protected:
		typedef std::map<std::string, ClassInitializer*> ClassInitializerMap;
		typedef std::list<std::string> StringList;

		VirtualMachine* mVM;
		GarbageCollector* mClassHeap;
		ClassInitializerMap mClassInitializers;
		StringList mBeingLoaded;

	public:
		ClassLoader(VirtualMachine* vm);
		virtual ~ClassLoader();

		void addClassInitializer(std::string classname, ClassInitializer* initializer);
		void removeClassInitializer(std::string classname);
		ClassInitializer* getClassInitializer(std::string classname);
		bool hasClassInitializer(std::string name) const;

		//virtual void loadInternalClasses(); // lazy loaded

		void loadClass(std::string name);
		bool canLoadClass(std::string name);
		//void unloadClass(std::string name) // TODO
		
		template <typename T>
		INLINE T* createClass(std::string name, uint16 parents, uint16 properties, uint16 methods)
		{
			T* klass = /*reinterpret_cast*/static_cast<T*>(createClass(name, sizeof(T), /*T::CHILDREN_COUNT*/0, parents, properties, methods));
			new(klass) T(); // init __vtable
			//mVM->addClass(klass); // TODO: move somewhere else

			uint32 addr = *reinterpret_cast<uint32*>(klass);
			NULL_ASSERT(addr);

			return klass;
		}

		template <typename T>
		INLINE T* createMethod(std::string name, std::string selector, uint16 returns, uint16 params)
		{
			T* method = static_cast<T*>(createMethod(name, selector, sizeof(T), /*T::CHILDREN_COUNT*/0, returns, params));
			new(method) T(); // init __vtable
			return method;
		}

		template <typename T>
		INLINE T* createParam(std::string name)
		{
			T* param = static_cast<T*>(createParam(name, sizeof(T), /*T::CHILDREN_COUNT*/0));
			new(param) T(); // init __vtable
			return param;
		}

	protected:
		// classes
		ClassReflection* createClass(std::string name, uint32 staticSize, uint16 childrenCount, uint16 parents, uint16 properties, uint16 methods);
		void initClass(ClassReflection* klass, std::string name, uint32 staticSize, uint16 parents, uint16 properties, uint16 methods);
		uint32 countClassDynamicSize(std::string name, uint16 parents, uint16 properties, uint16 methods);

		// methods
		MethodReflection* createMethod(std::string name, std::string selector, uint32 staticSize, uint16 childrenCount, uint16 returns, uint16 params);
		void initMethod(MethodReflection* method, std::string name, std::string selector, uint32 staticSize, uint16 returns, uint16 params);
		uint32 countMethodDynamicSize(std::string name, std::string selector, uint16 returns, uint16 params);

		// params
		ParamReflection* createParam(std::string name, uint32 staticSize, uint16 childrenCount);
		void initParam(ParamReflection* param, std::string name, uint32 staticSize);
		uint32 countParamDynamicSize(std::string name);
	};
};