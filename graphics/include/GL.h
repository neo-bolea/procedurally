﻿#pragma once

#include "GLTypes.h"

#include "Mesh.h"
#include "Shader.h"

#define GL_STATIC
#include "GL/glew.h"

#include <string>

//TODO: Add a texture manager with global references to all of the elements by an identifier (pointer, int, hash, path, etc.).	
#define GLDEFINED
namespace GL
{
	extern ProgramPool Programs;

	void GLAPIENTRY
		ErrorCallback(
			GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			const void* userParam);

	namespace
	{
		#define StaticGLGetInteger(name, handle) \
			int name();
	}

	StaticGLGetInteger(TextureMaxSize, GL_MAX_TEXTURE_SIZE)
	StaticGLGetInteger(Texture3DMaxSize, GL_MAX_3D_TEXTURE_SIZE)

	//dynamic GL_ACTIVE_TEXTURE
	//static  GL_ALIASED_LINE_WIDTH_RANGE
	//dynamic GL_ARRAY_BUFFER_BINDING
	//dynamic GL_BLEND
	//	     //GL_BLEND_COLOR
	//	     //GL_BLEND_DST_ALPHA
	//	     //GL_BLEND_DST_RGB
	//	     //GL_BLEND_EQUATION_RGB
	//dynamic GL_BLEND_EQUATION_ALPHA
	//	     //GL_BLEND_SRC_ALPHA
	//	     //GL_BLEND_SRC_RGB
	//dynamic GL_COLOR_CLEAR_VALUE
	//	     //GL_COLOR_LOGIC_OP
	//	     //GL_COLOR_WRITEMASK
	//	     //static GL_COMPRESSED_TEXTURE_FORMATS
	//static  GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS
	//static  GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS
	//static  GL_MAX_COMPUTE_UNIFORM_BLOCKS
	//static  GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS
	//static  GL_MAX_COMPUTE_UNIFORM_COMPONENTS
	//	     //GL_MAX_COMPUTE_ATOMIC_COUNTERS
	//	     //GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS
	//	     //GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS
	//static  GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS
	//static  array GL_MAX_COMPUTE_WORK_GROUP_COUNT
		//GL_MAX_COMPUTE_WORK_GROUP_SIZE
		//Accepted by the indexed versions of glGet. data the maximum size of a work groups that may be used during compilation of a compute shader. Indices 0, 1, and 2 correspond to the X, Y and Z dimensions, respectively.
		//
		//GL_DISPATCH_INDIRECT_BUFFER_BINDING
		//data returns a single value, the name of the buffer object currently bound to the target GL_DISPATCH_INDIRECT_BUFFER. If no buffer object is bound to this target, 0 is returned. The initial value is 0. See glBindBuffer.
		//
		//GL_MAX_DEBUG_GROUP_STACK_DEPTH
		//data returns a single value, the maximum depth of the debug message group stack.
		//
		//GL_DEBUG_GROUP_STACK_DEPTH
		//data returns a single value, the current depth of the debug message group stack.
		//
		//GL_CONTEXT_FLAGS
		//data returns one value, the flags with which the context was created (such as debugging functionality).
		//
		//GL_CULL_FACE
		//data returns a single boolean value indicating whether polygon culling is enabled. The initial value is GL_FALSE. See glCullFace.
		//
		//GL_CULL_FACE_MODE
		//data returns a single value indicating the mode of polygon culling. The initial value is GL_BACK. See glCullFace.
		//
		//GL_CURRENT_PROGRAM
		//data returns one value, the name of the program object that is currently active, or 0 if no program object is active. See glUseProgram.
		//
		//GL_DEPTH_CLEAR_VALUE
		//data returns one value, the value that is used to clear the depth buffer. Integer values, if requested, are linearly mapped from the internal floating-point representation such that 1.0 returns the most positive representable integer value, and −1.0 returns the most negative representable integer value. The initial value is 1. See glClearDepth.
		//
		//GL_DEPTH_FUNC
		//data returns one value, the symbolic constant that indicates the depth comparison function. The initial value is GL_LESS. See glDepthFunc.
		//
		//GL_DEPTH_RANGE
		//data returns two values: the near and far mapping limits for the depth buffer. Integer values, if requested, are linearly mapped from the internal floating-point representation such that 1.0 returns the most positive representable integer value, and −1.0 returns the most negative representable integer value. The initial value is (0, 1). See glDepthRange.
		//
		//GL_DEPTH_TEST
		//data returns a single boolean value indicating whether depth testing of fragments is enabled. The initial value is GL_FALSE. See glDepthFunc and glDepthRange.
		//
		//GL_DEPTH_WRITEMASK
		//data returns a single boolean value indicating if the depth buffer is enabled for writing. The initial value is GL_TRUE. See glDepthMask.
		//
		//GL_DITHER
		//data returns a single boolean value indicating whether dithering of fragment colors and indices is enabled. The initial value is GL_TRUE.
		//
		//GL_DOUBLEBUFFER
		//data returns a single boolean value indicating whether double buffering is supported.
		//
		//GL_DRAW_BUFFER
		//data returns one value, a symbolic constant indicating which buffers are being drawn to. See glDrawBuffer. The initial value is GL_BACK if there are back buffers, otherwise it is GL_FRONT.
		//
		//GL_DRAW_BUFFER i
		//data returns one value, a symbolic constant indicating which buffers are being drawn to by the corresponding output color. See glDrawBuffers. The initial value of GL_DRAW_BUFFER0 is GL_BACK if there are back buffers, otherwise it is GL_FRONT. The initial values of draw buffers for all other output colors is GL_NONE.
		//
		//GL_DRAW_FRAMEBUFFER_BINDING
		//data returns one value, the name of the framebuffer object currently bound to the GL_DRAW_FRAMEBUFFER target. If the default framebuffer is bound, this value will be zero. The initial value is zero. See glBindFramebuffer.
		//
		//GL_READ_FRAMEBUFFER_BINDING
		//data returns one value, the name of the framebuffer object currently bound to the GL_READ_FRAMEBUFFER target. If the default framebuffer is bound, this value will be zero. The initial value is zero. See glBindFramebuffer.
		//
		//GL_ELEMENT_ARRAY_BUFFER_BINDING
		//data returns a single value, the name of the buffer object currently bound to the target GL_ELEMENT_ARRAY_BUFFER. If no buffer object is bound to this target, 0 is returned. The initial value is 0. See glBindBuffer.
		//
		//GL_FRAGMENT_SHADER_DERIVATIVE_HINT
		//data returns one value, a symbolic constant indicating the mode of the derivative accuracy hint for fragment shaders. The initial value is GL_DONT_CARE. See glHint.
		//
		//GL_IMPLEMENTATION_COLOR_READ_FORMAT
		//data returns a single GLenum value indicating the implementation's preferred pixel data format. See glReadPixels.
		//
		//GL_IMPLEMENTATION_COLOR_READ_TYPE
		//data returns a single GLenum value indicating the implementation's preferred pixel data type. See glReadPixels.
		//
		//GL_LINE_SMOOTH
		//data returns a single boolean value indicating whether antialiasing of lines is enabled. The initial value is GL_FALSE. See glLineWidth.
		//
		//GL_LINE_SMOOTH_HINT
		//data returns one value, a symbolic constant indicating the mode of the line antialiasing hint. The initial value is GL_DONT_CARE. See glHint.
		//
		//GL_LINE_WIDTH
		//data returns one value, the line width as specified with glLineWidth. The initial value is 1.
		//
		//GL_LAYER_PROVOKING_VERTEX
		//data returns one value, the implementation dependent specifc vertex of a primitive that is used to select the rendering layer. If the value returned is equivalent to GL_PROVOKING_VERTEX, then the vertex selection follows the convention specified by glProvokingVertex. If the value returned is equivalent to GL_FIRST_VERTEX_CONVENTION, then the selection is always taken from the first vertex in the primitive. If the value returned is equivalent to GL_LAST_VERTEX_CONVENTION, then the selection is always taken from the last vertex in the primitive. If the value returned is equivalent to GL_UNDEFINED_VERTEX, then the selection is not guaranteed to be taken from any specific vertex in the primitive.
		//
		//GL_LOGIC_OP_MODE
		//data returns one value, a symbolic constant indicating the selected logic operation mode. The initial value is GL_COPY. See glLogicOp.
		//
		//GL_MAJOR_VERSION
		//data returns one value, the major version number of the OpenGL API supported by the current context.
		//
		//GL_MAX_3D_TEXTURE_SIZE
		//data returns one value, a rough estimate of the largest 3D texture that the GL can handle. The value must be at least 64. Use GL_PROXY_TEXTURE_3D to determine if a texture is too large. See glTexImage3D.
		//
		//GL_MAX_ARRAY_TEXTURE_LAYERS
		//data returns one value. The value indicates the maximum number of layers allowed in an array texture, and must be at least 256. See glTexImage2D.
		//
		//GL_MAX_CLIP_DISTANCES
		//data returns one value, the maximum number of application-defined clipping distances. The value must be at least 8.
		//
		//GL_MAX_COLOR_TEXTURE_SAMPLES
		//data returns one value, the maximum number of samples in a color multisample texture.
		//
		//GL_MAX_COMBINED_ATOMIC_COUNTERS
		//data returns a single value, the maximum number of atomic counters available to all active shaders.
		//
		//GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS
		//data returns one value, the number of words for fragment shader uniform variables in all uniform blocks (including default). The value must be at least 1. See glUniform.
		//
		//GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS
		//data returns one value, the number of words for geometry shader uniform variables in all uniform blocks (including default). The value must be at least 1. See glUniform.
		//
		//GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
		//data returns one value, the maximum supported texture image units that can be used to access texture maps from the vertex shader and the fragment processor combined. If both the vertex shader and the fragment processing stage access the same texture image unit, then that counts as using two texture image units against this limit. The value must be at least 48. See glActiveTexture.
		//
		//GL_MAX_COMBINED_UNIFORM_BLOCKS
		//data returns one value, the maximum number of uniform blocks per program. The value must be at least 70. See glUniformBlockBinding.
		//
		//GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS
		//data returns one value, the number of words for vertex shader uniform variables in all uniform blocks (including default). The value must be at least 1. See glUniform.
		//
		//GL_MAX_CUBE_MAP_TEXTURE_SIZE
		//data returns one value. The value gives a rough estimate of the largest cube-map texture that the GL can handle. The value must be at least 1024. Use GL_PROXY_TEXTURE_CUBE_MAP to determine if a texture is too large. See glTexImage2D.
		//
		//GL_MAX_DEPTH_TEXTURE_SAMPLES
		//data returns one value, the maximum number of samples in a multisample depth or depth-stencil texture.
		//
		//GL_MAX_DRAW_BUFFERS
		//data returns one value, the maximum number of simultaneous outputs that may be written in a fragment shader. The value must be at least 8. See glDrawBuffers.
		//
		//GL_MAX_DUAL_SOURCE_DRAW_BUFFERS
		//data returns one value, the maximum number of active draw buffers when using dual-source blending. The value must be at least 1. See glBlendFunc and glBlendFuncSeparate.
		//
		//GL_MAX_ELEMENTS_INDICES
		//data returns one value, the recommended maximum number of vertex array indices. See glDrawRangeElements.
		//
		//GL_MAX_ELEMENTS_VERTICES
		//data returns one value, the recommended maximum number of vertex array vertices. See glDrawRangeElements.
		//
		//GL_MAX_FRAGMENT_ATOMIC_COUNTERS
		//data returns a single value, the maximum number of atomic counters available to fragment shaders.
		//
		//GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS
		//data returns one value, the maximum number of active shader storage blocks that may be accessed by a fragment shader.
		//
		//GL_MAX_FRAGMENT_INPUT_COMPONENTS
		//data returns one value, the maximum number of components of the inputs read by the fragment shader, which must be at least 128.
		//
		//GL_MAX_FRAGMENT_UNIFORM_COMPONENTS
		//data returns one value, the maximum number of individual floating-point, integer, or boolean values that can be held in uniform variable storage for a fragment shader. The value must be at least 1024. See glUniform.
		//
		//GL_MAX_FRAGMENT_UNIFORM_VECTORS
		//data returns one value, the maximum number of individual 4-vectors of floating-point, integer, or boolean values that can be held in uniform variable storage for a fragment shader. The value is equal to the value of GL_MAX_FRAGMENT_UNIFORM_COMPONENTS divided by 4 and must be at least 256. See glUniform.
		//
		//GL_MAX_FRAGMENT_UNIFORM_BLOCKS
		//data returns one value, the maximum number of uniform blocks per fragment shader. The value must be at least 12. See glUniformBlockBinding.
		//
		//GL_MAX_FRAMEBUFFER_WIDTH
		//data returns one value, the maximum width for a framebuffer that has no attachments, which must be at least 16384. See glFramebufferParameter.
		//
		//GL_MAX_FRAMEBUFFER_HEIGHT
		//data returns one value, the maximum height for a framebuffer that has no attachments, which must be at least 16384. See glFramebufferParameter.
		//
		//GL_MAX_FRAMEBUFFER_LAYERS
		//data returns one value, the maximum number of layers for a framebuffer that has no attachments, which must be at least 2048. See glFramebufferParameter.
		//
		//GL_MAX_FRAMEBUFFER_SAMPLES
		//data returns one value, the maximum samples in a framebuffer that has no attachments, which must be at least 4. See glFramebufferParameter.
		//
		//GL_MAX_GEOMETRY_ATOMIC_COUNTERS
		//data returns a single value, the maximum number of atomic counters available to geometry shaders.
		//
		//GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS
		//data returns one value, the maximum number of active shader storage blocks that may be accessed by a geometry shader.
		//
		//GL_MAX_GEOMETRY_INPUT_COMPONENTS
		//data returns one value, the maximum number of components of inputs read by a geometry shader, which must be at least 64.
		//
		//GL_MAX_GEOMETRY_OUTPUT_COMPONENTS
		//data returns one value, the maximum number of components of outputs written by a geometry shader, which must be at least 128.
		//
		//GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS
		//data returns one value, the maximum supported texture image units that can be used to access texture maps from the geometry shader. The value must be at least 16. See glActiveTexture.
		//
		//GL_MAX_GEOMETRY_UNIFORM_BLOCKS
		//data returns one value, the maximum number of uniform blocks per geometry shader. The value must be at least 12. See glUniformBlockBinding.
		//
		//GL_MAX_GEOMETRY_UNIFORM_COMPONENTS
		//data returns one value, the maximum number of individual floating-point, integer, or boolean values that can be held in uniform variable storage for a geometry shader. The value must be at least 1024. See glUniform.
		//
		//GL_MAX_INTEGER_SAMPLES
		//data returns one value, the maximum number of samples supported in integer format multisample buffers.
		//
		//GL_MIN_MAP_BUFFER_ALIGNMENT
		//data returns one value, the minimum alignment in basic machine units of pointers returned fromglMapBuffer and glMapBufferRange. This value must be a power of two and must be at least 64.
		//
		//GL_MAX_LABEL_LENGTH
		//data returns one value, the maximum length of a label that may be assigned to an object. See glObjectLabel and glObjectPtrLabel.
		//
		//GL_MAX_PROGRAM_TEXEL_OFFSET
		//data returns one value, the maximum texel offset allowed in a texture lookup, which must be at least 7.
		//
		//GL_MIN_PROGRAM_TEXEL_OFFSET
		//data returns one value, the minimum texel offset allowed in a texture lookup, which must be at most -8.
		//
		//GL_MAX_RECTANGLE_TEXTURE_SIZE
		//data returns one value. The value gives a rough estimate of the largest rectangular texture that the GL can handle. The value must be at least 1024. Use GL_PROXY_TEXTURE_RECTANGLE to determine if a texture is too large. See glTexImage2D.
		//
		//GL_MAX_RENDERBUFFER_SIZE
		//data returns one value. The value indicates the maximum supported size for renderbuffers. See glFramebufferRenderbuffer.
		//
		//GL_MAX_SAMPLE_MASK_WORDS
		//data returns one value, the maximum number of sample mask words.
		//
		//GL_MAX_SERVER_WAIT_TIMEOUT
		//data returns one value, the maximum glWaitSync timeout interval.
		//
		//GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS
		//data returns one value, the maximum number of shader storage buffer binding points on the context, which must be at least 8.
		//
		//GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS
		//data returns a single value, the maximum number of atomic counters available to tessellation control shaders.
		//
		//GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS
		//data returns a single value, the maximum number of atomic counters available to tessellation evaluation shaders.
		//
		//GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS
		//data returns one value, the maximum number of active shader storage blocks that may be accessed by a tessellation control shader.
		//
		//GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS
		//data returns one value, the maximum number of active shader storage blocks that may be accessed by a tessellation evaluation shader.
		//
		//GL_MAX_TEXTURE_BUFFER_SIZE
		//data returns one value. The value gives the maximum number of texels allowed in the texel array of a texture buffer object. Value must be at least 65536.
		//
		//GL_MAX_TEXTURE_IMAGE_UNITS
		//data returns one value, the maximum supported texture image units that can be used to access texture maps from the fragment shader. The value must be at least 16. See glActiveTexture.
		//
		//GL_MAX_TEXTURE_LOD_BIAS
		//data returns one value, the maximum, absolute value of the texture level-of-detail bias. The value must be at least 2.0.
		//
		//GL_MAX_TEXTURE_SIZE
		//data returns one value. The value gives a rough estimate of the largest texture that the GL can handle. The value must be at least 1024. Use a proxy texture target such as GL_PROXY_TEXTURE_1D or GL_PROXY_TEXTURE_2D to determine if a texture is too large. See glTexImage1D and glTexImage2D.
		//
		//GL_MAX_UNIFORM_BUFFER_BINDINGS
		//data returns one value, the maximum number of uniform buffer binding points on the context, which must be at least 36.
		//
		//GL_MAX_UNIFORM_BLOCK_SIZE
		//data returns one value, the maximum size in basic machine units of a uniform block, which must be at least 16384.
		//
		//GL_MAX_UNIFORM_LOCATIONS
		//data returns one value, the maximum number of explicitly assignable uniform locations, which must be at least 1024.
		//
		//GL_MAX_VARYING_COMPONENTS
		//data returns one value, the number components for varying variables, which must be at least 60.
		//
		//GL_MAX_VARYING_VECTORS
		//data returns one value, the number 4-vectors for varying variables, which is equal to the value of GL_MAX_VARYING_COMPONENTS and must be at least 15.
		//
		//GL_MAX_VARYING_FLOATS
		//data returns one value, the maximum number of interpolators available for processing varying variables used by vertex and fragment shaders. This value represents the number of individual floating-point values that can be interpolated; varying variables declared as vectors, matrices, and arrays will all consume multiple interpolators. The value must be at least 32.
		//
		//GL_MAX_VERTEX_ATOMIC_COUNTERS
		//data returns a single value, the maximum number of atomic counters available to vertex shaders.
		//
		//GL_MAX_VERTEX_ATTRIBS
		//data returns one value, the maximum number of 4-component generic vertex attributes accessible to a vertex shader. The value must be at least 16. See glVertexAttrib.
		//
		//GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS
		//data returns one value, the maximum number of active shader storage blocks that may be accessed by a vertex shader.
		//
		//GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS
		//data returns one value, the maximum supported texture image units that can be used to access texture maps from the vertex shader. The value may be at least 16. See glActiveTexture.
		//
		//GL_MAX_VERTEX_UNIFORM_COMPONENTS
		//data returns one value, the maximum number of individual floating-point, integer, or boolean values that can be held in uniform variable storage for a vertex shader. The value must be at least 1024. See glUniform.
		//
		//GL_MAX_VERTEX_UNIFORM_VECTORS
		//data returns one value, the maximum number of 4-vectors that may be held in uniform variable storage for the vertex shader. The value of GL_MAX_VERTEX_UNIFORM_VECTORS is equal to the value of GL_MAX_VERTEX_UNIFORM_COMPONENTS and must be at least 256.
		//
		//GL_MAX_VERTEX_OUTPUT_COMPONENTS
		//data returns one value, the maximum number of components of output written by a vertex shader, which must be at least 64.
		//
		//GL_MAX_VERTEX_UNIFORM_BLOCKS
		//data returns one value, the maximum number of uniform blocks per vertex shader. The value must be at least 12. See glUniformBlockBinding.
		//
		//GL_MAX_VIEWPORT_DIMS
		//data returns two values: the maximum supported width and height of the viewport. These must be at least as large as the visible dimensions of the display being rendered to. See glViewport.
		//
		//GL_MAX_VIEWPORTS
		//data returns one value, the maximum number of simultaneous viewports that are supported. The value must be at least 16. See glViewportIndexed.
		//
		//GL_MINOR_VERSION
		//data returns one value, the minor version number of the OpenGL API supported by the current context.
		//
		//GL_NUM_COMPRESSED_TEXTURE_FORMATS
		//data returns a single integer value indicating the number of available compressed texture formats. The minimum value is 4. See glCompressedTexImage2D.
		//
		//GL_NUM_EXTENSIONS
		//data returns one value, the number of extensions supported by the GL implementation for the current context. See glGetString.
		//
		//GL_NUM_PROGRAM_BINARY_FORMATS
		//data returns one value, the number of program binary formats supported by the implementation.
		//
		//GL_NUM_SHADER_BINARY_FORMATS
		//data returns one value, the number of binary shader formats supported by the implementation. If this value is greater than zero, then the implementation supports loading binary shaders. If it is zero, then the loading of binary shaders by the implementation is not supported.
		//
		//GL_PACK_ALIGNMENT
		//data returns one value, the byte alignment used for writing pixel data to memory. The initial value is 4. See glPixelStore.
		//
		//GL_PACK_IMAGE_HEIGHT
		//data returns one value, the image height used for writing pixel data to memory. The initial value is 0. See glPixelStore.
		//
		//GL_PACK_LSB_FIRST
		//data returns a single boolean value indicating whether single-bit pixels being written to memory are written first to the least significant bit of each unsigned byte. The initial value is GL_FALSE. See glPixelStore.
		//
		//GL_PACK_ROW_LENGTH
		//data returns one value, the row length used for writing pixel data to memory. The initial value is 0. See glPixelStore.
		//
		//GL_PACK_SKIP_IMAGES
		//data returns one value, the number of pixel images skipped before the first pixel is written into memory. The initial value is 0. See glPixelStore.
		//
		//GL_PACK_SKIP_PIXELS
		//data returns one value, the number of pixel locations skipped before the first pixel is written into memory. The initial value is 0. See glPixelStore.
		//
		//GL_PACK_SKIP_ROWS
		//data returns one value, the number of rows of pixel locations skipped before the first pixel is written into memory. The initial value is 0. See glPixelStore.
		//
		//GL_PACK_SWAP_BYTES
		//data returns a single boolean value indicating whether the bytes of two-byte and four-byte pixel indices and components are swapped before being written to memory. The initial value is GL_FALSE. See glPixelStore.
		//
		//GL_PIXEL_PACK_BUFFER_BINDING
		//data returns a single value, the name of the buffer object currently bound to the target GL_PIXEL_PACK_BUFFER. If no buffer object is bound to this target, 0 is returned. The initial value is 0. See glBindBuffer.
		//
		//GL_PIXEL_UNPACK_BUFFER_BINDING
		//data returns a single value, the name of the buffer object currently bound to the target GL_PIXEL_UNPACK_BUFFER. If no buffer object is bound to this target, 0 is returned. The initial value is 0. See glBindBuffer.
		//
		//GL_POINT_FADE_THRESHOLD_SIZE
		//data returns one value, the point size threshold for determining the point size. See glPointParameter.
		//
		//GL_PRIMITIVE_RESTART_INDEX
		//data returns one value, the current primitive restart index. The initial value is 0. See glPrimitiveRestartIndex.
		//
		//GL_PROGRAM_BINARY_FORMATS
		//data an array of GL_NUM_PROGRAM_BINARY_FORMATS values, indicating the proram binary formats supported by the implementation.
		//
		//GL_PROGRAM_PIPELINE_BINDING
		//data a single value, the name of the currently bound program pipeline object, or zero if no program pipeline object is bound. See glBindProgramPipeline.
		//
		//GL_PROGRAM_POINT_SIZE
		//data returns a single boolean value indicating whether vertex program point size mode is enabled. If enabled, then the point size is taken from the shader built-in gl_PointSize. If disabled, then the point size is taken from the point state as specified by glPointSize. The initial value is GL_FALSE.
		//
		//GL_PROVOKING_VERTEX
		//data returns one value, the currently selected provoking vertex convention. The initial value is GL_LAST_VERTEX_CONVENTION. See glProvokingVertex.
		//
		//GL_POINT_SIZE
		//data returns one value, the point size as specified by glPointSize. The initial value is 1.
		//
		//GL_POINT_SIZE_GRANULARITY
		//data returns one value, the size difference between adjacent supported sizes for antialiased points. See glPointSize.
		//
		//GL_POINT_SIZE_RANGE
		//data returns two values: the smallest and largest supported sizes for antialiased points. The smallest size must be at most 1, and the largest size must be at least 1. See glPointSize.
		//
		//GL_POLYGON_OFFSET_FACTOR
		//data returns one value, the scaling factor used to determine the variable offset that is added to the depth value of each fragment generated when a polygon is rasterized. The initial value is 0. See glPolygonOffset.
		//
		//GL_POLYGON_OFFSET_UNITS
		//data returns one value. This value is multiplied by an implementation-specific value and then added to the depth value of each fragment generated when a polygon is rasterized. The initial value is 0. See glPolygonOffset.
		//
		//GL_POLYGON_OFFSET_FILL
		//data returns a single boolean value indicating whether polygon offset is enabled for polygons in fill mode. The initial value is GL_FALSE. See glPolygonOffset.
		//
		//GL_POLYGON_OFFSET_LINE
		//data returns a single boolean value indicating whether polygon offset is enabled for polygons in line mode. The initial value is GL_FALSE. See glPolygonOffset.
		//
		//GL_POLYGON_OFFSET_POINT
		//data returns a single boolean value indicating whether polygon offset is enabled for polygons in point mode. The initial value is GL_FALSE. See glPolygonOffset.
		//
		//GL_POLYGON_SMOOTH
		//data returns a single boolean value indicating whether antialiasing of polygons is enabled. The initial value is GL_FALSE. See glPolygonMode.
		//
		//GL_POLYGON_SMOOTH_HINT
		//data returns one value, a symbolic constant indicating the mode of the polygon antialiasing hint. The initial value is GL_DONT_CARE. See glHint.
		//
		//GL_READ_BUFFER
		//data returns one value, a symbolic constant indicating which color buffer is selected for reading. The initial value is GL_BACK if there is a back buffer, otherwise it is GL_FRONT. See glReadPixels.
		//
		//GL_RENDERBUFFER_BINDING
		//data returns a single value, the name of the renderbuffer object currently bound to the target GL_RENDERBUFFER. If no renderbuffer object is bound to this target, 0 is returned. The initial value is 0. See glBindRenderbuffer.
		//
		//GL_SAMPLE_BUFFERS
		//data returns a single integer value indicating the number of sample buffers associated with the framebuffer. See glSampleCoverage.
		//
		//GL_SAMPLE_COVERAGE_VALUE
		//data returns a single positive floating-point value indicating the current sample coverage value. See glSampleCoverage.
		//
		//GL_SAMPLE_COVERAGE_INVERT
		//data returns a single boolean value indicating if the temporary coverage value should be inverted. See glSampleCoverage.
		//
		//GL_SAMPLER_BINDING
		//data returns a single value, the name of the sampler object currently bound to the active texture unit. The initial value is 0. See glBindSampler.
		//
		//GL_SAMPLES
		//data returns a single integer value indicating the coverage mask size. See glSampleCoverage.
		//
		//GL_SCISSOR_BOX
		//data returns four values: the x and y window coordinates of the scissor box, followed by its width and height. Initially the x and y window coordinates are both 0 and the width and height are set to the size of the window. See glScissor.
		//
		//GL_SCISSOR_TEST
		//data returns a single boolean value indicating whether scissoring is enabled. The initial value is GL_FALSE. See glScissor.
		//
		//GL_SHADER_COMPILER
		//data returns a single boolean value indicating whether an online shader compiler is present in the implementation. All desktop OpenGL implementations must support online shader compilations, and therefore the value of GL_SHADER_COMPILER will always be GL_TRUE.
		//
		//GL_SHADER_STORAGE_BUFFER_BINDING
		//When used with non-indexed variants of glGet (such as glGetIntegerv), data returns a single value, the name of the buffer object currently bound to the target GL_SHADER_STORAGE_BUFFER. If no buffer object is bound to this target, 0 is returned. When used with indexed variants of glGet (such as glGetIntegeri_v), data returns a single value, the name of the buffer object bound to the indexed shader storage buffer binding points. The initial value is 0 for all targets. See glBindBuffer, glBindBufferBase, and glBindBufferRange.
		//
		//GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT
		//data returns a single value, the minimum required alignment for shader storage buffer sizes and offset. The initial value is 1. See glShaderStorageBlockBinding.
		//
		//GL_SHADER_STORAGE_BUFFER_START
		//When used with indexed variants of glGet (such as glGetInteger64i_v), data returns a single value, the start offset of the binding range for each indexed shader storage buffer binding. The initial value is 0 for all bindings. See glBindBufferRange.
		//
		//GL_SHADER_STORAGE_BUFFER_SIZE
		//When used with indexed variants of glGet (such as glGetInteger64i_v), data returns a single value, the size of the binding range for each indexed shader storage buffer binding. The initial value is 0 for all bindings. See glBindBufferRange.
		//
		//GL_SMOOTH_LINE_WIDTH_RANGE
		//data returns a pair of values indicating the range of widths supported for smooth (antialiased) lines. See glLineWidth.
		//
		//GL_SMOOTH_LINE_WIDTH_GRANULARITY
		//data returns a single value indicating the level of quantization applied to smooth line width parameters.
		//
		//GL_STENCIL_BACK_FAIL
		//data returns one value, a symbolic constant indicating what action is taken for back-facing polygons when the stencil test fails. The initial value is GL_KEEP. See glStencilOpSeparate.
		//
		//GL_STENCIL_BACK_FUNC
		//data returns one value, a symbolic constant indicating what function is used for back-facing polygons to compare the stencil reference value with the stencil buffer value. The initial value is GL_ALWAYS. See glStencilFuncSeparate.
		//
		//GL_STENCIL_BACK_PASS_DEPTH_FAIL
		//data returns one value, a symbolic constant indicating what action is taken for back-facing polygons when the stencil test passes, but the depth test fails. The initial value is GL_KEEP. See glStencilOpSeparate.
		//
		//GL_STENCIL_BACK_PASS_DEPTH_PASS
		//data returns one value, a symbolic constant indicating what action is taken for back-facing polygons when the stencil test passes and the depth test passes. The initial value is GL_KEEP. See glStencilOpSeparate.
		//
		//GL_STENCIL_BACK_REF
		//data returns one value, the reference value that is compared with the contents of the stencil buffer for back-facing polygons. The initial value is 0. See glStencilFuncSeparate.
		//
		//GL_STENCIL_BACK_VALUE_MASK
		//data returns one value, the mask that is used for back-facing polygons to mask both the stencil reference value and the stencil buffer value before they are compared. The initial value is all 1's. See glStencilFuncSeparate.
		//
		//GL_STENCIL_BACK_WRITEMASK
		//data returns one value, the mask that controls writing of the stencil bitplanes for back-facing polygons. The initial value is all 1's. See glStencilMaskSeparate.
		//
		//GL_STENCIL_CLEAR_VALUE
		//data returns one value, the index to which the stencil bitplanes are cleared. The initial value is 0. See glClearStencil.
		//
		//GL_STENCIL_FAIL
		//data returns one value, a symbolic constant indicating what action is taken when the stencil test fails. The initial value is GL_KEEP. See glStencilOp. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilOpSeparate.
		//
		//GL_STENCIL_FUNC
		//data returns one value, a symbolic constant indicating what function is used to compare the stencil reference value with the stencil buffer value. The initial value is GL_ALWAYS. See glStencilFunc. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilFuncSeparate.
		//
		//GL_STENCIL_PASS_DEPTH_FAIL
		//data returns one value, a symbolic constant indicating what action is taken when the stencil test passes, but the depth test fails. The initial value is GL_KEEP. See glStencilOp. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilOpSeparate.
		//
		//GL_STENCIL_PASS_DEPTH_PASS
		//data returns one value, a symbolic constant indicating what action is taken when the stencil test passes and the depth test passes. The initial value is GL_KEEP. See glStencilOp. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilOpSeparate.
		//
		//GL_STENCIL_REF
		//data returns one value, the reference value that is compared with the contents of the stencil buffer. The initial value is 0. See glStencilFunc. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilFuncSeparate.
		//
		//GL_STENCIL_TEST
		//data returns a single boolean value indicating whether stencil testing of fragments is enabled. The initial value is GL_FALSE. See glStencilFunc and glStencilOp.
		//
		//GL_STENCIL_VALUE_MASK
		//data returns one value, the mask that is used to mask both the stencil reference value and the stencil buffer value before they are compared. The initial value is all 1's. See glStencilFunc. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilFuncSeparate.
		//
		//GL_STENCIL_WRITEMASK
		//data returns one value, the mask that controls writing of the stencil bitplanes. The initial value is all 1's. See glStencilMask. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilMaskSeparate.
		//
		//GL_STEREO
		//data returns a single boolean value indicating whether stereo buffers (left and right) are supported.
		//
		//GL_SUBPIXEL_BITS
		//data returns one value, an estimate of the number of bits of subpixel resolution that are used to position rasterized geometry in window coordinates. The value must be at least 4.
		//
		//GL_TEXTURE_BINDING_1D
		//data returns a single value, the name of the texture currently bound to the target GL_TEXTURE_1D. The initial value is 0. See glBindTexture.
		//
		//GL_TEXTURE_BINDING_1D_ARRAY
		//data returns a single value, the name of the texture currently bound to the target GL_TEXTURE_1D_ARRAY. The initial value is 0. See glBindTexture.
		//
		//GL_TEXTURE_BINDING_2D
		//data returns a single value, the name of the texture currently bound to the target GL_TEXTURE_2D. The initial value is 0. See glBindTexture.
		//
		//GL_TEXTURE_BINDING_2D_ARRAY
		//data returns a single value, the name of the texture currently bound to the target GL_TEXTURE_2D_ARRAY. The initial value is 0. See glBindTexture.
		//
		//GL_TEXTURE_BINDING_2D_MULTISAMPLE
		//data returns a single value, the name of the texture currently bound to the target GL_TEXTURE_2D_MULTISAMPLE. The initial value is 0. See glBindTexture.
		//
		//GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY
		//data returns a single value, the name of the texture currently bound to the target GL_TEXTURE_2D_MULTISAMPLE_ARRAY. The initial value is 0. See glBindTexture.
		//
		//GL_TEXTURE_BINDING_3D
		//data returns a single value, the name of the texture currently bound to the target GL_TEXTURE_3D. The initial value is 0. See glBindTexture.
		//
		//GL_TEXTURE_BINDING_BUFFER
		//data returns a single value, the name of the texture currently bound to the target GL_TEXTURE_BUFFER. The initial value is 0. See glBindTexture.
		//
		//GL_TEXTURE_BINDING_CUBE_MAP
		//data returns a single value, the name of the texture currently bound to the target GL_TEXTURE_CUBE_MAP. The initial value is 0. See glBindTexture.
		//
		//GL_TEXTURE_BINDING_RECTANGLE
		//data returns a single value, the name of the texture currently bound to the target GL_TEXTURE_RECTANGLE. The initial value is 0. See glBindTexture.
		//
		//GL_TEXTURE_COMPRESSION_HINT
		//data returns a single value indicating the mode of the texture compression hint. The initial value is GL_DONT_CARE.
		//
		//GL_TEXTURE_BINDING_BUFFER
		//data returns a single value, the name of the buffer object currently bound to the GL_TEXTURE_BUFFER buffer binding point. The initial value is 0. See glBindBuffer.
		//
		//GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT
		//data returns a single value, the minimum required alignment for texture buffer sizes and offset. The initial value is 1. See glUniformBlockBinding.
		//
		//GL_TIMESTAMP
		//data returns a single value, the 64-bit value of the current GL time. See glQueryCounter.
		//
		//GL_TRANSFORM_FEEDBACK_BUFFER_BINDING
		//When used with non-indexed variants of glGet (such as glGetIntegerv), data returns a single value, the name of the buffer object currently bound to the target GL_TRANSFORM_FEEDBACK_BUFFER. If no buffer object is bound to this target, 0 is returned. When used with indexed variants of glGet (such as glGetIntegeri_v), data returns a single value, the name of the buffer object bound to the indexed transform feedback attribute stream. The initial value is 0 for all targets. See glBindBuffer, glBindBufferBase, and glBindBufferRange.
		//
		//GL_TRANSFORM_FEEDBACK_BUFFER_START
		//When used with indexed variants of glGet (such as glGetInteger64i_v), data returns a single value, the start offset of the binding range for each transform feedback attribute stream. The initial value is 0 for all streams. See glBindBufferRange.
		//
		//GL_TRANSFORM_FEEDBACK_BUFFER_SIZE
		//When used with indexed variants of glGet (such as glGetInteger64i_v), data returns a single value, the size of the binding range for each transform feedback attribute stream. The initial value is 0 for all streams. See glBindBufferRange.
		//
		//GL_UNIFORM_BUFFER_BINDING
		//When used with non-indexed variants of glGet (such as glGetIntegerv), data returns a single value, the name of the buffer object currently bound to the target GL_UNIFORM_BUFFER. If no buffer object is bound to this target, 0 is returned. When used with indexed variants of glGet (such as glGetIntegeri_v), data returns a single value, the name of the buffer object bound to the indexed uniform buffer binding point. The initial value is 0 for all targets. See glBindBuffer, glBindBufferBase, and glBindBufferRange.
		//
		//GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT
		//data returns a single value, the minimum required alignment for uniform buffer sizes and offset. The initial value is 1. See glUniformBlockBinding.
		//
		//GL_UNIFORM_BUFFER_SIZE
		//When used with indexed variants of glGet (such as glGetInteger64i_v), data returns a single value, the size of the binding range for each indexed uniform buffer binding. The initial value is 0 for all bindings. See glBindBufferRange.
		//
		//GL_UNIFORM_BUFFER_START
		//When used with indexed variants of glGet (such as glGetInteger64i_v), data returns a single value, the start offset of the binding range for each indexed uniform buffer binding. The initial value is 0 for all bindings. See glBindBufferRange.
		//
		//GL_UNPACK_ALIGNMENT
		//data returns one value, the byte alignment used for reading pixel data from memory. The initial value is 4. See glPixelStore.
		//
		//GL_UNPACK_IMAGE_HEIGHT
		//data returns one value, the image height used for reading pixel data from memory. The initial is 0. See glPixelStore.
		//
		//GL_UNPACK_LSB_FIRST
		//data returns a single boolean value indicating whether single-bit pixels being read from memory are read first from the least significant bit of each unsigned byte. The initial value is GL_FALSE. See glPixelStore.
		//
		//GL_UNPACK_ROW_LENGTH
		//data returns one value, the row length used for reading pixel data from memory. The initial value is 0. See glPixelStore.
		//
		//GL_UNPACK_SKIP_IMAGES
		//data returns one value, the number of pixel images skipped before the first pixel is read from memory. The initial value is 0. See glPixelStore.
		//
		//GL_UNPACK_SKIP_PIXELS
		//data returns one value, the number of pixel locations skipped before the first pixel is read from memory. The initial value is 0. See glPixelStore.
		//
		//GL_UNPACK_SKIP_ROWS
		//data returns one value, the number of rows of pixel locations skipped before the first pixel is read from memory. The initial value is 0. See glPixelStore.
		//
		//GL_UNPACK_SWAP_BYTES
		//data returns a single boolean value indicating whether the bytes of two-byte and four-byte pixel indices and components are swapped after being read from memory. The initial value is GL_FALSE. See glPixelStore.
		//
		//GL_VERTEX_ARRAY_BINDING
		//data returns a single value, the name of the vertex array object currently bound to the context. If no vertex array object is bound to the context, 0 is returned. The initial value is 0. See glBindVertexArray.
		//
		//GL_VERTEX_BINDING_DIVISOR
		//Accepted by the indexed forms. data returns a single integer value representing the instance step divisor of the first element in the bound buffer's data store for vertex attribute bound to index.
		//
		//GL_VERTEX_BINDING_OFFSET
		//Accepted by the indexed forms. data returns a single integer value representing the byte offset of the first element in the bound buffer's data store for vertex attribute bound to index.
		//
		//GL_VERTEX_BINDING_STRIDE
		//Accepted by the indexed forms. data returns a single integer value representing the byte offset between the start of each element in the bound buffer's data store for vertex attribute bound to index.
		//
		//GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET
		//data returns a single integer value containing the maximum offset that may be added to a vertex binding offset.
		//
		//GL_MAX_VERTEX_ATTRIB_BINDINGS
		//data returns a single integer value containing the maximum number of vertex buffers that may be bound.
		//
		//GL_VIEWPORT
		//When used with non-indexed variants of glGet (such as glGetIntegerv), data returns four values: the x and y window coordinates of the viewport, followed by its width and height. Initially the x and y window coordinates are both set to 0, and the width and height are set to the width and height of the window into which the GL will do its rendering. See glViewport.
		//
		//When used with indexed variants of glGet (such as glGetIntegeri_v), data returns four values: the x and y window coordinates of the indexed viewport, followed by its width and height. Initially the x and y window coordinates are both set to 0, and the width and height are set to the width and height of the window into which the GL will do its rendering. See glViewportIndexedf.
		//
		//GL_VIEWPORT_BOUNDS_RANGE
		//data returns two values, the minimum and maximum viewport bounds range. The minimum range should be at least [-32768, 32767].
		//
		//GL_VIEWPORT_INDEX_PROVOKING_VERTEX
		//data returns one value, the implementation dependent specifc vertex of a primitive that is used to select the viewport index. If the value returned is equivalent to GL_PROVOKING_VERTEX, then the vertex selection follows the convention specified by glProvokingVertex. If the value returned is equivalent to GL_FIRST_VERTEX_CONVENTION, then the selection is always taken from the first vertex in the primitive. If the value returned is equivalent to GL_LAST_VERTEX_CONVENTION, then the selection is always taken from the last vertex in the primitive. If the value returned is equivalent to GL_UNDEFINED_VERTEX, then the selection is not guaranteed to be taken from any specific vertex in the primitive.
		//
		//GL_VIEWPORT_SUBPIXEL_BITS
		//data returns a single value, the number of bits of sub-pixel precision which the GL uses to interpret the floating point viewport bounds. The minimum value is 0.
		//
		//GL_MAX_ELEMENT_INDEX
		//data returns a single value, the maximum index that may be specified during the transfer of generic vertex attributes to the GL.

	Model LoadModel(const std::string &_path);
}

namespace GLHelper
{
	template<typename Arr>
	void SetVBOData(
		const Arr &data,
		uint dataSize,
		uint index,
		uint drawType = GL_STATIC_DRAW,
		uint dataType = GL_FLOAT,
		bool normalized = false)
	{
		using T = typename Arr::value_type;
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), drawType);

		glVertexAttribPointer(index, dataSize, dataType, normalized, sizeof(T), (void *)0);
		glEnableVertexAttribArray(index);
	}

	template<typename Arr>
	void SetVBOData(
		uint vbo,
		const Arr &data,
		uint dataSize,
		uint index,
		uint drawType = GL_STATIC_DRAW,
		uint dataType = GL_FLOAT,
		bool normalized = false)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		SetVBOData(data, dataSize, index, drawType, dataType, normalized);
	}

	uint CreateUBO(uint bindingPoint, uint size, GL::DrawType drawType = GL::StaticDraw);
	void BindUBOData(uint offset, uint size, void *data);

	void DebugDrawDC(const std::vector<Vector2> &arr, float size = 5.f, GLenum mode = GL_POINTS);
	void DebugDrawScreen(const std::vector<Vector2> &screenArr, float size = 5.f, GLenum mode = GL_POINTS);
}