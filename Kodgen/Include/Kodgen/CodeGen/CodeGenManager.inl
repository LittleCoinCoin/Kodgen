/**
*	Copyright (c) 2020 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the LICENSE.md file for full license details.
*/

template <typename FileParserType, typename CodeGenUnitType>
void CodeGenManager::oneGenerateForEachParsedFile(FileParserType& fileParser, CodeGenUnitType& codeGenUnit, std::set<fs::path> const& toProcessFiles, CodeGenResult& out_genResult) noexcept
{
	std::vector<std::shared_ptr<TaskBase>>	generationTasks;
	uint8									iterationCount = codeGenUnit.getIterationCount();

	//Reserve enough space for all tasks
	generationTasks.reserve(toProcessFiles.size() * iterationCount);

	//Launch all parsing -> generation processes
	std::shared_ptr<TaskBase> parsingTask;
	
	for (int i = 0; i < iterationCount; i++)
	{
		//Lock the thread pool until all tasks have been pushed to avoid competing for the tasks mutex
		_threadPool.setIsRunning(false);

		for (fs::path const& file : toProcessFiles)
		{
			auto parsingTaskLambda = [&fileParser, &file](TaskBase*) -> FileParsingResult
			{
				//Copy a parser for this task
				FileParserType		fileParserCopy = fileParser;
				FileParsingResult	parsingResult;

				fileParserCopy.parse(file, parsingResult);

				return parsingResult;
			};

			auto generationTaskLambda = [&codeGenUnit](TaskBase* parsingTask) -> CodeGenResult
			{
				CodeGenResult out_generationResult;

				//Copy the generation unit model to have a fresh one for this generation unit
				CodeGenUnitType	generationUnit = codeGenUnit;
				CodeGenEnv* env = generationUnit.createCodeGenEnv();

				//Get the result of the parsing task
				FileParsingResult parsingResult = TaskHelper::getDependencyResult<FileParsingResult>(parsingTask, 0u);

				//If you assert/crash here, means the createCodeGenEnv method returned nullptr
				//Check the implementation in the CodeGenUnit you use.
				assert(env != nullptr);

				//Generate the file if no errors occured during parsing
				if (parsingResult.errors.empty())
				{
					//Pre-generation step
					bool result = generationUnit.preGenerateCode(parsingResult, *env);

					//Generation step (per module/entity pair), runs only if the pre-generation step succeeded
					if (result)
					{
						result &= generationUnit.generateCode(parsingResult, *env);

						//Post-generation step, runs only if the generation step succeeded
						if (result)
						{
							result &= generationUnit.postGenerateCode(*env);
						}
					}

					out_generationResult.completed = result;
				}

				delete env;

				return out_generationResult;
			};

			//Add file to the list of parsed files before starting the task to avoid having to synchronize threads
			out_genResult.parsedFiles.push_back(file);

			//Parse files
			//For multiple iterations on a same file, the parsing task depends on the previous generation task for the same file
			parsingTask = _threadPool.submitTask(std::string("Parsing ") + std::to_string(i), parsingTaskLambda);

			//Generate code
			generationTasks.emplace_back(_threadPool.submitTask(std::string("Generation ") + std::to_string(i), generationTaskLambda, { parsingTask }));
		}

		//Wait for this iteration to complete before continuing any further
		//(an iteration N depends on the iteration N - 1)
		_threadPool.setIsRunning(true);
		_threadPool.joinWorkers();
	}

	//Merge all generation results together
	for (std::shared_ptr<TaskBase>& task : generationTasks)
	{
		out_genResult.mergeResult(TaskHelper::getResult<CodeGenResult>(task.get()));
	}
}

template <typename FileParserType, typename CodeGenUnitType>
void	oneGenerateForAllParsedFiles(FileParserType& fileParser, CodeGenUnitType& codeGenUnit, std::set<fs::path> const& toProcessFiles, CodeGenResult& out_genResult)	noexcept
{
	uint8	iterationCount = codeGenUnit.getIterationCount();

	//Launch all parsing -> generation processes
	std::vector<std::shared_ptr<TaskBase>> parsingTasks;
	parsingTasks.reserve(toProcessFiles.size() * iterationCount);
	std::vector<FileParsingResult> parsingResults;
	parsingResults.reserve(toProcessFiles.size() * iterationCount);

	for (int i = 0; i < iterationCount; i++)
	{
		//Lock the thread pool until all tasks have been pushed to avoid competing for the tasks mutex
		_threadPool.setIsRunning(false);

		for (fs::path const& file : toProcessFiles)
		{
			auto parsingTaskLambda = [&fileParser, &file](TaskBase*) -> FileParsingResult
				{
					//Copy a parser for this task
					FileParserType		fileParserCopy = fileParser;
					FileParsingResult	parsingResult;

					fileParserCopy.parse(file, parsingResult);

					return parsingResult;
				};

			//Add file to the list of parsed files before starting the task to avoid having to synchronize threads
			out_genResult.parsedFiles.push_back(file);

			//Parse files
			//For multiple iterations on a same file, the parsing task depends on the previous generation task for the same file
			parsingTasks.emplace_back(_threadPool.submitTask(std::string("Parsing ") + std::to_string(i), parsingTaskLambda));
		}

		//Wait for this iteration to complete before continuing any further
		//(an iteration N depends on the iteration N - 1)
		_threadPool.setIsRunning(true);
		_threadPool.joinWorkers();
	}

	bool noParsingError = true;
	for (std::shared_ptr<TaskBase>& task : parsingTasks)
	{
		parsingResults.emplace_back(TaskHelper::getDependencyResult<FileParsingResult>(parsingTask, 0u));
		noParsingError &= parsingResults.back().errors.empty();
	}

	if (noParsingError)
	{
		//Copy the generation unit model to have a fresh one for this generation unit
		CodeGenUnitType	generationUnit = codeGenUnit;
		CodeGenEnv* env = generationUnit.createCodeGenEnv();

		//Get the result of the parsing task
		FileParsingResult parsingResult = TaskHelper::getDependencyResult<FileParsingResult>(parsingTask, 0u);

		//If you assert/crash here, means the createCodeGenEnv method returned nullptr
		//Check the implementation in the CodeGenUnit you use.
		assert(env != nullptr);

		//Pre-generation step
		bool result = generationUnit.preGenerateCode(parsingResult, *env);

		//Generation step (per module/entity pair), runs only if the pre-generation step succeeded
		for (FileParsingResult const& parsingResult : parsingResults)
		{
			if (result)
			{
				result &= generationUnit.generateCode(parsingResult, *env);
			}
		}

		//Post-generation step, runs only if the generation step succeeded
		if (result)
		{
			result &= generationUnit.postGenerateCode(*env);
		}

		out_generationResult.completed = result;

		delete env;
	}
}

template <typename FileParserType, typename CodeGenUnitType>
CodeGenResult CodeGenManager::run(FileParserType& fileParser, CodeGenUnitType& codeGenUnit, EGenerationStrategies generationStrategies) noexcept
{
	//Check FileParser validity
	static_assert(std::is_base_of_v<FileParser, FileParserType>, "fileParser type must be a derived class of kodgen::FileParser.");
	static_assert(std::is_copy_constructible_v<FileParserType>, "The provided file parser must be copy-constructible.");

	//Check FileGenerationUnit validity
	static_assert(std::is_base_of_v<CodeGenUnit, CodeGenUnitType>, "codeGenUnit type must be a derived class of kodgen::CodeGenUnit.");
	static_assert(std::is_copy_constructible_v<CodeGenUnitType>, "The CodeGenUnit you provide must be copy-constructible.");

	CodeGenResult genResult;
	genResult.completed = true;

	if (!checkGenerationSetup(fileParser, codeGenUnit))
	{
		genResult.completed = false;
	}
	else
	{
		//Start timer here
		auto				start			= std::chrono::high_resolution_clock::now();
		std::set<fs::path>	filesToProcess	= identifyFilesToProcess(codeGenUnit, genResult, generationStrategies);

		//Don't setup anything if there are no files to generate
		if (filesToProcess.size() > 0u)
		{
			//Initialize the parsing settings to setup parser compilation arguments.
			//parsingSettings can't be nullptr since it has been checked in the checkGenerationSetup call.
			fileParser.getSettings().init(logger);

			generateMacrosFile(fileParser.getSettings(), codeGenUnit.getSettings()->getOutputDirectory());

			//Start files processing
			if (generationStrategies && EGenerationStrategies::OneGenerateForEachFile)
			{
				oneGenerateForEachParsedFile(fileParser, codeGenUnit, filesToProcess, genResult);
			}
			else if (generationStrategies && EGenerationStrategies::OneGenerateForAllFiles)
			{
				oneGenerateForAllParsedFiles(fileParser, codeGenUnit, filesToProcess, genResult);
			}
			else
			{
				assert(false);//Unhandled generation strategy
			}
		}

		genResult.duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() * 0.001f;
	}
	
	return genResult;
}