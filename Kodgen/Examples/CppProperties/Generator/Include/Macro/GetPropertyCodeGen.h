/**
*	Copyright (c) 2020 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the LICENSE.md file for full license details.
*/

#pragma once

#include <string>

#include "Kodgen/CodeGen/Macro/MacroPropertyCodeGen.h"

class GetPropertyCodeGen : public kodgen::MacroPropertyCodeGen
{
	public:
		GetPropertyCodeGen() noexcept:
			kodgen::MacroPropertyCodeGen("Get", kodgen::EEntityType::Field)
		{}

		virtual bool preGenerateCodeForEntity(kodgen::EntityInfo const& /* entity */, kodgen::Property const& property, kodgen::uint8 /* propertyIndex */, kodgen::MacroCodeGenEnv& env) noexcept override
		{
			std::string errorMessage;

			//Can't have * and & at the same time
			if (std::find(property.arguments.cbegin(), property.arguments.cend(), "*") != property.arguments.cend() &&
				std::find(property.arguments.cbegin(), property.arguments.cend(), "&") != property.arguments.cend())
			{
				errorMessage = "Get property can't accept both '*' and '&' at the same time.";
			}
			else
			{
				//Check that Get property arguments are valid
				for (std::string const& arg : property.arguments)
				{
					if (arg != "*" && arg != "&" && arg != "const" && arg != "explicit")
					{
						errorMessage = "Get property only accepts '*', '&' and 'explicit' arguments.";
						break;
					}
				}
			}

			if (!errorMessage.empty())
			{
				//Log error message and abort generation
				if (env.getLogger() != nullptr)
				{
					env.getLogger()->log(errorMessage, kodgen::ILogger::ELogSeverity::Error);
				}

				return false;
			}

			//If arguments are valid, dispatch the generation call normally
			return true;
		}

		virtual bool generateClassFooterCodeForEntity(kodgen::EntityInfo const& entity, kodgen::Property const& property, kodgen::uint8 /* propertyIndex */,
													  kodgen::MacroCodeGenEnv& env, std::string& inout_result) noexcept override
		{
			kodgen::FieldInfo const& field = reinterpret_cast<kodgen::FieldInfo const&>(entity);

			std::string postQualifiers;
			std::string rawReturnType	= field.type.getCanonicalName() + " ";
			std::string returnName		= field.name;
			bool		isConst			= false;
			bool		isRef			= false;
			bool		isPtr			= false;

			for (std::string const& subprop : property.arguments)
			{
				if (!isConst && subprop.at(0) == 'c')			//const
				{
					isConst = true;
				}
				else if (!(isRef || isPtr))
				{
					if (subprop.at(0) == '&')					//&
					{
						isRef = true;
					}
					else if (subprop.at(0) == '*')				//*
					{
						isPtr = true;
					}
				}
			}

			std::string preTypeQualifiers;

			//Upper case the first field info char if applicable
			std::string methodName = field.name;
			methodName.replace(0, 1, 1, static_cast<char>(std::toupper(methodName.at(0))));
			methodName.insert(0, "get");
			methodName += "()";

			if (field.isStatic)
			{
				preTypeQualifiers = "static";
				postQualifiers.clear();	//remove the const
			}
			else if (isConst || !(isRef || isPtr))	//Field can't be const and static so else if is fine
			{
				postQualifiers = " const";	//A const field or a getter-by-value implies a const getter even if not specified
			}

			if (isConst && (isRef || isPtr))
			{
				rawReturnType += " const ";
			}

			if (isRef)
			{
				rawReturnType += "& ";
			}
			else if (isPtr)
			{
				rawReturnType += "* ";
				returnName.insert(0, "&");
			}

			inout_result += preTypeQualifiers + rawReturnType + methodName + postQualifiers + ";" + env.getSeparator();

			return true;
		}

		virtual bool generateSourceFileHeaderCodeForEntity(kodgen::EntityInfo const& entity, kodgen::Property const& property, kodgen::uint8 /* propertyIndex */,
														   kodgen::MacroCodeGenEnv& env, std::string& inout_result) noexcept override
		{
			kodgen::FieldInfo const& field = static_cast<kodgen::FieldInfo const&>(entity);

			std::string postQualifiers;
			std::string rawReturnType	= field.type.getCanonicalName() + " ";
			std::string returnName		= field.name;
			bool		isConst			= false;
			bool		isRef			= false;
			bool		isPtr			= false;

			for (std::string const& subprop : property.arguments)
			{
				if (!isConst && subprop.at(0) == 'c')			//const
				{
					isConst = true;
				}
				else if (subprop.at(0) == 'e')	//explicit
				{
					//Don't generate getter definition if it is marked as explicit
					return true;
				}
				else if (!(isRef || isPtr))
				{
					if (subprop.at(0) == '&')					//&
					{
						isRef = true;
					}
					else if (subprop.at(0) == '*')				//*
					{
						isPtr = true;
					}
				}
			}
			
			//Upper case the first field info char if applicable
			std::string methodName = field.name;
			methodName.replace(0, 1, 1, static_cast<char>(std::toupper(methodName.at(0))));
			methodName.insert(0, "get");
			methodName += "()";

			if (!field.isStatic && (isConst || !(isRef || isPtr)))	//Field can't be const and static so else if is fine
			{
				postQualifiers = " const";	//A const field or a getter-by-value implies a const getter even if not specified
			}

			if (isConst && (isRef || isPtr))
			{
				rawReturnType += " const ";
			}

			if (isRef)
			{
				rawReturnType += "& ";
			}
			else if (isPtr)
			{
				rawReturnType += "* ";
				returnName.insert(0, "&");
			}

			inout_result += rawReturnType + entity.outerEntity->getFullName() + "::" + methodName + postQualifiers + " { return " + returnName + "; }" + env.getSeparator();

			return true;
		}
};