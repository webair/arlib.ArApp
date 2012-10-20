package ch.bfh.bachelor.ar.opengl;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;

import android.opengl.GLSurfaceView;
import android.util.Log;
import ch.bfh.bachelor.ar.ArLib;

public class CameraRenderer implements GLSurfaceView.Renderer, 
android.hardware.Camera.PreviewCallback, SensorEventListener {

    private static final String TAG = "CameraRenderer";
    
	public int frameWidth = 0;
	public int frameHeight = 0;
	public byte[] frame = null;
	public byte[] buffer = null;	

	//new getOrientation
	int sensorDelay;
	private SensorManager sm;
	private float[] mags;
	private float[] accels;
	private float[] gyro;
    private static final int matrix_size = 16;
    float[] RE = new float[matrix_size];
    float[] outR = new float[matrix_size];
    float[] I = new float[matrix_size];
    float[] values = new float[3]; 

    double[] azimuthArr;
    double[] pitchArr;
    double[] rollArr;
    int bufsize;
    int bufpoint;
	int bufmiddle;
	
	public boolean hasImage = false;
	private Camera cam;
	
	public CameraRenderer(Context context)
	{
		//CHANGE SENSOR DELAY HERE!
		sensorDelay = sm.SENSOR_DELAY_FASTEST;
		
		
		sm = (SensorManager)context.getSystemService(context.SENSOR_SERVICE);
		sm.registerListener(this, sm.getDefaultSensor(Sensor.TYPE_GYROSCOPE), sensorDelay);
		sm.registerListener(this, sm.getDefaultSensor(Sensor.TYPE_ACCELEROMETER), sensorDelay);
		sm.registerListener(this, sm.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD), sensorDelay);
	    if(sensorDelay == sm.SENSOR_DELAY_FASTEST)
	    {
			bufsize=11;
		    bufmiddle=5;
	    }
	    else if(sensorDelay == sm.SENSOR_DELAY_GAME)
	    {
			bufsize=11;
		    bufmiddle=5;
	    }
	    else if(sensorDelay == sm.SENSOR_DELAY_NORMAL)
	    {
			bufsize=11;
		    bufmiddle=5;
	    }
	    else if(sensorDelay == sm.SENSOR_DELAY_UI)
	    {
			bufsize=11;
		    bufmiddle=1;
	    }	    
	    
	    bufpoint=0;
	    azimuthArr = new double[bufsize];
	    pitchArr= new double[bufsize];
	    rollArr = new double[bufsize];
	    
	}
	public synchronized void onDrawFrame(GL10 gl) {
		
		double tmpAzimuthArr[][] = new double[bufsize][2];
		double tmpPitchArr[][] = new double[bufsize][2];
		double tmpRollArr[][] = new double[bufsize][2];
		float azimuth;
		float pitch;
		float roll;
		synchronized (this) {
			
			double pi =Math.PI;
			double rad2deg = 180/pi;
			for(int i=0; i < bufsize;i++)
			{
				tmpAzimuthArr[i][0]=azimuthArr[i];
				tmpPitchArr[i][0]=pitchArr[i];
				tmpRollArr[i][0]=rollArr[i];
				
				if(azimuthArr[i]>180)
				{
					tmpAzimuthArr[i][1] = 180 - azimuthArr[i]; 
				}
				else
				{
					tmpAzimuthArr[i][1] = azimuthArr[i];
				}
				
				if(pitchArr[i]>0)
				{
					tmpPitchArr[i][1] = 0-pitchArr[i];
				}
				else
				{
					tmpPitchArr[i][1] = pitchArr[i];
				}
				
				if(rollArr[i]>0)
				{
					tmpRollArr[i][1] = 0-rollArr[i];
				}
				else
				{
					tmpRollArr[i][1] = rollArr[i]; 
				}
			}
		}
			int fpoint=bufsize-1;
			for(int i=0; i < bufsize; i++)
			{
				for(int j=0;j<fpoint;j++)
				{
					if(tmpAzimuthArr[j][1]>tmpAzimuthArr[j+1][1])
					{
						double tf=tmpAzimuthArr[j][0];
						double ts=tmpAzimuthArr[j][1];
						tmpAzimuthArr[j][0] =tmpAzimuthArr[j+1][0];
						tmpAzimuthArr[j][1] =tmpAzimuthArr[j+1][1];
						tmpAzimuthArr[j+1][0] =tf;
						tmpAzimuthArr[j+1][1] =ts;
					}
					if(tmpPitchArr[j][1]>tmpPitchArr[j+1][1])
					{
						double tf=tmpPitchArr[j][0];
						double ts=tmpPitchArr[j][1];
						tmpPitchArr[j][0] =tmpPitchArr[j+1][0];
						tmpPitchArr[j][1] =tmpPitchArr[j+1][1];
						tmpPitchArr[j+1][0] =tf;
						tmpPitchArr[j+1][1] =ts;
					}
					if(tmpRollArr[j][1]>tmpRollArr[j+1][1])
					{
						double tf=tmpRollArr[j][0];
						double ts=tmpRollArr[j][1];
						tmpRollArr[j][0] =tmpRollArr[j+1][0];
						tmpRollArr[j][1] =tmpRollArr[j+1][1];
						tmpRollArr[j+1][0] =tf;
						tmpRollArr[j+1][1] =ts;
					}
				}
				fpoint--;
			}
			
			azimuth = (float) tmpAzimuthArr[bufmiddle][0];
			pitch = (float) tmpPitchArr[bufmiddle][0];
			roll = (float) tmpRollArr[bufmiddle][0];
		
		
		if (hasImage) {
		ArLib.precessImage(frame, azimuth, pitch, roll);
			this.notify();
		}
	}
	
	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {

        synchronized (this) {
            System.arraycopy(data, 0, this.frame, 0, data.length);
            hasImage = true;
            this.notify();
        }
        cam.addCallbackBuffer(buffer);
	}

	public void onSurfaceChanged(GL10 gl, int width, int height) {
		openCamera();
		setupCamera(1134, 720);
		ArLib.initArLib(width, height, frameWidth, frameHeight);
	}

	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		// stub method
	}


	
    public void openCamera() {
        Log.i(TAG, "openCamera");
        releaseCamera();
        cam = Camera.open();
        if(cam == null) {
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
        };
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
                    int  minDiff = Integer.MAX_VALUE;
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
                if (FocusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO))
                {
                	params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
                }            
                
                cam.setParameters(params);
                
                /* Now allocate the buffer */
                params = cam.getParameters();
                frameWidth = params.getPreviewSize().width;
                frameHeight = params.getPreviewSize().height;
                Log.d(TAG, String.format("frame size: %dx%d",frameWidth, frameHeight));

                int size = params.getPreviewSize().width * params.getPreviewSize().height;
                Log.d(TAG, String.format("preview format: %d",params.getPreviewFormat()));
                Log.d(TAG, String.format("bits per pixel: %d",ImageFormat.getBitsPerPixel(params.getPreviewFormat())));
                size  = size * ImageFormat.getBitsPerPixel(params.getPreviewFormat()) / 8;
                buffer = new byte[size];
                /* The buffer where the current frame will be copied */
                frame = new byte [size];
                Log.i(TAG,String.format("frame length: %d", frame.length));
                
                cam.addCallbackBuffer(buffer);

    		    try {
					cam.setPreviewTexture( new SurfaceTexture(10) );
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}

                /* Notify that the preview is about to be started and deliver preview size */
                //onPreviewStarted(params.getPreviewSize().width, params.getPreviewSize().height);

                /* Now we can start a preview */
                cam.startPreview();
            }
        }
    }
	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {
		// TODO Auto-generated method stub
		
	}
	@Override
	public void onSensorChanged(SensorEvent event) {
		
		switch (event.sensor.getType())
        {
        	case Sensor.TYPE_MAGNETIC_FIELD:
            mags = event.values.clone();
            break;
        	case Sensor.TYPE_ACCELEROMETER:
            accels = event.values.clone();
            break;     
        	case Sensor.TYPE_GYROSCOPE:
        	gyro = event.values.clone();
        	break;
        }
        
        if (mags != null && accels != null) 
        {
            SensorManager.getRotationMatrix(RE, I, accels, mags);
            // Correct if screen is in Landscape
            SensorManager.remapCoordinateSystem(RE, SensorManager.AXIS_X,SensorManager.AXIS_Z, outR);
            //Calculate Orientation
            SensorManager.getOrientation(outR, values);
            
            bufpoint=(bufpoint +1)%bufsize;

            azimuthArr[bufpoint] = (Math.toDegrees(values[0])+360)%360;
            pitchArr[bufpoint] = Math.toDegrees(values[1]);//(Math.toDegrees(values[1])+360)%360;
            rollArr[bufpoint] = Math.toDegrees(values[2]);//(Math.toDegrees(values[2])+360)%360;
            
            mags=null;
            accels=null;
            

        }
        /*
        if(gyro != null)
        {
        	if(Math.round(Math.toDegrees(gyro[0]))!=0)
        		//outputOrTv1.setText(Double.toString(Math.round(Math.toDegrees(gyro[0])))+deg);
        	if(Math.round(Math.toDegrees(gyro[1]))!=0)
        		//outputOrTv2.setText(Double.toString(Math.round(Math.toDegrees(gyro[1])))+deg);
        	if(Math.round(Math.toDegrees(gyro[2]))!=0)
        		//outputOrTv3.setText(Double.toString(Math.round(Math.toDegrees(gyro[2])))+deg);
            gyro=null;
        }*/
	}
}
