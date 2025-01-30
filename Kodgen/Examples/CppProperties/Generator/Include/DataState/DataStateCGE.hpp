#pragma once

#include "Kodgen/CodeGen/CodeGenEnv.h"

class DataStateCGE : public kodgen::CodeGenEnv
{
	friend class DataStateCGU;

public:
	enum DSCodeLoc : kodgen::uint8
	{
		DSCodeLoc_Includes,
		DSCodeLoc_DataVectors,
		DSCodeLoc_EnumDataType,
		DSCodeLoc_TypeDefs,
		DSCodeLoc_GetDatasPtrVariantFuncDEF,
		DSCodeLoc_GetDatasPtrVariantFuncArr,
		DSCodeLoc_EmplaceBackDataFuncDEF,
		DSCodeLoc_EmplaceBackDataTemplateFuncs,

		DSCodeLoc_COUNT
	};

private:
	DSCodeLoc codeGenTarget = static_cast<DSCodeLoc>(0U);
	bool isFirst = true;

public:
	DataStateCGE() noexcept = default;
	DataStateCGE(DSCodeLoc _codeGenTarget) noexcept :
		codeGenTarget{ _codeGenTarget }
	{

	}

	inline const DSCodeLoc& getCodeGenTarget() noexcept
	{
		return codeGenTarget;
	}

	inline const bool& getIsFirst() noexcept
	{
		return isFirst;
	}

	inline void setCodeGenTarget(DSCodeLoc _codeGenTarget) noexcept
	{
		codeGenTarget = _codeGenTarget;
	}

	inline void setIsFirst(bool _isFirst) noexcept
	{
		isFirst = _isFirst;
	}
};