package ija.petri.server;

import ija.petri.server.connection.ServerConnection;
import ija.petri.server.files.ServerFilesMain;
import java.io.File;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * Main class of server application
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
class ServerMain
{
	protected File serverFolder;

	/**
	 * Main class for server
	 * @param argv first parameter could be port 
	 */
	public static void main(String argv[])
	{
		System.out.println("Main server");
		
		// Create server folder, if not exists
		ServerFilesMain files = new ServerFilesMain();
		files.createServerFolder();
		
		int port = 2000;
		if(argv.length == 1)
		{
			try
			{
				port = Integer.parseInt(argv[0]);
			}
			catch(Exception e) {}
		}
		System.err.println("Starting server at port: " + port);
		
		// Socket
		ServerSocket serverSocket = null;
		try
		{
			serverSocket = new ServerSocket(port);
		}
		catch (Exception e)
		{
			System.err.println("Error: main():" + e);
		}
		
		// Listen
		while(true)
		{
			try
			{
				Socket socket = serverSocket.accept();
				if(socket != null)
				{
					ServerConnection connection = new ServerConnection(port, socket, serverSocket);
					new Thread(connection).start();
				}
			}
			catch(Exception e)
			{
				System.err.println("error: main():" + e);
			}
		}
	}
}
