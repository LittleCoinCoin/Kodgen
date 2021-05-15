/**
*	Copyright (c) 2020 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the README.md file for full license details.
*/

#pragma once

#include "Kodgen/Properties/DefaultPropertyRule.h"

namespace kodgen
{
	class ParseAllNestedPropertyRule : public DefaultPropertyRule
	{
		public:
			ParseAllNestedPropertyRule()									noexcept;
			ParseAllNestedPropertyRule(ParseAllNestedPropertyRule const&)	= default;
			ParseAllNestedPropertyRule(ParseAllNestedPropertyRule&&)		= default;
			virtual ~ParseAllNestedPropertyRule()							= default;
	};
}