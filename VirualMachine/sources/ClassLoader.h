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

		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name) = 0;
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass) = 0;
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

		//virtual void loadInternalClasses(); // lazy loaded

		void loadClass(String* name);
		bool canLoadClass(string name);
		//void unloadClass(string name) // TODO
		
		template <typename T>
		INLINE T* createClass(String* name, uint16 parents, uint16 properties, uint16 methods)
		{
			T* klass = static_cast<T*>(createClass(name, sizeof(T), parents, properties, methods));
			//new(klass) T(); // init __vtable, Warning: may cause troubles in DEBUG, ctor sets debug values
	
			ClassInitializer* initializer = getClassInitializer(name->c_str());
			if(initializer) initializer->initClass(mVM, this, klass);

			return klass;
		}

		void addMethod(Class* klass, Method* method, const char_t* selector);
		void addMethod(Class* klass, const char_t* name, const char_t* selector, Cb fn, uint16 returns, uint16 params);
		Method* createMethod(Cb fn, uint16 returns, uint16 params);
		
		template <typename T>
		INLINE void addMethod(Class* klass, const char_t* name, const char_t* selector, T fn, uint16 returns, uint16 params)
		{
			addMethod(klass, name, selector, reinterpret_cast<Cb>(fn), returns, params);
		}

		template <typename T>
		INLINE Method* createMethod(T fn, uint16 returns, uint16 params)
		{
			return createMethod(reinterpret_cast<Cb>(fn), returns, params);
		}

		Param* createParam();
		Property* createProperty();

		void extendClass(Class* klass, Class* extending); // deprecated
		void extendClass(StackRef<Class> klass, StackRef<Class> extending);

	protected:
		Class* createClass(String* name, uint32 staticSize, uint16 parents, uint16 properties, uint16 methods);
		
	};
};