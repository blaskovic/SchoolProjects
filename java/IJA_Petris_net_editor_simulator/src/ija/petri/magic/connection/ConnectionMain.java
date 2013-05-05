package ija.petri.magic.connection;

import java.io.InputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * Main connection class which is extended by server and client sides.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class ConnectionMain
{
	protected String address;
	protected int port;
	protected Socket socket;
	protected boolean connected;
	// Client
	protected ObjectOutputStream objectOutputStream;
	protected OutputStream outputStream;
	// Server
	protected ServerSocket serverSocket;
	protected InputStream inputStream;
	protected ObjectInputStream objectInputStream;
	
	public ConnectionMain()
	{
		
	}
	
	/**
	 * Get object from stream.
	 * @return new received object
	 */
	public Object getObject()
	{
		try
		{
			//ObjectInputStream ois = new ObjectInputStream(this.inputStream);
			Object received = (Object)this.objectInputStream.readObject();
			if(received != null)
			{
				return received;
			}
			
			//ois.close();
			return null;
		}
		catch(Exception e)
		{
			System.err.println("Error: getObject(): " + e);
			return null;
		}
	}
	
	/**
	 * Send serialized object to stream.
	 * @param toSend 
	 */
	public void sendObject(Object toSend)
	{
		try
		{
			this.objectOutputStream.writeObject(toSend);
		}
		catch(Exception e)
		{
			System.err.println("Error: sendObject(): " + e);
		}
	}
	
	/**
	 * Hash password.
	 * @param input
	 * @return hashed password
	 */
	public int hashIt(String input)
	{
		return input.hashCode();
	}
}
