
#include <ShaderTranspilerCore.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

enum class OutputType
{
	GLSL,
	VULKAN_GLSL,
	MSL,
	HLSL,
	Max,
};

int main(int argc, char* argv[])
{

	std::vector<std::string> args;

	for (int i = 1; i < argc; i++)
	{
		args.emplace_back(argv[i]);
	}

	LLGI::ShaderStageType shaderStage = LLGI::ShaderStageType::Max;
	OutputType outputType = OutputType::Max;
	std::string code;
	std::string inputPath;
	std::string outputPath;
	bool isES = false;
	bool isDX12 = false;
	bool plain = false;
	int shaderModel = 0;
	std::vector<LLGI::SPIRVGeneratorMacro> macros;

	for (size_t i = 0; i < args.size();)
	{
		if (args[i] == "--vert")
		{
			shaderStage = LLGI::ShaderStageType::Vertex;
			i += 1;
		}
		else if (args[i] == "--frag")
		{
			shaderStage = LLGI::ShaderStageType::Pixel;
			i += 1;
		}
		else if (args[i] == "-G")
		{
			outputType = OutputType::GLSL;
			i += 1;
		}
		else if (args[i] == "-M")
		{
			outputType = OutputType::MSL;
			i += 1;
		}
		else if (args[i] == "-H")
		{
			outputType = OutputType::HLSL;
			i += 1;
		}
		else if (args[i] == "-V")
		{
			outputType = OutputType::VULKAN_GLSL;
			i += 1;
		}
		else if (args[i] == "-D")
		{
			macros.push_back(LLGI::SPIRVGeneratorMacro(args[i + 1].c_str(), args[i + 2].c_str()));
			i += 3;
		}
		else if (args[i] == "--sm")
		{
			shaderModel = atoi(args[i + 1].c_str());
			i += 2;
		}
		else if (args[i] == "--es")
		{
			isES = true;
			i += 1;
		}
		else if (args[i] == "--plain")
		{
			plain = true;
			i += 1;
		}
		else if (args[i] == "--dx12")
		{
			isDX12 = true;
			i += 1;
		}
		else if (args[i] == "--input")
		{
			if (i == args.size() - 1)
			{
				std::cout << "Invald input" << std::endl;
				return 0;
			}

			std::ifstream ifs(args[i + 1]);
			if (ifs.fail())
			{
				std::cout << "Invald input" << std::endl;
				return 0;
			}
			code = std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			inputPath = args[i + 1];
			i += 2;
		}
		else if (args[i] == "--output")
		{
			if (i == args.size() - 1)
			{
				std::cout << "Invald output" << std::endl;
				return 0;
			}

			outputPath = args[i + 1];

			i += 2;
		}
		else
		{
			i++;
		}
	}

	if (outputType == OutputType::Max)
	{
		std::cout << "Unknown type" << std::endl;
		return 0;
	}

	if (shaderStage == LLGI::ShaderStageType::Max)
	{
		std::cout << "Unknown ShaderStage" << std::endl;
		return 0;
	}

	if (outputPath == "")
	{
		std::cout << "Invalid output type" << std::endl;
		return 0;
	}

	auto loadFunc = [](std::string s) -> std::vector<uint8_t> {
		std::ifstream file(s, std::ios_base::binary | std::ios_base::ate);
		if (file)
		{
			std::vector<uint8_t> ret;
			auto size = (int)file.tellg();
			ret.resize(size);
			file.seekg(0, file.beg);
			file.read((char*)ret.data(), size);
			return ret;
		}
		return std::vector<uint8_t>();
	};

	auto generator = std::make_shared<LLGI::SPIRVGenerator>(loadFunc);

	auto spirv = generator->Generate(inputPath.c_str(), code.c_str(), macros, shaderStage, outputType == OutputType::VULKAN_GLSL);

	if (spirv->GetData().size() == 0)
	{
		std::cout << spirv->GetError() << std::endl;
		return 0;
	}

	auto transpiler = std::shared_ptr<LLGI::SPIRVTranspiler>();

	if (outputType == OutputType::GLSL)
	{
		transpiler = std::make_shared<LLGI::SPIRVToGLSLTranspiler>(false, shaderModel != 0 ? shaderModel : 420, isES, plain);
	}
	else if (outputType == OutputType::VULKAN_GLSL)
	{
		transpiler = std::make_shared<LLGI::SPIRVToGLSLTranspiler>(true);
	}
	else if (outputType == OutputType::MSL)
	{
		transpiler = std::make_shared<LLGI::SPIRVToMSLTranspiler>();
	}
	else if (outputType == OutputType::HLSL)
	{
		transpiler = std::make_shared<LLGI::SPIRVToHLSLTranspiler>(shaderModel != 0 ? shaderModel : 40, isDX12);
	}

	std::cout << inputPath << " -> " << outputPath << " ShaderModel=" << shaderModel << std::endl;
	if (!transpiler->Transpile(spirv))
	{
		std::cout << transpiler->GetErrorCode() << std::endl;
		return 0;
	}

	std::ofstream outputfile(outputPath);
	if (outputfile.bad())
	{
		std::cout << "Invald output" << std::endl;
		return 0;
	}

	outputfile << transpiler->GetCode();

	return 0;
}
