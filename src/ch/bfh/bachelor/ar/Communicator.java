package ch.bfh.bachelor.ar;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.io.StreamCorruptedException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.ArrayList;

import android.util.Log;
import ch.bfh.arApp.com.GetObjects;
import ch.bfh.arApp.com.SendObjects;
import ch.bfh.arApp.model3D.Object3D;



public class Communicator implements Runnable
{
	public String msg;
	public ArrayList<Object3D> l;
	String ip;
	int port;
	private Socket socket;
	private LocManager lm;
	/*
	 * Constructor with the Connectionparameter
	 */
	public Communicator (String ip, int port, LocManager lm)
	{
		this.lm = lm;
		this.ip=ip;
		this.port=port;
	}
	//Function to request Objects
	private void sendObject(Serializable s)
	{
		ObjectOutputStream oos =null;
		try {
			oos = new ObjectOutputStream(socket.getOutputStream());
			oos.writeObject(s);
			oos.flush();
		} catch (IOException e) {
			System.out.println("CLIENTFEHLER!");
			e.printStackTrace();
		}
	}
	//Function to receive Objects
	private void getObject()
	{
		
		ObjectInputStream ois;
		try {
			ois = new ObjectInputStream(new BufferedInputStream(socket.getInputStream()));
			Object o = ois.readObject();
			if(o instanceof String)
			{
				msg = (String) o;
			}
			if(o instanceof Object3D)
			{
				msg = "it's an Model3D";
			}
			if(o instanceof SendObjects)
			{
				
				l = ((SendObjects) o).getModel3D();
				msg = "it's an SendObjects!: "+l.get(0).northangle;
			}
			else
			{
				msg="unknown object...";
			}
		} catch (StreamCorruptedException e) {
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	public void run() {
		
		GetObjects go = new GetObjects(lm.getCurrentLon(), lm.getCurrentLat(), 10);
		try {
			//request Objects
			socket = new Socket(ip, port);
			Log.w("msg1", "request objects");
			this.sendObject(go);
			//get Objects
			Log.w("msg1", "waiting for requested objects");
			this.getObject();
			
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
