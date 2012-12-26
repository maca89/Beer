#include "stdafx.h"

BeerlangCompiler::SemanticAnalysis::ProgramInfo::ProgramInfo()
{
}

BeerlangCompiler::SemanticAnalysis::ClassInfo * BeerlangCompiler::SemanticAnalysis::ProgramInfo::getClass(std::string name)
{
	ClassMapIterator it = mClassMap.find(name);

	if (it == mClassMap.end()) return NULL;

	return it->second;
}

/*BeerlangCompiler::SemanticAnalysis::ClassInfo * BeerlangCompiler::SemanticAnalysis::ProgramInfo::addExternalClass(std::string name, class_flags flags, Output & out)
{
	ClassInfo * cls = getClass(name);

	if (cls)
	{	
		out.addError("Mutiple classes with name " + name);

		return NULL;
	}

	cls = new ClassInfo(this, mDictionary.addStringConstant(name), flags);

	mExternalClasses.push_back(cls);
	mClassMap[name] = cls;

	return cls;
}*/

BeerlangCompiler::SemanticAnalysis::ClassInfo * BeerlangCompiler::SemanticAnalysis::ProgramInfo::addClass(std::string name, class_flags flags, Output & out)
{
	ClassInfo * cls = getClass(name);

	if (cls)
	{	
		out.addError("Mutiple classes with name " + name);

		return NULL;
	}

	bool native = (flags & ClassInfo::Native) != 0;

	cls = new ClassInfo(this, mDictionary.addStringConstant(name), flags);

	if (native)
	{
		mExternalClasses.push_back(cls);
	}
	else
	{
		mClasses.push_back(cls);	
		mDictionary.addDescriptor(cls);
	}
	
	mClassMap[name] = cls;

	return cls;
}
