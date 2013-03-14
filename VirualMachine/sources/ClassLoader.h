#pragma once
#include "prereq.h"
#include "Frame.h"


namespace Beer
{
	class VirtualMachine;
	class ClassLoader;
	class Class;
	class Method;
	class Param;
	class Property;
	class Heap;

	class ClassInitializer
	{
	public:
		virtual ~ClassInitializer() {}

		virtual void createClass(Thread* thread, ClassLoader* loader, StackRef<String> name, StackRef<Class> ret) = 0;
		virtual void initClass(Thread* thread, ClassLoader* loader, StackRef<Class> klass) = 0;
	};

	class ClassLoader // TODO: interface
	{
	protected:
		typedef std::map<string, ClassInitializer*> ClassInitializerMap;
		typedef std::list<string> StringList;

		VirtualMachine* mVM;
		Heap* mClassHeap;
		ClassInitializerMap mClassInitializers;
		StringList mBeingLoaded;

	public:
		ClassLoader(VirtualMachine* vm, Heap* heap);
		virtual ~ClassLoader();

		void addClassInitializer(string classname, ClassInitializer* initializer);
		void removeClassInitializer(string classname);
		ClassInitializer* getClassInitializer(string classname);
		bool hasClassInitializer(string name) const;

		void loadClass(Thread* thread, StackRef<String> name);
		bool canLoadClass(string name);
		//void unloadClass(string name) // TODO
		
		template <typename T>
		INLINE void createClass(Thread* thread, StackRef<String> name, StackRef<Class> ret, uint16 parents, uint16 properties, uint16 methods)
		{
			createClass(thread, name, ret, sizeof(T), parents, properties, methods);
	
			ClassInitializer* initializer = getClassInitializer(name->c_str());
			if(initializer) initializer->initClass(thread, this, ret);
		}

		void addMethod(StackRef<Class> klass, StackRef<Method> method, const char_t* selector);
		void addMethod(StackRef<Class> klass, const char_t* name, const char_t* selector, Cb fn, uint16 returns, uint16 params);
		void createMethod(StackRef<Method> ret, Cb fn, uint16 returns, uint16 params);
		
		template <typename T>
		INLINE void addMethod(StackRef<Class> klass, const char_t* name, const char_t* selector, T fn, uint16 returns, uint16 params)
		{
			addMethod(klass, name, selector, reinterpret_cast<Cb>(fn), returns, params);
		}

		void createParam(StackRef<Param> ret);
		void createProperty(StackRef<Property> ret);

		void extendClass(StackRef<Class> klass, StackRef<Class> extending);

	protected:
		void createClass(Thread* thread, StackRef<String> name, StackRef<Class> ret, uint32 staticSize, uint16 parents, uint16 properties, uint16 methods);
		
	};
};