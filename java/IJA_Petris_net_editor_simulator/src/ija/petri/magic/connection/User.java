package ija.petri.magic.connection;

import java.io.Serializable;

/**
 * Object which has information about user.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class User implements Serializable
{
	String nick;
	int password;
	
	public User()
	{
		
	}
	
	/**
	 * Sets nick and password for user.
	 * @param nick
	 * @param password 
	 */
	public void setNickPassword(String nick, int password)
	{
		this.nick = nick;
		this.password = password;
	}
	
	/**
	 * 
	 * @return user's nick
	 */
	public String getNick()
	{
		return this.nick;
	}
	
	/**
	 * 
	 * @return user's hashed password
	 */
	public int getPassword()
	{
		return this.password;
	}
}
