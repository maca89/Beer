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
		
		virtual Class* createClass(Thread* thread, ClassLoader* loader, String* name) = 0;
		virtual void initClass(Thread* thread, ClassLoader* loader, Class* klass) = 0;
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

		Method* addMethod(Thread* thread, Class* klass, Method* method, const char_t* selector);
		Method* addMethod(Thread* thread, Class* klass, const char_t* name, const char_t* selector, Cb fn, uint16 returns, uint16 params);

		Class* createClass(Thread* thread, String* name, uint32 staticSize, uint16 parents, uint16 properties, uint16 methods);
		Method* createMethod(Thread* thread, Cb fn, uint16 returns, uint16 params);
		void createParam(Thread* thread, StackRef<Param> ret);
		void createProperty(Thread* thread, StackRef<Property> ret);

		template <typename T>
		Method* addMethod(Thread* thread, Class* klass, const char_t* name, const char_t* selector, T fn, uint16 returns, uint16 params)
		{
			return addMethod(thread, klass, name, selector, reinterpret_cast<Cb>(fn), returns, params);
		}

		template <typename T>
		INLINE Class* createClass(Thread* thread, String* name, uint16 parents, uint16 properties, uint16 methods)
		{
			return createClass(thread, name, sizeof(T), parents, properties, methods);
		}

	protected:
		
		
	};
};