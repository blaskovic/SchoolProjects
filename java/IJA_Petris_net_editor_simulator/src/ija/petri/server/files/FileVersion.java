package ija.petri.server.files;

import ija.petri.magic.files.XMLReader;
import ija.petri.magic.files.XMLWriter;
import ija.petri.magic.petrinet.PetriXMLGenerator;
import ija.petri.magic.petrinet.PetriXMLReader;
import ija.petri.magic.petrinet.PetrinetMain;
import java.util.Vector;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

/**
 * Class which works with XML file of file versions saved on server.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class FileVersion extends ServerFilesMain
{
	protected String fileName = this.serverFolderName + "/file_versions.xml";

	// XML stuff
	protected String xmlActual;
	protected String netId;
	protected String netName;
	protected String netDescription;
	protected String netAuthor;
	protected String netVersion;
	protected String netDate;
	protected Vector<PetrinetMain> files;

	public FileVersion()
	{
		this.files = new Vector<PetrinetMain>();
		System.out.println(this.fileName);
	}

	/**
	 * Add new net and save it as XML files on server.
	 * @param petriNet 
	 */
	public void addNewNet(PetrinetMain petriNet)
	{
		loadFiles();

		int id = petriNet.getId();
		int version = petriNet.getVersion();
		
		// Not saved before?
		if(id == 0)
		{
			id = getLastId() + 1;
		}
		
		// Update object
		petriNet.setInfo(id, petriNet.getName(), petriNet.getDescription(), petriNet.getAuthor(), getLastVersion(id) + 1, (int)(System.currentTimeMillis()/1000L));

		// Generate XML for this net
		PetriXMLGenerator xml = new PetriXMLGenerator(petriNet.getObjects());
		xml.generateXML(this.serverFolderName + "/petrinet" + petriNet.getId() + "-" + petriNet.getVersion() + ".xml");
		
		// Save it
		this.files.addElement(petriNet);
		saveFiles();

	}

	/**
	 * Get files vector.
	 * @return vector of files on server.
	 */
	public Vector<PetrinetMain> getFiles()
	{
		loadFiles();
		return this.files;
	}
	
	/**
	 * 
	 * @param id
	 * @return last version of net's id
	 */
	private int getLastVersion(int id)
	{
		loadFiles();
		// Loop through files, select matching ID and find biggest version
		int biggestVersion = 0;
		for(int i = 0; i < this.files.size(); i++)
		{
			if(this.files.elementAt(i).getId() == id && biggestVersion < this.files.elementAt(i).getVersion())
				biggestVersion = this.files.elementAt(i).getVersion();
		}

		return biggestVersion;
	}

	/**
	 * Auto-increment method.
	 * @return last used id
	 */
	private int getLastId()
	{
		loadFiles();	
		// Loop through files and save the biggest ID
		int biggestId = 0;
		for(int i = 0; i < this.files.size(); i++)
		{
			if(biggestId < this.files.elementAt(i).getId())
				biggestId = this.files.elementAt(i).getId();
		}

		return biggestId;
	}

	/**
	 * Save files vector as XML.
	 */
	private void saveFiles()
	{
		XMLWriter writer = new XMLWriter();
		writer.setOutputFile(this.fileName);

		for (int i = 0; i < this.files.size(); i++)
		{
			String line;
			line = writer.createElement(
					"petrinet",
					writer.createElement("netId", String.format("%d", this.files.elementAt(i).getId())) +
					writer.createElement("netName", this.files.elementAt(i).getName()) +
					writer.createElement("netDescription", this.files.elementAt(i).getDescription()) +
					writer.createElement("netAuthor", this.files.elementAt(i).getAuthor()) +
					writer.createElement("netVersion", String.format("%d", this.files.elementAt(i).getVersion())) +
					writer.createElement("netDate", String.format("%d", this.files.elementAt(i).getDate())),
					true,
					false
			);
				
			writer.appendOutput(line);
		}
		
		writer.echoOutput("root");

	}

	/**
	 * Loads files from XML file.
	 */
	private void loadFiles()
	{
		this.files.clear();
		
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

				if(qName.equalsIgnoreCase("netName"))
				{
					netName = xmlActual;
				}

				if(qName.equalsIgnoreCase("netDescription"))
				{
					netDescription = xmlActual;
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
				if(qName.equals("petrinet"))
				{
					PetriXMLReader objects = new PetriXMLReader(serverFolderName + "/petrinet" + Integer.parseInt(netId) + "-" + Integer.parseInt(netVersion) + ".xml");
					PetrinetMain petriNet = new PetrinetMain(objects.parse());
					petriNet.setInfo(Integer.parseInt(netId), netName, netDescription, netAuthor, Integer.parseInt(netVersion), Integer.parseInt(netDate));
					files.addElement(petriNet);
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

	/**
	 * Gets Petri's net by id and version.
	 * @param id
	 * @param version
	 * @return reference to net object
	 */
	public PetrinetMain getPetrinet(int id, int version)
	{
		loadFiles();
		for(int i = 0; i < this.files.size(); i++)
		{
			if(this.files.elementAt(i).getId() == id && this.files.elementAt(i).getVersion() == version)
			{
				return this.files.elementAt(i);
			}
		}

		return null;
	}
}
