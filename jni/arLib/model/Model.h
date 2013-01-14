#ifndef AR_MODEL
#define AR_MODEL

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "arLib/Definitions.h"

using namespace glm;

class Model {

public:
	/*
	 * constructor to create the model and calculate the needed matrices
	 */
	Model(int modelId, GLfloat* vnt, int numberOfVNT,
		  GLushort* faces, int numberOfFaces,
		  GLfloat* centerOfGravity, GLfloat* boundingBox,
		  GLfloat northAngle, Location location
	);
	~Model();

	/*
	 * returns the unique model id for referencing it
	 */
	int getModelId();

	/*
	 * returns the number of vnt elements
	 */
	GLuint getNumberOfVNT();

	/*
	 * return a array containing all vnt elements
	 */
	GLfloat* getVNT();

	/*
	 * returns the number of faces elements
	 */
	GLuint getNumberOfFaces();

	/*
	 * return a array containing all faces
	 */
	GLushort* getFaces();

	/*
	 * sets the number of materials, materials have to be created external,
	 * so low coupling is achieved
	 */
	void setNumberOfMaterials(int nr);

	/*
	 * returns the number of materials
	 */
	GLuint getNumberOfMaterials();

	/*
	 * returns all material objects in an array
	 */
	Material* getMaterials();

	/*
	 * set the material array
	 */
	void setMaterials(Material *materialArray);

	/*
	 * returns a array with the x,y and z value of the point of gravity
	 */
	GLfloat* getCenterOfGravity();

	/*
	 * returns an array with the eight vertices to represant the bounding box
	 */
	GLfloat* getBoundingBox();

	/*
	 * sets the world matrix (must be done external) to translate the object in to
	 * the position in the virtual world
	 */
	void setWorldMatrix(mat4 modelMatrix);

	/*
	 * return the model matrix, this includes the orientation (rotation), the translation to the point of
	 * gravity and the world matrix (translation from zero coordinate)
	 */
	mat4 getModelMatrix();

	/*
	 * returns the location in latitude and longitude for the object
	 */
	Location getLocation();


protected:
	int modelId;

	GLuint numberOfVNT;
	GLfloat* vnt;

	GLuint numberOfFaces;
	GLushort *faces;

	GLuint numberOfMaterials;
	Material *materials;

	GLfloat centerOfGravity[3];
	GLfloat boundingBox[24];


	mat4* worldMatrix;

	//defines the matrix to move the object to 0 coord and rotate relative to north angle
	mat4* objectMatrix;
	Location location;
};

#endif
