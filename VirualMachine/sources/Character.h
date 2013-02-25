#pragma once
#include "prereq.h"
#include "Object.h"


namespace Beer
{
	class Character : public Object
	{
	public:
		typedef char_t CharacterData;
		static const int SignatureBits = 4;

	protected:
		//CharacterData mData; // not used

	public:
		INLINE CharacterData getData() const
		{
			if(this == NULL)
			{
				return '\0'; // just a workaround, TODO
			}

			return (reinterpret_cast<uint32>(this) >> SignatureBits);
		}

		INLINE static Character* makeInlineValue(CharacterData data)
		{
			return reinterpret_cast<Character*>((data << SignatureBits) | 7);
		}
	};
};