package ija.petri.server.files;

import java.io.File;

/**
 * Class for creating and setting name of server data folder.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class ServerFilesMain
{
	protected String serverFolderName = "server_data";		
	protected File serverFolder;
	
	/**
	 * Created server folder if not exists.
	 */
	public void createServerFolder()
	{
		this.serverFolder = new File(this.serverFolderName);
		try
		{
			if(!this.serverFolder.isDirectory())
			{
				this.serverFolder.mkdir();
			}
		}
		catch(Exception e)
		{
			System.err.println("Error: createServerFolder(): " + e);
		}
	}	
}
