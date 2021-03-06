#include "cg/rnd/opengl/opengl_def.h"


PFNGLCULLFACEPROC glCullFace;
PFNGLFRONTFACEPROC glFrontFace;
PFNGLHINTPROC glHint;
PFNGLLINEWIDTHPROC glLineWidth;
PFNGLPOINTSIZEPROC glPointSize;
PFNGLPOLYGONMODEPROC glPolygonMode;
PFNGLSCISSORPROC glScissor;
PFNGLTEXPARAMETERFPROC glTexParameterf;
PFNGLTEXPARAMETERFVPROC glTexParameterfv;
PFNGLTEXPARAMETERIPROC glTexParameteri;
PFNGLTEXPARAMETERIVPROC glTexParameteriv;
PFNGLTEXIMAGE1DPROC glTexImage1D;
PFNGLTEXIMAGE2DPROC glTexImage2D;
PFNGLDRAWBUFFERPROC glDrawBuffer;
PFNGLCLEARPROC glClear;
PFNGLCLEARCOLORPROC glClearColor;
PFNGLCLEARSTENCILPROC glClearStencil;
PFNGLCLEARDEPTHPROC glClearDepth;
PFNGLSTENCILMASKPROC glStencilMask;
PFNGLCOLORMASKPROC glColorMask;
PFNGLDEPTHMASKPROC glDepthMask;
PFNGLDISABLEPROC glDisable;
PFNGLENABLEPROC glEnable;
PFNGLFINISHPROC glFinish;
PFNGLFLUSHPROC glFlush;
PFNGLBLENDFUNCPROC glBlendFunc;
PFNGLLOGICOPPROC glLogicOp;
PFNGLSTENCILFUNCPROC glStencilFunc;
PFNGLSTENCILOPPROC glStencilOp;
PFNGLDEPTHFUNCPROC glDepthFunc;
PFNGLPIXELSTOREFPROC glPixelStoref;
PFNGLPIXELSTOREIPROC glPixelStorei;
PFNGLREADBUFFERPROC glReadBuffer;
PFNGLREADPIXELSPROC glReadPixels;
PFNGLGETBOOLEANVPROC glGetBooleanv;
PFNGLGETDOUBLEVPROC glGetDoublev;
PFNGLGETERRORPROC glGetError;
PFNGLGETFLOATVPROC glGetFloatv;
PFNGLGETINTEGERVPROC glGetIntegerv;
PFNGLGETSTRINGPROC glGetString;
PFNGLGETTEXIMAGEPROC glGetTexImage;
PFNGLGETTEXPARAMETERFVPROC glGetTexParameterfv;
PFNGLGETTEXPARAMETERIVPROC glGetTexParameteriv;
PFNGLGETTEXLEVELPARAMETERFVPROC glGetTexLevelParameterfv;
PFNGLGETTEXLEVELPARAMETERIVPROC glGetTexLevelParameteriv;
PFNGLISENABLEDPROC glIsEnabled;
PFNGLDEPTHRANGEPROC glDepthRange;
PFNGLVIEWPORTPROC glViewport;
PFNGLDRAWARRAYSPROC glDrawArrays;
PFNGLDRAWELEMENTSPROC glDrawElements;
PFNGLGETPOINTERVPROC glGetPointerv;
PFNGLPOLYGONOFFSETPROC glPolygonOffset;
PFNGLCOPYTEXIMAGE1DPROC glCopyTexImage1D;
PFNGLCOPYTEXIMAGE2DPROC glCopyTexImage2D;
PFNGLCOPYTEXSUBIMAGE1DPROC glCopyTexSubImage1D;
PFNGLCOPYTEXSUBIMAGE2DPROC glCopyTexSubImage2D;
PFNGLTEXSUBIMAGE1DPROC glTexSubImage1D;
PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D;
PFNGLBINDTEXTUREPROC glBindTexture;
PFNGLDELETETEXTURESPROC glDeleteTextures;
PFNGLGENTEXTURESPROC glGenTextures;
PFNGLISTEXTUREPROC glIsTexture;
PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;
PFNGLTEXIMAGE3DPROC glTexImage3D;
PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;
PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D;
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLSAMPLECOVERAGEPROC glSampleCoverage;
PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
PFNGLCOMPRESSEDTEXIMAGE1DPROC glCompressedTexImage1D;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D;
PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage;
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays;
PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements;
PFNGLPOINTPARAMETERFPROC glPointParameterf;
PFNGLPOINTPARAMETERFVPROC glPointParameterfv;
PFNGLPOINTPARAMETERIPROC glPointParameteri;
PFNGLPOINTPARAMETERIVPROC glPointParameteriv;
PFNGLBLENDCOLORPROC glBlendColor;
PFNGLBLENDEQUATIONPROC glBlendEquation;
PFNGLGENQUERIESPROC glGenQueries;
PFNGLDELETEQUERIESPROC glDeleteQueries;
PFNGLISQUERYPROC glIsQuery;
PFNGLBEGINQUERYPROC glBeginQuery;
PFNGLENDQUERYPROC glEndQuery;
PFNGLGETQUERYIVPROC glGetQueryiv;
PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv;
PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLISBUFFERPROC glIsBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData;
PFNGLMAPBUFFERPROC glMapBuffer;
PFNGLUNMAPBUFFERPROC glUnmapBuffer;
PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
PFNGLGETBUFFERPOINTERVPROC glGetBufferPointerv;
PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;
PFNGLDRAWBUFFERSPROC glDrawBuffers;
PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate;
PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate;
PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDETACHSHADERPROC glDetachShader;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;
PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;
PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETSHADERSOURCEPROC glGetShaderSource;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLGETUNIFORMFVPROC glGetUniformfv;
PFNGLGETUNIFORMIVPROC glGetUniformiv;
PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv;
PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv;
PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv;
PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv;
PFNGLISPROGRAMPROC glIsProgram;
PFNGLISSHADERPROC glIsShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM2FPROC glUniform2f;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLUNIFORM4FPROC glUniform4f;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM2IPROC glUniform2i;
PFNGLUNIFORM3IPROC glUniform3i;
PFNGLUNIFORM4IPROC glUniform4i;
PFNGLUNIFORM1FVPROC glUniform1fv;
PFNGLUNIFORM2FVPROC glUniform2fv;
PFNGLUNIFORM3FVPROC glUniform3fv;
PFNGLUNIFORM4FVPROC glUniform4fv;
PFNGLUNIFORM1IVPROC glUniform1iv;
PFNGLUNIFORM2IVPROC glUniform2iv;
PFNGLUNIFORM3IVPROC glUniform3iv;
PFNGLUNIFORM4IVPROC glUniform4iv;
PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLVALIDATEPROGRAMPROC glValidateProgram;
PFNGLVERTEXATTRIB1DPROC glVertexAttrib1d;
PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dv;
PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f;
PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv;
PFNGLVERTEXATTRIB1SPROC glVertexAttrib1s;
PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1sv;
PFNGLVERTEXATTRIB2DPROC glVertexAttrib2d;
PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dv;
PFNGLVERTEXATTRIB2FPROC glVertexAttrib2f;
PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv;
PFNGLVERTEXATTRIB2SPROC glVertexAttrib2s;
PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2sv;
PFNGLVERTEXATTRIB3DPROC glVertexAttrib3d;
PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dv;
PFNGLVERTEXATTRIB3FPROC glVertexAttrib3f;
PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv;
PFNGLVERTEXATTRIB3SPROC glVertexAttrib3s;
PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3sv;
PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4Nbv;
PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4Niv;
PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4Nsv;
PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4Nub;
PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4Nubv;
PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4Nuiv;
PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4Nusv;
PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bv;
PFNGLVERTEXATTRIB4DPROC glVertexAttrib4d;
PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dv;
PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f;
PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv;
PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4iv;
PFNGLVERTEXATTRIB4SPROC glVertexAttrib4s;
PFNGLVERTEXATTRIB4SVPROC glVertexAttrib4sv;
PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubv;
PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uiv;
PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usv;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv;
PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv;
PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv;
PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv;
PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv;
PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv;
PFNGLCOLORMASKIPROC glColorMaski;
PFNGLGETBOOLEANI_VPROC glGetBooleani_v;
PFNGLGETINTEGERI_VPROC glGetIntegeri_v;
PFNGLENABLEIPROC glEnablei;
PFNGLDISABLEIPROC glDisablei;
PFNGLISENABLEDIPROC glIsEnabledi;
PFNGLBEGINTRANSFORMFEEDBACKPROC glBeginTransformFeedback;
PFNGLENDTRANSFORMFEEDBACKPROC glEndTransformFeedback;
PFNGLBINDBUFFERRANGEPROC glBindBufferRange;
PFNGLBINDBUFFERBASEPROC glBindBufferBase;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC glTransformFeedbackVaryings;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying;
PFNGLCLAMPCOLORPROC glClampColor;
PFNGLBEGINCONDITIONALRENDERPROC glBeginConditionalRender;
PFNGLENDCONDITIONALRENDERPROC glEndConditionalRender;
PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
PFNGLGETVERTEXATTRIBIIVPROC glGetVertexAttribIiv;
PFNGLGETVERTEXATTRIBIUIVPROC glGetVertexAttribIuiv;
PFNGLVERTEXATTRIBI1IPROC glVertexAttribI1i;
PFNGLVERTEXATTRIBI2IPROC glVertexAttribI2i;
PFNGLVERTEXATTRIBI3IPROC glVertexAttribI3i;
PFNGLVERTEXATTRIBI4IPROC glVertexAttribI4i;
PFNGLVERTEXATTRIBI1UIPROC glVertexAttribI1ui;
PFNGLVERTEXATTRIBI2UIPROC glVertexAttribI2ui;
PFNGLVERTEXATTRIBI3UIPROC glVertexAttribI3ui;
PFNGLVERTEXATTRIBI4UIPROC glVertexAttribI4ui;
PFNGLVERTEXATTRIBI1IVPROC glVertexAttribI1iv;
PFNGLVERTEXATTRIBI2IVPROC glVertexAttribI2iv;
PFNGLVERTEXATTRIBI3IVPROC glVertexAttribI3iv;
PFNGLVERTEXATTRIBI4IVPROC glVertexAttribI4iv;
PFNGLVERTEXATTRIBI1UIVPROC glVertexAttribI1uiv;
PFNGLVERTEXATTRIBI2UIVPROC glVertexAttribI2uiv;
PFNGLVERTEXATTRIBI3UIVPROC glVertexAttribI3uiv;
PFNGLVERTEXATTRIBI4UIVPROC glVertexAttribI4uiv;
PFNGLVERTEXATTRIBI4BVPROC glVertexAttribI4bv;
PFNGLVERTEXATTRIBI4SVPROC glVertexAttribI4sv;
PFNGLVERTEXATTRIBI4UBVPROC glVertexAttribI4ubv;
PFNGLVERTEXATTRIBI4USVPROC glVertexAttribI4usv;
PFNGLGETUNIFORMUIVPROC glGetUniformuiv;
PFNGLBINDFRAGDATALOCATIONPROC glBindFragDataLocation;
PFNGLGETFRAGDATALOCATIONPROC glGetFragDataLocation;
PFNGLUNIFORM1UIPROC glUniform1ui;
PFNGLUNIFORM2UIPROC glUniform2ui;
PFNGLUNIFORM3UIPROC glUniform3ui;
PFNGLUNIFORM4UIPROC glUniform4ui;
PFNGLUNIFORM1UIVPROC glUniform1uiv;
PFNGLUNIFORM2UIVPROC glUniform2uiv;
PFNGLUNIFORM3UIVPROC glUniform3uiv;
PFNGLUNIFORM4UIVPROC glUniform4uiv;
PFNGLTEXPARAMETERIIVPROC glTexParameterIiv;
PFNGLTEXPARAMETERIUIVPROC glTexParameterIuiv;
PFNGLGETTEXPARAMETERIIVPROC glGetTexParameterIiv;
PFNGLGETTEXPARAMETERIUIVPROC glGetTexParameterIuiv;
PFNGLCLEARBUFFERIVPROC glClearBufferiv;
PFNGLCLEARBUFFERUIVPROC glClearBufferuiv;
PFNGLCLEARBUFFERFVPROC glClearBufferfv;
PFNGLCLEARBUFFERFIPROC glClearBufferfi;
PFNGLGETSTRINGIPROC glGetStringi;
PFNGLISRENDERBUFFERPROC glIsRenderbuffer;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv;
PFNGLISFRAMEBUFFERPROC glIsFramebuffer;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
PFNGLFRAMEBUFFERTEXTURE1DPROC glFramebufferTexture1D;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer;
PFNGLMAPBUFFERRANGEPROC glMapBufferRange;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLISVERTEXARRAYPROC glIsVertexArray;
PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced;
PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
PFNGLTEXBUFFERPROC glTexBuffer;
PFNGLPRIMITIVERESTARTINDEXPROC glPrimitiveRestartIndex;
PFNGLCOPYBUFFERSUBDATAPROC glCopyBufferSubData;
PFNGLGETUNIFORMINDICESPROC glGetUniformIndices;
PFNGLGETACTIVEUNIFORMSIVPROC glGetActiveUniformsiv;
PFNGLGETACTIVEUNIFORMNAMEPROC glGetActiveUniformName;
PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC glGetActiveUniformBlockiv;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName;
PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding;
PFNGLDRAWELEMENTSBASEVERTEXPROC glDrawElementsBaseVertex;
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glDrawRangeElementsBaseVertex;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex;
PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glMultiDrawElementsBaseVertex;
PFNGLPROVOKINGVERTEXPROC glProvokingVertex;
PFNGLFENCESYNCPROC glFenceSync;
PFNGLISSYNCPROC glIsSync;
PFNGLDELETESYNCPROC glDeleteSync;
PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
PFNGLWAITSYNCPROC glWaitSync;
PFNGLGETINTEGER64VPROC glGetInteger64v;
PFNGLGETSYNCIVPROC glGetSynciv;
PFNGLGETINTEGER64I_VPROC glGetInteger64i_v;
PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v;
PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;
PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample;
PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample;
PFNGLGETMULTISAMPLEFVPROC glGetMultisamplefv;
PFNGLSAMPLEMASKIPROC glSampleMaski;
PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glBindFragDataLocationIndexed;
PFNGLGETFRAGDATAINDEXPROC glGetFragDataIndex;
PFNGLGENSAMPLERSPROC glGenSamplers;
PFNGLDELETESAMPLERSPROC glDeleteSamplers;
PFNGLISSAMPLERPROC glIsSampler;
PFNGLBINDSAMPLERPROC glBindSampler;
PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri;
PFNGLSAMPLERPARAMETERIVPROC glSamplerParameteriv;
PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf;
PFNGLSAMPLERPARAMETERFVPROC glSamplerParameterfv;
PFNGLSAMPLERPARAMETERIIVPROC glSamplerParameterIiv;
PFNGLSAMPLERPARAMETERIUIVPROC glSamplerParameterIuiv;
PFNGLGETSAMPLERPARAMETERIVPROC glGetSamplerParameteriv;
PFNGLGETSAMPLERPARAMETERIIVPROC glGetSamplerParameterIiv;
PFNGLGETSAMPLERPARAMETERFVPROC glGetSamplerParameterfv;
PFNGLGETSAMPLERPARAMETERIUIVPROC glGetSamplerParameterIuiv;
PFNGLQUERYCOUNTERPROC glQueryCounter;
PFNGLGETQUERYOBJECTI64VPROC glGetQueryObjecti64v;
PFNGLGETQUERYOBJECTUI64VPROC glGetQueryObjectui64v;
PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;
PFNGLVERTEXATTRIBP1UIPROC glVertexAttribP1ui;
PFNGLVERTEXATTRIBP1UIVPROC glVertexAttribP1uiv;
PFNGLVERTEXATTRIBP2UIPROC glVertexAttribP2ui;
PFNGLVERTEXATTRIBP2UIVPROC glVertexAttribP2uiv;
PFNGLVERTEXATTRIBP3UIPROC glVertexAttribP3ui;
PFNGLVERTEXATTRIBP3UIVPROC glVertexAttribP3uiv;
PFNGLVERTEXATTRIBP4UIPROC glVertexAttribP4ui;
PFNGLVERTEXATTRIBP4UIVPROC glVertexAttribP4uiv;
PFNGLMINSAMPLESHADINGPROC glMinSampleShading;
PFNGLBLENDEQUATIONIPROC glBlendEquationi;
PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei;
PFNGLBLENDFUNCIPROC glBlendFunci;
PFNGLBLENDFUNCSEPARATEIPROC glBlendFuncSeparatei;
PFNGLDRAWARRAYSINDIRECTPROC glDrawArraysIndirect;
PFNGLDRAWELEMENTSINDIRECTPROC glDrawElementsIndirect;
PFNGLUNIFORM1DPROC glUniform1d;
PFNGLUNIFORM2DPROC glUniform2d;
PFNGLUNIFORM3DPROC glUniform3d;
PFNGLUNIFORM4DPROC glUniform4d;
PFNGLUNIFORM1DVPROC glUniform1dv;
PFNGLUNIFORM2DVPROC glUniform2dv;
PFNGLUNIFORM3DVPROC glUniform3dv;
PFNGLUNIFORM4DVPROC glUniform4dv;
PFNGLUNIFORMMATRIX2DVPROC glUniformMatrix2dv;
PFNGLUNIFORMMATRIX3DVPROC glUniformMatrix3dv;
PFNGLUNIFORMMATRIX4DVPROC glUniformMatrix4dv;
PFNGLUNIFORMMATRIX2X3DVPROC glUniformMatrix2x3dv;
PFNGLUNIFORMMATRIX2X4DVPROC glUniformMatrix2x4dv;
PFNGLUNIFORMMATRIX3X2DVPROC glUniformMatrix3x2dv;
PFNGLUNIFORMMATRIX3X4DVPROC glUniformMatrix3x4dv;
PFNGLUNIFORMMATRIX4X2DVPROC glUniformMatrix4x2dv;
PFNGLUNIFORMMATRIX4X3DVPROC glUniformMatrix4x3dv;
PFNGLGETUNIFORMDVPROC glGetUniformdv;
PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC glGetSubroutineUniformLocation;
PFNGLGETSUBROUTINEINDEXPROC glGetSubroutineIndex;
PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC glGetActiveSubroutineUniformiv;
PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC glGetActiveSubroutineUniformName;
PFNGLGETACTIVESUBROUTINENAMEPROC glGetActiveSubroutineName;
PFNGLUNIFORMSUBROUTINESUIVPROC glUniformSubroutinesuiv;
PFNGLGETUNIFORMSUBROUTINEUIVPROC glGetUniformSubroutineuiv;
PFNGLGETPROGRAMSTAGEIVPROC glGetProgramStageiv;
PFNGLPATCHPARAMETERIPROC glPatchParameteri;
PFNGLPATCHPARAMETERFVPROC glPatchParameterfv;
PFNGLBINDTRANSFORMFEEDBACKPROC glBindTransformFeedback;
PFNGLDELETETRANSFORMFEEDBACKSPROC glDeleteTransformFeedbacks;
PFNGLGENTRANSFORMFEEDBACKSPROC glGenTransformFeedbacks;
PFNGLISTRANSFORMFEEDBACKPROC glIsTransformFeedback;
PFNGLPAUSETRANSFORMFEEDBACKPROC glPauseTransformFeedback;
PFNGLRESUMETRANSFORMFEEDBACKPROC glResumeTransformFeedback;
PFNGLDRAWTRANSFORMFEEDBACKPROC glDrawTransformFeedback;
PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC glDrawTransformFeedbackStream;
PFNGLBEGINQUERYINDEXEDPROC glBeginQueryIndexed;
PFNGLENDQUERYINDEXEDPROC glEndQueryIndexed;
PFNGLGETQUERYINDEXEDIVPROC glGetQueryIndexediv;
PFNGLRELEASESHADERCOMPILERPROC glReleaseShaderCompiler;
PFNGLSHADERBINARYPROC glShaderBinary;
PFNGLGETSHADERPRECISIONFORMATPROC glGetShaderPrecisionFormat;
PFNGLDEPTHRANGEFPROC glDepthRangef;
PFNGLCLEARDEPTHFPROC glClearDepthf;
PFNGLGETPROGRAMBINARYPROC glGetProgramBinary;
PFNGLPROGRAMBINARYPROC glProgramBinary;
PFNGLPROGRAMPARAMETERIPROC glProgramParameteri;
PFNGLUSEPROGRAMSTAGESPROC glUseProgramStages;
PFNGLACTIVESHADERPROGRAMPROC glActiveShaderProgram;
PFNGLCREATESHADERPROGRAMVPROC glCreateShaderProgramv;
PFNGLBINDPROGRAMPIPELINEPROC glBindProgramPipeline;
PFNGLDELETEPROGRAMPIPELINESPROC glDeleteProgramPipelines;
PFNGLGENPROGRAMPIPELINESPROC glGenProgramPipelines;
PFNGLISPROGRAMPIPELINEPROC glIsProgramPipeline;
PFNGLGETPROGRAMPIPELINEIVPROC glGetProgramPipelineiv;
PFNGLPROGRAMUNIFORM1IPROC glProgramUniform1i;
PFNGLPROGRAMUNIFORM1IVPROC glProgramUniform1iv;
PFNGLPROGRAMUNIFORM1FPROC glProgramUniform1f;
PFNGLPROGRAMUNIFORM1FVPROC glProgramUniform1fv;
PFNGLPROGRAMUNIFORM1DPROC glProgramUniform1d;
PFNGLPROGRAMUNIFORM1DVPROC glProgramUniform1dv;
PFNGLPROGRAMUNIFORM1UIPROC glProgramUniform1ui;
PFNGLPROGRAMUNIFORM1UIVPROC glProgramUniform1uiv;
PFNGLPROGRAMUNIFORM2IPROC glProgramUniform2i;
PFNGLPROGRAMUNIFORM2IVPROC glProgramUniform2iv;
PFNGLPROGRAMUNIFORM2FPROC glProgramUniform2f;
PFNGLPROGRAMUNIFORM2FVPROC glProgramUniform2fv;
PFNGLPROGRAMUNIFORM2DPROC glProgramUniform2d;
PFNGLPROGRAMUNIFORM2DVPROC glProgramUniform2dv;
PFNGLPROGRAMUNIFORM2UIPROC glProgramUniform2ui;
PFNGLPROGRAMUNIFORM2UIVPROC glProgramUniform2uiv;
PFNGLPROGRAMUNIFORM3IPROC glProgramUniform3i;
PFNGLPROGRAMUNIFORM3IVPROC glProgramUniform3iv;
PFNGLPROGRAMUNIFORM3FPROC glProgramUniform3f;
PFNGLPROGRAMUNIFORM3FVPROC glProgramUniform3fv;
PFNGLPROGRAMUNIFORM3DPROC glProgramUniform3d;
PFNGLPROGRAMUNIFORM3DVPROC glProgramUniform3dv;
PFNGLPROGRAMUNIFORM3UIPROC glProgramUniform3ui;
PFNGLPROGRAMUNIFORM3UIVPROC glProgramUniform3uiv;
PFNGLPROGRAMUNIFORM4IPROC glProgramUniform4i;
PFNGLPROGRAMUNIFORM4IVPROC glProgramUniform4iv;
PFNGLPROGRAMUNIFORM4FPROC glProgramUniform4f;
PFNGLPROGRAMUNIFORM4FVPROC glProgramUniform4fv;
PFNGLPROGRAMUNIFORM4DPROC glProgramUniform4d;
PFNGLPROGRAMUNIFORM4DVPROC glProgramUniform4dv;
PFNGLPROGRAMUNIFORM4UIPROC glProgramUniform4ui;
PFNGLPROGRAMUNIFORM4UIVPROC glProgramUniform4uiv;
PFNGLPROGRAMUNIFORMMATRIX2FVPROC glProgramUniformMatrix2fv;
PFNGLPROGRAMUNIFORMMATRIX3FVPROC glProgramUniformMatrix3fv;
PFNGLPROGRAMUNIFORMMATRIX4FVPROC glProgramUniformMatrix4fv;
PFNGLPROGRAMUNIFORMMATRIX2DVPROC glProgramUniformMatrix2dv;
PFNGLPROGRAMUNIFORMMATRIX3DVPROC glProgramUniformMatrix3dv;
PFNGLPROGRAMUNIFORMMATRIX4DVPROC glProgramUniformMatrix4dv;
PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glProgramUniformMatrix2x3fv;
PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glProgramUniformMatrix3x2fv;
PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glProgramUniformMatrix2x4fv;
PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glProgramUniformMatrix4x2fv;
PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glProgramUniformMatrix3x4fv;
PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glProgramUniformMatrix4x3fv;
PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC glProgramUniformMatrix2x3dv;
PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC glProgramUniformMatrix3x2dv;
PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC glProgramUniformMatrix2x4dv;
PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC glProgramUniformMatrix4x2dv;
PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC glProgramUniformMatrix3x4dv;
PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC glProgramUniformMatrix4x3dv;
PFNGLVALIDATEPROGRAMPIPELINEPROC glValidateProgramPipeline;
PFNGLGETPROGRAMPIPELINEINFOLOGPROC glGetProgramPipelineInfoLog;
PFNGLVERTEXATTRIBL1DPROC glVertexAttribL1d;
PFNGLVERTEXATTRIBL2DPROC glVertexAttribL2d;
PFNGLVERTEXATTRIBL3DPROC glVertexAttribL3d;
PFNGLVERTEXATTRIBL4DPROC glVertexAttribL4d;
PFNGLVERTEXATTRIBL1DVPROC glVertexAttribL1dv;
PFNGLVERTEXATTRIBL2DVPROC glVertexAttribL2dv;
PFNGLVERTEXATTRIBL3DVPROC glVertexAttribL3dv;
PFNGLVERTEXATTRIBL4DVPROC glVertexAttribL4dv;
PFNGLVERTEXATTRIBLPOINTERPROC glVertexAttribLPointer;
PFNGLGETVERTEXATTRIBLDVPROC glGetVertexAttribLdv;
PFNGLVIEWPORTARRAYVPROC glViewportArrayv;
PFNGLVIEWPORTINDEXEDFPROC glViewportIndexedf;
PFNGLVIEWPORTINDEXEDFVPROC glViewportIndexedfv;
PFNGLSCISSORARRAYVPROC glScissorArrayv;
PFNGLSCISSORINDEXEDPROC glScissorIndexed;
PFNGLSCISSORINDEXEDVPROC glScissorIndexedv;
PFNGLDEPTHRANGEARRAYVPROC glDepthRangeArrayv;
PFNGLDEPTHRANGEINDEXEDPROC glDepthRangeIndexed;
PFNGLGETFLOATI_VPROC glGetFloati_v;
PFNGLGETDOUBLEI_VPROC glGetDoublei_v;
PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC glDrawArraysInstancedBaseInstance;
PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC glDrawElementsInstancedBaseInstance;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC glDrawElementsInstancedBaseVertexBaseInstance;
PFNGLGETINTERNALFORMATIVPROC glGetInternalformativ;
PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC glGetActiveAtomicCounterBufferiv;
PFNGLBINDIMAGETEXTUREPROC glBindImageTexture;
PFNGLMEMORYBARRIERPROC glMemoryBarrier;
PFNGLTEXSTORAGE1DPROC glTexStorage1D;
PFNGLTEXSTORAGE2DPROC glTexStorage2D;
PFNGLTEXSTORAGE3DPROC glTexStorage3D;
PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC glDrawTransformFeedbackInstanced;
PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC glDrawTransformFeedbackStreamInstanced;
PFNGLCLEARBUFFERDATAPROC glClearBufferData;
PFNGLCLEARBUFFERSUBDATAPROC glClearBufferSubData;
PFNGLDISPATCHCOMPUTEPROC glDispatchCompute;
PFNGLDISPATCHCOMPUTEINDIRECTPROC glDispatchComputeIndirect;
PFNGLCOPYIMAGESUBDATAPROC glCopyImageSubData;
PFNGLFRAMEBUFFERPARAMETERIPROC glFramebufferParameteri;
PFNGLGETFRAMEBUFFERPARAMETERIVPROC glGetFramebufferParameteriv;
PFNGLGETINTERNALFORMATI64VPROC glGetInternalformati64v;
PFNGLINVALIDATETEXSUBIMAGEPROC glInvalidateTexSubImage;
PFNGLINVALIDATETEXIMAGEPROC glInvalidateTexImage;
PFNGLINVALIDATEBUFFERSUBDATAPROC glInvalidateBufferSubData;
PFNGLINVALIDATEBUFFERDATAPROC glInvalidateBufferData;
PFNGLINVALIDATEFRAMEBUFFERPROC glInvalidateFramebuffer;
PFNGLINVALIDATESUBFRAMEBUFFERPROC glInvalidateSubFramebuffer;
PFNGLMULTIDRAWARRAYSINDIRECTPROC glMultiDrawArraysIndirect;
PFNGLMULTIDRAWELEMENTSINDIRECTPROC glMultiDrawElementsIndirect;
PFNGLGETPROGRAMINTERFACEIVPROC glGetProgramInterfaceiv;
PFNGLGETPROGRAMRESOURCEINDEXPROC glGetProgramResourceIndex;
PFNGLGETPROGRAMRESOURCENAMEPROC glGetProgramResourceName;
PFNGLGETPROGRAMRESOURCEIVPROC glGetProgramResourceiv;
PFNGLGETPROGRAMRESOURCELOCATIONPROC glGetProgramResourceLocation;
PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC glGetProgramResourceLocationIndex;
PFNGLSHADERSTORAGEBLOCKBINDINGPROC glShaderStorageBlockBinding;
PFNGLTEXBUFFERRANGEPROC glTexBufferRange;
PFNGLTEXSTORAGE2DMULTISAMPLEPROC glTexStorage2DMultisample;
PFNGLTEXSTORAGE3DMULTISAMPLEPROC glTexStorage3DMultisample;
PFNGLTEXTUREVIEWPROC glTextureView;
PFNGLBINDVERTEXBUFFERPROC glBindVertexBuffer;
PFNGLVERTEXATTRIBFORMATPROC glVertexAttribFormat;
PFNGLVERTEXATTRIBIFORMATPROC glVertexAttribIFormat;
PFNGLVERTEXATTRIBLFORMATPROC glVertexAttribLFormat;
PFNGLVERTEXATTRIBBINDINGPROC glVertexAttribBinding;
PFNGLVERTEXBINDINGDIVISORPROC glVertexBindingDivisor;
PFNGLDEBUGMESSAGECONTROLPROC glDebugMessageControl;
PFNGLDEBUGMESSAGEINSERTPROC glDebugMessageInsert;
PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
PFNGLGETDEBUGMESSAGELOGPROC glGetDebugMessageLog;
PFNGLPUSHDEBUGGROUPPROC glPushDebugGroup;
PFNGLPOPDEBUGGROUPPROC glPopDebugGroup;
PFNGLOBJECTLABELPROC glObjectLabel;
PFNGLGETOBJECTLABELPROC glGetObjectLabel;
PFNGLOBJECTPTRLABELPROC glObjectPtrLabel;
PFNGLGETOBJECTPTRLABELPROC glGetObjectPtrLabel;
PFNGLBUFFERSTORAGEPROC glBufferStorage;
PFNGLCLEARTEXIMAGEPROC glClearTexImage;
PFNGLCLEARTEXSUBIMAGEPROC glClearTexSubImage;
PFNGLBINDBUFFERSBASEPROC glBindBuffersBase;
PFNGLBINDBUFFERSRANGEPROC glBindBuffersRange;
PFNGLBINDTEXTURESPROC glBindTextures;
PFNGLBINDSAMPLERSPROC glBindSamplers;
PFNGLBINDIMAGETEXTURESPROC glBindImageTextures;
PFNGLBINDVERTEXBUFFERSPROC glBindVertexBuffers;
PFNGLCLIPCONTROLPROC glClipControl;
PFNGLCREATETRANSFORMFEEDBACKSPROC glCreateTransformFeedbacks;
PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC glTransformFeedbackBufferBase;
PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC glTransformFeedbackBufferRange;
PFNGLGETTRANSFORMFEEDBACKIVPROC glGetTransformFeedbackiv;
PFNGLGETTRANSFORMFEEDBACKI_VPROC glGetTransformFeedbacki_v;
PFNGLGETTRANSFORMFEEDBACKI64_VPROC glGetTransformFeedbacki64_v;
PFNGLCREATEBUFFERSPROC glCreateBuffers;
PFNGLNAMEDBUFFERSTORAGEPROC glNamedBufferStorage;
PFNGLNAMEDBUFFERDATAPROC glNamedBufferData;
PFNGLNAMEDBUFFERSUBDATAPROC glNamedBufferSubData;
PFNGLCOPYNAMEDBUFFERSUBDATAPROC glCopyNamedBufferSubData;
PFNGLCLEARNAMEDBUFFERDATAPROC glClearNamedBufferData;
PFNGLCLEARNAMEDBUFFERSUBDATAPROC glClearNamedBufferSubData;
PFNGLMAPNAMEDBUFFERPROC glMapNamedBuffer;
PFNGLMAPNAMEDBUFFERRANGEPROC glMapNamedBufferRange;
PFNGLUNMAPNAMEDBUFFERPROC glUnmapNamedBuffer;
PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC glFlushMappedNamedBufferRange;
PFNGLGETNAMEDBUFFERPARAMETERIVPROC glGetNamedBufferParameteriv;
PFNGLGETNAMEDBUFFERPARAMETERI64VPROC glGetNamedBufferParameteri64v;
PFNGLGETNAMEDBUFFERPOINTERVPROC glGetNamedBufferPointerv;
PFNGLGETNAMEDBUFFERSUBDATAPROC glGetNamedBufferSubData;
PFNGLCREATEFRAMEBUFFERSPROC glCreateFramebuffers;
PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC glNamedFramebufferRenderbuffer;
PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC glNamedFramebufferParameteri;
PFNGLNAMEDFRAMEBUFFERTEXTUREPROC glNamedFramebufferTexture;
PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC glNamedFramebufferTextureLayer;
PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC glNamedFramebufferDrawBuffer;
PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC glNamedFramebufferDrawBuffers;
PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC glNamedFramebufferReadBuffer;
PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC glInvalidateNamedFramebufferData;
PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC glInvalidateNamedFramebufferSubData;
PFNGLCLEARNAMEDFRAMEBUFFERIVPROC glClearNamedFramebufferiv;
PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC glClearNamedFramebufferuiv;
PFNGLCLEARNAMEDFRAMEBUFFERFVPROC glClearNamedFramebufferfv;
PFNGLCLEARNAMEDFRAMEBUFFERFIPROC glClearNamedFramebufferfi;
PFNGLBLITNAMEDFRAMEBUFFERPROC glBlitNamedFramebuffer;
PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC glCheckNamedFramebufferStatus;
PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC glGetNamedFramebufferParameteriv;
PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetNamedFramebufferAttachmentParameteriv;
PFNGLCREATERENDERBUFFERSPROC glCreateRenderbuffers;
PFNGLNAMEDRENDERBUFFERSTORAGEPROC glNamedRenderbufferStorage;
PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC glNamedRenderbufferStorageMultisample;
PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC glGetNamedRenderbufferParameteriv;
PFNGLCREATETEXTURESPROC glCreateTextures;
PFNGLTEXTUREBUFFERPROC glTextureBuffer;
PFNGLTEXTUREBUFFERRANGEPROC glTextureBufferRange;
PFNGLTEXTURESTORAGE1DPROC glTextureStorage1D;
PFNGLTEXTURESTORAGE2DPROC glTextureStorage2D;
PFNGLTEXTURESTORAGE3DPROC glTextureStorage3D;
PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC glTextureStorage2DMultisample;
PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC glTextureStorage3DMultisample;
PFNGLTEXTURESUBIMAGE1DPROC glTextureSubImage1D;
PFNGLTEXTURESUBIMAGE2DPROC glTextureSubImage2D;
PFNGLTEXTURESUBIMAGE3DPROC glTextureSubImage3D;
PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC glCompressedTextureSubImage1D;
PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC glCompressedTextureSubImage2D;
PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC glCompressedTextureSubImage3D;
PFNGLCOPYTEXTURESUBIMAGE1DPROC glCopyTextureSubImage1D;
PFNGLCOPYTEXTURESUBIMAGE2DPROC glCopyTextureSubImage2D;
PFNGLCOPYTEXTURESUBIMAGE3DPROC glCopyTextureSubImage3D;
PFNGLTEXTUREPARAMETERFPROC glTextureParameterf;
PFNGLTEXTUREPARAMETERFVPROC glTextureParameterfv;
PFNGLTEXTUREPARAMETERIPROC glTextureParameteri;
PFNGLTEXTUREPARAMETERIIVPROC glTextureParameterIiv;
PFNGLTEXTUREPARAMETERIUIVPROC glTextureParameterIuiv;
PFNGLTEXTUREPARAMETERIVPROC glTextureParameteriv;
PFNGLGENERATETEXTUREMIPMAPPROC glGenerateTextureMipmap;
PFNGLBINDTEXTUREUNITPROC glBindTextureUnit;
PFNGLGETTEXTUREIMAGEPROC glGetTextureImage;
PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC glGetCompressedTextureImage;
PFNGLGETTEXTURELEVELPARAMETERFVPROC glGetTextureLevelParameterfv;
PFNGLGETTEXTURELEVELPARAMETERIVPROC glGetTextureLevelParameteriv;
PFNGLGETTEXTUREPARAMETERFVPROC glGetTextureParameterfv;
PFNGLGETTEXTUREPARAMETERIIVPROC glGetTextureParameterIiv;
PFNGLGETTEXTUREPARAMETERIUIVPROC glGetTextureParameterIuiv;
PFNGLGETTEXTUREPARAMETERIVPROC glGetTextureParameteriv;
PFNGLCREATEVERTEXARRAYSPROC glCreateVertexArrays;
PFNGLDISABLEVERTEXARRAYATTRIBPROC glDisableVertexArrayAttrib;
PFNGLENABLEVERTEXARRAYATTRIBPROC glEnableVertexArrayAttrib;
PFNGLVERTEXARRAYELEMENTBUFFERPROC glVertexArrayElementBuffer;
PFNGLVERTEXARRAYVERTEXBUFFERPROC glVertexArrayVertexBuffer;
PFNGLVERTEXARRAYVERTEXBUFFERSPROC glVertexArrayVertexBuffers;
PFNGLVERTEXARRAYATTRIBBINDINGPROC glVertexArrayAttribBinding;
PFNGLVERTEXARRAYATTRIBFORMATPROC glVertexArrayAttribFormat;
PFNGLVERTEXARRAYATTRIBIFORMATPROC glVertexArrayAttribIFormat;
PFNGLVERTEXARRAYATTRIBLFORMATPROC glVertexArrayAttribLFormat;
PFNGLVERTEXARRAYBINDINGDIVISORPROC glVertexArrayBindingDivisor;
PFNGLGETVERTEXARRAYIVPROC glGetVertexArrayiv;
PFNGLGETVERTEXARRAYINDEXEDIVPROC glGetVertexArrayIndexediv;
PFNGLGETVERTEXARRAYINDEXED64IVPROC glGetVertexArrayIndexed64iv;
PFNGLCREATESAMPLERSPROC glCreateSamplers;
PFNGLCREATEPROGRAMPIPELINESPROC glCreateProgramPipelines;
PFNGLCREATEQUERIESPROC glCreateQueries;
PFNGLGETQUERYBUFFEROBJECTI64VPROC glGetQueryBufferObjecti64v;
PFNGLGETQUERYBUFFEROBJECTIVPROC glGetQueryBufferObjectiv;
PFNGLGETQUERYBUFFEROBJECTUI64VPROC glGetQueryBufferObjectui64v;
PFNGLGETQUERYBUFFEROBJECTUIVPROC glGetQueryBufferObjectuiv;
PFNGLMEMORYBARRIERBYREGIONPROC glMemoryBarrierByRegion;
PFNGLGETTEXTURESUBIMAGEPROC glGetTextureSubImage;
PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC glGetCompressedTextureSubImage;
PFNGLGETGRAPHICSRESETSTATUSPROC glGetGraphicsResetStatus;
PFNGLGETNCOMPRESSEDTEXIMAGEPROC glGetnCompressedTexImage;
PFNGLGETNTEXIMAGEPROC glGetnTexImage;
PFNGLGETNUNIFORMDVPROC glGetnUniformdv;
PFNGLGETNUNIFORMFVPROC glGetnUniformfv;
PFNGLGETNUNIFORMIVPROC glGetnUniformiv;
PFNGLGETNUNIFORMUIVPROC glGetnUniformuiv;
PFNGLREADNPIXELSPROC glReadnPixels;
PFNGLTEXTUREBARRIERPROC glTextureBarrier;
