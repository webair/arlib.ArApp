
#include "Model.h"

#include "util/Logger.h"

using namespace std;

Model::Model() {
	verticesSize = 108;
	vertices = (GLfloat *) new GLfloat[verticesSize];
	centerPoint[0] = 0.0f;
	centerPoint[1] = 0.0f;
	centerPoint[2] = 0.0f;

	GLfloat values[] = {-1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f};
	GLfloat values2[] = {-22.0f, -22.0f, -20.0f, -20.0f, -20.0f, -22.0f};

	for (int i=0; i < verticesSize / 2; i++) {

		if (i%18 == 0) {
			if (i<18) {
			//front and back face
				for (int j=0; j<6; j++) {
					int offset = 0;
					vertices[i + j*3] = values[j];
					vertices[i + 1 + j*3] = values[(j + 4) % 6];
					vertices[i + 2 + j*3] = 1.0f;
					int backOffset = offset + 18;
					vertices[i + backOffset + j*3] = values[j];
					vertices[i + backOffset + 1 + j*3] = values[(j + 4) % 6];
					vertices[i + backOffset + 2 + j*3] = -1.0f;

				}

			} else if (i<36) {
			// left and right face
				int offset = 18;
				for (int j=0; j<6; j++) {
					vertices[i + offset + j*3] = -1.0f;
					vertices[i + offset + 1 + j*3] = values[j];
					vertices[i + offset + 2 + j*3] = values[(j + 4) % 6];
					int backOffset = offset + 18;
					vertices[i + backOffset + j*3] = 1.0f;
					vertices[i + backOffset + 1 + j*3] =values[(j + 4) % 6];
					vertices[i + backOffset + 2 + j*3] = values[j];
				}
			} else {
			// top and bottom face
				int offset = 36;
				for (int j=0; j<6; j++) {
					vertices[i + offset + j*3] = values[j];
					vertices[i + offset + 1 + j*3] = -1.0f;
					vertices[i + offset + 2 + j*3] = values[(j + 4) % 6];
					int backOffset = offset + 18;
					vertices[i + backOffset + j*3] = values[(j + 4) % 6];
					vertices[i + backOffset + 1 + j*3] = 1.0f;
					vertices[i + backOffset + 2 + j*3] = values[j];
				}
			}
		}
	}
}

Model::~Model() {
	delete [] vertices;  // When done, free memory pointed to by a
	vertices = NULL;
	delete modelView;
	modelView = NULL;

}
