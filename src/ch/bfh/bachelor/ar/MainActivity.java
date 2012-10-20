package ch.bfh.bachelor.ar;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ListView;
import ch.bfh.bachelor.ar.opengl.CameraRenderer;
import ch.bfh.bachelor.ar.opengl.OpenGLSurfaceView;

public class MainActivity extends Activity {
    private static final String TAG = "MainActivity";
	private CameraRenderer cr;
	
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
    	return false;
    }
	
		
}
