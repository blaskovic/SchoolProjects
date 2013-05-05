package ija.petri.server.connection;

import ija.petri.magic.connection.Communication;
import ija.petri.magic.connection.ConnectionMain;
import ija.petri.magic.petrinet.PetrinetMain;
import ija.petri.server.files.FileVersion;
import ija.petri.server.files.SimulationHistory;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * Class which is working as thread on server.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class ServerConnection extends ConnectionMain implements Runnable
{
	private boolean userLogged;
	private String userName;
	
	/**
	 * Set connection details.
	 * @param port
	 * @param socket
	 * @param serverSocket 
	 */
	public ServerConnection(int port, Socket socket, ServerSocket serverSocket)
	{
		// Default
		this.userLogged = false;
		this.userName = "";
		this.port = port;
		this.socket = socket;
		this.serverSocket = serverSocket;
	}

	/**
	 * Create thread and wait in loop for communication commands.
	 */
	@Override
	public void run()
	{
		createSocket();
		Authentization authentization = new Authentization();
		FileVersion fileVersion = new FileVersion();
		
		Object received;
		
		// Main listening loop
		while(true)
		{
			received = getObject();

			if(received == null)
			{
				System.err.println("Error: listen(): null received (shutting down thread)");
				closeConnection();
				return;
			}
			if(received instanceof Communication)
			{
				Communication communicate = (Communication)received;
				String command = communicate.getCommand();
				
				// Which command do you want?
				if("login".equals(command))
				{
					String nick = (String)getObject();
					Integer pass = (Integer)getObject();
					System.err.println("Credentials: " + nick + ":" + pass);
					Boolean respond = authentization.isValidUser(nick, pass);
					userOnline(nick);	
					sendObject(respond);
				}
				if("register".equals(command))
				{
					String nick = (String)getObject();
					Integer pass = (Integer)getObject();
					System.err.println("Want register: " + nick + ":" + pass);
					// Empty nick?
					if("".equals(nick))
					{
						sendObject(new Communication("Empty nick.", false));
								
					}
					// Free nick?
					else if(authentization.userExists(nick))
					{
						sendObject(new Communication("User already exists", false));
					}
					// Add it
					else
					{
						authentization.addUser(nick, pass);
						sendObject(new Communication("User successfuly registered.", true));
					}
				}
				if("save".equals(command))
				{
					PetrinetMain petriNet = (PetrinetMain)getObject();
					String respond;
					// User not logged in
					if(!isLogged())
					{
						respond = "User not logged in.";	
					}
					// Empty name?
					else if("".equals(petriNet.getName()))
					{
						respond = "Empty net name.";
					}
					// Add it
					else
					{
						petriNet.setAuthor(this.userName);
						fileVersion.addNewNet(petriNet);						
						respond = "Petri's net saved on remote server.";
					}

					sendObject(respond);
					sendObject(petriNet);
				}
				if("getPetrinet".equals(command))
				{
					int id = (Integer)getObject();
					int version = (Integer)getObject();

					System.err.println("Get petrinet #" + id + " v:" + version);
					
					FileVersion files = new FileVersion();
					PetrinetMain petri = files.getPetrinet(id, version);
					sendObject(petri);
				}
				if("getFiles".equals(command))
				{
					FileVersion files = new FileVersion();
					sendObject(files.getFiles());
				}
				if("getHistory".equals(command))
				{
					SimulationHistory history = new SimulationHistory();
					sendObject(history.getHistory());
				}
				if("step".equals(command))
				{
					PetrinetMain petri = (PetrinetMain)getObject();
					petri.stepForward();
					PetrinetMain toSend = (PetrinetMain)petri.clone();
					sendObject(toSend);
				}
				if("simulate".equals(command))
				{
					PetrinetMain petri = (PetrinetMain)getObject();

					// Write down simulation
					if(petri.getId() > 0 && petri.getVersion() > 0)
					{
						SimulationHistory history = new SimulationHistory();
						history.addToHistory(userName, petri.getId(), petri.getVersion());
					}

					petri.simulate();
					
					// Send back
					PetrinetMain toSend = (PetrinetMain)petri.clone();
					sendObject(toSend);
				}
			}
			else
			{
				System.err.println("Error: listen(): Unexpected object received.");
			}
		}
	}
	
	/**
	 * Creates socket for connection.
	 */
	private void createSocket()
	{
		try
		{
			// Sockets
			//this.serverSocket = new ServerSocket(this.port);
			//this.socket = this.serverSocket.accept();
			// Output stream
			this.outputStream = this.socket.getOutputStream();
			this.objectOutputStream = new ObjectOutputStream(this.outputStream);
			// Input stream
			this.inputStream = this.socket.getInputStream();
			this.objectInputStream = new ObjectInputStream(this.inputStream);
		}
		catch(Exception e)
		{
			System.err.println("Error: createSocket(): " + e);
		}
	}

	/**
	 * Put user online.
	 * @param username 
	 */
	public void userOnline(String username)
	{
		this.userLogged = true;
		this.userName = username;
	}

	/**
	 * 
	 * @return true if user is logged in
	 */
	public boolean isLogged()
	{
		return this.userLogged;
	}
	
	/**
	 * Safely close connection.
	 */
	public void closeConnection()
	{
		try
		{
			System.err.println("Shutting down");
			this.objectInputStream.close();
			this.inputStream.close();
			this.outputStream.close();
			this.objectOutputStream.close();
			this.socket.close();
		}
		catch (Exception e)
		{
			System.err.println("Error: closeConnection(): " + e);
		}
	}

}
