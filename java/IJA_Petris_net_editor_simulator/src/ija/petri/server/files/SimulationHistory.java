package ija.petri.server.files;

import ija.petri.magic.files.XMLReader;
import ija.petri.magic.files.XMLWriter;
import ija.petri.server.simulation.Simulation;
import java.util.Vector;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

/**
 * Class for working with simulation history XML.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class SimulationHistory extends ServerFilesMain
{
	protected String fileName = this.serverFolderName + "/simulation_history.xml";
	// XML stuff
	protected String xmlActual;
	protected String netId;
	protected String netAuthor;
	protected String netVersion;
	protected String netDate;
	protected Vector<Simulation> history;

	public SimulationHistory()
	{
		this.history = new Vector<Simulation>();
	}
	
	/**
	 * Gets history vector.
	 * @return vector with history
	 */
	public Vector<Simulation> getHistory()
	{
		loadHistory();
		return this.history;
	}

	/**
	 * Add simulation to history
	 * @param netAuthor
	 * @param netId
	 * @param netVersion 
	 */
	public void addToHistory(String netAuthor, int netId, int netVersion)
	{
		Simulation simulation = new Simulation(netAuthor, netId, netVersion, (int)(System.currentTimeMillis()/1000L));
		loadHistory();
		this.history.addElement(simulation);
		saveHistory();
	}

	/**
	 * Save history vector as XML file.
	 */
	private void saveHistory()
	{
		XMLWriter writer = new XMLWriter();
		writer.setOutputFile(this.fileName);
		
		for (int i = 0; i < this.history.size(); i++)
		{
			Simulation obj = this.history.elementAt(i);

			String line;

			line = writer.createElement(
					"simulation",
					writer.createElement("netId", String.format("%d", obj.getId())) +
					writer.createElement("netVersion", String.format("%d", obj.getVersion())) +
					writer.createElement("netDate", String.format("%d", obj.getDate())) +
					writer.createElement("netAuthor", obj.getAuthor()),
					true,
					false
			);
			
			writer.appendOutput(line);
		}

		writer.echoOutput("root");
	}

	/**
	 * Loads history from XML file.
	 */
	private void loadHistory()
	{
		this.history.clear();
		
		DefaultHandler handler = new DefaultHandler()
		{
			@Override
			public void endElement(String uri, String localName, String qName) throws SAXException
			{
				// Atributes
				if(qName.equalsIgnoreCase("netId"))
				{
					netId = xmlActual;
				}

				if(qName.equalsIgnoreCase("netAuthor"))
				{
					netAuthor = xmlActual;
				}

				if(qName.equalsIgnoreCase("netVersion"))
				{
					netVersion = xmlActual;
				}

				if(qName.equalsIgnoreCase("netDate"))
				{
					netDate = xmlActual;
				}

				// Objects
				if(qName.equals("simulation"))
				{
					Simulation simulation = new Simulation(netAuthor, Integer.parseInt(netId), Integer.parseInt(netVersion), Integer.parseInt(netDate));
					history.addElement(simulation);
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
			System.err.println("Error: loadHistory(): " + e);
		}

	}
}
