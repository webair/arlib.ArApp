/*
 * shader to render the camera image without projection
 */
static const char cameraVerticeShader[] =
		"attribute vec4 a_position;"
		"attribute vec2 a_texCoord;"
		"varying vec2 v_camTexCoord;"
		"void main()"
		"{"
			"gl_Position = a_position;"
			"v_camTexCoord = a_texCoord;"
		"}";

static const char cameraFragmentShader[] =
		"precision mediump float;"
		"uniform sampler2D u_textureSampler;"
		"varying vec2 v_camTexCoord;"
		"void main()\n"
		"{"
			"gl_FragColor = texture2D( u_textureSampler, v_camTexCoord );"
		"}"
;

/*
 * shader to render to create a silhoute image from the models.
 * Will be used as a search pattern image.
 */
static const char searchPatternVerticeShader[] =

		"attribute vec4 a_position;"
		"attribute vec4 a_normal;"

		"uniform mat4 u_projectionView;"
		"uniform mat4 u_modelView;"

		"void main()"
		"{"
			//not used yet, but has to be declared
			"vec4 newNormal = u_projectionView * u_modelView * a_normal;"
			"gl_Position = u_projectionView * u_modelView * a_position;"
		"}"
;

static const char searchPatternFragmentShader[] =

		"precision mediump float;"
		"varying float depth;"
		"void main()\n"
		"{"
			"gl_FragColor = vec4(1.0,1.0,1.0,1.0);"
		"}"
;

/*
 * shader to render the model with texture data
 */
static const char objectVerticeShader[] =

		"attribute vec4 a_position;"
		"attribute vec4 a_normal;"
		"attribute vec2 a_texCoord;"

		"uniform mat4 u_projectionView;"
		"uniform mat4 u_modelView;"

		"varying vec2 v_objTexCoord;"

		"void main()"
		"{"
			//not used yet, but has to be declared
			"vec4 newNormal = u_projectionView * u_modelView * a_normal;"
			"v_objTexCoord = a_texCoord;"
			"gl_Position = u_projectionView * u_modelView * a_position;"
		"}"
;

static const char objectFragmentShader[] =

		"precision mediump float;"
		"uniform sampler2D u_textureSampler;"
		"varying vec2 v_objTexCoord;"
		"void main()\n"
		"{"
			"gl_FragColor = texture2D( u_textureSampler, v_objTexCoord );"
		"}"
;
