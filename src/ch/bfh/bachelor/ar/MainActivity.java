package ch.bfh.bachelor.ar;

import java.util.ArrayList;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;
import ch.bfh.arApp.model3D.Object3D;
import ch.bfh.bachelor.ar.opengl.CameraRenderer;
import ch.bfh.bachelor.ar.opengl.OpenGLSurfaceView;

public class MainActivity extends Activity {
    private static final String TAG = "MainActivity";
	private CameraRenderer cr;
	private  ArrayList<Object3D> l;
	private static final boolean loadModules=true;
	
    private BaseLoaderCallback  mOpenCVCallBack = new BaseLoaderCallback(this) {
    	@Override
    	public void onManagerConnected(int status) {
    		switch (status) {
				case LoaderCallbackInterface.SUCCESS:
				{
					Log.i(TAG, "OpenCV loaded successfully");

					//instantiate open gl renderer
					cr = new CameraRenderer(MainActivity.this);
					
					View mainView = new OpenGLSurfaceView(getApplication(), cr);					
					setContentView(mainView);
					
				} 
				break;
				default:
				{
					super.onManagerConnected(status);
				} break;
			}
    	}
	};
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		if(loadModules)
		{
		       l = null;
		       Communicator c = new Communicator("83.169.4.84", 3131);
		       Thread a = new Thread(c);
		       a.start();
		       try {
		    	   a.join();
		    	   l = c.l;
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
		       if(l!=null)
		       {
		    	   //Übergeben der Objekte..
		    	   for(int i=0;i<l.size();i++)
		    	   {
		    		   Object3D b = l.get(i);
		    		   short[] facesSh = new short[b.face.length];
		    		   float[] bbox = new float[24];
		    		   for(int q=0;q<8;q++)
		    		   {
		    			   for(int u=0;u<3;u++)
		    			   {
		    				   bbox[q*u] = b.bb[q][u];
		    			   }
		    		   }
		    		   for(int j =0;j<b.face.length;j++)
		    		   {
		    			   facesSh[j] = (short) b.face[j];
		    		   }
		    					   
		    		   ArLib.addModel(b.vnt, facesSh, b.pointOfGravity, bbox, b.northangle, b.lat, b.lon);
		    	   }
		       }
		}
		
        if (!OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_2, this, mOpenCVCallBack))
        {
        	Log.e(TAG, "Cannot connect to OpenCV Manager");
        }

	}
	
	@Override
	protected void onResume() {
		super.onResume();
		
	}
	
	@Override
	protected void onPause() {
		super.onPause();
		cr.releaseSensor();
		cr.releaseCamera();
	}
	
	//menu
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.activity_main, menu);
		return true;
	}
	
   @Override
    public boolean onOptionsItemSelected(MenuItem item)
    { 	
    	//check selected menu item
    	// R.id.exit is @+id/exit
    	if(item.getItemId() == R.id.menu_sensorCorrection){
    		cr.isSensorCorrectionEnabled = !cr.isSensorCorrectionEnabled;
    		if (cr.isSensorCorrectionEnabled)
    			item.setTitle(R.string.menu_sensorCorrection_off);
    		else
    			item.setTitle(R.string.menu_sensorCorrection_on);
    		item.setChecked(cr.isSensorCorrectionEnabled);
    	}
    	if(item.getItemId() == R.id.menu_sensorBuffer){
    		cr.isSensorBufferingEnable = !cr.isSensorBufferingEnable;
    		if (cr.isSensorBufferingEnable)
    			item.setTitle(R.string.menu_sensorBuffer_off);
    		else
    			item.setTitle(R.string.menu_sensorBuffer_on);
    		item.setChecked(cr.isSensorBufferingEnable);
    	}
    	if(item.getItemId() == R.id.menu_gyroCorrection){
    		cr.isGyroCorrectionEnable = !cr.isGyroCorrectionEnable;
    		if (cr.isGyroCorrectionEnable)
    			item.setTitle(R.string.menu_gyroCorrection_off);
    		else
    			item.setTitle(R.string.menu_gyroCorrection_on);
    		item.setChecked(cr.isGyroCorrectionEnable);
    	}
    	return false;
    }
	
		
}
