package ch.bfh.bachelor.ar.opengl;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.opengl.GLSurfaceView;
import android.util.Log;
import ch.bfh.arApp.model3D.Object3D;
import ch.bfh.bachelor.ar.ArLib;
import ch.bfh.bachelor.ar.LocManager;
/*
	This class handels the rendering and is the interface to the native 
	AR Library
*/
public class CameraRenderer implements GLSurfaceView.Renderer,
		android.hardware.Camera.PreviewCallback, SensorEventListener {

	private static final String TAG = "CameraRenderer";

	// switches
	public boolean isSensorCorrectionEnabled = true;
	public boolean isSensorRoundingEnabled = false;
	public boolean isSensorBufferingEnable = true;
	public boolean isGyroCorrectionEnable = true;

	public int frameWidth = 0;
	public int frameHeight = 0;
	public byte[] frame = null;
	public byte[] buffer = null;

	static final float LOW_PASS_ALPHA = 0.15f;

	private float cameraAngleVertical;

	// new getOrientation

	int sensorDelay;
	private SensorManager sm;
	private float[] mags;
	private float[] accels;
	private float[] gyro;
	float[] values = new float[3];
	private static final int matrix_size = 16;
	float[] RE = new float[matrix_size];
	float[] outR = new float[matrix_size];
	float[] I = new float[matrix_size];
	private float azimuth;
	private float pitch;
	private float roll;

	// SensorBuffering
	private int bufferSize;
	private int bufferMiddle;
	private int bufferPoint;
	private float[] aA;
	private float[] pA;
	private float[] rA;

	// Gyroscope Correction
	private boolean isMoving;
	private float gyroThresholdA;
	private float gyroThresholdB;
	private float gyroThresholdC;
	private int bufferSizeg;
	private int bufferMiddleg;
	private int bufferPointg;
	private float[] aAg;
	private float[] pAg;
	private float[] rAg;

	public boolean hasImage = false;
	private Camera cam;

	private ArrayList<Object3D> models;
	
	private LocManager lm;

	
	/*
		Constructor, starts the sensor and camera service
	*/
	public CameraRenderer(Context context, ArrayList<Object3D> models, LocManager lm) {
		this.lm = lm;
		this.models = models;
		// Change Sensor Delay Here
		sensorDelay = SensorManager.SENSOR_DELAY_FASTEST;
		sm = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);

		if (isSensorBufferingEnable) {
			bufferSize = 5;
			bufferMiddle = (bufferSize - 1) / 2;
			bufferPoint = 0;
			aA = new float[bufferSize];
			pA = new float[bufferSize];
			rA = new float[bufferSize];
		}
		if (isGyroCorrectionEnable) {
			this.gyroThresholdA = (float) 0.0005;
			this.gyroThresholdB = (float) 0.0005;
			this.gyroThresholdC = (float) 0.0005;
			this.isMoving = false;
			bufferSizeg = 5;
			bufferMiddleg = (bufferSize - 1) / 2;
			bufferPointg = 0;
			aAg = new float[bufferSize];
			pAg = new float[bufferSize];
			rAg = new float[bufferSize];
		}
	}
	/*
		overriden method, will be called if the openGL context is ready to draw
		a frame. it will call the native render method as soon a image is ready
	*/
	@Override
	public synchronized void onDrawFrame(GL10 gl) {
		if (hasImage) {
			float roll = (float) (this.roll + (Math.PI / 2));
			ArLib.precessImage(frame, azimuth, pitch, roll, lm.getCurrentLat(), lm.getCurrentLon());
			this.notify();
		}
	}
	/*
		overriden method, will be called if the camera serves a new image
	*/
	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {

		synchronized (this) {
			System.arraycopy(data, 0, this.frame, 0, data.length);
			hasImage = true;
			this.notify();
		}
		cam.addCallbackBuffer(buffer);
	}
	/*
		overriden method, will be called if the device changes orientation
	*/
	@Override
	public void onSurfaceChanged(GL10 gl, int width, int height) {
		openSensor();
		openCamera();
		setupCamera(1134, 720);
		Object3D b = models.get(0);
		short[] facesSh = new short[b.face.length];
		float[] bbox = new float[24];
		for (int q = 0; q < 8; q++) {
			for (int u = 0; u < 3; u++) {
				bbox[q * u] = b.bb[q][u];
			}
		}
		for (int j = 0; j < b.face.length; j++) {
			facesSh[j] = (short) b.face[j];
		}
		int[] dimension = new int[b.matPics.length*2];
		byte[][] picArray = new byte[b.matPics.length][];
		Bitmap bmp;
		ByteBuffer bb;
		for (int i = 0; i < b.matPics.length; i++) {
			bmp = BitmapFactory.decodeByteArray(b.matPics[i], 0,
					b.matPics[i].length);
			if (bmp == null) {
				picArray[i] = new byte[0];
				dimension[i*2] = 0;
				dimension[(i*2)+1] = 0;
			} else {
				dimension[i*2] = bmp.getWidth();
				dimension[(i*2)+1] = bmp.getHeight();
				bb = ByteBuffer.allocate(bmp.getHeight() * bmp.getWidth() * 4);
				bmp.copyPixelsToBuffer(bb);
				picArray[i] = bb.array();
			}
			

			Log.i(TAG, String.format("Picture Byte Array Length: %d, %s",
					picArray[i].length, b.matNames[i]));
		}

		int[] newFaceFromTo = new int[b.matToFace.length * 2];
		
		for (int i = 0; i < b.matToFace.length; i++) {
			newFaceFromTo[i*2] = b.matToFace[i][0];
			newFaceFromTo[(i*2)+1] = b.matToFace[i][1];
		}
		ArLib.initArLib(width, height, frameWidth, frameHeight,
				cameraAngleVertical);
		ArLib.addModel(b.objectid, b.vnt, facesSh, b.pointOfGravity, bbox,
				b.northangle, b.lat, b.lon, picArray, newFaceFromTo, dimension);

	}
	public static void reverse(int[][] data) {
	    int left = 0;
	    int right = data.length - 1;

	    while( left < right ) {
	        // swap the values at the left and right indices
	        int[] temp = data[left];
	        data[left] = data[right];
	        data[right] = temp;

	        // move the left and right index pointers in toward the center
	        left++;
	        right--;
	    }
	}
	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		// stub method
	}

	public void openCamera() {
		Log.i(TAG, "openCamera");
		releaseCamera();
		cam = Camera.open();
		if (cam == null) {
			Log.e(TAG, "Can't open camera!");

		}
		cam.setPreviewCallbackWithBuffer(this);

	}

	public void releaseCamera() {
		Log.i(TAG, "releaseCamera");
		synchronized (this) {
			if (cam != null) {
				cam.stopPreview();
				cam.setPreviewCallback(null);
				cam.release();
				cam = null;
			}
		}
		;
	}

	public void openSensor() {
		sm.registerListener(this, sm.getDefaultSensor(Sensor.TYPE_GYROSCOPE),
				sensorDelay);
		sm.registerListener(this,
				sm.getDefaultSensor(Sensor.TYPE_ACCELEROMETER), sensorDelay);
		sm.registerListener(this,
				sm.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD), sensorDelay);
	}

	public void releaseSensor() {
		Log.i(TAG, "release sensor");
		sm.unregisterListener(this);
	}

	public void setupCamera(int width, int height) {
		Log.i(TAG, "setupCamera");
		synchronized (this) {
			if (cam != null) {
				Camera.Parameters params = cam.getParameters();
				List<Camera.Size> sizes = params.getSupportedPreviewSizes();
				frameWidth = width;
				frameHeight = height;

				// selecting optimal camera preview size
				{
					int minDiff = Integer.MAX_VALUE;
					for (Camera.Size size : sizes) {
						if (Math.abs(size.height - height) < minDiff) {
							frameWidth = size.width;
							frameHeight = size.height;
							minDiff = Math.abs(size.height - height);
						}
					}
				}
				params.setPreviewSize(frameWidth, frameHeight);

				List<String> FocusModes = params.getSupportedFocusModes();
				if (FocusModes
						.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)) {
					params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
				}

				cam.setParameters(params);

				/* Now allocate the buffer */
				params = cam.getParameters();
				frameWidth = params.getPreviewSize().width;
				frameHeight = params.getPreviewSize().height;
				cameraAngleVertical = params.getVerticalViewAngle();

				int size = params.getPreviewSize().width
						* params.getPreviewSize().height;
				size = size
						* ImageFormat
								.getBitsPerPixel(params.getPreviewFormat()) / 8;
				buffer = new byte[size];
				
				//The buffer where the current frame will be copied 
				frame = new byte[size];
				Log.i(TAG, String.format("frame length: %d", frame.length));

				cam.addCallbackBuffer(buffer);

				try {
					cam.setPreviewTexture(new SurfaceTexture(10));
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}

				/* Now we can start a preview */
				cam.startPreview();
			}
		}
	}

	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {
		Log.i(TAG, String.format("%d", accuracy));

	}

	protected float[] correctSensorValues(float[] input, float[] output) {
		if (output == null) {
			// Log.i(TAG, "not low passed");
			return input;
		}
		// Log.i(TAG, "low passed values");
		for (int i = 0; i < input.length; i++) {
			// round value to get less shacky values
			output[i] = output[i] + LOW_PASS_ALPHA * (input[i] - output[i]);
			if (isSensorRoundingEnabled) {
				output[i] = (float) Math.round(output[i] * 10) / 10;
			}
		}
		return output;
	}

	@Override
	public void onSensorChanged(SensorEvent event) {

		switch (event.sensor.getType()) {
		case Sensor.TYPE_MAGNETIC_FIELD:

			if (isSensorCorrectionEnabled) {
				mags = correctSensorValues(event.values.clone(), mags);
			} else {
				mags = event.values.clone();
			}
			break;
		case Sensor.TYPE_ACCELEROMETER:
			if (isSensorCorrectionEnabled) {
				accels = correctSensorValues(event.values.clone(), accels);
			} else {
				accels = event.values.clone();
			}
		case Sensor.TYPE_GYROSCOPE:
			gyro = event.values.clone();
			break;
		}
		if (gyro != null && isGyroCorrectionEnable) {
			bufferPointg++;
			if (bufferPointg == bufferSizeg) {
				bufferPointg = 0;
				java.util.Arrays.sort(this.aAg);
				java.util.Arrays.sort(this.pAg);
				java.util.Arrays.sort(this.rAg);
				float a = this.aAg[this.bufferMiddleg];
				float b = this.pAg[this.bufferMiddleg];
				float c = this.rAg[this.bufferMiddleg];
				if (a >= this.gyroThresholdA || b >= this.gyroThresholdB
						|| c >= gyroThresholdC)
					this.isMoving = true;
				else
					this.isMoving = false;
				// Log.i(TAG,"C Value: "+String.valueOf(c));
			} else {
				aAg[bufferPointg] = Math.abs(Math.round(gyro[0]));
				pAg[bufferPointg] = Math.abs(Math.round(gyro[1]));
				rAg[bufferPointg] = Math.abs(Math.round(gyro[2]));
			}
			gyro = null;
		}

		if (mags != null && accels != null) {
			if (isGyroCorrectionEnable) {
				if (!this.isMoving) {
					return;
				}
			}
			SensorManager.getRotationMatrix(RE, I, accels, mags);
			// remap for landscape
			SensorManager.remapCoordinateSystem(RE, SensorManager.AXIS_X,
					SensorManager.AXIS_Z, outR);
			SensorManager.getOrientation(outR, values);

			if (isSensorBufferingEnable) {
				bufferPoint++;
				if (bufferPoint == bufferSize) {
					bufferPoint = 0;
					java.util.Arrays.sort(this.aA);
					java.util.Arrays.sort(this.pA);
					java.util.Arrays.sort(this.rA);

					azimuth = aA[bufferMiddle];
					pitch = pA[bufferMiddle];
					roll = rA[bufferMiddle];
				} else {
					aA[bufferPoint] = values[0];
					pA[bufferPoint] = values[1];
					rA[bufferPoint] = values[2];
				}
			} else {
				azimuth = values[0];
				pitch = values[1];
				roll = values[2];
			}
			mags = null;
			accels = null;
		}
	}
}
