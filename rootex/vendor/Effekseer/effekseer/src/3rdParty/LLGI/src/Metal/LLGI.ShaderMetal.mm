#include "LLGI.ShaderMetal.h"
#include "LLGI.GraphicsMetal.h"
#include "LLGI.Metal_Impl.h"
#import <MetalKit/MetalKit.h>

#define SUPPRESS_COMPILE_WARNINGS

namespace LLGI
{

Shader_Impl::Shader_Impl() {}

Shader_Impl::~Shader_Impl()
{
	if (library != nullptr)
	{
		[library release];
		library = nullptr;
	}
}

bool Shader_Impl::Initialize(Graphics_Impl* graphics, const void* data, int size)
{
	auto device = graphics->device;

	if (size < 7)
		return false;

	// check whether binary or code
	bool isCode = false;
	const char* code = static_cast<const char*>(data);

	if (code[0] == 'm' || code[1] == 't' || code[2] == 'l' || code[3] == 'c' || code[4] == 'o' || code[5] == 'd' || code[6] == 'e')
	{
		isCode = true;
	}

	if (isCode)
	{
		code += 7;
		NSString* code_ = [[NSString alloc] initWithUTF8String:code];

		id<MTLDevice> device = MTLCreateSystemDefaultDevice();

		NSError* libraryError = nil;
		id<MTLLibrary> lib = [device newLibraryWithSource:code_ options:NULL error:&libraryError];
		if (libraryError
#ifdef SUPPRESS_COMPILE_WARNINGS
			&& [libraryError.localizedDescription rangeOfString:@"succeeded"].location == NSNotFound
#endif
		)
		{
			Log(LogType::Error, libraryError.localizedDescription.UTF8String);
			return false;
		}

		this->library = lib;
	}
	else
	{
		NSError* libraryError = nil;
		id<MTLLibrary> lib = [device newLibraryWithData:(dispatch_data_t)data error:&libraryError];

		if (libraryError
#ifdef SUPPRESS_COMPILE_WARNINGS
			&& [libraryError.localizedDescription rangeOfString:@"succeeded"].location == NSNotFound
#endif
		)
		{
			Log(LogType::Error, libraryError.localizedDescription.UTF8String);
			return false;
		}

		this->library = lib;
	}

	return true;
}

ShaderMetal::ShaderMetal() { impl = new Shader_Impl(); }

ShaderMetal::~ShaderMetal()
{
	SafeDelete(impl);
	SafeRelease(graphics_);
}

bool ShaderMetal::Initialize(GraphicsMetal* graphics, DataStructure* data, int32_t count)
{
	SafeAddRef(graphics);
	SafeRelease(graphics_);
	graphics_ = graphics;

	return impl->Initialize(graphics_->GetImpl(), data[0].Data, data[0].Size);
}

}
