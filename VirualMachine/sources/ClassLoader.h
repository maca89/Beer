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

		ClassInitializerMap mClassInitializers;
		StringList mBeingLoaded;

	public:
		ClassLoader();
		virtual ~ClassLoader();

		void addClassInitializer(string classname, ClassInitializer* initializer);
		void removeClassInitializer(string classname);
		ClassInitializer* getClassInitializer(string classname);
		bool hasClassInitializer(string name) const;

		void loadClass(Thread* thread, StackRef<String> name);
		bool canLoadClass(string name);
		//void unloadClass(string name) // TODO

		void addMethod(Thread* thread, StackRef<Class> klass, StackRef<Method> method, const char_t* selector);
		void addMethod(Thread* thread, StackRef<Class> klass, const char_t* name, const char_t* selector, Cb fn, uint16 returns, uint16 params);

		void createClass(Thread* thread, StackRef<String> name, StackRef<Class> ret, uint32 staticSize, uint16 parents, uint16 properties, uint16 methods);
		void createMethod(Thread* thread, StackRef<Method> ret, Cb fn, uint16 returns, uint16 params);
		void createParam(Thread* thread, StackRef<Param> ret);
		void createProperty(Thread* thread, StackRef<Property> ret);

		template <typename T>
		INLINE void addMethod(Thread* thread, StackRef<Class> klass, const char_t* name, const char_t* selector, T fn, uint16 returns, uint16 params)
		{
			addMethod(thread, klass, name, selector, reinterpret_cast<Cb>(fn), returns, params);
		}

		/*template <typename T>
		INLINE void addAbstractMethod(Thread* thread, StackRef<Class> klass, const char_t* name, const char_t* selector, T fn, uint16 returns, uint16 params)
		{
			addMethod(thread, klass, name, selector, reinterpret_cast<Cb>(fn), returns, params);
		}*/

		template <typename T>
		INLINE void createClass(Thread* thread, StackRef<String> name, StackRef<Class> ret, uint16 parents, uint16 properties, uint16 methods)
		{
			createClass(thread, name, ret, sizeof(T), parents, properties, methods);
		}

	protected:
		
		
	};
};