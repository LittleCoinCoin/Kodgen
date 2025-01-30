#pragma once

#include <string>

#include "Generated/SomeClass.h.h"

namespace SomeNamespace KGNamespace()
{
	namespace np1 KGNamespace()
	{
		class KGClass(Data) SomeClass
		{
			public:
				class SomeNestedClass{};

			private:
				KGField(Get[explicit], Set)
				int					_someInt				= 2;

				KGField(Get[const, *], Set)
				float				_someFloat				= 3.14f;

				KGField(Get)
				char				_someChar				= '*';

				KGField(Get, Set)
				unsigned long long	_someUnsignedLongLong	= 42;

				KGField(Get, Set)
				SomeNestedClass*	_someNestedClass		= nullptr;

				KGField(Get[const, &], Set)
				std::string			_someString				= "This is a test";

			public:
				SomeClass() = default;

			SomeNamespace_np1_SomeClass_GENERATED
		};
	}
}

File_SomeClass_GENERATED