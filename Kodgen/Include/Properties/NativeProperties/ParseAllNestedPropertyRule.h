/**
*	Copyright (c) 2020 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the README.md file for full license details.
*/

#pragma once

#include "Properties/SimplePropertyRule2.h"

namespace kodgen
{
	class ParseAllNestedPropertyRule : public SimplePropertyRule2
	{
		public:
			ParseAllNestedPropertyRule()									= default;
			ParseAllNestedPropertyRule(ParseAllNestedPropertyRule const&)	= default;
			ParseAllNestedPropertyRule(ParseAllNestedPropertyRule&&)		= default;
			virtual ~ParseAllNestedPropertyRule()							= default;

			virtual bool	isMainPropSyntaxValid(std::string const& mainProperty,
												  EntityInfo::EType	 entityType)				const noexcept override;

			virtual bool	isPropertyGroupValid(PropertyGroup2 const&	propertyGroup,
												 uint8					propertyIndex,
												 std::string&			out_errorDescription)	const noexcept override;

			virtual bool	isEntityValid(EntityInfo const& entity,
										  uint8				propertyIndex,
										  std::string&		out_errorDescription)				const noexcept override;
	};
}