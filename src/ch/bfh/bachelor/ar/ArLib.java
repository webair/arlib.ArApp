package ch.bfh.bachelor.ar;
/*
	interface class to access the native c++ code
*/
public class ArLib {
	static {
		System.loadLibrary("ar-jni");
	}
	/*
		initialize the AR Library
	*/
	public static native void initArLib(int viewPortWidth, int viewPortHeight,
			int imageWith, int imageHeight, float cameraAngle
	);
	/*
		processes the image and starts the rendering
	*/
	public static native void precessImage(byte[] imageData, float azimuth, float pitch, float roll, float latitude, float longitude);
	/*
		add a model to the AR Library
	*/
	public static native void addModel(int modelId, float[] vnt, short[] faces, 
		float[] pointOfGravity, float[] boundingBox, float northAngle, 
		float latitude, float longitude, byte[][] textureData, int[] materials, int[] dimension
	);
}
