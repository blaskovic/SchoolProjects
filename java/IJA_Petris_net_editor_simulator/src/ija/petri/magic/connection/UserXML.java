package ija.petri.magic.connection;

import ija.petri.magic.files.XMLReader;
import ija.petri.magic.files.XMLWriter;
import ija.petri.server.files.ServerFilesMain;
import java.util.Vector;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

/**
 * Class for working with user's xml database.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class UserXML extends ServerFilesMain
{
	protected Vector<User> users;
	
	public String xmlActual;
	public String xmlNick;
	public String xmlPassword;
	private String fileName = this.serverFolderName + "/server_users.xml";
	
	public UserXML()
	{
		this.users = new Vector<User>();
	}
	
	/**
	 * Add user to user's xml file database.
	 * @param nick
	 * @param password 
	 */
	public void addUser(String nick, int password)
	{
		loadUsers();
		
		User newUser = new User();
		newUser.setNickPassword(nick, password);
		
		this.users.addElement(newUser);
		
		saveUsers();
	}
	
	/**
	 * Get users from XML.
	 * @return user's vector
	 */
	@SuppressWarnings("UseOfObsoleteCollectionType")
	public Vector<User> getUsers()
	{
		loadUsers();
		return this.users;
	}
	
	/**
	 * Save user's vector as XML.
	 */
	private void saveUsers()
	{
		XMLWriter writer = new XMLWriter();
		writer.setOutputFile(this.fileName);

		for (int i = 0; i < this.users.size(); i++)
		{
			String line = "";
			line = writer.createElement(
					"user",
					writer.createElement("nick", this.users.elementAt(i).getNick()) +
					writer.createElement("password", String.format("%d", this.users.elementAt(i).getPassword())),
					true,
					false
			);
				
			writer.appendOutput(line);
		}
		
		writer.echoOutput("root");
	}
	
	/**
	 * Get user from vector.
	 * @param nick
	 * @return reference to User object.
	 */
	public User getUser(String nick)
	{
		loadUsers();
		for (int i = 0; i < this.users.size(); i++)
		{
			if(nick.equals(this.users.elementAt(i).getNick()))
			{
				return this.users.elementAt(i);
			}
		}
		return null;
	}
	
	/**
	 * 
	 * @param nick
	 * @return true, if user exists
	 */
	public boolean userExists(String nick)
	{
		loadUsers();
		for (int i = 0; i < this.users.size(); i++)
		{
			if(nick.equals(this.users.elementAt(i).getNick()))
			{
				return true;
			}
		}
		return false;
	}
	
	/**
	 * Loads users from XML.
	 */
	private void loadUsers()
	{
		this.users.clear();
		DefaultHandler handler = new DefaultHandler()
		{

			@Override
			public void endElement(String uri, String localName, String qName) throws SAXException
			{
				// Atributes
				if(qName.equalsIgnoreCase("nick"))
				{
					xmlNick = xmlActual;
				}

				if(qName.equalsIgnoreCase("password"))
				{
					xmlPassword = xmlActual;
				}

				// Objects
				if(qName.equals("user"))
				{
					User user = new User();
					user.setNickPassword(xmlNick, Integer.parseInt(xmlPassword));
					users.addElement(user);
				}
			}

			@Override
			public void characters(char ch[], int start, int length) throws SAXException
			{
				xmlActual = new String(ch, start, length);
			}
		};

		try
		{
			XMLReader read = new XMLReader(this.fileName);
			read.setHandler(handler);
			read.startParse();
		}
		catch(Exception e)
		{
			System.err.println("Error: loadUsers(): " + e);
		}
	}
}
