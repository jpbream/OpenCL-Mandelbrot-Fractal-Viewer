#include "OpenCL.h"
#include <CL/opencl.h>
#include <Windows.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <cstdint>

namespace CL {

#define COMP_OPTIONS "-cl-fast-relaxed-math"
#define BUILD_INFO_LOG_SIZE 16000
#define cl_exit() exit(1)

	static void CL_CALLBACK _CL_ErrorCallback(
		const char* errInfo,
		const void* private_info,
		size_t cb,
		void* user_data)
	{
		std::cerr << "OPENCL Error @ Line " << (int)user_data << ": " << errInfo << std::endl;
		cl_exit();
	}

	static void (CL_CALLBACK* CL_ErrorCallback)(
		const char* errInfo,
		const void* private_info,
		size_t cb,
		void* user_data) =
#ifdef _DEBUG
		_CL_ErrorCallback
#else
		NULL
#endif
		;

	static const char* cl_geterror(cl_int result) {
		switch (result) {
			case -1: return "CL_DEVICE_NOT_FOUND";
			case -2: return "CL_DEVICE_NOT_AVAILABLE";
			case -3: return "CL_COMPILER_NOT_AVAILABLE";
			case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
			case -5: return "CL_OUT_OF_RESOURCES";
			case -6: return "CL_OUT_OF_HOST_MEMORY";
			case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
			case -8: return "CL_MEM_COPY_OVERLAP";
			case -9: return "CL_IMAGE_FORMAT_MISMATCH";
			case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
			case -12: return "CL_MAP_FAILURE";
			case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
			case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
			case -15: return "CL_COMPILE_PROGRAM_FAILURE";
			case -16: return "CL_LINKER_NOT_AVAILABLE";
			case -17: return "CL_LINK_PROGRAM_FAILURE";
			case -18: return "CL_DEVICE_PARTITION_FAILED";
			case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
			case -30: return "CL_INVALID_VALUE";
			case -31: return "CL_INVALID_DEVICE_TYPE";
			case -32: return "CL_INVALID_PLATFORM";
			case -33: return "CL_INVALID_DEVICE";
			case -34: return "CL_INVALID_CONTEXT";
			case -35: return "CL_INVALID_QUEUE_PROPERTIES";
			case -36: return "CL_INVALID_COMMAND_QUEUE";
			case -37: return "CL_INVALID_HOST_PTR";
			case -38: return "CL_INVALID_MEM_OBJECT";
			case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
			case -40: return "CL_INVALID_IMAGE_SIZE";
			case -41: return "CL_INVALID_SAMPLER";
			case -42: return "CL_INVALID_BINARY";
			case -43: return "CL_INVALID_BUILD_OPTIONS";
			case -44: return "CL_INVALID_PROGRAM";
			case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
			case -46: return "CL_INVALID_KERNEL_NAME";
			case -47: return "CL_INVALID_KERNEL_DEFINITION";
			case -48: return "CL_INVALID_KERNEL";
			case -49: return "CL_INVALID_ARG_INDEX";
			case -50: return "CL_INVALID_ARG_VALUE";
			case -51: return "CL_INVALID_ARG_SIZE";
			case -52: return "CL_INVALID_KERNEL_ARGS";
			case -53: return "CL_INVALID_WORK_DIMENSION";
			case -54: return "CL_INVALID_WORK_GROUP_SIZE";
			case -55: return "CL_INVALID_WORK_ITEM_SIZE";
			case -56: return "CL_INVALID_GLOBAL_OFFSET";
			case -57: return "CL_INVALID_EVENT_WAIT_LIST";
			case -58: return "CL_INVALID_EVENT";
			case -59: return "CL_INVALID_OPERATION";
			case -60: return "CL_INVALID_GL_OBJECT";
			case -61: return "CL_INVALID_BUFFER_SIZE";
			case -62: return "CL_INVALID_MIP_LEVEL";
			case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
			case -64: return "CL_INVALID_PROPERTY";
			case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
			case -66: return "CL_INVALID_COMPILER_OPTIONS";
			case -67: return "CL_INVALID_LINKER_OPTIONS";
			case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";
			case -69: return "CL_INVALID_PIPE_SIZE";
			case -70: return "CL_INVALID_DEVICE_QUEUE";
			case -71: return "CL_INVALID_SPEC_ID";
			case -72: return "CL_MAX_SIZE_RESTRICTION_EXCEEDED";
			case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
			case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
			case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
			case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
			case -1006: return "CL_INVALID_D3D11_DEVICE_KHR";
			case -1007: return "CL_INVALID_D3D11_RESOURCE_KHR";
			case -1008: return "CL_D3D11_RESOURCE_ALREADY_ACQUIRED_KHR";
			case -1009: return "CL_D3D11_RESOURCE_NOT_ACQUIRED_KHR";
			case -1010: return "CL_INVALID_DX9_MEDIA_ADAPTER_KHR";
			case -1011: return "CL_INVALID_DX9_MEDIA_SURFACE_KHR";
			case -1012: return "CL_DX9_MEDIA_SURFACE_ALREADY_ACQUIRED_KHR";
			case -1013: return "CL_DX9_MEDIA_SURFACE_NOT_ACQUIRED_KHR";
			case -1093: return "CL_INVALID_EGL_OBJECT_KHR";
			case -1092: return "CL_EGL_RESOURCE_NOT_ACQUIRED_KHR";
			case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
			case -1057: return "CL_DEVICE_PARTITION_FAILED_EXT";
			case -1058: return "CL_INVALID_PARTITION_COUNT_EXT";
			case -1059: return "CL_INVALID_PARTITION_NAME_EXT";
			case -1094: return "CL_INVALID_ACCELERATOR_INTEL";
			case -1095: return "CL_INVALID_ACCELERATOR_TYPE_INTEL";
			case -1096: return "CL_INVALID_ACCELERATOR_DESCRIPTOR_INTEL";
			case -1097: return "CL_ACCELERATOR_TYPE_NOT_SUPPORTED_INTEL";
			case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
			case -1098: return "CL_INVALID_VA_API_MEDIA_ADAPTER_INTEL";
			case -1099: return "CL_INVALID_VA_API_MEDIA_SURFACE_INTEL";
			case -1100: return "CL_VA_API_MEDIA_SURFACE_ALREADY_ACQUIRED_INTEL";
			case -1101: return "CL_VA_API_MEDIA_SURFACE_NOT_ACQUIRED_INTEL";
			default: return "CL_UNKNOWN_ERROR";
		}
	}

#ifdef _DEBUG
#define cl_assert(R) if (R != CL_SUCCESS) { std::cerr << "OPENCL Error @ Line " \
	<< __LINE__ << ": " << cl_geterror(R) << std::endl;  cl_exit(); }
#else
#define cl_assert(R)
#endif

	class Runtime {
	public:
		static constexpr int MAX_PLATFORMS = 10;
		cl_platform_id platforms[MAX_PLATFORMS];
		cl_uint numPlatforms;

		static constexpr int MAX_DEVICES = 10;
		cl_device_id devices[MAX_DEVICES];
		cl_uint numDevices;

		cl_command_queue commandQueue;

		cl_context context;
		size_t maxWorkGroupSize;
	
		Runtime()
			: numDevices(0), numPlatforms(0)
		{
			static int result;

			// create the platforms
			result = clGetPlatformIDs(
				MAX_PLATFORMS, 
				platforms, 
				&numPlatforms
			);
			cl_assert(result);
			assert(numPlatforms > 0);

			// create the devices for platform 0
			result = clGetDeviceIDs(
				platforms[0], 
				CL_DEVICE_TYPE_DEFAULT, 
				MAX_DEVICES, 
				devices, 
				&numDevices
			);
			cl_assert(result);
			assert(numDevices > 0);

			HGLRC hGLRC = wglGetCurrentContext();
			HDC hDC = wglGetCurrentDC();

			// properties for opengl compatible context
			cl_context_properties properties[] = {
				CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[0],
				CL_GL_CONTEXT_KHR, (cl_context_properties)hGLRC,
				CL_WGL_HDC_KHR, (cl_context_properties)hDC,
				0
			};

			// create the context
			context = clCreateContext(
				properties, 
				numDevices, 
				devices, 
				CL_ErrorCallback, 
				(void*)__LINE__,
				&result
			);
			cl_assert(result);

			clGetGLContextInfoKHR_fn pclGetContextInfoKHR = (clGetGLContextInfoKHR_fn)
				clGetExtensionFunctionAddressForPlatform(platforms[0], "clGetGLContextInfoKHR");
			assert(pclGetContextInfoKHR != NULL);

			// get opengl compatible device
			cl_device_id openglDevice = 0;
			result = pclGetContextInfoKHR(
				properties,
				CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR,
				sizeof(cl_uint),
				&openglDevice,
				NULL
			);
			cl_assert(result);

			cl_command_queue_properties cmdQueProps = 0;
#ifdef _DEBUG
			cmdQueProps |= CL_QUEUE_PROFILING_ENABLE;
#endif

			// create the command queue
			commandQueue = clCreateCommandQueue(
				context,
				openglDevice,
				cmdQueProps,
				&result
			);
			cl_assert(result);

			// return the max work group size for this device
			result = clGetDeviceInfo(
				openglDevice,
				CL_DEVICE_MAX_WORK_GROUP_SIZE,
				sizeof(size_t),
				&maxWorkGroupSize,
				NULL
			);
			cl_assert(result);
		}
		~Runtime()
		{
			clReleaseContext(context);
			clReleaseCommandQueue(commandQueue);
			
			for (int i = 0; i < numDevices; ++i)
				clReleaseDevice(devices[i]);
		}
	};

	static Runtime* g_Runtime = NULL;

	static void CL_CALLBACK _CL_BuildCallback(
		cl_program program,
		void* user_data)
	{
		int result;

		int status = -1;
		result = clGetProgramBuildInfo(
			program,
			g_Runtime->devices[0],
			CL_PROGRAM_BUILD_STATUS,
			sizeof(int), &status, NULL
		);
		cl_assert(result);

		if (status == CL_BUILD_SUCCESS)
			return;

		static char errorLog[BUILD_INFO_LOG_SIZE];
		result = clGetProgramBuildInfo(
			program,
			g_Runtime->devices[0],
			CL_PROGRAM_BUILD_LOG,
			BUILD_INFO_LOG_SIZE,
			errorLog,
			NULL
		);
		cl_assert(result);

		const char* filename = (const char*)user_data;

		std::cerr << "OPENCL Build Error (" << filename << "): " << errorLog << std::endl;
		cl_exit();
	}
	static void (CL_CALLBACK* CL_BuildCallback)(
		cl_program program,
		void* user_data) =
#ifdef _DEBUG
		_CL_BuildCallback
#else
		NULL
#endif
		;

	void InitializeRuntime()
	{
		g_Runtime = new Runtime();
	}
	void DestroyRuntime()
	{
		delete g_Runtime;
		g_Runtime = NULL;
	}

	void Finish()
	{
		clFinish(g_Runtime->commandQueue);
	}
	
	Memory::Memory(Access access, int numBytes, void* host)
	{
		static int result;

		cl_mem_flags flags = 0;
		switch (access) {
		case Access::READ:
			flags = CL_MEM_READ_ONLY;
			break;
		case Access::WRITE:
			flags = CL_MEM_WRITE_ONLY;
			break;
		case Access::READ_WRITE:
			flags = CL_MEM_READ_WRITE;
			break;
		}

		if (host != NULL)
			flags |= CL_MEM_COPY_HOST_PTR;

		deviceMemory = clCreateBuffer(
			g_Runtime->context,
			flags,
			numBytes,
			host,
			&result
		);
		cl_assert(result);
	}
	Memory::~Memory()
	{
		if (!openGLResource)
			clReleaseMemObject(deviceMemory);
	}
	void Memory::WriteToDevice(int numBytes, void* data)
	{
		int result = clEnqueueWriteBuffer(
			g_Runtime->commandQueue,
			deviceMemory,
			CL_TRUE,
			0,
			numBytes,
			data,
			0, NULL, NULL // events
		);
		cl_assert(result);
	}
	void Memory::ReadFromDevice(int numBytes, void* buffer)
	{
		int result = clEnqueueReadBuffer(
			g_Runtime->commandQueue,
			deviceMemory,
			CL_TRUE,
			0,
			numBytes,
			buffer,
			0, NULL, NULL // events
		);
		cl_assert(result);
	}
	Memory Memory::CreateFromOpenGLResource(unsigned int texType, unsigned int texName)
	{
		static int result;

		Memory m;
		m.openGLResource = true;
		m.deviceMemory = clCreateFromGLTexture(
			g_Runtime->context,
			CL_MEM_READ_WRITE,
			texType,
			0,
			texName,
			&result
		);
		cl_assert(result);

		return m;
	}
	void Memory::AcquireFromOpenGL()
	{
		static int result;
		result = clEnqueueAcquireGLObjects(
			g_Runtime->commandQueue,
			1,
			&deviceMemory,
			0, NULL, NULL
		);
		cl_assert(result);
	}
	void Memory::ReleaseToOpenGL()
	{
		static int result;
		result = clEnqueueReleaseGLObjects(
			g_Runtime->commandQueue,
			1,
			&deviceMemory,
			0, NULL, NULL
		);
		cl_assert(result);
	}

	Program::Program(const std::string& binaryFile)
	{
		int length;
		const unsigned char* binary = GetFileBinary(binaryFile, &length);

		size_t* lengths = new size_t[g_Runtime->numDevices];
		const unsigned char** binaries = new const unsigned char* [g_Runtime->numDevices];

		static int result;
		int* results = new int[g_Runtime->numDevices];

		for (int i = 0; i < g_Runtime->numDevices; ++i) {
			lengths[i] = length;
			binaries[i] = binary;
		}

		// expects a different binary for each device,
		// going to just use the same one for each
		program = clCreateProgramWithBinary(
			g_Runtime->context,
			g_Runtime->numDevices,
			g_Runtime->devices,
			lengths,
			binaries,
			results,
			&result
		);

		for (int i = 0; i < g_Runtime->numDevices; ++i)
			cl_assert(results[i]);
		cl_assert(result);

		delete[] binary;
		delete[] lengths;
		delete[] binaries;
		delete[] results;

		int type = -1;
		result = clGetProgramBuildInfo(
			program,
			g_Runtime->devices[0],
			CL_PROGRAM_BINARY_TYPE,
			sizeof(type), &type, NULL
		);
		cl_assert(result);

		if (type == CL_PROGRAM_BINARY_TYPE_COMPILED_OBJECT) {
			cl_program exe = clLinkProgram(
				g_Runtime->context,
				g_Runtime->numDevices,
				g_Runtime->devices,
				"", // link options
				1,
				&program,
				CL_BuildCallback,
				(void*)"Executable",
				&result
			);
			cl_assert(result);

			clReleaseProgram(program);
			program = exe;
		}

#ifdef _DEBUG
		result = clGetProgramBuildInfo(
			program,
			g_Runtime->devices[0],
			CL_PROGRAM_BINARY_TYPE,
			sizeof(type), &type, NULL
		);
		cl_assert(result);
		assert(type == CL_PROGRAM_BINARY_TYPE_EXECUTABLE);
#endif
	}
	Program::Program(int numFiles, std::string sourceFiles[])
	{
		static int result;

		cl_program* programs = new cl_program[numFiles];
		for (int i = 0; i < numFiles; ++i) {
			
			int length;
			const unsigned char* sourceCode = GetFileText(sourceFiles[i], &length);

			programs[i] = clCreateProgramWithSource(
				g_Runtime->context,
				1,
				(const char**)&sourceCode,
				NULL, // if null, sourceCode must be null terminated
				&result
			);
			cl_assert(result);

			result = clCompileProgram(
				programs[i],
				g_Runtime->numDevices,
				g_Runtime->devices,
				COMP_OPTIONS,
				0, NULL, NULL, // header stuff
				CL_BuildCallback,
				CL_BuildCallback != NULL ? (void*)sourceFiles[i].c_str() : NULL // pass in file name as user data
			);
			cl_assert(result);

			delete[] sourceCode;
		}

		program = clLinkProgram(
			g_Runtime->context,
			g_Runtime->numDevices,
			g_Runtime->devices,
			"", // link options
			numFiles,
			programs,
			CL_BuildCallback,
			CL_BuildCallback != NULL ? (void*)"Executable" : NULL,
			&result
		);
		cl_assert(result);

		for (int i = 0; i < numFiles; ++i)
			clReleaseProgram(programs[i]);
	}
	Program::~Program()
	{
		clReleaseProgram(program);
	}
	const unsigned char* Program::GetFileText(const std::string& filename, int* outLength) const
	{
		FILE* file = fopen(filename.c_str(), "r");
		fseek(file, 0, SEEK_END);
		long length = ftell(file);
		fseek(file, 0, SEEK_SET);

		unsigned char* programSrc = new unsigned char[length + 1];

		char curr; int idx = 0;
		while ((curr = fgetc(file)) != EOF) {
			programSrc[idx++] = (unsigned char)curr;
		}
		programSrc[idx] = 0;

		fclose(file);
		*outLength = length;
		return programSrc;
	}
	const unsigned char* Program::GetFileBinary(const std::string& filename, int* outLength) const
	{
		FILE* file = fopen(filename.c_str(), "r");
		fseek(file, 0, SEEK_END);
		long length = ftell(file);
		fseek(file, 0, SEEK_SET);

		unsigned char* programBin = new unsigned char[length + 1];

		fread(programBin, sizeof(char), length, file);
		programBin[length] = 0;

		fclose(file);
		*outLength = length;
		return programBin;
	}
	void Program::DumpBinary(const std::string& filename) const
	{
		static int result;

		std::ofstream out(filename);
		
#ifdef _DEBUG
		int type = -1;
		result = clGetProgramBuildInfo(
			program,
			g_Runtime->devices[0],
			CL_PROGRAM_BINARY_TYPE,
			sizeof(type), &type, NULL
		);
		cl_assert(result);
		assert(type == CL_PROGRAM_BINARY_TYPE_EXECUTABLE
		|| type == CL_PROGRAM_BINARY_TYPE_COMPILED_OBJECT);
#endif

		size_t size;
		result = clGetProgramInfo(
			program, 
			CL_PROGRAM_BINARY_SIZES, 
			sizeof(size_t), &size, NULL
		);
		cl_assert(result);

		unsigned char* binaries = new unsigned char[size];

		result = clGetProgramInfo(
			program, 
			CL_PROGRAM_BINARIES, 
			size, &binaries, NULL
		);
		cl_assert(result);

		out << binaries;
		out.close();
		delete[] binaries;
	}

	Program::Kernel::Kernel(const Program& program, const std::string& fName)
	{
		static int result;

		kernel = clCreateKernel(
			program.program,
			fName.c_str(),
			&result
		);

		cl_assert(result);
	}
	Program::Kernel::~Kernel()
	{
		clReleaseKernel(kernel);
	}
	void Program::Kernel::SetArgument(int argIdx, const Memory& arg)
	{
		CL_SetArgument(argIdx, sizeof(arg.deviceMemory), (void*)&arg.deviceMemory);
	}
	void Program::Kernel::CL_SetArgument(int argIdx, size_t argSize, void* argAddr)
	{
		clSetKernelArg(
			kernel,
			argIdx,
			argSize,
			argAddr
		);
	}
	void Program::Kernel::Execute(int dimensions, size_t numThreads[], size_t blockSizeHints[])
	{
		static int result;

#ifdef _DEBUG
		static int totalFrames = 0;

		cl_event eventTime;
		uint64_t start, end;

		result = clEnqueueNDRangeKernel(
			g_Runtime->commandQueue,
			kernel,
			dimensions,
			0,
			numThreads,
			blockSizeHints,
			0, NULL,
			&eventTime
		);
		cl_assert(result);

		result = clFinish(g_Runtime->commandQueue);
		cl_assert(result);

		result = clGetEventProfilingInfo(
			eventTime,
			CL_PROFILING_COMMAND_START,
			sizeof(start), &start,
			NULL
		);
		cl_assert(result);

		result = clGetEventProfilingInfo(
			eventTime,
			CL_PROFILING_COMMAND_END,
			sizeof(end), &end,
			NULL
		);
		cl_assert(result);

		double elapsed = (end - start) / 1000000.0;
		totalFrames++;

		if (totalFrames >= 250 || elapsed > 50) {
			printf("Sample Kernel Execution Time: %.2lf ms\n", elapsed);
			totalFrames = 0;
		}
#else
		result = clEnqueueNDRangeKernel(
			g_Runtime->commandQueue,
			kernel,
			dimensions,
			0,
			numThreads,
			blockSizeHints,
			0, NULL,
			NULL
		);
#endif	
	}
	int Program::Kernel::GetWorkGroupSize() const
	{
		static int result;
		size_t size;

		result = clGetKernelWorkGroupInfo(
			kernel,
			g_Runtime->devices[0],
			CL_KERNEL_WORK_GROUP_SIZE,
			sizeof(size),
			&size,
			NULL
		);
		cl_assert(result);

		return size;
	}
}