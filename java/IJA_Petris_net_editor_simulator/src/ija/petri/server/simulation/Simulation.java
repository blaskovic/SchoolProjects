package ija.petri.server.simulation;

import java.io.Serializable;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Class which has info about 1 simulation.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class Simulation implements Serializable
{
	private String netAuthor;
	private int netId;
	private int netVersion;
	private int netDate;
	
	/**
	 * Create simulation object with provided settings.
	 * @param netAuthor
	 * @param netId
	 * @param netVersion
	 * @param netDate 
	 */
	public Simulation(String netAuthor, int netId, int netVersion, int netDate)
	{
		this.netAuthor = netAuthor;
		this.netId = netId;
		this.netVersion = netVersion;
		this.netDate = netDate;
	}

	public String getAuthor()
	{
		return this.netAuthor;
	}

	public int getId()
	{
		return this.netId;
	}

	public int getVersion()
	{
		return this.netVersion;
	}

	public int getDate()
	{
		return this.netDate;
	}

	/**
	 * Gets date as formated string.
	 * @return String with formated date 
	 */
	public String getDateFormat()
	{
		Date date = new Date((long)this.netDate*1000);
		SimpleDateFormat sdf = new SimpleDateFormat("d.MM.yyyy H:mm");
		String formattedDate = sdf.format(date);
		return formattedDate;
	}

}
