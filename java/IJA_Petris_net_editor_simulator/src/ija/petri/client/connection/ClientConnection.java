package ija.petri.client.connection;

import ija.petri.client.gui.Editor;
import ija.petri.magic.connection.ConnectionMain;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;

/**
 * Class for network connection on client's side.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class ClientConnection extends ConnectionMain
{
	protected Editor editor;

	public ClientConnection()
	{
		this.connected = false;
		this.address = "";
	}

	/**
	 * Set editor class.
	 * @param editor 
	 */
	public void setEditor(Editor editor)
	{
		this.editor = editor;
	}
	
	/**
	 * Set server details for connection.
	 * @param address
	 * @param port 
	 */
	public void setServerDetails(String address, int port)
	{
		this.address = address;
		this.port = port;
	}
	
	/**
	 * Try to connect to server via provided settings.
	 */
	public void connectToServer()
	{
		try
		{
			this.socket = new Socket(this.address, this.port);
			// Output stream
			this.outputStream = this.socket.getOutputStream();
			this.objectOutputStream = new ObjectOutputStream(this.outputStream);
			// Input stream
			this.inputStream = this.socket.getInputStream();
			this.objectInputStream = new ObjectInputStream(this.inputStream);
			this.connected = true;
			this.editor.menuBar.toggleServerOperations(true);
		}
		catch (Exception e)
		{
			this.editor.menuBar.toggleServerOperations(false);
			System.err.println("Error: connectToServer(): " + e);
			this.connected = false;
		}
	}
	
	public boolean isConnected()
	{
		return this.connected;
	}
}
