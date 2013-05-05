package ija.petri.server.connection;

import ija.petri.magic.connection.User;
import ija.petri.magic.connection.UserXML;

/**
 * Class for validating logged users.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class Authentization extends UserXML
{
	public Authentization()
	{

	}
	
	/**
	 * Check for valid user.
	 * @param nick
	 * @param password
	 * @return true if user is valid
	 */
	public boolean isValidUser(String nick, int password)
	{
		if(userExists(nick))
		{
			User user = getUser(nick);
			if(user != null)
			{
				if(user.getPassword() == password)
				{
					return true;
				}
			}
		}
		
		return false;
	}
}
