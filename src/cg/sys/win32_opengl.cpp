#include "cg/sys/app.h"

#include <cassert>
#include <memory>
#include <windows.h>
#include "cg/base/base.h"
#include "cg/opengl/opengl.h"


namespace {

using cg::enforce;
using cg::sys::IRender_context;

extern "C" using func_t = void(*);

inline func_t load_dll_func(HMODULE dll, const char* func_name)
{
	assert(dll);
	assert(func_name);

	func_t f = static_cast<func_t>(GetProcAddress(dll, func_name));
	cg::enforce(f, func_name);

	return f;
}

inline func_t load_opengl_func(const char* func_name)
{
	assert(func_name);

	func_t f = static_cast<func_t>(wglGetProcAddress(func_name));
	cg::enforce(f, func_name);

	return f;
}


class Opengl_render_context final : public IRender_context {
public:

	Opengl_render_context(HWND hwnd);

	~Opengl_render_context() noexcept override;

	void swap_color_buffers() noexcept override;

private:

	void load_opengl_11() const;

	void load_opengl_45() const;

	// Opengl_render_context does not own and is not responsible for _hwnd object.
	HWND _hwnd = nullptr;
	HDC _hdc = nullptr;
	HGLRC _hglrc = nullptr;
	HMODULE _opengl_dll = nullptr;
	int _version_major = 0;
	int _version_minor = 0;
};

Opengl_render_context::Opengl_render_context(HWND hwnd)
	: _hwnd(hwnd)
{
	assert(_hwnd);

	_hdc = GetDC(_hwnd);
	assert(_hdc);

	PIXELFORMATDESCRIPTOR pixel_fmt_desc = {};
	pixel_fmt_desc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixel_fmt_desc.nVersion = 1;
	pixel_fmt_desc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixel_fmt_desc.iPixelType = PFD_TYPE_RGBA;
	pixel_fmt_desc.cColorBits = 32;
	pixel_fmt_desc.cDepthBits = 32;
	pixel_fmt_desc.iLayerType = PFD_MAIN_PLANE;

	int pixel_format = ChoosePixelFormat(_hdc, &pixel_fmt_desc);
	assert(pixel_format > 0);
	SetPixelFormat(_hdc, pixel_format, &pixel_fmt_desc);

	_hglrc = wglCreateContext(_hdc);
	assert(_hglrc);
	wglMakeCurrent(_hdc, _hglrc);

	// load opengl funcs
	_opengl_dll = LoadLibrary("opengl32.dll");
	load_opengl_11();

	glGetIntegerv(GL_MAJOR_VERSION, &_version_major);
	glGetIntegerv(GL_MINOR_VERSION, &_version_minor);
	enforce(_version_major == 4 && _version_minor == 5, "Opengl version must be 4.5 or greater.");
	load_opengl_45();
}

Opengl_render_context::~Opengl_render_context() noexcept
{
	if (_opengl_dll) {
		FreeLibrary(_opengl_dll);
		_opengl_dll = nullptr;
	}

	if (_hglrc) {
		wglMakeCurrent(_hdc, nullptr);
		wglDeleteContext(_hglrc);
		_hglrc = nullptr;
	}

	if (_hdc) {
		ReleaseDC(_hwnd, _hdc);
		_hdc = nullptr;
	}
}

void Opengl_render_context::load_opengl_11() const
{
	// opengl 1.0
	glCullFace = static_cast<PFNGLCULLFACEPROC>(load_dll_func(_opengl_dll, "glCullFace"));
	glFrontFace = static_cast<PFNGLFRONTFACEPROC>(load_dll_func(_opengl_dll, "glFrontFace"));
	glHint = static_cast<PFNGLHINTPROC>(load_dll_func(_opengl_dll, "glHint"));
	glLineWidth = static_cast<PFNGLLINEWIDTHPROC>(load_dll_func(_opengl_dll, "glLineWidth"));
	glPointSize = static_cast<PFNGLPOINTSIZEPROC>(load_dll_func(_opengl_dll, "glPointSize"));
	glPolygonMode = static_cast<PFNGLPOLYGONMODEPROC>(load_dll_func(_opengl_dll, "glPolygonMode"));
	glScissor = static_cast<PFNGLSCISSORPROC>(load_dll_func(_opengl_dll, "glScissor"));
	glTexParameterf = static_cast<PFNGLTEXPARAMETERFPROC>(load_dll_func(_opengl_dll, "glTexParameterf"));
	glTexParameterfv = static_cast<PFNGLTEXPARAMETERFVPROC>(load_dll_func(_opengl_dll, "glTexParameterfv"));
	glTexParameteri = static_cast<PFNGLTEXPARAMETERIPROC>(load_dll_func(_opengl_dll, "glTexParameteri"));
	glTexParameteriv = static_cast<PFNGLTEXPARAMETERIVPROC>(load_dll_func(_opengl_dll, "glTexParameteriv"));
	glTexImage1D = static_cast<PFNGLTEXIMAGE1DPROC>(load_dll_func(_opengl_dll, "glTexImage1D"));
	glTexImage2D = static_cast<PFNGLTEXIMAGE2DPROC>(load_dll_func(_opengl_dll, "glTexImage2D"));
	glDrawBuffer = static_cast<PFNGLDRAWBUFFERPROC>(load_dll_func(_opengl_dll, "glDrawBuffer"));
	glClear = static_cast<PFNGLCLEARPROC>(load_dll_func(_opengl_dll, "glClear"));
	glClearColor = static_cast<PFNGLCLEARCOLORPROC>(load_dll_func(_opengl_dll, "glClearColor"));
	glClearStencil = static_cast<PFNGLCLEARSTENCILPROC>(load_dll_func(_opengl_dll, "glClearStencil"));
	glClearDepth = static_cast<PFNGLCLEARDEPTHPROC>(load_dll_func(_opengl_dll, "glClearDepth"));
	glStencilMask = static_cast<PFNGLSTENCILMASKPROC>(load_dll_func(_opengl_dll, "glStencilMask"));
	glColorMask = static_cast<PFNGLCOLORMASKPROC>(load_dll_func(_opengl_dll, "glColorMask"));
	glDepthMask = static_cast<PFNGLDEPTHMASKPROC>(load_dll_func(_opengl_dll, "glDepthMask"));
	glDisable = static_cast<PFNGLDISABLEPROC>(load_dll_func(_opengl_dll, "glDisable"));
	glEnable = static_cast<PFNGLENABLEPROC>(load_dll_func(_opengl_dll, "glEnable"));
	glFinish = static_cast<PFNGLFINISHPROC>(load_dll_func(_opengl_dll, "glFinish"));
	glFlush = static_cast<PFNGLFLUSHPROC>(load_dll_func(_opengl_dll, "glFlush"));
	glBlendFunc = static_cast<PFNGLBLENDFUNCPROC>(load_dll_func(_opengl_dll, "glBlendFunc"));
	glLogicOp = static_cast<PFNGLLOGICOPPROC>(load_dll_func(_opengl_dll, "glLogicOp"));
	glStencilFunc = static_cast<PFNGLSTENCILFUNCPROC>(load_dll_func(_opengl_dll, "glStencilFunc"));
	glStencilOp = static_cast<PFNGLSTENCILOPPROC>(load_dll_func(_opengl_dll, "glStencilOp"));
	glDepthFunc = static_cast<PFNGLDEPTHFUNCPROC>(load_dll_func(_opengl_dll, "glDepthFunc"));
	glPixelStoref = static_cast<PFNGLPIXELSTOREFPROC>(load_dll_func(_opengl_dll, "glPixelStoref"));
	glPixelStorei = static_cast<PFNGLPIXELSTOREIPROC>(load_dll_func(_opengl_dll, "glPixelStorei"));
	glReadBuffer = static_cast<PFNGLREADBUFFERPROC>(load_dll_func(_opengl_dll, "glReadBuffer"));
	glReadPixels = static_cast<PFNGLREADPIXELSPROC>(load_dll_func(_opengl_dll, "glReadPixels"));
	glGetBooleanv = static_cast<PFNGLGETBOOLEANVPROC>(load_dll_func(_opengl_dll, "glGetBooleanv"));
	glGetDoublev = static_cast<PFNGLGETDOUBLEVPROC>(load_dll_func(_opengl_dll, "glGetDoublev"));
	glGetError = static_cast<PFNGLGETERRORPROC>(load_dll_func(_opengl_dll, "glGetError"));
	glGetFloatv = static_cast<PFNGLGETFLOATVPROC>(load_dll_func(_opengl_dll, "glGetFloatv"));
	glGetIntegerv = static_cast<PFNGLGETINTEGERVPROC>(load_dll_func(_opengl_dll, "glGetIntegerv"));
	glGetString = static_cast<PFNGLGETSTRINGPROC>(load_dll_func(_opengl_dll, "glGetString"));
	glGetTexImage = static_cast<PFNGLGETTEXIMAGEPROC>(load_dll_func(_opengl_dll, "glGetTexImage"));
	glGetTexParameterfv = static_cast<PFNGLGETTEXPARAMETERFVPROC>(load_dll_func(_opengl_dll, "glGetTexParameterfv"));
	glGetTexParameteriv = static_cast<PFNGLGETTEXPARAMETERIVPROC>(load_dll_func(_opengl_dll, "glGetTexParameteriv"));
	glGetTexLevelParameterfv = static_cast<PFNGLGETTEXLEVELPARAMETERFVPROC>(load_dll_func(_opengl_dll, "glGetTexLevelParameterfv"));
	glGetTexLevelParameteriv = static_cast<PFNGLGETTEXLEVELPARAMETERIVPROC>(load_dll_func(_opengl_dll, "glGetTexLevelParameteriv"));
	glIsEnabled = static_cast<PFNGLISENABLEDPROC>(load_dll_func(_opengl_dll, "glIsEnabled"));
	glDepthRange = static_cast<PFNGLDEPTHRANGEPROC>(load_dll_func(_opengl_dll, "glDepthRange"));
	glViewport = static_cast<PFNGLVIEWPORTPROC>(load_dll_func(_opengl_dll, "glViewport"));
	// opengl 1.1
	glDrawArrays = static_cast<PFNGLDRAWARRAYSPROC>(load_dll_func(_opengl_dll, "glDrawArrays"));
	glDrawElements = static_cast<PFNGLDRAWELEMENTSPROC>(load_dll_func(_opengl_dll, "glDrawElements"));
	glGetPointerv = static_cast<PFNGLGETPOINTERVPROC>(load_dll_func(_opengl_dll, "glGetPointerv"));
	glPolygonOffset = static_cast<PFNGLPOLYGONOFFSETPROC>(load_dll_func(_opengl_dll, "glPolygonOffset"));
	glCopyTexImage1D = static_cast<PFNGLCOPYTEXIMAGE1DPROC>(load_dll_func(_opengl_dll, "glCopyTexImage1D"));
	glCopyTexImage2D = static_cast<PFNGLCOPYTEXIMAGE2DPROC>(load_dll_func(_opengl_dll, "glCopyTexImage2D"));
	glCopyTexSubImage1D = static_cast<PFNGLCOPYTEXSUBIMAGE1DPROC>(load_dll_func(_opengl_dll, "glCopyTexSubImage1D"));
	glCopyTexSubImage2D = static_cast<PFNGLCOPYTEXSUBIMAGE2DPROC>(load_dll_func(_opengl_dll, "glCopyTexSubImage2D"));
	glTexSubImage1D = static_cast<PFNGLTEXSUBIMAGE1DPROC>(load_dll_func(_opengl_dll, "glTexSubImage1D"));
	glTexSubImage2D = static_cast<PFNGLTEXSUBIMAGE2DPROC>(load_dll_func(_opengl_dll, "glTexSubImage2D"));
	glBindTexture = static_cast<PFNGLBINDTEXTUREPROC>(load_dll_func(_opengl_dll, "glBindTexture"));
	glDeleteTextures = static_cast<PFNGLDELETETEXTURESPROC>(load_dll_func(_opengl_dll, "glDeleteTextures"));
	glGenTextures = static_cast<PFNGLGENTEXTURESPROC>(load_dll_func(_opengl_dll, "glGenTextures"));
	glIsTexture = static_cast<PFNGLISTEXTUREPROC>(load_dll_func(_opengl_dll, "glIsTexture"));
}

void Opengl_render_context::load_opengl_45() const
{
	// opengl 1.2
	glDrawRangeElements = static_cast<PFNGLDRAWRANGEELEMENTSPROC>(load_opengl_func("glDrawRangeElements"));
	glTexImage3D = static_cast<PFNGLTEXIMAGE3DPROC>(load_opengl_func("glTexImage3D"));
	glTexSubImage3D = static_cast<PFNGLTEXSUBIMAGE3DPROC>(load_opengl_func("glTexSubImage3D"));
	glCopyTexSubImage3D = static_cast<PFNGLCOPYTEXSUBIMAGE3DPROC>(load_opengl_func("glCopyTexSubImage3D"));
	// opengl 1.3
	glActiveTexture = static_cast<PFNGLACTIVETEXTUREPROC>(load_opengl_func("glActiveTexture"));
	glSampleCoverage = static_cast<PFNGLSAMPLECOVERAGEPROC>(load_opengl_func("glSampleCoverage"));
	glCompressedTexImage3D = static_cast<PFNGLCOMPRESSEDTEXIMAGE3DPROC>(load_opengl_func("glCompressedTexImage3D"));
	glCompressedTexImage2D = static_cast<PFNGLCOMPRESSEDTEXIMAGE2DPROC>(load_opengl_func("glCompressedTexImage2D"));
	glCompressedTexImage1D = static_cast<PFNGLCOMPRESSEDTEXIMAGE1DPROC>(load_opengl_func("glCompressedTexImage1D"));
	glCompressedTexSubImage3D = static_cast<PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC>(load_opengl_func("glCompressedTexSubImage3D"));
	glCompressedTexSubImage2D = static_cast<PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC>(load_opengl_func("glCompressedTexSubImage2D"));
	glCompressedTexSubImage1D = static_cast<PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC>(load_opengl_func("glCompressedTexSubImage1D"));
	glGetCompressedTexImage = static_cast<PFNGLGETCOMPRESSEDTEXIMAGEPROC>(load_opengl_func("glGetCompressedTexImage"));
	// opengl 1.4
	glBlendFuncSeparate = static_cast<PFNGLBLENDFUNCSEPARATEPROC>(load_opengl_func("glBlendFuncSeparate"));
	glMultiDrawArrays = static_cast<PFNGLMULTIDRAWARRAYSPROC>(load_opengl_func("glMultiDrawArrays"));
	glMultiDrawElements = static_cast<PFNGLMULTIDRAWELEMENTSPROC>(load_opengl_func("glMultiDrawElements"));
	glPointParameterf = static_cast<PFNGLPOINTPARAMETERFPROC>(load_opengl_func("glPointParameterf"));
	glPointParameterfv = static_cast<PFNGLPOINTPARAMETERFVPROC>(load_opengl_func("glPointParameterfv"));
	glPointParameteri = static_cast<PFNGLPOINTPARAMETERIPROC>(load_opengl_func("glPointParameteri"));
	glPointParameteriv = static_cast<PFNGLPOINTPARAMETERIVPROC>(load_opengl_func("glPointParameteriv"));
	glBlendColor = static_cast<PFNGLBLENDCOLORPROC>(load_opengl_func("glBlendColor"));
	glBlendEquation = static_cast<PFNGLBLENDEQUATIONPROC>(load_opengl_func("glBlendEquation"));
	// opengl 1.5
	glGenQueries = static_cast<PFNGLGENQUERIESPROC>(load_opengl_func("glGenQueries"));
	glDeleteQueries = static_cast<PFNGLDELETEQUERIESPROC>(load_opengl_func("glDeleteQueries"));
	glIsQuery = static_cast<PFNGLISQUERYPROC>(load_opengl_func("glIsQuery"));
	glBeginQuery = static_cast<PFNGLBEGINQUERYPROC>(load_opengl_func("glBeginQuery"));
	glEndQuery = static_cast<PFNGLENDQUERYPROC>(load_opengl_func("glEndQuery"));
	glGetQueryiv = static_cast<PFNGLGETQUERYIVPROC>(load_opengl_func("glGetQueryiv"));
	glGetQueryObjectiv = static_cast<PFNGLGETQUERYOBJECTIVPROC>(load_opengl_func("glGetQueryObjectiv"));
	glGetQueryObjectuiv = static_cast<PFNGLGETQUERYOBJECTUIVPROC>(load_opengl_func("glGetQueryObjectuiv"));
	glBindBuffer = static_cast<PFNGLBINDBUFFERPROC>(load_opengl_func("glBindBuffer"));
	glDeleteBuffers = static_cast<PFNGLDELETEBUFFERSPROC>(load_opengl_func("glDeleteBuffers"));
	glGenBuffers = static_cast<PFNGLGENBUFFERSPROC>(load_opengl_func("glGenBuffers"));
	glIsBuffer = static_cast<PFNGLISBUFFERPROC>(load_opengl_func("glIsBuffer"));
	glBufferData = static_cast<PFNGLBUFFERDATAPROC>(load_opengl_func("glBufferData"));
	glBufferSubData = static_cast<PFNGLBUFFERSUBDATAPROC>(load_opengl_func("glBufferSubData"));
	glGetBufferSubData = static_cast<PFNGLGETBUFFERSUBDATAPROC>(load_opengl_func("glGetBufferSubData"));
	glMapBuffer = static_cast<PFNGLMAPBUFFERPROC>(load_opengl_func("glMapBuffer"));
	glUnmapBuffer = static_cast<PFNGLUNMAPBUFFERPROC>(load_opengl_func("glUnmapBuffer"));
	glGetBufferParameteriv = static_cast<PFNGLGETBUFFERPARAMETERIVPROC>(load_opengl_func("glGetBufferParameteriv"));
	glGetBufferPointerv = static_cast<PFNGLGETBUFFERPOINTERVPROC>(load_opengl_func("glGetBufferPointerv"));
	// opengl 2.0
	glBlendEquationSeparate = static_cast<PFNGLBLENDEQUATIONSEPARATEPROC>(load_opengl_func("glBlendEquationSeparate"));
	glDrawBuffers = static_cast<PFNGLDRAWBUFFERSPROC>(load_opengl_func("glDrawBuffers"));
	glStencilOpSeparate = static_cast<PFNGLSTENCILOPSEPARATEPROC>(load_opengl_func("glStencilOpSeparate"));
	glStencilFuncSeparate = static_cast<PFNGLSTENCILFUNCSEPARATEPROC>(load_opengl_func("glStencilFuncSeparate"));
	glStencilMaskSeparate = static_cast<PFNGLSTENCILMASKSEPARATEPROC>(load_opengl_func("glStencilMaskSeparate"));
	glAttachShader = static_cast<PFNGLATTACHSHADERPROC>(load_opengl_func("glAttachShader"));
	glBindAttribLocation = static_cast<PFNGLBINDATTRIBLOCATIONPROC>(load_opengl_func("glBindAttribLocation"));
	glCompileShader = static_cast<PFNGLCOMPILESHADERPROC>(load_opengl_func("glCompileShader"));
	glCreateProgram = static_cast<PFNGLCREATEPROGRAMPROC>(load_opengl_func("glCreateProgram"));
	glCreateShader = static_cast<PFNGLCREATESHADERPROC>(load_opengl_func("glCreateShader"));
	glDeleteProgram = static_cast<PFNGLDELETEPROGRAMPROC>(load_opengl_func("glDeleteProgram"));
	glDeleteShader = static_cast<PFNGLDELETESHADERPROC>(load_opengl_func("glDeleteShader"));
	glDetachShader = static_cast<PFNGLDETACHSHADERPROC>(load_opengl_func("glDetachShader"));
	glDisableVertexAttribArray = static_cast<PFNGLDISABLEVERTEXATTRIBARRAYPROC>(load_opengl_func("glDisableVertexAttribArray"));
	glEnableVertexAttribArray = static_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(load_opengl_func("glEnableVertexAttribArray"));
	glGetActiveAttrib = static_cast<PFNGLGETACTIVEATTRIBPROC>(load_opengl_func("glGetActiveAttrib"));
	glGetActiveUniform = static_cast<PFNGLGETACTIVEUNIFORMPROC>(load_opengl_func("glGetActiveUniform"));
	glGetAttachedShaders = static_cast<PFNGLGETATTACHEDSHADERSPROC>(load_opengl_func("glGetAttachedShaders"));
	glGetAttribLocation = static_cast<PFNGLGETATTRIBLOCATIONPROC>(load_opengl_func("glGetAttribLocation"));
	glGetProgramiv = static_cast<PFNGLGETPROGRAMIVPROC>(load_opengl_func("glGetProgramiv"));
	glGetProgramInfoLog = static_cast<PFNGLGETPROGRAMINFOLOGPROC>(load_opengl_func("glGetProgramInfoLog"));
	glGetShaderiv = static_cast<PFNGLGETSHADERIVPROC>(load_opengl_func("glGetShaderiv"));
	glGetShaderInfoLog = static_cast<PFNGLGETSHADERINFOLOGPROC>(load_opengl_func("glGetShaderInfoLog"));
	glGetShaderSource = static_cast<PFNGLGETSHADERSOURCEPROC>(load_opengl_func("glGetShaderSource"));
	glGetUniformLocation = static_cast<PFNGLGETUNIFORMLOCATIONPROC>(load_opengl_func("glGetUniformLocation"));
	glGetUniformfv = static_cast<PFNGLGETUNIFORMFVPROC>(load_opengl_func("glGetUniformfv"));
	glGetUniformiv = static_cast<PFNGLGETUNIFORMIVPROC>(load_opengl_func("glGetUniformiv"));
	glGetVertexAttribdv = static_cast<PFNGLGETVERTEXATTRIBDVPROC>(load_opengl_func("glGetVertexAttribdv"));
	glGetVertexAttribfv = static_cast<PFNGLGETVERTEXATTRIBFVPROC>(load_opengl_func("glGetVertexAttribfv"));
	glGetVertexAttribiv = static_cast<PFNGLGETVERTEXATTRIBIVPROC>(load_opengl_func("glGetVertexAttribiv"));
	glGetVertexAttribPointerv = static_cast<PFNGLGETVERTEXATTRIBPOINTERVPROC>(load_opengl_func("glGetVertexAttribPointerv"));
	glIsProgram = static_cast<PFNGLISPROGRAMPROC>(load_opengl_func("glIsProgram"));
	glIsShader = static_cast<PFNGLISSHADERPROC>(load_opengl_func("glIsShader"));
	glLinkProgram = static_cast<PFNGLLINKPROGRAMPROC>(load_opengl_func("glLinkProgram"));
	glShaderSource = static_cast<PFNGLSHADERSOURCEPROC>(load_opengl_func("glShaderSource"));
	glUseProgram = static_cast<PFNGLUSEPROGRAMPROC>(load_opengl_func("glUseProgram"));
	glUniform1f = static_cast<PFNGLUNIFORM1FPROC>(load_opengl_func("glUniform1f"));
	glUniform2f = static_cast<PFNGLUNIFORM2FPROC>(load_opengl_func("glUniform2f"));
	glUniform3f = static_cast<PFNGLUNIFORM3FPROC>(load_opengl_func("glUniform3f"));
	glUniform4f = static_cast<PFNGLUNIFORM4FPROC>(load_opengl_func("glUniform4f"));
	glUniform1i = static_cast<PFNGLUNIFORM1IPROC>(load_opengl_func("glUniform1i"));
	glUniform2i = static_cast<PFNGLUNIFORM2IPROC>(load_opengl_func("glUniform2i"));
	glUniform3i = static_cast<PFNGLUNIFORM3IPROC>(load_opengl_func("glUniform3i"));
	glUniform4i = static_cast<PFNGLUNIFORM4IPROC>(load_opengl_func("glUniform4i"));
	glUniform1fv = static_cast<PFNGLUNIFORM1FVPROC>(load_opengl_func("glUniform1fv"));
	glUniform2fv = static_cast<PFNGLUNIFORM2FVPROC>(load_opengl_func("glUniform2fv"));
	glUniform3fv = static_cast<PFNGLUNIFORM3FVPROC>(load_opengl_func("glUniform3fv"));
	glUniform4fv = static_cast<PFNGLUNIFORM4FVPROC>(load_opengl_func("glUniform4fv"));
	glUniform1iv = static_cast<PFNGLUNIFORM1IVPROC>(load_opengl_func("glUniform1iv"));
	glUniform2iv = static_cast<PFNGLUNIFORM2IVPROC>(load_opengl_func("glUniform2iv"));
	glUniform3iv = static_cast<PFNGLUNIFORM3IVPROC>(load_opengl_func("glUniform3iv"));
	glUniform4iv = static_cast<PFNGLUNIFORM4IVPROC>(load_opengl_func("glUniform4iv"));
	glUniformMatrix2fv = static_cast<PFNGLUNIFORMMATRIX2FVPROC>(load_opengl_func("glUniformMatrix2fv"));
	glUniformMatrix3fv = static_cast<PFNGLUNIFORMMATRIX3FVPROC>(load_opengl_func("glUniformMatrix3fv"));
	glUniformMatrix4fv = static_cast<PFNGLUNIFORMMATRIX4FVPROC>(load_opengl_func("glUniformMatrix4fv"));
	glValidateProgram = static_cast<PFNGLVALIDATEPROGRAMPROC>(load_opengl_func("glValidateProgram"));
	glVertexAttrib1d = static_cast<PFNGLVERTEXATTRIB1DPROC>(load_opengl_func("glVertexAttrib1d"));
	glVertexAttrib1dv = static_cast<PFNGLVERTEXATTRIB1DVPROC>(load_opengl_func("glVertexAttrib1dv"));
	glVertexAttrib1f = static_cast<PFNGLVERTEXATTRIB1FPROC>(load_opengl_func("glVertexAttrib1f"));
	glVertexAttrib1fv = static_cast<PFNGLVERTEXATTRIB1FVPROC>(load_opengl_func("glVertexAttrib1fv"));
	glVertexAttrib1s = static_cast<PFNGLVERTEXATTRIB1SPROC>(load_opengl_func("glVertexAttrib1s"));
	glVertexAttrib1sv = static_cast<PFNGLVERTEXATTRIB1SVPROC>(load_opengl_func("glVertexAttrib1sv"));
	glVertexAttrib2d = static_cast<PFNGLVERTEXATTRIB2DPROC>(load_opengl_func("glVertexAttrib2d"));
	glVertexAttrib2dv = static_cast<PFNGLVERTEXATTRIB2DVPROC>(load_opengl_func("glVertexAttrib2dv"));
	glVertexAttrib2f = static_cast<PFNGLVERTEXATTRIB2FPROC>(load_opengl_func("glVertexAttrib2f"));
	glVertexAttrib2fv = static_cast<PFNGLVERTEXATTRIB2FVPROC>(load_opengl_func("glVertexAttrib2fv"));
	glVertexAttrib2s = static_cast<PFNGLVERTEXATTRIB2SPROC>(load_opengl_func("glVertexAttrib2s"));
	glVertexAttrib2sv = static_cast<PFNGLVERTEXATTRIB2SVPROC>(load_opengl_func("glVertexAttrib2sv"));
	glVertexAttrib3d = static_cast<PFNGLVERTEXATTRIB3DPROC>(load_opengl_func("glVertexAttrib3d"));
	glVertexAttrib3dv = static_cast<PFNGLVERTEXATTRIB3DVPROC>(load_opengl_func("glVertexAttrib3dv"));
	glVertexAttrib3f = static_cast<PFNGLVERTEXATTRIB3FPROC>(load_opengl_func("glVertexAttrib3f"));
	glVertexAttrib3fv = static_cast<PFNGLVERTEXATTRIB3FVPROC>(load_opengl_func("glVertexAttrib3fv"));
	glVertexAttrib3s = static_cast<PFNGLVERTEXATTRIB3SPROC>(load_opengl_func("glVertexAttrib3s"));
	glVertexAttrib3sv = static_cast<PFNGLVERTEXATTRIB3SVPROC>(load_opengl_func("glVertexAttrib3sv"));
	glVertexAttrib4Nbv = static_cast<PFNGLVERTEXATTRIB4NBVPROC>(load_opengl_func("glVertexAttrib4Nbv"));
	glVertexAttrib4Niv = static_cast<PFNGLVERTEXATTRIB4NIVPROC>(load_opengl_func("glVertexAttrib4Niv"));
	glVertexAttrib4Nsv = static_cast<PFNGLVERTEXATTRIB4NSVPROC>(load_opengl_func("glVertexAttrib4Nsv"));
	glVertexAttrib4Nub = static_cast<PFNGLVERTEXATTRIB4NUBPROC>(load_opengl_func("glVertexAttrib4Nub"));
	glVertexAttrib4Nubv = static_cast<PFNGLVERTEXATTRIB4NUBVPROC>(load_opengl_func("glVertexAttrib4Nubv"));
	glVertexAttrib4Nuiv = static_cast<PFNGLVERTEXATTRIB4NUIVPROC>(load_opengl_func("glVertexAttrib4Nuiv"));
	glVertexAttrib4Nusv = static_cast<PFNGLVERTEXATTRIB4NUSVPROC>(load_opengl_func("glVertexAttrib4Nusv"));
	glVertexAttrib4bv = static_cast<PFNGLVERTEXATTRIB4BVPROC>(load_opengl_func("glVertexAttrib4bv"));
	glVertexAttrib4d = static_cast<PFNGLVERTEXATTRIB4DPROC>(load_opengl_func("glVertexAttrib4d"));
	glVertexAttrib4dv = static_cast<PFNGLVERTEXATTRIB4DVPROC>(load_opengl_func("glVertexAttrib4dv"));
	glVertexAttrib4f = static_cast<PFNGLVERTEXATTRIB4FPROC>(load_opengl_func("glVertexAttrib4f"));
	glVertexAttrib4fv = static_cast<PFNGLVERTEXATTRIB4FVPROC>(load_opengl_func("glVertexAttrib4fv"));
	glVertexAttrib4iv = static_cast<PFNGLVERTEXATTRIB4IVPROC>(load_opengl_func("glVertexAttrib4iv"));
	glVertexAttrib4s = static_cast<PFNGLVERTEXATTRIB4SPROC>(load_opengl_func("glVertexAttrib4s"));
	glVertexAttrib4sv = static_cast<PFNGLVERTEXATTRIB4SVPROC>(load_opengl_func("glVertexAttrib4sv"));
	glVertexAttrib4ubv = static_cast<PFNGLVERTEXATTRIB4UBVPROC>(load_opengl_func("glVertexAttrib4ubv"));
	glVertexAttrib4uiv = static_cast<PFNGLVERTEXATTRIB4UIVPROC>(load_opengl_func("glVertexAttrib4uiv"));
	glVertexAttrib4usv = static_cast<PFNGLVERTEXATTRIB4USVPROC>(load_opengl_func("glVertexAttrib4usv"));
	glVertexAttribPointer = static_cast<PFNGLVERTEXATTRIBPOINTERPROC>(load_opengl_func("glVertexAttribPointer"));
	// opengl 2.1
	glUniformMatrix2x3fv = static_cast<PFNGLUNIFORMMATRIX2X3FVPROC>(load_opengl_func("glUniformMatrix2x3fv"));
	glUniformMatrix3x2fv = static_cast<PFNGLUNIFORMMATRIX3X2FVPROC>(load_opengl_func("glUniformMatrix3x2fv"));
	glUniformMatrix2x4fv = static_cast<PFNGLUNIFORMMATRIX2X4FVPROC>(load_opengl_func("glUniformMatrix2x4fv"));
	glUniformMatrix4x2fv = static_cast<PFNGLUNIFORMMATRIX4X2FVPROC>(load_opengl_func("glUniformMatrix4x2fv"));
	glUniformMatrix3x4fv = static_cast<PFNGLUNIFORMMATRIX3X4FVPROC>(load_opengl_func("glUniformMatrix3x4fv"));
	glUniformMatrix4x3fv = static_cast<PFNGLUNIFORMMATRIX4X3FVPROC>(load_opengl_func("glUniformMatrix4x3fv"));
	// opengl 3.0
	glColorMaski = static_cast<PFNGLCOLORMASKIPROC>(load_opengl_func("glColorMaski"));
	glGetBooleani_v = static_cast<PFNGLGETBOOLEANI_VPROC>(load_opengl_func("glGetBooleani_v"));
	glGetIntegeri_v = static_cast<PFNGLGETINTEGERI_VPROC>(load_opengl_func("glGetIntegeri_v"));
	glEnablei = static_cast<PFNGLENABLEIPROC>(load_opengl_func("glEnablei"));
	glDisablei = static_cast<PFNGLDISABLEIPROC>(load_opengl_func("glDisablei"));
	glIsEnabledi = static_cast<PFNGLISENABLEDIPROC>(load_opengl_func("glIsEnabledi"));
	glBeginTransformFeedback = static_cast<PFNGLBEGINTRANSFORMFEEDBACKPROC>(load_opengl_func("glBeginTransformFeedback"));
	glEndTransformFeedback = static_cast<PFNGLENDTRANSFORMFEEDBACKPROC>(load_opengl_func("glEndTransformFeedback"));
	glBindBufferRange = static_cast<PFNGLBINDBUFFERRANGEPROC>(load_opengl_func("glBindBufferRange"));
	glBindBufferBase = static_cast<PFNGLBINDBUFFERBASEPROC>(load_opengl_func("glBindBufferBase"));
	glTransformFeedbackVaryings = static_cast<PFNGLTRANSFORMFEEDBACKVARYINGSPROC>(load_opengl_func("glTransformFeedbackVaryings"));
	glGetTransformFeedbackVarying = static_cast<PFNGLGETTRANSFORMFEEDBACKVARYINGPROC>(load_opengl_func("glGetTransformFeedbackVarying"));
	glClampColor = static_cast<PFNGLCLAMPCOLORPROC>(load_opengl_func("glClampColor"));
	glBeginConditionalRender = static_cast<PFNGLBEGINCONDITIONALRENDERPROC>(load_opengl_func("glBeginConditionalRender"));
	glEndConditionalRender = static_cast<PFNGLENDCONDITIONALRENDERPROC>(load_opengl_func("glEndConditionalRender"));
	glVertexAttribIPointer = static_cast<PFNGLVERTEXATTRIBIPOINTERPROC>(load_opengl_func("glVertexAttribIPointer"));
	glGetVertexAttribIiv = static_cast<PFNGLGETVERTEXATTRIBIIVPROC>(load_opengl_func("glGetVertexAttribIiv"));
	glGetVertexAttribIuiv = static_cast<PFNGLGETVERTEXATTRIBIUIVPROC>(load_opengl_func("glGetVertexAttribIuiv"));
	glVertexAttribI1i = static_cast<PFNGLVERTEXATTRIBI1IPROC>(load_opengl_func("glVertexAttribI1i"));
	glVertexAttribI2i = static_cast<PFNGLVERTEXATTRIBI2IPROC>(load_opengl_func("glVertexAttribI2i"));
	glVertexAttribI3i = static_cast<PFNGLVERTEXATTRIBI3IPROC>(load_opengl_func("glVertexAttribI3i"));
	glVertexAttribI4i = static_cast<PFNGLVERTEXATTRIBI4IPROC>(load_opengl_func("glVertexAttribI4i"));
	glVertexAttribI1ui = static_cast<PFNGLVERTEXATTRIBI1UIPROC>(load_opengl_func("glVertexAttribI1ui"));
	glVertexAttribI2ui = static_cast<PFNGLVERTEXATTRIBI2UIPROC>(load_opengl_func("glVertexAttribI2ui"));
	glVertexAttribI3ui = static_cast<PFNGLVERTEXATTRIBI3UIPROC>(load_opengl_func("glVertexAttribI3ui"));
	glVertexAttribI4ui = static_cast<PFNGLVERTEXATTRIBI4UIPROC>(load_opengl_func("glVertexAttribI4ui"));
	glVertexAttribI1iv = static_cast<PFNGLVERTEXATTRIBI1IVPROC>(load_opengl_func("glVertexAttribI1iv"));
	glVertexAttribI2iv = static_cast<PFNGLVERTEXATTRIBI2IVPROC>(load_opengl_func("glVertexAttribI2iv"));
	glVertexAttribI3iv = static_cast<PFNGLVERTEXATTRIBI3IVPROC>(load_opengl_func("glVertexAttribI3iv"));
	glVertexAttribI4iv = static_cast<PFNGLVERTEXATTRIBI4IVPROC>(load_opengl_func("glVertexAttribI4iv"));
	glVertexAttribI1uiv = static_cast<PFNGLVERTEXATTRIBI1UIVPROC>(load_opengl_func("glVertexAttribI1uiv"));
	glVertexAttribI2uiv = static_cast<PFNGLVERTEXATTRIBI2UIVPROC>(load_opengl_func("glVertexAttribI2uiv"));
	glVertexAttribI3uiv = static_cast<PFNGLVERTEXATTRIBI3UIVPROC>(load_opengl_func("glVertexAttribI3uiv"));
	glVertexAttribI4uiv = static_cast<PFNGLVERTEXATTRIBI4UIVPROC>(load_opengl_func("glVertexAttribI4uiv"));
	glVertexAttribI4bv = static_cast<PFNGLVERTEXATTRIBI4BVPROC>(load_opengl_func("glVertexAttribI4bv"));
	glVertexAttribI4sv = static_cast<PFNGLVERTEXATTRIBI4SVPROC>(load_opengl_func("glVertexAttribI4sv"));
	glVertexAttribI4ubv = static_cast<PFNGLVERTEXATTRIBI4UBVPROC>(load_opengl_func("glVertexAttribI4ubv"));
	glVertexAttribI4usv = static_cast<PFNGLVERTEXATTRIBI4USVPROC>(load_opengl_func("glVertexAttribI4usv"));
	glGetUniformuiv = static_cast<PFNGLGETUNIFORMUIVPROC>(load_opengl_func("glGetUniformuiv"));
	glBindFragDataLocation = static_cast<PFNGLBINDFRAGDATALOCATIONPROC>(load_opengl_func("glBindFragDataLocation"));
	glGetFragDataLocation = static_cast<PFNGLGETFRAGDATALOCATIONPROC>(load_opengl_func("glGetFragDataLocation"));
	glUniform1ui = static_cast<PFNGLUNIFORM1UIPROC>(load_opengl_func("glUniform1ui"));
	glUniform2ui = static_cast<PFNGLUNIFORM2UIPROC>(load_opengl_func("glUniform2ui"));
	glUniform3ui = static_cast<PFNGLUNIFORM3UIPROC>(load_opengl_func("glUniform3ui"));
	glUniform4ui = static_cast<PFNGLUNIFORM4UIPROC>(load_opengl_func("glUniform4ui"));
	glUniform1uiv = static_cast<PFNGLUNIFORM1UIVPROC>(load_opengl_func("glUniform1uiv"));
	glUniform2uiv = static_cast<PFNGLUNIFORM2UIVPROC>(load_opengl_func("glUniform2uiv"));
	glUniform3uiv = static_cast<PFNGLUNIFORM3UIVPROC>(load_opengl_func("glUniform3uiv"));
	glUniform4uiv = static_cast<PFNGLUNIFORM4UIVPROC>(load_opengl_func("glUniform4uiv"));
	glTexParameterIiv = static_cast<PFNGLTEXPARAMETERIIVPROC>(load_opengl_func("glTexParameterIiv"));
	glTexParameterIuiv = static_cast<PFNGLTEXPARAMETERIUIVPROC>(load_opengl_func("glTexParameterIuiv"));
	glGetTexParameterIiv = static_cast<PFNGLGETTEXPARAMETERIIVPROC>(load_opengl_func("glGetTexParameterIiv"));
	glGetTexParameterIuiv = static_cast<PFNGLGETTEXPARAMETERIUIVPROC>(load_opengl_func("glGetTexParameterIuiv"));
	glClearBufferiv = static_cast<PFNGLCLEARBUFFERIVPROC>(load_opengl_func("glClearBufferiv"));
	glClearBufferuiv = static_cast<PFNGLCLEARBUFFERUIVPROC>(load_opengl_func("glClearBufferuiv"));
	glClearBufferfv = static_cast<PFNGLCLEARBUFFERFVPROC>(load_opengl_func("glClearBufferfv"));
	glClearBufferfi = static_cast<PFNGLCLEARBUFFERFIPROC>(load_opengl_func("glClearBufferfi"));
	glGetStringi = static_cast<PFNGLGETSTRINGIPROC>(load_opengl_func("glGetStringi"));
	glIsRenderbuffer = static_cast<PFNGLISRENDERBUFFERPROC>(load_opengl_func("glIsRenderbuffer"));
	glBindRenderbuffer = static_cast<PFNGLBINDRENDERBUFFERPROC>(load_opengl_func("glBindRenderbuffer"));
	glDeleteRenderbuffers = static_cast<PFNGLDELETERENDERBUFFERSPROC>(load_opengl_func("glDeleteRenderbuffers"));
	glGenRenderbuffers = static_cast<PFNGLGENRENDERBUFFERSPROC>(load_opengl_func("glGenRenderbuffers"));
	glRenderbufferStorage = static_cast<PFNGLRENDERBUFFERSTORAGEPROC>(load_opengl_func("glRenderbufferStorage"));
	glGetRenderbufferParameteriv = static_cast<PFNGLGETRENDERBUFFERPARAMETERIVPROC>(load_opengl_func("glGetRenderbufferParameteriv"));
	glIsFramebuffer = static_cast<PFNGLISFRAMEBUFFERPROC>(load_opengl_func("glIsFramebuffer"));
	glBindFramebuffer = static_cast<PFNGLBINDFRAMEBUFFERPROC>(load_opengl_func("glBindFramebuffer"));
	glDeleteFramebuffers = static_cast<PFNGLDELETEFRAMEBUFFERSPROC>(load_opengl_func("glDeleteFramebuffers"));
	glGenFramebuffers = static_cast<PFNGLGENFRAMEBUFFERSPROC>(load_opengl_func("glGenFramebuffers"));
	glCheckFramebufferStatus = static_cast<PFNGLCHECKFRAMEBUFFERSTATUSPROC>(load_opengl_func("glCheckFramebufferStatus"));
	glFramebufferTexture1D = static_cast<PFNGLFRAMEBUFFERTEXTURE1DPROC>(load_opengl_func("glFramebufferTexture1D"));
	glFramebufferTexture2D = static_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(load_opengl_func("glFramebufferTexture2D"));
	glFramebufferTexture3D = static_cast<PFNGLFRAMEBUFFERTEXTURE3DPROC>(load_opengl_func("glFramebufferTexture3D"));
	glFramebufferRenderbuffer = static_cast<PFNGLFRAMEBUFFERRENDERBUFFERPROC>(load_opengl_func("glFramebufferRenderbuffer"));
	glGetFramebufferAttachmentParameteriv = static_cast<PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC>(load_opengl_func("glGetFramebufferAttachmentParameteriv"));
	glGenerateMipmap = static_cast<PFNGLGENERATEMIPMAPPROC>(load_opengl_func("glGenerateMipmap"));
	glBlitFramebuffer = static_cast<PFNGLBLITFRAMEBUFFERPROC>(load_opengl_func("glBlitFramebuffer"));
	glRenderbufferStorageMultisample = static_cast<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC>(load_opengl_func("glRenderbufferStorageMultisample"));
	glFramebufferTextureLayer = static_cast<PFNGLFRAMEBUFFERTEXTURELAYERPROC>(load_opengl_func("glFramebufferTextureLayer"));
	glMapBufferRange = static_cast<PFNGLMAPBUFFERRANGEPROC>(load_opengl_func("glMapBufferRange"));
	glFlushMappedBufferRange = static_cast<PFNGLFLUSHMAPPEDBUFFERRANGEPROC>(load_opengl_func("glFlushMappedBufferRange"));
	glBindVertexArray = static_cast<PFNGLBINDVERTEXARRAYPROC>(load_opengl_func("glBindVertexArray"));
	glDeleteVertexArrays = static_cast<PFNGLDELETEVERTEXARRAYSPROC>(load_opengl_func("glDeleteVertexArrays"));
	glGenVertexArrays = static_cast<PFNGLGENVERTEXARRAYSPROC>(load_opengl_func("glGenVertexArrays"));
	glIsVertexArray = static_cast<PFNGLISVERTEXARRAYPROC>(load_opengl_func("glIsVertexArray"));
	// opengl 3.1
	glDrawArraysInstanced = static_cast<PFNGLDRAWARRAYSINSTANCEDPROC>(load_opengl_func("glDrawArraysInstanced"));
	glDrawElementsInstanced = static_cast<PFNGLDRAWELEMENTSINSTANCEDPROC>(load_opengl_func("glDrawElementsInstanced"));
	glTexBuffer = static_cast<PFNGLTEXBUFFERPROC>(load_opengl_func("glTexBuffer"));
	glPrimitiveRestartIndex = static_cast<PFNGLPRIMITIVERESTARTINDEXPROC>(load_opengl_func("glPrimitiveRestartIndex"));
	glCopyBufferSubData = static_cast<PFNGLCOPYBUFFERSUBDATAPROC>(load_opengl_func("glCopyBufferSubData"));
	glGetUniformIndices = static_cast<PFNGLGETUNIFORMINDICESPROC>(load_opengl_func("glGetUniformIndices"));
	glGetActiveUniformsiv = static_cast<PFNGLGETACTIVEUNIFORMSIVPROC>(load_opengl_func("glGetActiveUniformsiv"));
	glGetActiveUniformName = static_cast<PFNGLGETACTIVEUNIFORMNAMEPROC>(load_opengl_func("glGetActiveUniformName"));
	glGetUniformBlockIndex = static_cast<PFNGLGETUNIFORMBLOCKINDEXPROC>(load_opengl_func("glGetUniformBlockIndex"));
	glGetActiveUniformBlockiv = static_cast<PFNGLGETACTIVEUNIFORMBLOCKIVPROC>(load_opengl_func("glGetActiveUniformBlockiv"));
	glGetActiveUniformBlockName = static_cast<PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC>(load_opengl_func("glGetActiveUniformBlockName"));
	glUniformBlockBinding = static_cast<PFNGLUNIFORMBLOCKBINDINGPROC>(load_opengl_func("glUniformBlockBinding"));
	// opengl 3.2
	glDrawElementsBaseVertex = static_cast<PFNGLDRAWELEMENTSBASEVERTEXPROC>(load_opengl_func("glDrawElementsBaseVertex"));
	glDrawRangeElementsBaseVertex = static_cast<PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC>(load_opengl_func("glDrawRangeElementsBaseVertex"));
	glDrawElementsInstancedBaseVertex = static_cast<PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC>(load_opengl_func("glDrawElementsInstancedBaseVertex"));
	glMultiDrawElementsBaseVertex = static_cast<PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC>(load_opengl_func("glMultiDrawElementsBaseVertex"));
	glProvokingVertex = static_cast<PFNGLPROVOKINGVERTEXPROC>(load_opengl_func("glProvokingVertex"));
	glFenceSync = static_cast<PFNGLFENCESYNCPROC>(load_opengl_func("glFenceSync"));
	glIsSync = static_cast<PFNGLISSYNCPROC>(load_opengl_func("glIsSync"));
	glDeleteSync = static_cast<PFNGLDELETESYNCPROC>(load_opengl_func("glDeleteSync"));
	glClientWaitSync = static_cast<PFNGLCLIENTWAITSYNCPROC>(load_opengl_func("glClientWaitSync"));
	glWaitSync = static_cast<PFNGLWAITSYNCPROC>(load_opengl_func("glWaitSync"));
	glGetInteger64v = static_cast<PFNGLGETINTEGER64VPROC>(load_opengl_func("glGetInteger64v"));
	glGetSynciv = static_cast<PFNGLGETSYNCIVPROC>(load_opengl_func("glGetSynciv"));
	glGetInteger64i_v = static_cast<PFNGLGETINTEGER64I_VPROC>(load_opengl_func("glGetInteger64i_v"));
	glGetBufferParameteri64v = static_cast<PFNGLGETBUFFERPARAMETERI64VPROC>(load_opengl_func("glGetBufferParameteri64v"));
	glFramebufferTexture = static_cast<PFNGLFRAMEBUFFERTEXTUREPROC>(load_opengl_func("glFramebufferTexture"));
	glTexImage2DMultisample = static_cast<PFNGLTEXIMAGE2DMULTISAMPLEPROC>(load_opengl_func("glTexImage2DMultisample"));
	glTexImage3DMultisample = static_cast<PFNGLTEXIMAGE3DMULTISAMPLEPROC>(load_opengl_func("glTexImage3DMultisample"));
	glGetMultisamplefv = static_cast<PFNGLGETMULTISAMPLEFVPROC>(load_opengl_func("glGetMultisamplefv"));
	glSampleMaski = static_cast<PFNGLSAMPLEMASKIPROC>(load_opengl_func("glSampleMaski"));
	// opengl 3.3
	glBindFragDataLocationIndexed = static_cast<PFNGLBINDFRAGDATALOCATIONINDEXEDPROC>(load_opengl_func("glBindFragDataLocationIndexed"));
	glGetFragDataIndex = static_cast<PFNGLGETFRAGDATAINDEXPROC>(load_opengl_func("glGetFragDataIndex"));
	glGenSamplers = static_cast<PFNGLGENSAMPLERSPROC>(load_opengl_func("glGenSamplers"));
	glDeleteSamplers = static_cast<PFNGLDELETESAMPLERSPROC>(load_opengl_func("glDeleteSamplers"));
	glIsSampler = static_cast<PFNGLISSAMPLERPROC>(load_opengl_func("glIsSampler"));
	glBindSampler = static_cast<PFNGLBINDSAMPLERPROC>(load_opengl_func("glBindSampler"));
	glSamplerParameteri = static_cast<PFNGLSAMPLERPARAMETERIPROC>(load_opengl_func("glSamplerParameteri"));
	glSamplerParameteriv = static_cast<PFNGLSAMPLERPARAMETERIVPROC>(load_opengl_func("glSamplerParameteriv"));
	glSamplerParameterf = static_cast<PFNGLSAMPLERPARAMETERFPROC>(load_opengl_func("glSamplerParameterf"));
	glSamplerParameterfv = static_cast<PFNGLSAMPLERPARAMETERFVPROC>(load_opengl_func("glSamplerParameterfv"));
	glSamplerParameterIiv = static_cast<PFNGLSAMPLERPARAMETERIIVPROC>(load_opengl_func("glSamplerParameterIiv"));
	glSamplerParameterIuiv = static_cast<PFNGLSAMPLERPARAMETERIUIVPROC>(load_opengl_func("glSamplerParameterIuiv"));
	glGetSamplerParameteriv = static_cast<PFNGLGETSAMPLERPARAMETERIVPROC>(load_opengl_func("glGetSamplerParameteriv"));
	glGetSamplerParameterIiv = static_cast<PFNGLGETSAMPLERPARAMETERIIVPROC>(load_opengl_func("glGetSamplerParameterIiv"));
	glGetSamplerParameterfv = static_cast<PFNGLGETSAMPLERPARAMETERFVPROC>(load_opengl_func("glGetSamplerParameterfv"));
	glGetSamplerParameterIuiv = static_cast<PFNGLGETSAMPLERPARAMETERIUIVPROC>(load_opengl_func("glGetSamplerParameterIuiv"));
	glQueryCounter = static_cast<PFNGLQUERYCOUNTERPROC>(load_opengl_func("glQueryCounter"));
	glGetQueryObjecti64v = static_cast<PFNGLGETQUERYOBJECTI64VPROC>(load_opengl_func("glGetQueryObjecti64v"));
	glGetQueryObjectui64v = static_cast<PFNGLGETQUERYOBJECTUI64VPROC>(load_opengl_func("glGetQueryObjectui64v"));
	glVertexAttribDivisor = static_cast<PFNGLVERTEXATTRIBDIVISORPROC>(load_opengl_func("glVertexAttribDivisor"));
	glVertexAttribP1ui = static_cast<PFNGLVERTEXATTRIBP1UIPROC>(load_opengl_func("glVertexAttribP1ui"));
	glVertexAttribP1uiv = static_cast<PFNGLVERTEXATTRIBP1UIVPROC>(load_opengl_func("glVertexAttribP1uiv"));
	glVertexAttribP2ui = static_cast<PFNGLVERTEXATTRIBP2UIPROC>(load_opengl_func("glVertexAttribP2ui"));
	glVertexAttribP2uiv = static_cast<PFNGLVERTEXATTRIBP2UIVPROC>(load_opengl_func("glVertexAttribP2uiv"));
	glVertexAttribP3ui = static_cast<PFNGLVERTEXATTRIBP3UIPROC>(load_opengl_func("glVertexAttribP3ui"));
	glVertexAttribP3uiv = static_cast<PFNGLVERTEXATTRIBP3UIVPROC>(load_opengl_func("glVertexAttribP3uiv"));
	glVertexAttribP4ui = static_cast<PFNGLVERTEXATTRIBP4UIPROC>(load_opengl_func("glVertexAttribP4ui"));
	glVertexAttribP4uiv = static_cast<PFNGLVERTEXATTRIBP4UIVPROC>(load_opengl_func("glVertexAttribP4uiv"));
	// opengl 4.0
	glMinSampleShading = static_cast<PFNGLMINSAMPLESHADINGPROC>(load_opengl_func("glMinSampleShading"));
	glBlendEquationi = static_cast<PFNGLBLENDEQUATIONIPROC>(load_opengl_func("glBlendEquationi"));
	glBlendEquationSeparatei = static_cast<PFNGLBLENDEQUATIONSEPARATEIPROC>(load_opengl_func("glBlendEquationSeparatei"));
	glBlendFunci = static_cast<PFNGLBLENDFUNCIPROC>(load_opengl_func("glBlendFunci"));
	glBlendFuncSeparatei = static_cast<PFNGLBLENDFUNCSEPARATEIPROC>(load_opengl_func("glBlendFuncSeparatei"));
	glDrawArraysIndirect = static_cast<PFNGLDRAWARRAYSINDIRECTPROC>(load_opengl_func("glDrawArraysIndirect"));
	glDrawElementsIndirect = static_cast<PFNGLDRAWELEMENTSINDIRECTPROC>(load_opengl_func("glDrawElementsIndirect"));
	glUniform1d = static_cast<PFNGLUNIFORM1DPROC>(load_opengl_func("glUniform1d"));
	glUniform2d = static_cast<PFNGLUNIFORM2DPROC>(load_opengl_func("glUniform2d"));
	glUniform3d = static_cast<PFNGLUNIFORM3DPROC>(load_opengl_func("glUniform3d"));
	glUniform4d = static_cast<PFNGLUNIFORM4DPROC>(load_opengl_func("glUniform4d"));
	glUniform1dv = static_cast<PFNGLUNIFORM1DVPROC>(load_opengl_func("glUniform1dv"));
	glUniform2dv = static_cast<PFNGLUNIFORM2DVPROC>(load_opengl_func("glUniform2dv"));
	glUniform3dv = static_cast<PFNGLUNIFORM3DVPROC>(load_opengl_func("glUniform3dv"));
	glUniform4dv = static_cast<PFNGLUNIFORM4DVPROC>(load_opengl_func("glUniform4dv"));
	glUniformMatrix2dv = static_cast<PFNGLUNIFORMMATRIX2DVPROC>(load_opengl_func("glUniformMatrix2dv"));
	glUniformMatrix3dv = static_cast<PFNGLUNIFORMMATRIX3DVPROC>(load_opengl_func("glUniformMatrix3dv"));
	glUniformMatrix4dv = static_cast<PFNGLUNIFORMMATRIX4DVPROC>(load_opengl_func("glUniformMatrix4dv"));
	glUniformMatrix2x3dv = static_cast<PFNGLUNIFORMMATRIX2X3DVPROC>(load_opengl_func("glUniformMatrix2x3dv"));
	glUniformMatrix2x4dv = static_cast<PFNGLUNIFORMMATRIX2X4DVPROC>(load_opengl_func("glUniformMatrix2x4dv"));
	glUniformMatrix3x2dv = static_cast<PFNGLUNIFORMMATRIX3X2DVPROC>(load_opengl_func("glUniformMatrix3x2dv"));
	glUniformMatrix3x4dv = static_cast<PFNGLUNIFORMMATRIX3X4DVPROC>(load_opengl_func("glUniformMatrix3x4dv"));
	glUniformMatrix4x2dv = static_cast<PFNGLUNIFORMMATRIX4X2DVPROC>(load_opengl_func("glUniformMatrix4x2dv"));
	glUniformMatrix4x3dv = static_cast<PFNGLUNIFORMMATRIX4X3DVPROC>(load_opengl_func("glUniformMatrix4x3dv"));
	glGetUniformdv = static_cast<PFNGLGETUNIFORMDVPROC>(load_opengl_func("glGetUniformdv"));
	glGetSubroutineUniformLocation = static_cast<PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC>(load_opengl_func("glGetSubroutineUniformLocation"));
	glGetSubroutineIndex = static_cast<PFNGLGETSUBROUTINEINDEXPROC>(load_opengl_func("glGetSubroutineIndex"));
	glGetActiveSubroutineUniformiv = static_cast<PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC>(load_opengl_func("glGetActiveSubroutineUniformiv"));
	glGetActiveSubroutineUniformName = static_cast<PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC>(load_opengl_func("glGetActiveSubroutineUniformName"));
	glGetActiveSubroutineName = static_cast<PFNGLGETACTIVESUBROUTINENAMEPROC>(load_opengl_func("glGetActiveSubroutineName"));
	glUniformSubroutinesuiv = static_cast<PFNGLUNIFORMSUBROUTINESUIVPROC>(load_opengl_func("glUniformSubroutinesuiv"));
	glGetUniformSubroutineuiv = static_cast<PFNGLGETUNIFORMSUBROUTINEUIVPROC>(load_opengl_func("glGetUniformSubroutineuiv"));
	glGetProgramStageiv = static_cast<PFNGLGETPROGRAMSTAGEIVPROC>(load_opengl_func("glGetProgramStageiv"));
	glPatchParameteri = static_cast<PFNGLPATCHPARAMETERIPROC>(load_opengl_func("glPatchParameteri"));
	glPatchParameterfv = static_cast<PFNGLPATCHPARAMETERFVPROC>(load_opengl_func("glPatchParameterfv"));
	glBindTransformFeedback = static_cast<PFNGLBINDTRANSFORMFEEDBACKPROC>(load_opengl_func("glBindTransformFeedback"));
	glDeleteTransformFeedbacks = static_cast<PFNGLDELETETRANSFORMFEEDBACKSPROC>(load_opengl_func("glDeleteTransformFeedbacks"));
	glGenTransformFeedbacks = static_cast<PFNGLGENTRANSFORMFEEDBACKSPROC>(load_opengl_func("glGenTransformFeedbacks"));
	glIsTransformFeedback = static_cast<PFNGLISTRANSFORMFEEDBACKPROC>(load_opengl_func("glIsTransformFeedback"));
	glPauseTransformFeedback = static_cast<PFNGLPAUSETRANSFORMFEEDBACKPROC>(load_opengl_func("glPauseTransformFeedback"));
	glResumeTransformFeedback = static_cast<PFNGLRESUMETRANSFORMFEEDBACKPROC>(load_opengl_func("glResumeTransformFeedback"));
	glDrawTransformFeedback = static_cast<PFNGLDRAWTRANSFORMFEEDBACKPROC>(load_opengl_func("glDrawTransformFeedback"));
	glDrawTransformFeedbackStream = static_cast<PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC>(load_opengl_func("glDrawTransformFeedbackStream"));
	glBeginQueryIndexed = static_cast<PFNGLBEGINQUERYINDEXEDPROC>(load_opengl_func("glBeginQueryIndexed"));
	glEndQueryIndexed = static_cast<PFNGLENDQUERYINDEXEDPROC>(load_opengl_func("glEndQueryIndexed"));
	glGetQueryIndexediv = static_cast<PFNGLGETQUERYINDEXEDIVPROC>(load_opengl_func("glGetQueryIndexediv"));
	// opengl 4.1
	glReleaseShaderCompiler = static_cast<PFNGLRELEASESHADERCOMPILERPROC>(load_opengl_func("glReleaseShaderCompiler"));
	glShaderBinary = static_cast<PFNGLSHADERBINARYPROC>(load_opengl_func("glShaderBinary"));
	glGetShaderPrecisionFormat = static_cast<PFNGLGETSHADERPRECISIONFORMATPROC>(load_opengl_func("glGetShaderPrecisionFormat"));
	glDepthRangef = static_cast<PFNGLDEPTHRANGEFPROC>(load_opengl_func("glDepthRangef"));
	glClearDepthf = static_cast<PFNGLCLEARDEPTHFPROC>(load_opengl_func("glClearDepthf"));
	glGetProgramBinary = static_cast<PFNGLGETPROGRAMBINARYPROC>(load_opengl_func("glGetProgramBinary"));
	glProgramBinary = static_cast<PFNGLPROGRAMBINARYPROC>(load_opengl_func("glProgramBinary"));
	glProgramParameteri = static_cast<PFNGLPROGRAMPARAMETERIPROC>(load_opengl_func("glProgramParameteri"));
	glUseProgramStages = static_cast<PFNGLUSEPROGRAMSTAGESPROC>(load_opengl_func("glUseProgramStages"));
	glActiveShaderProgram = static_cast<PFNGLACTIVESHADERPROGRAMPROC>(load_opengl_func("glActiveShaderProgram"));
	glCreateShaderProgramv = static_cast<PFNGLCREATESHADERPROGRAMVPROC>(load_opengl_func("glCreateShaderProgramv"));
	glBindProgramPipeline = static_cast<PFNGLBINDPROGRAMPIPELINEPROC>(load_opengl_func("glBindProgramPipeline"));
	glDeleteProgramPipelines = static_cast<PFNGLDELETEPROGRAMPIPELINESPROC>(load_opengl_func("glDeleteProgramPipelines"));
	glGenProgramPipelines = static_cast<PFNGLGENPROGRAMPIPELINESPROC>(load_opengl_func("glGenProgramPipelines"));
	glIsProgramPipeline = static_cast<PFNGLISPROGRAMPIPELINEPROC>(load_opengl_func("glIsProgramPipeline"));
	glGetProgramPipelineiv = static_cast<PFNGLGETPROGRAMPIPELINEIVPROC>(load_opengl_func("glGetProgramPipelineiv"));
	glProgramUniform1i = static_cast<PFNGLPROGRAMUNIFORM1IPROC>(load_opengl_func("glProgramUniform1i"));
	glProgramUniform1iv = static_cast<PFNGLPROGRAMUNIFORM1IVPROC>(load_opengl_func("glProgramUniform1iv"));
	glProgramUniform1f = static_cast<PFNGLPROGRAMUNIFORM1FPROC>(load_opengl_func("glProgramUniform1f"));
	glProgramUniform1fv = static_cast<PFNGLPROGRAMUNIFORM1FVPROC>(load_opengl_func("glProgramUniform1fv"));
	glProgramUniform1d = static_cast<PFNGLPROGRAMUNIFORM1DPROC>(load_opengl_func("glProgramUniform1d"));
	glProgramUniform1dv = static_cast<PFNGLPROGRAMUNIFORM1DVPROC>(load_opengl_func("glProgramUniform1dv"));
	glProgramUniform1ui = static_cast<PFNGLPROGRAMUNIFORM1UIPROC>(load_opengl_func("glProgramUniform1ui"));
	glProgramUniform1uiv = static_cast<PFNGLPROGRAMUNIFORM1UIVPROC>(load_opengl_func("glProgramUniform1uiv"));
	glProgramUniform2i = static_cast<PFNGLPROGRAMUNIFORM2IPROC>(load_opengl_func("glProgramUniform2i"));
	glProgramUniform2iv = static_cast<PFNGLPROGRAMUNIFORM2IVPROC>(load_opengl_func("glProgramUniform2iv"));
	glProgramUniform2f = static_cast<PFNGLPROGRAMUNIFORM2FPROC>(load_opengl_func("glProgramUniform2f"));
	glProgramUniform2fv = static_cast<PFNGLPROGRAMUNIFORM2FVPROC>(load_opengl_func("glProgramUniform2fv"));
	glProgramUniform2d = static_cast<PFNGLPROGRAMUNIFORM2DPROC>(load_opengl_func("glProgramUniform2d"));
	glProgramUniform2dv = static_cast<PFNGLPROGRAMUNIFORM2DVPROC>(load_opengl_func("glProgramUniform2dv"));
	glProgramUniform2ui = static_cast<PFNGLPROGRAMUNIFORM2UIPROC>(load_opengl_func("glProgramUniform2ui"));
	glProgramUniform2uiv = static_cast<PFNGLPROGRAMUNIFORM2UIVPROC>(load_opengl_func("glProgramUniform2uiv"));
	glProgramUniform3i = static_cast<PFNGLPROGRAMUNIFORM3IPROC>(load_opengl_func("glProgramUniform3i"));
	glProgramUniform3iv = static_cast<PFNGLPROGRAMUNIFORM3IVPROC>(load_opengl_func("glProgramUniform3iv"));
	glProgramUniform3f = static_cast<PFNGLPROGRAMUNIFORM3FPROC>(load_opengl_func("glProgramUniform3f"));
	glProgramUniform3fv = static_cast<PFNGLPROGRAMUNIFORM3FVPROC>(load_opengl_func("glProgramUniform3fv"));
	glProgramUniform3d = static_cast<PFNGLPROGRAMUNIFORM3DPROC>(load_opengl_func("glProgramUniform3d"));
	glProgramUniform3dv = static_cast<PFNGLPROGRAMUNIFORM3DVPROC>(load_opengl_func("glProgramUniform3dv"));
	glProgramUniform3ui = static_cast<PFNGLPROGRAMUNIFORM3UIPROC>(load_opengl_func("glProgramUniform3ui"));
	glProgramUniform3uiv = static_cast<PFNGLPROGRAMUNIFORM3UIVPROC>(load_opengl_func("glProgramUniform3uiv"));
	glProgramUniform4i = static_cast<PFNGLPROGRAMUNIFORM4IPROC>(load_opengl_func("glProgramUniform4i"));
	glProgramUniform4iv = static_cast<PFNGLPROGRAMUNIFORM4IVPROC>(load_opengl_func("glProgramUniform4iv"));
	glProgramUniform4f = static_cast<PFNGLPROGRAMUNIFORM4FPROC>(load_opengl_func("glProgramUniform4f"));
	glProgramUniform4fv = static_cast<PFNGLPROGRAMUNIFORM4FVPROC>(load_opengl_func("glProgramUniform4fv"));
	glProgramUniform4d = static_cast<PFNGLPROGRAMUNIFORM4DPROC>(load_opengl_func("glProgramUniform4d"));
	glProgramUniform4dv = static_cast<PFNGLPROGRAMUNIFORM4DVPROC>(load_opengl_func("glProgramUniform4dv"));
	glProgramUniform4ui = static_cast<PFNGLPROGRAMUNIFORM4UIPROC>(load_opengl_func("glProgramUniform4ui"));
	glProgramUniform4uiv = static_cast<PFNGLPROGRAMUNIFORM4UIVPROC>(load_opengl_func("glProgramUniform4uiv"));
	glProgramUniformMatrix2fv = static_cast<PFNGLPROGRAMUNIFORMMATRIX2FVPROC>(load_opengl_func("glProgramUniformMatrix2fv"));
	glProgramUniformMatrix3fv = static_cast<PFNGLPROGRAMUNIFORMMATRIX3FVPROC>(load_opengl_func("glProgramUniformMatrix3fv"));
	glProgramUniformMatrix4fv = static_cast<PFNGLPROGRAMUNIFORMMATRIX4FVPROC>(load_opengl_func("glProgramUniformMatrix4fv"));
	glProgramUniformMatrix2dv = static_cast<PFNGLPROGRAMUNIFORMMATRIX2DVPROC>(load_opengl_func("glProgramUniformMatrix2dv"));
	glProgramUniformMatrix3dv = static_cast<PFNGLPROGRAMUNIFORMMATRIX3DVPROC>(load_opengl_func("glProgramUniformMatrix3dv"));
	glProgramUniformMatrix4dv = static_cast<PFNGLPROGRAMUNIFORMMATRIX4DVPROC>(load_opengl_func("glProgramUniformMatrix4dv"));
	glProgramUniformMatrix2x3fv = static_cast<PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC>(load_opengl_func("glProgramUniformMatrix2x3fv"));
	glProgramUniformMatrix3x2fv = static_cast<PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC>(load_opengl_func("glProgramUniformMatrix3x2fv"));
	glProgramUniformMatrix2x4fv = static_cast<PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC>(load_opengl_func("glProgramUniformMatrix2x4fv"));
	glProgramUniformMatrix4x2fv = static_cast<PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC>(load_opengl_func("glProgramUniformMatrix4x2fv"));
	glProgramUniformMatrix3x4fv = static_cast<PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC>(load_opengl_func("glProgramUniformMatrix3x4fv"));
	glProgramUniformMatrix4x3fv = static_cast<PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC>(load_opengl_func("glProgramUniformMatrix4x3fv"));
	glProgramUniformMatrix2x3dv = static_cast<PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC>(load_opengl_func("glProgramUniformMatrix2x3dv"));
	glProgramUniformMatrix3x2dv = static_cast<PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC>(load_opengl_func("glProgramUniformMatrix3x2dv"));
	glProgramUniformMatrix2x4dv = static_cast<PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC>(load_opengl_func("glProgramUniformMatrix2x4dv"));
	glProgramUniformMatrix4x2dv = static_cast<PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC>(load_opengl_func("glProgramUniformMatrix4x2dv"));
	glProgramUniformMatrix3x4dv = static_cast<PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC>(load_opengl_func("glProgramUniformMatrix3x4dv"));
	glProgramUniformMatrix4x3dv = static_cast<PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC>(load_opengl_func("glProgramUniformMatrix4x3dv"));
	glValidateProgramPipeline = static_cast<PFNGLVALIDATEPROGRAMPIPELINEPROC>(load_opengl_func("glValidateProgramPipeline"));
	glGetProgramPipelineInfoLog = static_cast<PFNGLGETPROGRAMPIPELINEINFOLOGPROC>(load_opengl_func("glGetProgramPipelineInfoLog"));
	glVertexAttribL1d = static_cast<PFNGLVERTEXATTRIBL1DPROC>(load_opengl_func("glVertexAttribL1d"));
	glVertexAttribL2d = static_cast<PFNGLVERTEXATTRIBL2DPROC>(load_opengl_func("glVertexAttribL2d"));
	glVertexAttribL3d = static_cast<PFNGLVERTEXATTRIBL3DPROC>(load_opengl_func("glVertexAttribL3d"));
	glVertexAttribL4d = static_cast<PFNGLVERTEXATTRIBL4DPROC>(load_opengl_func("glVertexAttribL4d"));
	glVertexAttribL1dv = static_cast<PFNGLVERTEXATTRIBL1DVPROC>(load_opengl_func("glVertexAttribL1dv"));
	glVertexAttribL2dv = static_cast<PFNGLVERTEXATTRIBL2DVPROC>(load_opengl_func("glVertexAttribL2dv"));
	glVertexAttribL3dv = static_cast<PFNGLVERTEXATTRIBL3DVPROC>(load_opengl_func("glVertexAttribL3dv"));
	glVertexAttribL4dv = static_cast<PFNGLVERTEXATTRIBL4DVPROC>(load_opengl_func("glVertexAttribL4dv"));
	glVertexAttribLPointer = static_cast<PFNGLVERTEXATTRIBLPOINTERPROC>(load_opengl_func("glVertexAttribLPointer"));
	glGetVertexAttribLdv = static_cast<PFNGLGETVERTEXATTRIBLDVPROC>(load_opengl_func("glGetVertexAttribLdv"));
	glViewportArrayv = static_cast<PFNGLVIEWPORTARRAYVPROC>(load_opengl_func("glViewportArrayv"));
	glViewportIndexedf = static_cast<PFNGLVIEWPORTINDEXEDFPROC>(load_opengl_func("glViewportIndexedf"));
	glViewportIndexedfv = static_cast<PFNGLVIEWPORTINDEXEDFVPROC>(load_opengl_func("glViewportIndexedfv"));
	glScissorArrayv = static_cast<PFNGLSCISSORARRAYVPROC>(load_opengl_func("glScissorArrayv"));
	glScissorIndexed = static_cast<PFNGLSCISSORINDEXEDPROC>(load_opengl_func("glScissorIndexed"));
	glScissorIndexedv = static_cast<PFNGLSCISSORINDEXEDVPROC>(load_opengl_func("glScissorIndexedv"));
	glDepthRangeArrayv = static_cast<PFNGLDEPTHRANGEARRAYVPROC>(load_opengl_func("glDepthRangeArrayv"));
	glDepthRangeIndexed = static_cast<PFNGLDEPTHRANGEINDEXEDPROC>(load_opengl_func("glDepthRangeIndexed"));
	glGetFloati_v = static_cast<PFNGLGETFLOATI_VPROC>(load_opengl_func("glGetFloati_v"));
	glGetDoublei_v = static_cast<PFNGLGETDOUBLEI_VPROC>(load_opengl_func("glGetDoublei_v"));
	// opengl 4.2
	glDrawArraysInstancedBaseInstance = static_cast<PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC>(load_opengl_func("glDrawArraysInstancedBaseInstance"));
	glDrawElementsInstancedBaseInstance = static_cast<PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC>(load_opengl_func("glDrawElementsInstancedBaseInstance"));
	glDrawElementsInstancedBaseVertexBaseInstance = static_cast<PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC>(load_opengl_func("glDrawElementsInstancedBaseVertexBaseInstance"));
	glGetInternalformativ = static_cast<PFNGLGETINTERNALFORMATIVPROC>(load_opengl_func("glGetInternalformativ"));
	glGetActiveAtomicCounterBufferiv = static_cast<PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC>(load_opengl_func("glGetActiveAtomicCounterBufferiv"));
	glBindImageTexture = static_cast<PFNGLBINDIMAGETEXTUREPROC>(load_opengl_func("glBindImageTexture"));
	glMemoryBarrier = static_cast<PFNGLMEMORYBARRIERPROC>(load_opengl_func("glMemoryBarrier"));
	glTexStorage1D = static_cast<PFNGLTEXSTORAGE1DPROC>(load_opengl_func("glTexStorage1D"));
	glTexStorage2D = static_cast<PFNGLTEXSTORAGE2DPROC>(load_opengl_func("glTexStorage2D"));
	glTexStorage3D = static_cast<PFNGLTEXSTORAGE3DPROC>(load_opengl_func("glTexStorage3D"));
	glDrawTransformFeedbackInstanced = static_cast<PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC>(load_opengl_func("glDrawTransformFeedbackInstanced"));
	glDrawTransformFeedbackStreamInstanced = static_cast<PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC>(load_opengl_func("glDrawTransformFeedbackStreamInstanced"));
	// opengl 4.3
	glClearBufferData = static_cast<PFNGLCLEARBUFFERDATAPROC>(load_opengl_func("glClearBufferData"));
	glClearBufferSubData = static_cast<PFNGLCLEARBUFFERSUBDATAPROC>(load_opengl_func("glClearBufferSubData"));
	glDispatchCompute = static_cast<PFNGLDISPATCHCOMPUTEPROC>(load_opengl_func("glDispatchCompute"));
	glDispatchComputeIndirect = static_cast<PFNGLDISPATCHCOMPUTEINDIRECTPROC>(load_opengl_func("glDispatchComputeIndirect"));
	glCopyImageSubData = static_cast<PFNGLCOPYIMAGESUBDATAPROC>(load_opengl_func("glCopyImageSubData"));
	glFramebufferParameteri = static_cast<PFNGLFRAMEBUFFERPARAMETERIPROC>(load_opengl_func("glFramebufferParameteri"));
	glGetFramebufferParameteriv = static_cast<PFNGLGETFRAMEBUFFERPARAMETERIVPROC>(load_opengl_func("glGetFramebufferParameteriv"));
	glGetInternalformati64v = static_cast<PFNGLGETINTERNALFORMATI64VPROC>(load_opengl_func("glGetInternalformati64v"));
	glInvalidateTexSubImage = static_cast<PFNGLINVALIDATETEXSUBIMAGEPROC>(load_opengl_func("glInvalidateTexSubImage"));
	glInvalidateTexImage = static_cast<PFNGLINVALIDATETEXIMAGEPROC>(load_opengl_func("glInvalidateTexImage"));
	glInvalidateBufferSubData = static_cast<PFNGLINVALIDATEBUFFERSUBDATAPROC>(load_opengl_func("glInvalidateBufferSubData"));
	glInvalidateBufferData = static_cast<PFNGLINVALIDATEBUFFERDATAPROC>(load_opengl_func("glInvalidateBufferData"));
	glInvalidateFramebuffer = static_cast<PFNGLINVALIDATEFRAMEBUFFERPROC>(load_opengl_func("glInvalidateFramebuffer"));
	glInvalidateSubFramebuffer = static_cast<PFNGLINVALIDATESUBFRAMEBUFFERPROC>(load_opengl_func("glInvalidateSubFramebuffer"));
	glMultiDrawArraysIndirect = static_cast<PFNGLMULTIDRAWARRAYSINDIRECTPROC>(load_opengl_func("glMultiDrawArraysIndirect"));
	glMultiDrawElementsIndirect = static_cast<PFNGLMULTIDRAWELEMENTSINDIRECTPROC>(load_opengl_func("glMultiDrawElementsIndirect"));
	glGetProgramInterfaceiv = static_cast<PFNGLGETPROGRAMINTERFACEIVPROC>(load_opengl_func("glGetProgramInterfaceiv"));
	glGetProgramResourceIndex = static_cast<PFNGLGETPROGRAMRESOURCEINDEXPROC>(load_opengl_func("glGetProgramResourceIndex"));
	glGetProgramResourceName = static_cast<PFNGLGETPROGRAMRESOURCENAMEPROC>(load_opengl_func("glGetProgramResourceName"));
	glGetProgramResourceiv = static_cast<PFNGLGETPROGRAMRESOURCEIVPROC>(load_opengl_func("glGetProgramResourceiv"));
	glGetProgramResourceLocation = static_cast<PFNGLGETPROGRAMRESOURCELOCATIONPROC>(load_opengl_func("glGetProgramResourceLocation"));
	glGetProgramResourceLocationIndex = static_cast<PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC>(load_opengl_func("glGetProgramResourceLocationIndex"));
	glShaderStorageBlockBinding = static_cast<PFNGLSHADERSTORAGEBLOCKBINDINGPROC>(load_opengl_func("glShaderStorageBlockBinding"));
	glTexBufferRange = static_cast<PFNGLTEXBUFFERRANGEPROC>(load_opengl_func("glTexBufferRange"));
	glTexStorage2DMultisample = static_cast<PFNGLTEXSTORAGE2DMULTISAMPLEPROC>(load_opengl_func("glTexStorage2DMultisample"));
	glTexStorage3DMultisample = static_cast<PFNGLTEXSTORAGE3DMULTISAMPLEPROC>(load_opengl_func("glTexStorage3DMultisample"));
	glTextureView = static_cast<PFNGLTEXTUREVIEWPROC>(load_opengl_func("glTextureView"));
	glBindVertexBuffer = static_cast<PFNGLBINDVERTEXBUFFERPROC>(load_opengl_func("glBindVertexBuffer"));
	glVertexAttribFormat = static_cast<PFNGLVERTEXATTRIBFORMATPROC>(load_opengl_func("glVertexAttribFormat"));
	glVertexAttribIFormat = static_cast<PFNGLVERTEXATTRIBIFORMATPROC>(load_opengl_func("glVertexAttribIFormat"));
	glVertexAttribLFormat = static_cast<PFNGLVERTEXATTRIBLFORMATPROC>(load_opengl_func("glVertexAttribLFormat"));
	glVertexAttribBinding = static_cast<PFNGLVERTEXATTRIBBINDINGPROC>(load_opengl_func("glVertexAttribBinding"));
	glVertexBindingDivisor = static_cast<PFNGLVERTEXBINDINGDIVISORPROC>(load_opengl_func("glVertexBindingDivisor"));
	glDebugMessageControl = static_cast<PFNGLDEBUGMESSAGECONTROLPROC>(load_opengl_func("glDebugMessageControl"));
	glDebugMessageInsert = static_cast<PFNGLDEBUGMESSAGEINSERTPROC>(load_opengl_func("glDebugMessageInsert"));
	glDebugMessageCallback = static_cast<PFNGLDEBUGMESSAGECALLBACKPROC>(load_opengl_func("glDebugMessageCallback"));
	glGetDebugMessageLog = static_cast<PFNGLGETDEBUGMESSAGELOGPROC>(load_opengl_func("glGetDebugMessageLog"));
	glPushDebugGroup = static_cast<PFNGLPUSHDEBUGGROUPPROC>(load_opengl_func("glPushDebugGroup"));
	glPopDebugGroup = static_cast<PFNGLPOPDEBUGGROUPPROC>(load_opengl_func("glPopDebugGroup"));
	glObjectLabel = static_cast<PFNGLOBJECTLABELPROC>(load_opengl_func("glObjectLabel"));
	glGetObjectLabel = static_cast<PFNGLGETOBJECTLABELPROC>(load_opengl_func("glGetObjectLabel"));
	glObjectPtrLabel = static_cast<PFNGLOBJECTPTRLABELPROC>(load_opengl_func("glObjectPtrLabel"));
	glGetObjectPtrLabel = static_cast<PFNGLGETOBJECTPTRLABELPROC>(load_opengl_func("glGetObjectPtrLabel"));
	// opengl 4.4
	glBufferStorage = static_cast<PFNGLBUFFERSTORAGEPROC>(load_opengl_func("glBufferStorage"));
	glClearTexImage = static_cast<PFNGLCLEARTEXIMAGEPROC>(load_opengl_func("glClearTexImage"));
	glClearTexSubImage = static_cast<PFNGLCLEARTEXSUBIMAGEPROC>(load_opengl_func("glClearTexSubImage"));
	glBindBuffersBase = static_cast<PFNGLBINDBUFFERSBASEPROC>(load_opengl_func("glBindBuffersBase"));
	glBindBuffersRange = static_cast<PFNGLBINDBUFFERSRANGEPROC>(load_opengl_func("glBindBuffersRange"));
	glBindTextures = static_cast<PFNGLBINDTEXTURESPROC>(load_opengl_func("glBindTextures"));
	glBindSamplers = static_cast<PFNGLBINDSAMPLERSPROC>(load_opengl_func("glBindSamplers"));
	glBindImageTextures = static_cast<PFNGLBINDIMAGETEXTURESPROC>(load_opengl_func("glBindImageTextures"));
	glBindVertexBuffers = static_cast<PFNGLBINDVERTEXBUFFERSPROC>(load_opengl_func("glBindVertexBuffers"));
	// opengl 4.5
	glClipControl = static_cast<PFNGLCLIPCONTROLPROC>(load_opengl_func("glClipControl"));
	glCreateTransformFeedbacks = static_cast<PFNGLCREATETRANSFORMFEEDBACKSPROC>(load_opengl_func("glCreateTransformFeedbacks"));
	glTransformFeedbackBufferBase = static_cast<PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC>(load_opengl_func("glTransformFeedbackBufferBase"));
	glTransformFeedbackBufferRange = static_cast<PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC>(load_opengl_func("glTransformFeedbackBufferRange"));
	glGetTransformFeedbackiv = static_cast<PFNGLGETTRANSFORMFEEDBACKIVPROC>(load_opengl_func("glGetTransformFeedbackiv"));
	glGetTransformFeedbacki_v = static_cast<PFNGLGETTRANSFORMFEEDBACKI_VPROC>(load_opengl_func("glGetTransformFeedbacki_v"));
	glGetTransformFeedbacki64_v = static_cast<PFNGLGETTRANSFORMFEEDBACKI64_VPROC>(load_opengl_func("glGetTransformFeedbacki64_v"));
	glCreateBuffers = static_cast<PFNGLCREATEBUFFERSPROC>(load_opengl_func("glCreateBuffers"));
	glNamedBufferStorage = static_cast<PFNGLNAMEDBUFFERSTORAGEPROC>(load_opengl_func("glNamedBufferStorage"));
	glNamedBufferData = static_cast<PFNGLNAMEDBUFFERDATAPROC>(load_opengl_func("glNamedBufferData"));
	glNamedBufferSubData = static_cast<PFNGLNAMEDBUFFERSUBDATAPROC>(load_opengl_func("glNamedBufferSubData"));
	glCopyNamedBufferSubData = static_cast<PFNGLCOPYNAMEDBUFFERSUBDATAPROC>(load_opengl_func("glCopyNamedBufferSubData"));
	glClearNamedBufferData = static_cast<PFNGLCLEARNAMEDBUFFERDATAPROC>(load_opengl_func("glClearNamedBufferData"));
	glClearNamedBufferSubData = static_cast<PFNGLCLEARNAMEDBUFFERSUBDATAPROC>(load_opengl_func("glClearNamedBufferSubData"));
	glMapNamedBuffer = static_cast<PFNGLMAPNAMEDBUFFERPROC>(load_opengl_func("glMapNamedBuffer"));
	glMapNamedBufferRange = static_cast<PFNGLMAPNAMEDBUFFERRANGEPROC>(load_opengl_func("glMapNamedBufferRange"));
	glUnmapNamedBuffer = static_cast<PFNGLUNMAPNAMEDBUFFERPROC>(load_opengl_func("glUnmapNamedBuffer"));
	glFlushMappedNamedBufferRange = static_cast<PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC>(load_opengl_func("glFlushMappedNamedBufferRange"));
	glGetNamedBufferParameteriv = static_cast<PFNGLGETNAMEDBUFFERPARAMETERIVPROC>(load_opengl_func("glGetNamedBufferParameteriv"));
	glGetNamedBufferParameteri64v = static_cast<PFNGLGETNAMEDBUFFERPARAMETERI64VPROC>(load_opengl_func("glGetNamedBufferParameteri64v"));
	glGetNamedBufferPointerv = static_cast<PFNGLGETNAMEDBUFFERPOINTERVPROC>(load_opengl_func("glGetNamedBufferPointerv"));
	glGetNamedBufferSubData = static_cast<PFNGLGETNAMEDBUFFERSUBDATAPROC>(load_opengl_func("glGetNamedBufferSubData"));
	glCreateFramebuffers = static_cast<PFNGLCREATEFRAMEBUFFERSPROC>(load_opengl_func("glCreateFramebuffers"));
	glNamedFramebufferRenderbuffer = static_cast<PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC>(load_opengl_func("glNamedFramebufferRenderbuffer"));
	glNamedFramebufferParameteri = static_cast<PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC>(load_opengl_func("glNamedFramebufferParameteri"));
	glNamedFramebufferTexture = static_cast<PFNGLNAMEDFRAMEBUFFERTEXTUREPROC>(load_opengl_func("glNamedFramebufferTexture"));
	glNamedFramebufferTextureLayer = static_cast<PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC>(load_opengl_func("glNamedFramebufferTextureLayer"));
	glNamedFramebufferDrawBuffer = static_cast<PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC>(load_opengl_func("glNamedFramebufferDrawBuffer"));
	glNamedFramebufferDrawBuffers = static_cast<PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC>(load_opengl_func("glNamedFramebufferDrawBuffers"));
	glNamedFramebufferReadBuffer = static_cast<PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC>(load_opengl_func("glNamedFramebufferReadBuffer"));
	glInvalidateNamedFramebufferData = static_cast<PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC>(load_opengl_func("glInvalidateNamedFramebufferData"));
	glInvalidateNamedFramebufferSubData = static_cast<PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC>(load_opengl_func("glInvalidateNamedFramebufferSubData"));
	glClearNamedFramebufferiv = static_cast<PFNGLCLEARNAMEDFRAMEBUFFERIVPROC>(load_opengl_func("glClearNamedFramebufferiv"));
	glClearNamedFramebufferuiv = static_cast<PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC>(load_opengl_func("glClearNamedFramebufferuiv"));
	glClearNamedFramebufferfv = static_cast<PFNGLCLEARNAMEDFRAMEBUFFERFVPROC>(load_opengl_func("glClearNamedFramebufferfv"));
	glClearNamedFramebufferfi = static_cast<PFNGLCLEARNAMEDFRAMEBUFFERFIPROC>(load_opengl_func("glClearNamedFramebufferfi"));
	glBlitNamedFramebuffer = static_cast<PFNGLBLITNAMEDFRAMEBUFFERPROC>(load_opengl_func("glBlitNamedFramebuffer"));
	glCheckNamedFramebufferStatus = static_cast<PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC>(load_opengl_func("glCheckNamedFramebufferStatus"));
	glGetNamedFramebufferParameteriv = static_cast<PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC>(load_opengl_func("glGetNamedFramebufferParameteriv"));
	glGetNamedFramebufferAttachmentParameteriv = static_cast<PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC>(load_opengl_func("glGetNamedFramebufferAttachmentParameteriv"));
	glCreateRenderbuffers = static_cast<PFNGLCREATERENDERBUFFERSPROC>(load_opengl_func("glCreateRenderbuffers"));
	glNamedRenderbufferStorage = static_cast<PFNGLNAMEDRENDERBUFFERSTORAGEPROC>(load_opengl_func("glNamedRenderbufferStorage"));
	glNamedRenderbufferStorageMultisample = static_cast<PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC>(load_opengl_func("glNamedRenderbufferStorageMultisample"));
	glGetNamedRenderbufferParameteriv = static_cast<PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC>(load_opengl_func("glGetNamedRenderbufferParameteriv"));
	glCreateTextures = static_cast<PFNGLCREATETEXTURESPROC>(load_opengl_func("glCreateTextures"));
	glTextureBuffer = static_cast<PFNGLTEXTUREBUFFERPROC>(load_opengl_func("glTextureBuffer"));
	glTextureBufferRange = static_cast<PFNGLTEXTUREBUFFERRANGEPROC>(load_opengl_func("glTextureBufferRange"));
	glTextureStorage1D = static_cast<PFNGLTEXTURESTORAGE1DPROC>(load_opengl_func("glTextureStorage1D"));
	glTextureStorage2D = static_cast<PFNGLTEXTURESTORAGE2DPROC>(load_opengl_func("glTextureStorage2D"));
	glTextureStorage3D = static_cast<PFNGLTEXTURESTORAGE3DPROC>(load_opengl_func("glTextureStorage3D"));
	glTextureStorage2DMultisample = static_cast<PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC>(load_opengl_func("glTextureStorage2DMultisample"));
	glTextureStorage3DMultisample = static_cast<PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC>(load_opengl_func("glTextureStorage3DMultisample"));
	glTextureSubImage1D = static_cast<PFNGLTEXTURESUBIMAGE1DPROC>(load_opengl_func("glTextureSubImage1D"));
	glTextureSubImage2D = static_cast<PFNGLTEXTURESUBIMAGE2DPROC>(load_opengl_func("glTextureSubImage2D"));
	glTextureSubImage3D = static_cast<PFNGLTEXTURESUBIMAGE3DPROC>(load_opengl_func("glTextureSubImage3D"));
	glCompressedTextureSubImage1D = static_cast<PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC>(load_opengl_func("glCompressedTextureSubImage1D"));
	glCompressedTextureSubImage2D = static_cast<PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC>(load_opengl_func("glCompressedTextureSubImage2D"));
	glCompressedTextureSubImage3D = static_cast<PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC>(load_opengl_func("glCompressedTextureSubImage3D"));
	glCopyTextureSubImage1D = static_cast<PFNGLCOPYTEXTURESUBIMAGE1DPROC>(load_opengl_func("glCopyTextureSubImage1D"));
	glCopyTextureSubImage2D = static_cast<PFNGLCOPYTEXTURESUBIMAGE2DPROC>(load_opengl_func("glCopyTextureSubImage2D"));
	glCopyTextureSubImage3D = static_cast<PFNGLCOPYTEXTURESUBIMAGE3DPROC>(load_opengl_func("glCopyTextureSubImage3D"));
	glTextureParameterf = static_cast<PFNGLTEXTUREPARAMETERFPROC>(load_opengl_func("glTextureParameterf"));
	glTextureParameterfv = static_cast<PFNGLTEXTUREPARAMETERFVPROC>(load_opengl_func("glTextureParameterfv"));
	glTextureParameteri = static_cast<PFNGLTEXTUREPARAMETERIPROC>(load_opengl_func("glTextureParameteri"));
	glTextureParameterIiv = static_cast<PFNGLTEXTUREPARAMETERIIVPROC>(load_opengl_func("glTextureParameterIiv"));
	glTextureParameterIuiv = static_cast<PFNGLTEXTUREPARAMETERIUIVPROC>(load_opengl_func("glTextureParameterIuiv"));
	glTextureParameteriv = static_cast<PFNGLTEXTUREPARAMETERIVPROC>(load_opengl_func("glTextureParameteriv"));
	glGenerateTextureMipmap = static_cast<PFNGLGENERATETEXTUREMIPMAPPROC>(load_opengl_func("glGenerateTextureMipmap"));
	glBindTextureUnit = static_cast<PFNGLBINDTEXTUREUNITPROC>(load_opengl_func("glBindTextureUnit"));
	glGetTextureImage = static_cast<PFNGLGETTEXTUREIMAGEPROC>(load_opengl_func("glGetTextureImage"));
	glGetCompressedTextureImage = static_cast<PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC>(load_opengl_func("glGetCompressedTextureImage"));
	glGetTextureLevelParameterfv = static_cast<PFNGLGETTEXTURELEVELPARAMETERFVPROC>(load_opengl_func("glGetTextureLevelParameterfv"));
	glGetTextureLevelParameteriv = static_cast<PFNGLGETTEXTURELEVELPARAMETERIVPROC>(load_opengl_func("glGetTextureLevelParameteriv"));
	glGetTextureParameterfv = static_cast<PFNGLGETTEXTUREPARAMETERFVPROC>(load_opengl_func("glGetTextureParameterfv"));
	glGetTextureParameterIiv = static_cast<PFNGLGETTEXTUREPARAMETERIIVPROC>(load_opengl_func("glGetTextureParameterIiv"));
	glGetTextureParameterIuiv = static_cast<PFNGLGETTEXTUREPARAMETERIUIVPROC>(load_opengl_func("glGetTextureParameterIuiv"));
	glGetTextureParameteriv = static_cast<PFNGLGETTEXTUREPARAMETERIVPROC>(load_opengl_func("glGetTextureParameteriv"));
	glCreateVertexArrays = static_cast<PFNGLCREATEVERTEXARRAYSPROC>(load_opengl_func("glCreateVertexArrays"));
	glDisableVertexArrayAttrib = static_cast<PFNGLDISABLEVERTEXARRAYATTRIBPROC>(load_opengl_func("glDisableVertexArrayAttrib"));
	glEnableVertexArrayAttrib = static_cast<PFNGLENABLEVERTEXARRAYATTRIBPROC>(load_opengl_func("glEnableVertexArrayAttrib"));
	glVertexArrayElementBuffer = static_cast<PFNGLVERTEXARRAYELEMENTBUFFERPROC>(load_opengl_func("glVertexArrayElementBuffer"));
	glVertexArrayVertexBuffer = static_cast<PFNGLVERTEXARRAYVERTEXBUFFERPROC>(load_opengl_func("glVertexArrayVertexBuffer"));
	glVertexArrayVertexBuffers = static_cast<PFNGLVERTEXARRAYVERTEXBUFFERSPROC>(load_opengl_func("glVertexArrayVertexBuffers"));
	glVertexArrayAttribBinding = static_cast<PFNGLVERTEXARRAYATTRIBBINDINGPROC>(load_opengl_func("glVertexArrayAttribBinding"));
	glVertexArrayAttribFormat = static_cast<PFNGLVERTEXARRAYATTRIBFORMATPROC>(load_opengl_func("glVertexArrayAttribFormat"));
	glVertexArrayAttribIFormat = static_cast<PFNGLVERTEXARRAYATTRIBIFORMATPROC>(load_opengl_func("glVertexArrayAttribIFormat"));
	glVertexArrayAttribLFormat = static_cast<PFNGLVERTEXARRAYATTRIBLFORMATPROC>(load_opengl_func("glVertexArrayAttribLFormat"));
	glVertexArrayBindingDivisor = static_cast<PFNGLVERTEXARRAYBINDINGDIVISORPROC>(load_opengl_func("glVertexArrayBindingDivisor"));
	glGetVertexArrayiv = static_cast<PFNGLGETVERTEXARRAYIVPROC>(load_opengl_func("glGetVertexArrayiv"));
	glGetVertexArrayIndexediv = static_cast<PFNGLGETVERTEXARRAYINDEXEDIVPROC>(load_opengl_func("glGetVertexArrayIndexediv"));
	glGetVertexArrayIndexed64iv = static_cast<PFNGLGETVERTEXARRAYINDEXED64IVPROC>(load_opengl_func("glGetVertexArrayIndexed64iv"));
	glCreateSamplers = static_cast<PFNGLCREATESAMPLERSPROC>(load_opengl_func("glCreateSamplers"));
	glCreateProgramPipelines = static_cast<PFNGLCREATEPROGRAMPIPELINESPROC>(load_opengl_func("glCreateProgramPipelines"));
	glCreateQueries = static_cast<PFNGLCREATEQUERIESPROC>(load_opengl_func("glCreateQueries"));
	glGetQueryBufferObjecti64v = static_cast<PFNGLGETQUERYBUFFEROBJECTI64VPROC>(load_opengl_func("glGetQueryBufferObjecti64v"));
	glGetQueryBufferObjectiv = static_cast<PFNGLGETQUERYBUFFEROBJECTIVPROC>(load_opengl_func("glGetQueryBufferObjectiv"));
	glGetQueryBufferObjectui64v = static_cast<PFNGLGETQUERYBUFFEROBJECTUI64VPROC>(load_opengl_func("glGetQueryBufferObjectui64v"));
	glGetQueryBufferObjectuiv = static_cast<PFNGLGETQUERYBUFFEROBJECTUIVPROC>(load_opengl_func("glGetQueryBufferObjectuiv"));
	glMemoryBarrierByRegion = static_cast<PFNGLMEMORYBARRIERBYREGIONPROC>(load_opengl_func("glMemoryBarrierByRegion"));
	glGetTextureSubImage = static_cast<PFNGLGETTEXTURESUBIMAGEPROC>(load_opengl_func("glGetTextureSubImage"));
	glGetCompressedTextureSubImage = static_cast<PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC>(load_opengl_func("glGetCompressedTextureSubImage"));
	glGetGraphicsResetStatus = static_cast<PFNGLGETGRAPHICSRESETSTATUSPROC>(load_opengl_func("glGetGraphicsResetStatus"));
	glGetnCompressedTexImage = static_cast<PFNGLGETNCOMPRESSEDTEXIMAGEPROC>(load_opengl_func("glGetnCompressedTexImage"));
	glGetnTexImage = static_cast<PFNGLGETNTEXIMAGEPROC>(load_opengl_func("glGetnTexImage"));
	glGetnUniformdv = static_cast<PFNGLGETNUNIFORMDVPROC>(load_opengl_func("glGetnUniformdv"));
	glGetnUniformfv = static_cast<PFNGLGETNUNIFORMFVPROC>(load_opengl_func("glGetnUniformfv"));
	glGetnUniformiv = static_cast<PFNGLGETNUNIFORMIVPROC>(load_opengl_func("glGetnUniformiv"));
	glGetnUniformuiv = static_cast<PFNGLGETNUNIFORMUIVPROC>(load_opengl_func("glGetnUniformuiv"));
	glReadnPixels = static_cast<PFNGLREADNPIXELSPROC>(load_opengl_func("glReadnPixels"));
	glTextureBarrier = static_cast<PFNGLTEXTUREBARRIERPROC>(load_opengl_func("glTextureBarrier"));
}

void Opengl_render_context::swap_color_buffers() noexcept {
	SwapBuffers(_hdc);
}

} // namespace

namespace cg {
namespace sys {

std::unique_ptr<IRender_context> make_win32_opengl_render_context(HWND hwnd)
{
	return std::make_unique<Opengl_render_context>(hwnd);
}

} // namespace sys
} // namespace cg