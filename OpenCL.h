#pragma once
#include <string>

struct _cl_mem;
struct _cl_program;
struct _cl_kernel;

namespace CL {

	void InitializeRuntime();
	void DestroyRuntime();

	void Finish();
	
	class Memory;

	class Program {
		friend class Kernel;
	private:
		_cl_program* program;

		const unsigned char* GetFileText(const std::string& filename, int* outLength) const;
		const unsigned char* GetFileBinary(const std::string& filename, int* outLength) const;
	public:
		Program(const std::string& binaryFile);
		Program(int numFiles, std::string sourceFiles[]);
		~Program();

		void DumpBinary(const std::string& filename) const;

		class Kernel {
		private:
			_cl_kernel* kernel;

			void CL_SetArgument(int argIdx, size_t argSize, void* argAddr);
		public:
			Kernel(const Program& program, const std::string& fName);
			~Kernel();

			void Execute(int dimensions, size_t numThreads[], size_t blockSizeHints[]);

			template <typename T>
			void SetArgument(int argIdx, const T& arg)
			{
				CL_SetArgument(argIdx, sizeof(arg), (void*)&arg);
			}

			void SetArgument(int argIdx, const Memory& arg);
			int GetWorkGroupSize() const;
		};
	};

	class Memory {
		friend class Program::Kernel;
	private:
		_cl_mem* deviceMemory;
		bool openGLResource = false;

		Memory() = default;
	public:
		enum class Access {
			READ,
			WRITE,
			READ_WRITE
		};

		Memory(Access access, int numBytes, void* host = NULL);
		~Memory();

		void WriteToDevice(int numBytes, void* data);
		void ReadFromDevice(int numBytes, void* buffer);

		void AcquireFromOpenGL();
		void ReleaseToOpenGL();

		static Memory CreateFromOpenGLResource(unsigned int texType, unsigned int texName);
	};	
}
