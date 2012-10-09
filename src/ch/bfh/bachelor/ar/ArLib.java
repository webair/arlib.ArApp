package ch.bfh.bachelor.ar;

public class ArLib {
	static {
		System.loadLibrary("ar-jni");
	}

	public static native void initArLib(int viewPortWidth, int viewPortHeight,
			int imageWith, int imageHeight);

	public static native void precessImage(byte[] imageData);

}
