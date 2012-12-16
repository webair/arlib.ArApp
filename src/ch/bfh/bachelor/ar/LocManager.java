package ch.bfh.bachelor.ar;

import java.util.Timer;
import java.util.TimerTask;

import android.content.Context;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.util.Log;

public class LocManager implements LocationListener{
	//Location
	LocationManager lm;
	private static final long MINIMUM_DISTANCE_CHANGE_FOR_UPDATES = 1;
	private static final long MINIMUM_TIME_BETWEEN_UPDATES = 500;
	private float currentLat;
	private float currentLon;
	private boolean gpsSet;
	private Timer timer;
	private TimerTask timerTask;
	private static final String TAG="LocationManager";
	private boolean testing;
	
	public LocManager(Context context, boolean testing)
	{
		this.testing = testing;
		if(!testing)
		{
			this.gpsSet = false;
			timer = new Timer();
			timer.scheduleAtFixedRate(new TimerTask() {
				@Override
				public void run() {
					if(gpsSet)
					{
						timer.cancel();
						Log.i(TAG, "Location found");
					}
					else
					{
						Log.i(TAG, "Location not found");
					}
				}
			}, 0, 500l);
			lm = (LocationManager) context.getSystemService(context.LOCATION_SERVICE);
	         
	        lm.requestLocationUpdates(
		    LocationManager.GPS_PROVIDER,
		    MINIMUM_TIME_BETWEEN_UPDATES,
		    MINIMUM_DISTANCE_CHANGE_FOR_UPDATES,
		    this
	        );
		}
		else
		{
			//schreg vordrann
			//this.currentLat=46.947337f;
			///this.currentLon=7.443957f;
			//grad vordranne
			this.currentLat=46.947319f;
			this.currentLon=7.443999f;
		}
	}
	@Override
	public void onLocationChanged(Location location) {
		if(!testing)
		{
		this.currentLat = (float) location.getLatitude();
		this.currentLon = (float) location.getLongitude();
		this.gpsSet = true;
		}
	}

	@Override
	public void onProviderDisabled(String provider) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onProviderEnabled(String provider) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onStatusChanged(String provider, int status, Bundle extras) {
		// TODO Auto-generated method stub
		
	}
	public float getCurrentLat() {
		return currentLat;
	}
	public void setCurrentLat(float currentLat) {
		this.currentLat = currentLat;
	}
	public float getCurrentLon() {
		return currentLon;
	}
	public void setCurrentLon(float currentLon) {
		this.currentLon = currentLon;
	}
	public boolean isTesting() {
		return testing;
	}
	public void setTesting(boolean testing) {
		this.testing = testing;
	}
}
