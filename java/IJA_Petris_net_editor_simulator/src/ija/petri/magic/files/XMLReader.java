package ija.petri.magic.files;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import org.xml.sax.helpers.DefaultHandler;

/**
 * Reading XML files - main class.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class XMLReader 
{
	private DefaultHandler handler;
	private final String parseFile;
	
	/**
	 * Set filename of file to parse.
	 * @param parseFile 
	 */
	public XMLReader(String parseFile)
	{
		this.parseFile = parseFile;
	}

	/**
	 * Start parse with SAX library.
	 */
	public void startParse()
	{
		try
		{
			SAXParserFactory factory = SAXParserFactory.newInstance();
			SAXParser saxParser = factory.newSAXParser();
			saxParser.parse(parseFile, this.handler);
		}
		catch (Exception e)
		{
			//System.err.println("Error: XMLReader::startParse(): problem :)");
		}

	}

	/**
	 * Sets handler for configuration handler.
	 * @param handler 
	 */
	public void setHandler(DefaultHandler handler)
	{
		this.handler = handler;
	}
}
