#include "stdafx.h"
#include "ObjectInspector.h"
#include "Object.h"
#include "Thread.h"
#include "Class.h"
#include "String.h"

using namespace Beer;


ObjectInspector::ObjectInspector(uint32 level)
	: mLevel(level), mMaxLevel(3)
{
}

ObjectInspector::~ObjectInspector()
{
}

void ObjectInspector::start(Thread* thread, Object* object)
{
	mThread = thread;
	mObject = object;
	mChildren.clear();

	if(!Object::isInlineValue(mObject))
	{
		if(mObject != NULL)
		{
			Class* type = mThread->getType(mObject);

			if(type != NULL)
			{
				Class::Traverser traverser = type->getTraverser();
				if(traverser != NULL)
				{
					traverser(this, type, mObject);
				}
			}
		}
	}
}

void ObjectInspector::nextChild()
{
	mChildren.pop_front();
}

bool ObjectInspector::hasChild()	
{
	return mChildren.size() > 0;
}

Object* ObjectInspector::getChild()	
{
	return mChildren.front();
}

void ObjectInspector::traverseObjectPtr(Object** ptrToObject)
{
	mChildren.push_back(*ptrToObject);
}

void ObjectInspector::printPadding()
{
	for(uint32 i = 0; i < mLevel; i++)
	{
		cout << "  ";
	}
}

void ObjectInspector::debugPrintObject()
{
	printPadding();
	cout << BEER_WIDEN("- ");

	cout << mObject;

	if(mObject != NULL)
	{
		Class* type = mThread->getType(mObject);
		if(type != NULL)
		{
			cout << BEER_WIDEN(" : ") << type->getName()->c_str();

			if(mLevel >= mMaxLevel)
			{
				cout << " *Too deep recursion*";
				//cout << "\n";
			}
			else if(!Object::isInlineValue(mObject))
			{
				cout << "\n";
				printPadding();
				cout << "[\n";

				while(hasChild())
				{
					Object* child = getChild();
							
					ObjectInspector objectInspector(mLevel + 1);
					objectInspector.start(mThread, child);
					objectInspector.debugPrintObject();
					cout << "\n";

					nextChild();
				}

				printPadding();
				cout << "]";
			}
		}
		else
		{
			cout << " : no class";
		}
	}
}

void ObjectInspector::debugPrintObject(Thread* thread, Object* object)
{
	//object = thread->getGC()->getIdentity(object);

	ObjectInspector objectInspector;
	objectInspector.start(thread, object);
	objectInspector.debugPrintObject();
}