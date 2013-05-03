#include "stdafx.h"

#include "Class.h"

#include "DescriptorDictionary.h"
#include "MethodGroup.h"
#include "MethodParameterNode.h"
#include "NameBuilder.h"
#include "Output.h"
#include "Program.h"
#include "SemanticException.h"
#include "StringDescriptor.h"

using namespace Beer::SemanticAnalysis;

typedef std::list<Class *> ClassList;

Class::Class(Program * program, StringDescriptor * name, flags flags)
	:	mProgram(program),
	mId(0),
	mFlags(flags),
	mName(name),
	mMemberId(FIRST_MEMBER_ID),
	mMethodId(FIRST_FUNCTION_ID),
	mContainsConstructor(false)
{
	addMethodGroup(this);
}

Class::~Class()
{
}


bool Class::hasMethod(const std::string & fullName) const
{
	for (MethodGroupMap::const_iterator it = mMethodGroups.begin(); it != mMethodGroups.end(); it++)
	{
		if (it->second->hasMethod(fullName)) return true;
	}

	return false;
}


Member * Class::getMember(const std::string & name)
{
	MemberMap::iterator it = mMemberMap.find(name);

	if (it == mMemberMap.end())
	{
		for (ClassList::iterator it = mParents.begin(); it != mParents.end(); it++)
		{
			Member * member = (*it)->getMember(name);

			if (member) return member;
		}

		return NULL;
	}

	return it->second;
}

Method * Class::getMethod(const std::string & qualifiedName)
{
	Method * method = NULL;

	for (MethodGroupMap::iterator it = mMethodGroups.begin(); it != mMethodGroups.end(); it++)
	{
		method = it->second->getMethod(qualifiedName);

		if (method) return method;
	}

	return NULL;
}

bool Class::isSubtype(Class * cls)
{
	return this == cls || hasAncestor(cls);
}

bool Class::hasAncestor(Class * cls)
{
	for (ClassList::iterator it = mParents.begin(); it != mParents.end(); it++)
	{
		if (*it == cls || (*it)->hasAncestor(cls))
		{
			return true;
		}
	}

	return false;
}

void Class::addParent(Class * parent, ::Beer::Output & out)
{
	mParents.push_back(parent);

	if (parent->mMembers.size() > 0)
	{
		if (mMemberId != FIRST_MEMBER_ID + mMembers.size())
		{
			out.addError("Class cannot inherit from more than one abstract class. But it can implement more interfaces.");
		}
		else
		{
			mMemberId = parent->mMemberId;

			for (MemberList::iterator it = mMembers.begin(); it != mMembers.end(); it++)
			{
				(*it)->setId(mMemberId++);
			}
		}
	}
	
	for (MethodGroupMap::iterator git = parent->mMethodGroups.begin(); git != parent->mMethodGroups.end(); git++)
	{
		addMethodGroup(git->second->getDeclarator());

		MethodMap * methods = git->second->getMethods();

		for (MethodMap::iterator mit = methods->begin(); mit != methods->end(); mit++)
		{
			addMethod(mit->second);
		}
	}
}

Member * Class::addMember(Member::flags flags, const std::string & type, const std::string & name, ::Beer::Output & out)
{
	if (hasMember(name))
	{
		out.addError("Multiple member variables with name " + name + " in class " + mName->getValue());

		return NULL;
	}

	DescriptorDictionary * desc = mProgram->getDescriptorDictionary();

	Member * member = new Member(
		this,
		mMemberId++,
		flags,
		mProgram->getClass(type),
		desc->addStringDescriptor(name),
		mName->getValue() + "::" + name);

	mMembers.push_back(member);
	mMemberMap[name] = member;

	desc->addDescriptor(member);

	return member;
}

Method * Class::addConstructor(Method::flags flags, ::Beer::AST::MethodParameterList * params, ::Beer::Output & out)
{
	if (flags & Method::Abstract)
	{
		out.addError("Constructor cannot be abstract");

		return NULL;
	}

	AST::StringList returnParams;

	returnParams.push_back(mName->getValue());

	Method * ctor = addMethod(Method::Constructor, flags, &returnParams, mName->getValue(), params, out);

	if (ctor)
	{
		mContainsConstructor = true;
	}

	return ctor;
}

Method * Class::addConversion(Method::flags flags, const std::string & type, ::Beer::Output & out)
{
	AST::StringList returnParams;

	returnParams.push_back(type);

	return addMethod(Method::Conversion, flags, &returnParams, type, NULL, out);
}

Method * Class::addMethod(Method::flags flags, const std::string & name, ::Beer::AST::MethodParameterList * params, ::Beer::Output & out)
{
	return addMethod(Method::Normal, flags, NULL, name, params, out);
}

Method * Class::addMethod(Method::flags flags, ::Beer::AST::StringList * returnParams, const std::string & name, ::Beer::AST::MethodParameterList * params, ::Beer::Output & out)
{
	return addMethod(Method::Normal, flags, returnParams, name, params, out);
}

void Class::addMethod(Method * method)
{
	MethodGroup * group = getMethodGroup(method->getDeclarator());

	if (group)
	{
		group->addMethod(method);		
	}

	if (method->getImplementor() == this || method->getDeclarator() == this)
	{
		mMethods.push_back(method);
	}

	checkAbstract();
}

void Class::checkAbstract()
{
	bool abs = false;

	for (MethodGroupMap::const_iterator it = mMethodGroups.begin(); it != mMethodGroups.end(); it++)
	{
		if (it->second->isAbstract()) abs |= true;
	}

	mFlags = abs ? mFlags | Abstract : mFlags & ~Abstract;
}

MethodGroup * Class::getMethodGroup(Class * declarator)
{
	MethodGroupMap::iterator it = mMethodGroups.find(declarator->getName()->getValue());

	if (it == mMethodGroups.end()) return NULL;

	return it->second;
}

void Class::addMethodGroup(Class * declarator)
{
	const std::string & name = declarator->getName()->getValue();

	if (mMethodGroups.find(name) != mMethodGroups.end()) throw SemanticException("There is already method group " + name);

	mMethodGroups[name] = new MethodGroup(declarator);
}

Method * Class::addMethod(Method::Type type, Method::flags flags, ::Beer::AST::StringList * returnParams, const std::string & name, ::Beer::AST::MethodParameterList * params, ::Beer::Output & out)
{
	DescriptorDictionary * dict = mProgram->getDescriptorDictionary();

	std::string qualName = NameBuilder::buildQualifiedName(name, params);
	std::string fullName = NameBuilder::buildFullName(mName->getValue(), name, params);

	Class * decl = this;
	Class * impl = this;

	Method * method = getMethod(qualName);

	if (method)
	{
		if (method->getDeclarator() != this)
		{
			decl = method->getDeclarator();
			method->isOverriden(true);
			removeMethod(method);
		}
		else
		{
			out.addError("Method " + fullName + " is already defined");

			return NULL;
		}
	}	

	if (decl != this) flags |= Method::Override;

	if (flags & Method::Abstract) impl = NULL;

	method = new Method(
		decl,
		impl,
		mMethodId++,
		flags,
		dict->addStringDescriptor(name), 
		dict->addStringDescriptor(fullName),
		type);


	if (params)
	{
		for (AST::MethodParameterList::iterator it = params->begin(); it != params->end(); it++)
		{
			method->addParameter((*it)->getType(), (*it)->getName(), out);
		}
	}

	if (returnParams)
	{
		for (AST::StringList::iterator it = returnParams->begin(); it != returnParams->end(); it++)
		{
			method->addReturnParameter(*it, out);
		}
	}

	addMethod(method);

	dict->addDescriptor(method);

	checkAbstract();

	return method;
}

void Class::removeMethod(const std::string & qualifiedName)
{
	for (MethodGroupMap::iterator it = mMethodGroups.begin(); it != mMethodGroups.end(); it++)
	{
		Method * method = it->second->getMethod(qualifiedName);

		if (method)
		{
			it->second->removeMethod(method);
			mMethods.remove(method);
			return;
		}
	}
}

void Class::removeMethod(Method * method)
{
	MethodGroup * group = getMethodGroup(method->getDeclarator());

	if (group)
	{
		group->removeMethod(method);
	}

	checkAbstract();
}