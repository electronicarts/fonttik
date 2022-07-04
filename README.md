# TinEye

TinEye is a simple console application created by EA Compliance & Certification that processes images and videos highlighting compliant and not compliant text according to a set of guidelines. The guidelines checked by Tin Eye ensure text has a minimum size relative to screen resolution or DPI and that it has sufficient luminance contrast against its background to be readable by color vision deficient people

## Configuration

TinEye can be configured by a .json file, default configuration provided under [Data](./Backend/CoreCpp/TinEye/data/config.json). Config is automatically copied over when building with CMake. You can also use a different configuration when running the application by using the `-c` option.

- AppSettings configuration for how the tool should function:
	- SaveLuminanceMap: whether to save the luminance map generated when processing the image;
	- SaveTextboxOutline: whether to save the resulting images with the textboxes indicating if each textbox passed the guidelines test.
	- TextboxOutlineColors: Sets of RGB values (0 to 255) for the different results the tool can output.
	- SaveSeparateTextboxes: whether to save separately each of the textboxes recognized as image files.
	- SaveHistograms: whether to generate and save luminance histograms for each textbox recognized.
	- SaveRawTextboxOutline: whether to generate an image with the textboxes as recognized by EAST with no additional processing.
	- SaveLuminanceMasks: whether to save to a file the different masks used for luminance testing.
	- UseTextRecognition: whether to use text recognition. If deactivated execution will be faster but character width won't be measured.
	- UseDPI: Whether to compare measurements to DPI-based guidelines or resolution-based guidelines. 
	- TargetDPI: Only used if UseDPI is set to true. Determine the target DPI of the image to be tested.
	- TargetResolution: Only used if UseDPI is set to false and TargetResolution's value is different to 0. Useful if you want to analyze a cropped image and want to set its source resolution.
	- SaveLogs: Whether to create the txt log file at the end of execution.
	- PrintValuesOnResults: Whether to print the obtained measurements to the side of detected text boxes in the image output to facilitate reviewing.
	- FramesToSkip: When processing video, set how many frames to skip between checks. With high framerates it may not be useful to check all frames as a human takes more than one frame to read text present.
	- VideoImageOutputInterval: Sets the frame interval to wait between video frames to export an image of a failing frame. For example, if processing a 30 fps video and -     VideoImageOutputInterval is set to 30 it'll export one failing frame of the video each second so it can be better looked at by testers. Exported frames can be found in the same    folder as the results and have their frame number in the filename. By default set to 0 (deactivated).
		- Important! If videoImageOutputInterval is set to a value smaller than FramesToSkip you can get exported frames that have repeat and inaccurate values.
	- FocusMask: Start and finish width and height in a range of 0 to 1 of the part of the image that is to be recognized. By default the whole image is processed.
	- IgnoreMask: Regions to be ignored when processing the image. Format is the same as FocusMask.
- TextRecognition configuration for the models used for text recognition:
	- RecognitionModel: Name of the file for a trained neural network to be used for text recognition.
	- DecodeType: Sets the decoding method of translating the network output into string, can be 'CTC-greedy' or 'CTC-prefix-beam-search'.
	- VocabularyFile: Path of the file with the symbols that can be recognized by the program.
	- Scale:  Multiplier for frame values. By default is 1/127.5.
	- Mean: Scalar with mean values which are subtracted from channels. By default it's the same for all three color channels.
- TextDetection configuration for EAST and extra parameters for pruning:
	- DetectionModel: Name of the file for a trained neural network to be used for text detection.
	- Confidence: Minimum confidence that the neural network has to have for text to be considered a textbox.
	- NmsThreshold: Threshold for automatic merge algorithm. Increasing or decreasing this value might result in textboxes being cut off or various similar textboxes stacking on top of each other.
	- DetectionScale: Values given by the OpenCV EAST documentation.
	- DetectionMean:  Values given by the OpenCV EAST documentation.
	- RotationThersholdDegree: How many degrees can a textbox be rotated before being pruned. Useful to prune environmental text that you don't want recognized, since it usually isn't parallel to the screen.
	- MergeThreshold: Percentage that two separate textboxes have to overlap in any direction (horizontal or vertical) before they are merged into one. This is worth changing if the tool is separating some words into two or more when recognizing due to special fonts or effects.
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

## Setup and compilation

TinEye has been created as a CMake project and can be compiled in any C++ compatible system. There are various subprojects, all of them compile their necessary inter-project dependencies as well:
- TinEyeLib: Logical backend for TinEye, has everything needed for analysis.
- TinEyeApp: Standard TinEye executable, connects to TinEyeLib.
- TinEyeApp_MT: CPU multithreaded TinEye executable, it'll run on as many cores as available in the system.
- TinEyeLib.Benchmarks: Multithreading benchmarks for different TinEye operations
- TinEyeLib.Tests: All of the acceptance, integration and unit tests for the TinEye code. Any change to the code must pass or update these tests.

All of the dependencies for TinEye are specified in [vcpk.json](./Backend/CoreCpp/vcpkg.json). vcpkg is a cross-platform package manager for C++ developed by Microsoft. Installation instructions for vcpkg can be found in its [repository](https://github.com/Microsoft/vcpkg).  
If you want CMake to automatically run vcpkg and compile all dependencies for you, you must set your VCPKG root folder as an environment variable called "VCPKG_ROOT".

## Running the tool

When executing TinEye you must input the path to where the media you want to analyze is. You can either specify a folder, in which case every piece of media in the folder and subfolders will be analyzed, or a file, in which case that specific image will be analyzed.

Example of execution command when the terminal is running in TinEye's directory

`>TinEyeApp.exe ./resources/bf2042/chat_window_closed.png`

TinEye would proceed to analyze the file 'chat_window_closed.png' with the configuration you specified beforehand. The filepath can be relative to where you are executing TinEye or absolute (for example an absolute path pointing to your documents would be C:\Users\[YourUserName]\Documents)

### Optional arguments

When running TinEye the following optional arguments can be passed to alter the functionality of the tool:

- `-c`: Specify configuration file. Given a path to a specific configuration file uses that one during this execution. By default TinEye looks for config.json in its own folder.

## LICENSE

Tineye is provided under the BSD 3 Clause License, see [LICENSE](./LICENSE.txt) for details.

Tineye utilizes open source software, see [NOTICE](./NOTICE.txt) licenses and details.

## CONTRIBUTING

Before you can contribute, EA must have a Contributor License Agreement (CLA) on file that has been signed by each contributor. You can sign here: [CLA](https://electronicarts.na1.echosign.com/public/esignWidget?wid=CBFCIBAA3AAABLblqZhByHRvZqmltGtliuExmuV-WNzlaJGPhbSRg2ufuPsM3P0QmILZjLpkGslg24-UJtek*)