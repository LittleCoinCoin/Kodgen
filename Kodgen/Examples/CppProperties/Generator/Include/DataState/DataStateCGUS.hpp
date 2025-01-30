#pragma once

#include "Kodgen/CodeGen/CodeGenUnitSettings.h"

class DataStateCGUS : public kodgen::CodeGenUnitSettings
{

private:
	std::string namespaceName = "";
	std::string className = "DataState";
	std::string fileName = "DataState";
	std::string headerFileExtension = ".h.hpp";

public:
	inline const std::string& getNamespaceName() const noexcept
	{
		return namespaceName;
	}

	inline const std::string& getClassName() const noexcept
	{
		return className;
	}

	inline const std::string& getFileName() const noexcept
	{
		return fileName;
	}

	inline const std::string& getHeaderFileExtension() const noexcept
	{
		return headerFileExtension;
	}

	inline void setNamespaceName(std::string const& _namespaceName) noexcept
	{
		namespaceName = _namespaceName;
	}

	inline void setClassName(std::string const& _className) noexcept
	{
		className = _className;
	}

	inline void setFileName(std::string const& _fileName) noexcept
	{
		fileName = _fileName;
	}

	inline void setHeaderFileExtension(std::string const& _headerFileExtension) noexcept
	{
		headerFileExtension = _headerFileExtension;
	}
};