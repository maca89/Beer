#pragma once

#include "prereq.h"
#include "Descriptor.h"
#include "MethodParameter.h"

namespace Beer
{
	class Output;

	namespace AST
	{
		class BlockNode;
	}

	namespace SemanticAnalysis
	{
		class Block;
		class Class;
		
		typedef std::list<MethodParameter *> MethodParameterList;

		/*
		 * 
		 */
		class Method : public Descriptor
		{
		public:

			typedef int8 flags;
			typedef int16 id;			

			enum Flag
			{
				NoFlag = 0,
				Native = 1,
				Abstract = 2,
				Special = 4,
				Static = 8,
				Override = 16
			};

			enum Type
			{
				Constructor,
				Conversion,
				Normal
			};

		protected:

			Class *							mDeclarator;
			Class *							mImplementor;

			id								mId;
			flags							mFlags;
			StringDescriptor *				mName;
			StringDescriptor *				mFullName;
			Type							mType;
			MethodParameterList				mReturnParameters;
			MethodParameterList				mParameters;
			bool							mOverriden;
			Block *							mBlock;
			bool							mTerminated;

			MethodParameter::id				mParameterId; // ID which will be assigned to new parameter

		public:

			INLINE Method(Class * declarator, Class * implementor, id id, flags flags, StringDescriptor * name, StringDescriptor * fullName, Type type)
				:	mDeclarator(declarator),
					mImplementor(implementor),
					mId(id),
					mFlags(flags),
					mName(name),
					mFullName(fullName),
					mType(type),
					mOverriden(false),
					mBlock(NULL),
					mTerminated(false),
					mParameterId(FIRST_PARAMETER_ID)
			{ }


			~Method()
			{ }

			
			INLINE Class * getDeclarator()
			{
				return mDeclarator;
			}

			INLINE Class * getImplementor()
			{
				return mImplementor;
			}

			INLINE id getId() const
			{
				return mId;
			}

			INLINE flags getFlags() const
			{
				return mFlags;
			}
			
			INLINE StringDescriptor * getName()
			{
				return mName;
			}
			
			INLINE StringDescriptor * getFullName()
			{
				return mFullName;
			}

			INLINE Type getType() const
			{
				return mType;
			}

			INLINE bool hasFlags(flags flags) const
			{
				return (mFlags & flags) == flags;
			}

			INLINE bool hasSomeFlag(flags flags) const
			{
				return (mFlags & flags) != 0;
			}

			INLINE bool isImplemented() const
			{
				return !hasSomeFlag(Abstract | Native);
			}

			MethodParameter::id getParameterCount() const;
			MethodParameter * getParameter(const std::string & name);
			
			INLINE MethodParameterList * getParameters()
			{
				return &mParameters;
			}

			INLINE MethodParameterList * getReturnParameters()
			{
				return &mReturnParameters;
			}

			INLINE bool	isOverriden() const
			{
				return mOverriden;
			}

			INLINE Block * getBlock()
			{
				return mBlock;
			}

			INLINE bool isTerminated() const
			{
				return mTerminated;
			}

			INLINE void setFlags(flags flags)
			{
				mFlags = flags;
			}

			INLINE void isOverriden(bool overriden)
			{
				mOverriden = overriden;
			}

			Block * setBlock(AST::BlockNode * block);

			INLINE void isTerminated(bool terminated)
			{
				mTerminated = terminated;
			}


			bool hasParameter(const std::string & name) const;
					
			MethodParameter * addParameter(const std::string & type, const std::string & name, Output & out);

			MethodParameter * addReturnParameter(const std::string & type, Output & out);
		};
	}
}

