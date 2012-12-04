package ch.bfh.bachelor.ar;

public class ArLib {
	static {
		System.loadLibrary("ar-jni");
	}

	public static native void initArLib(int viewPortWidth, int viewPortHeight,
			int imageWith, int imageHeight, float cameraAngle
	);

	public static native void precessImage(byte[] imageData, float azimuth, float pitch, float roll, float[] rotationMatrix);

	public static native void addModel(int modelId, float[] vnt, short[] faces, 
		float[] pointOfGravity, float[] boundingBox, float northAngle, 
		float latitude, float longitude, byte[][] textureData, int[] materials, int[] dimension
	);
}
