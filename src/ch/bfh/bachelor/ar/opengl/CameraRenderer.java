package ch.bfh.bachelor.ar.opengl;

import java.io.IOException;
import java.util.List;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLSurfaceView;
import android.util.Log;
import ch.bfh.bachelor.ar.ArLib;

public class CameraRenderer implements GLSurfaceView.Renderer,
		android.hardware.Camera.PreviewCallback {
    private static final String TAG = "CameraRenderer";
    
	public int frameWidth = 0;
	public int frameHeight = 0;
	public byte[] frame = null;
	public byte[] buffer = null;	
	
	
	
	public boolean hasImage = false;
	private Camera cam;
	
	public synchronized void onDrawFrame(GL10 gl) {
		if (hasImage) {
			ArLib.precessImage(frame);
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

}
