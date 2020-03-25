#ifndef _DENOISER_TONEMAPPER_COMMAND_LINE_HANDLER_
#define _DENOISER_TONEMAPPER_COMMAND_LINE_HANDLER_

#include <iostream>
#include <cstdio>
#include <irrlicht.h>

#define PROPER_CMD_ARGUMENTS_AMOUNT 8
#define PROPER_BATCH_FILE_ARGUMENTS_AMOUNT 3

enum COMMAND_LINE_MODE
{
	CLM_UNKNOWN,
	CLM_CMD_LIST,
	CLM_BATCH_INPUT,

	CLM_COUNT
};

constexpr std::string_view requiredArgumentsMessage = R"(
Pass appripiate arguments to launch the example or load them using predefined file!
* To load them with a arguments file usage type DenoiserTonemapper.exe -batch <yourargumentfile.txt>
* To load them passing arguments through cmd.
Loading syntax:
-OPENEXR_FILE=filename
-CHANNEL_NAMES=colorChannelName,albedoChannelName,normalChannelName
-CAMERA_TRANSFORM=value,value,value,...
-EXPOSURE_BIAS=value
-DENOISER_BLEND_FACTOR=value
-BLOOM_SIZE=x,y
-TONEMAPPER=tonemapper=arg1,arg2,arg3,...
-OUTPUT=file.choosenextension
Note there mustn't be any space characters!
Also you mustn't put another data like comments or behaviour will be undefined, the app'll crash (TODO: not crash on parse failure)

Description and usage:
OPENEXR_FILE: OpenEXR file containing various channels - type without extension
CHANNEL_NAMES: name of denoiser input channels, first is mandatory rest is optional - split each next channel using ","
CAMERA_TRANSFORM: values as "initializer list" for camera transform matrix with
row_major layout (max 9 values - extra values will be ignored)
EXPOSURE_BIAS: exposure bias value used in shader
DENOISER_BLEND_FACTOR: denoiser blend factor used in shader
BLOOM_SIZE: bloom size
TONEMAPPER: tonemapper - choose between "REINHARD" and "ACES". After specifing it
you have to assing arguments to revelant tonemapper. For "REINHARD" tonemapper
there are no arguments, so you should not type anything else, but for "ACES"
you have to specify some arguments for it's curve function. They are following:
arg1=value
arg2=value
arg3=value
arg4=value
arg5=value
where function is:
f(x) = clamp((x * (arg1 * x + arg2)) / (x * (arg3 * x + arg4) + arg5), 0.0, 1.0)
so for example, specifing "REINHARD" tonemapper looks like:
-TONEMAPPER=REINHARD
and specifing "ACES" looks like:
-TONEMAPPER=ACES=arg1,arg2,arg3,arg4,arg5
OUTPUT: output file with specified extension 
An example file is put into the DenoiserTonemapperExample folder
)";

constexpr std::string_view OPENEXR_FILE = "OPENEXR_FILE";
constexpr std::string_view CHANNEL_NAMES = "CHANNEL_NAMES";
constexpr std::string_view CAMERA_TRANSFORM = "CAMERA_TRANSFORM";
constexpr std::string_view EXPOSURE_BIAS = "EXPOSURE_BIAS";
constexpr std::string_view DENOISER_BLEND_FACTOR = "DENOISER_BLEND_FACTOR";
constexpr std::string_view BLOOM_SIZE = "BLOOM_SIZE";
constexpr std::string_view TONEMAPPER = "TONEMAPPER";
constexpr std::string_view REINHARD = "REINHARD";
constexpr std::string_view ACES = "ACES";
constexpr std::string_view OUTPUT = "OUTPUT";

enum DENOISER_TONEMAPPER_EXAMPLE_ARGUMENTS
{
	DTEA_OPENEXR_FILE,
	DTEA_CHANNEL_NAMES,
	DTEA_CAMERA_TRANSFORM,
	DTEA_EXPOSURE_BIAS,
	DTEA_DENOISER_BLEND_FACTOR,
	DTEA_BLOOM_SIZE,
	DTEA_REINHARD,
	DTEA_ACES,
	DTEA_OUTPUT,

	DTEA_COUNT
};

enum ACES_ARGUMENTS
{
	AA_ARG1,
	AA_ARG2,
	AA_ARG3,
	AA_ARG4,
	AA_ARG5,

	AA_COUNT
};

using cmdVariableName = std::string;
using rawValuesOfcmdVariable = std::string;
using variablesType = std::array<std::pair<cmdVariableName, irr::core::vector<rawValuesOfcmdVariable>>, DTEA_COUNT>;

class CommandLineHandler
{
	public:

		CommandLineHandler(irr::core::vector<std::string> argv, irr::asset::IAssetManager* am);

		auto getInputFilesAmount()
		{
			return rawVariables.size();
		}

		auto& getFileNamesBundle() const
		{
			return fileNamesBundle;
		}

		auto& getChannelNamesBundle() const
		{
			return channelNamesBundle;
		}

		auto& getCameraTransformBundle() const
		{
			return cameraTransformBundle;
		}

		auto& getExposureBiasBundle() const
		{
			return exposureBiasBundle;
		}

		auto& getDenoiserBlendFactorBundle() const
		{
			return denoiserBlendFactorBundle;
		}

		auto& getBloomSizeBundle() const
		{
			return bloomSizeBundle;
		}

		auto& getTonemapperBundle() const
		{
			return tonemapperBundle;
		}

		auto& getOutputFileBundle() const
		{
			return outputFileBundle;
		}

		auto getStatus() { return status; }
		auto getMode() { return mode; }
		auto doesItSupportManyInputFiles() { return mode == CLM_BATCH_INPUT; }

	private:

		void initializeMatchingMap(variablesType& rawVariablesPerFile)
		{
			rawVariablesPerFile[DTEA_OPENEXR_FILE].first = OPENEXR_FILE;
			rawVariablesPerFile[DTEA_CHANNEL_NAMES].first = CHANNEL_NAMES;
			rawVariablesPerFile[DTEA_CAMERA_TRANSFORM].first = CAMERA_TRANSFORM;
			rawVariablesPerFile[DTEA_EXPOSURE_BIAS].first = EXPOSURE_BIAS;
			rawVariablesPerFile[DTEA_DENOISER_BLEND_FACTOR].first = DENOISER_BLEND_FACTOR;
			rawVariablesPerFile[DTEA_BLOOM_SIZE].first = BLOOM_SIZE;
			rawVariablesPerFile[DTEA_REINHARD].first = REINHARD;
			rawVariablesPerFile[DTEA_ACES].first = ACES;
			rawVariablesPerFile[DTEA_OUTPUT].first = OUTPUT;
		}

		auto getFileName(uint64_t id = 0)
		{
			return rawVariables[id][DTEA_OPENEXR_FILE].second[0];
		}

		auto getChannelNames(uint64_t id = 0)
		{
			return rawVariables[id][DTEA_CHANNEL_NAMES].second;
		}

		auto getCameraTransform(uint64_t id = 0)
		{
			irr::core::matrix3x4SIMD cameraTransform;
			const auto send = rawVariables[id][DTEA_CAMERA_TRANSFORM].second.end();
			auto sit = rawVariables[id][DTEA_CAMERA_TRANSFORM].second.begin();
			for (auto i=0; i<3u&&sit!=send; i++)
			for (auto j=0; j<3u&&sit!=send; j++)
				cameraTransform(i,j) = std::stof(*(sit++));

			return cameraTransform;
		}

		auto getExposureBias(uint64_t id = 0)
		{
			return std::stof(rawVariables[id][DTEA_EXPOSURE_BIAS].second[0]);
		}

		auto getDenoiserBlendFactor(uint64_t id = 0)
		{
			return std::stof(rawVariables[id][DTEA_DENOISER_BLEND_FACTOR].second[0]);
		}

		auto getBloomSize(uint64_t id = 0)
		{
			return irr::core::vector2df(std::stof(rawVariables[id][DTEA_BLOOM_SIZE].second[0]), std::stof(rawVariables[id][DTEA_BLOOM_SIZE].second[1]));
		}

		auto getTonemapper(uint64_t id = 0)
		{
			const bool isChoosenReinhard = rawVariables[id][DTEA_ACES].second.empty();
			auto tonemapper = isChoosenReinhard ? rawVariables[id][DTEA_REINHARD] : rawVariables[id][DTEA_ACES];
			irr::core::vector<float> values(isChoosenReinhard ? 0 : AA_COUNT);

			for (auto i = 0; i < values.size(); ++i)
				*(values.begin() + i) = std::stof(tonemapper.second[i]);

			return std::make_pair(tonemapper.first, values);
		}

		auto getOutputFile(uint64_t id = 0)
		{
			return rawVariables[id][DTEA_OUTPUT].second[0];
		}

		void performFInalStepForUsefulVariables()
		{
			const auto inputFilesAmount = getInputFilesAmount();
			fileNamesBundle.reserve(inputFilesAmount);
			channelNamesBundle.reserve(inputFilesAmount);
			cameraTransformBundle.reserve(inputFilesAmount);
			exposureBiasBundle.reserve(inputFilesAmount);
			denoiserBlendFactorBundle.reserve(inputFilesAmount);
			bloomSizeBundle.reserve(inputFilesAmount);
			tonemapperBundle.reserve(inputFilesAmount);
			outputFileBundle.reserve(inputFilesAmount);

			for (auto i = 0ul; i < inputFilesAmount; ++i)
			{
				fileNamesBundle.push_back(getFileName(i));
				channelNamesBundle.push_back(getChannelNames(i));
				cameraTransformBundle.push_back(getCameraTransform(i));
				exposureBiasBundle.push_back(getExposureBias(i));
				denoiserBlendFactorBundle.push_back(getDenoiserBlendFactor(i));
				bloomSizeBundle.push_back(getBloomSize(i));
				tonemapperBundle.push_back(getTonemapper(i));
				outputFileBundle.push_back(getOutputFile(i));
			}
		}

		bool status;
		COMMAND_LINE_MODE mode;
		irr::core::vector<variablesType> rawVariables;

		// I want to deduce those types bellow by using type from functions above
		// like deduce type of getTonemapper()

		irr::core::vector<std::string> fileNamesBundle;
		irr::core::vector<irr::core::vector<std::string>> channelNamesBundle;
		irr::core::vector<irr::core::matrix3x4SIMD> cameraTransformBundle;
		irr::core::vector<float> exposureBiasBundle;
		irr::core::vector<float> denoiserBlendFactorBundle;
		irr::core::vector<irr::core::vector2df> bloomSizeBundle;
		irr::core::vector<std::pair<std::string, irr::core::vector<float>>> tonemapperBundle;
		irr::core::vector<std::string> outputFileBundle;
};

#endif // _DENOISER_TONEMAPPER_COMMAND_LINE_HANDLER_