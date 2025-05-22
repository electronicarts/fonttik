# Fonttik

Fonttik is a console application and library created by EA that processes images and videos highlighting text that meets and does not meet desired criteria. The guidelines checked by Fonttik ensure text has a minimum size relative to screen resolution or DPI and that it has sufficient luminance contrast against its background, according to desired criteria, to be readable or more likely to be readeable by color vision deficient people. Fonttik is not intended to guarantee, certify or otherwise validate an image or video's compliance with legal, regulatory or other requirements.

## Platforms
 * Linux
 * Windows
 * MacOS

## How to build

Fonttik has been created as a CMake project and can be compiled in any C++ compatible system. There are various subprojects, all of them compile their necessary inter-project dependencies as well:
- FonttikLib: Logical backend for Fonttik, has everything needed for analysis.
- FonttikApp: Standard Fonttik example executable, links to FonttikLib.
- Fonttik.Tests: All of the acceptance, integration and unit tests for the Fonttik code. Any change to the code must pass or update these tests.

All of the dependencies for Fonttik are specified in [vcpk.json](./Backend/CoreCpp/vcpkg.json). vcpkg is a cross-platform package manager for C++ developed by Microsoft. Installation instructions for vcpkg can be found in its [repository](https://github.com/Microsoft/vcpkg).  
If you want CMake to automatically run vcpkg and compile all dependencies for you, you must set your VCPKG root folder as an environment variable called "VCPKG_ROOT" or add it as a variable in the CMakePresets.json.

Fonttik uses cmake presets to build with different configurations, e.g.

`>cmake --preset windows-release`   
`>cmake --build --preset windows-release`

Note: There are some CUDA presets that allow to build OpenCV with hardware acceleration. To be able to use CUDA you will need an NVidia GPU and their compilers, for more information you can read the [OpenCV docs on the topic](https://docs.opencv.org/2.4/modules/gpu/doc/introduction.html).

More presets can be added to the CMakePresets.json file or defined in a CMakeUserPresets.json file.

Build options:
- BUILD_EXAMPLE_APP: build Fonttik example console application
- BUILD_SHARED_LIBS: build Fonttik as a shared library
- EXPORT_FONTTIK: export and install library
- BUILD_TESTS: build library unit tests
- BUILD_COVERAGE: build code coverage (only available for Linux)

## Running the tool

When executing Fonttik you must input the path to where the media you want to analyze is. You can either specify a folder, in which case every piece of media in the folder and subfolders will be analyzed, or a file, in which case that specific image will be analyzed.

Example of execution command when the terminal is running in Fonttik's directory

`>FonttikApp.exe ./resources/bf2042/chat_window_closed.png`

Fonttik would proceed to analyze the file 'chat_window_closed.png' with the configuration you specified beforehand. The filepath can be relative to where you are executing Fonttik or absolute (for example an absolute path pointing to your documents would be C:\Users\[YourUserName]\Documents)


While Fonttik is analyzing an image it outputs the results it finds while operating through the console. If any text box doesn't pass the size or contrast checks it is notified and its position and detected values are output. At the end of the analysis an overall pass or fail result is decided for both size and contrast checks. These results are also written to a log file at the end.

For a more human-friendly output, Fonttik also outputs two images (one for contrast and another for size checks with all of the text boxes detected overlaid. These text boxes will be red if they fail or green if they pass by default. If the user has decided they want to use recommended values for checks, textboxes will be orange if they pass the minimum requirement but don't meet the recommended criteria. The colors can be customised by changing TextboxOutlineColors in the configuration file.

If you have activated the printValuesOnResults option in the configuration, you'll see the detected value for each check next to its corresponding text box in the image.

Results will be saved depending on the appsetting options enabled (explained in Configuration) and will be stored in the same location as the media analysed.

### Optional arguments

When running Fonttik the following optional arguments can be passed to alter the functionality of the tool:

- `-c`: Specify configuration file. Given a path to a specific configuration file uses that one during this execution. By default Fonttik looks for config.json in its own folder.
- `-a`: Store results as the analysis runs asynchronously 

## Configuration

Fonttik can be configured by a .json file, default configuration provided under [Data](./Backend/CoreCpp/Fonttik/data/config.json). Config is automatically copied over when building with CMake. You can also use a different configuration when running the application by using the `-c` option.

- AppSettings configuration for how the tool should function:
	- DetectionBackend: Fonttik supports the EAST and DB (differential binarization) backends from OpenCV.
	- SaveTextboxOutline: whether to save the resulting images with the textboxes indicating if each textbox passed the guidelines test.
	- TextboxOutlineColors: Sets of RGB values (0 to 255) for the different results the tool can output.
	- UseTextRecognition: whether to use text recognition. If deactivated execution will be faster but character width won't be measured.
	- UseDPI: Whether to compare measurements to DPI-based guidelines or resolution-based guidelines. 
	- TargetDPI: Only used if UseDPI is set to true. Determine the target DPI of the image to be tested.
	- TargetResolution: Only used if UseDPI is set to false and TargetResolution's value is different to 0. Useful if you want to analyze a cropped image and want to set its source resolution.
	- SaveLogs: Whether to create the txt log file at the end of execution.
	- PrintResultValues: Whether to print the obtained measurements to the side of detected text boxes in the image output to facilitate reviewing.
	- FocusMask: Start and finish width and height in a range of 0 to 1 of the part of the image that is to be recognized. By default the whole image is processed.
	- IgnoreMask: Regions to be ignored when processing the image. Format is the same as FocusMask.
- TextRecognition configuration for the models used for text recognition:
	- RecognitionModel: Name of the file for a trained neural network to be used for text recognition.
	- DecodeType: Sets the decoding method of translating the network output into string, can be 'CTC-greedy' or 'CTC-prefix-beam-search'.
	- VocabularyFile: Path of the file with the symbols that can be recognized by the program.
	- Scale:  Multiplier for frame values. By default is 1/127.5.
	- Mean: Scalar with mean values which are subtracted from channels. By default it's the same for all three color channels.
- TextDetection configuration for EAST, DB and extra parameters for pruning:
	- RotationThresholdDegree: How many degrees can a textbox be rotated before being pruned. Useful to prune environmental text that you don't want recognized, since it usually isn't parallel to the screen.
	- MergeThreshold: Percentage that two separate textboxes have to overlap in any direction (horizontal or vertical) before they are merged into one. This is worth changing if the tool is separating some words into two or more when recognizing due to special fonts or effects.
	- Confidence: Minimum confidence that the neural network has to have for text to be considered a textbox.
	- PreferredBackend: DEFAULT. Text detection backend for EAST, DB or CUDA. CUDA is our default option, it will use hardware acceleration to boost performance.
	- PreferredTarget: CPU, OPENCL or CUDA. Text detection target for EAST or DB, if the machine where Fonttik is running has a non NVidia GPU, OPENCL option will be selected. CUDA will only be used with NVidia GPUs. This two options will lead to better performance than CPU, but there is also the option to change this for CPU if you wish to do so.
	- EAST specific configuration
		- DetectionModel: Name of the file for a trained neural network to be used for text detection.
		- NmsThreshold: Threshold for automatic merge algorithm. Increasing or decreasing this value might result in textboxes being cut off or various similar textboxes stacking on top of each other.
		- DetectionScale: Values given by the OpenCV EAST documentation.
		- DetectionMean:  Values given by the OpenCV EAST documentation.
	- DB specific configuration
		- DetectionModel: Name of the file for a trained neural network to be used for text detection.
    	- BinaryThreshold: OpenCV post processing parameter. 
    	- PolygonThreshold: OpenCV post processing parameter. 
    	- MaxCandidates: maximum number of detected textboxes.
		- UnclipRatio: Non-maximum suppression equivalent from EAST 
		- Scale: OpenCV normalization parameter.
		- DetectionMean: OpenCV normalization parameter.
		- InputSize: OpenCV normalization parameter.
- Guideline configuration for guidelines that are to be applied:
	- Contrast: The minimum contrast ratio detected text has to have with its background. By default 4.5 according to WCAG 2 guidelines.
	- RecommendedContrast: If set higher than Contrast, any measured value that falls between Contrast and RecommendedContrast will be a warning, but won't fail the analysis.
	- TextBackgroundRadius: Radius around the text that is considered to be its background.
	- Resolutions: Sets of image resolutions that will be recognized, they are identified by the image height in pixels for that resolution. Guidelines are supplied for 720, 1080, and 4k resolutions, these may be customized and more can be added to the user's liking.
		- Width: The minimum average character width each word must have.
		- Height: The minimum character height each words must have.
	- ResolutionsRecommendations: Recommended sizes for resolutions, if set higher than Resolutions will raise a warning if a measured value falls between them but won't fail the 	analysis.
	- HeightPer100DPI: Only used if UseDPI is activated. DPI measurements scale linearly, so you only have to set a baseline and all necessary calculations are done automatically. Default value is 18 based on Microsoft guidelines.
- sRGBLinearizationValues: Precalculated values for sRGB linearization to prevent floating point errors when calculating them in real time.

## LICENSE

Fonttik is provided under the BSD 3 Clause License, see [LICENSE](./LICENSE.txt) for details.

Fonttik utilizes open source software, see [NOTICE](./NOTICE.txt) licenses and details.

## CONTRIBUTING

Before you can contribute, EA must have a Contributor License Agreement (CLA) on file that has been signed by each contributor. You can sign here: [CLA](https://electronicarts.na1.echosign.com/public/esignWidget?wid=CBFCIBAA3AAABLblqZhByHRvZqmltGtliuExmuV-WNzlaJGPhbSRg2ufuPsM3P0QmILZjLpkGslg24-UJtek*)
