package ija.petri.magic.connection;

import java.io.Serializable;

/**
 * Communication class for talking between client and server.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class Communication implements Serializable
{
	private String command;
	private boolean success;
	
	/**
	 * Create communication class with command and success flag.
	 * @param command
	 * @param success 
	 */
	public Communication(String command, boolean success)
	{
		this.command = command;
		this.success = success;
	}
	
	/**
	 * Create communication class with command and success flag with false.
	 * @param command 
	 */
	public Communication(String command)
	{
		this.command = command;
		this.success = false;
	}
	
	/**
	 * 
	 * @return command string
	 */
	public String getCommand()
	{
		return this.command;
	}

	public boolean success()
	{
		return this.success;
	}
}
