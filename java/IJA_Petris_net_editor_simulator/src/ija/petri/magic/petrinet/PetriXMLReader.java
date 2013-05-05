package ija.petri.magic.petrinet;

import ija.petri.magic.files.XMLReader;
import java.util.Vector;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

/**
 * XML files reader
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class PetriXMLReader
{
	private String file;
	public String startEl = "";
	public String endEl = "";

	// Values
	public String aid = "";
	public String aid1 = "";
	public String aid2 = "";
	public String ax = "";
	public String ay = "";
	public String numbers = "";
	public String equation = "";
	public String outputEquation = "";
	public String name = "";

	public String actual = "";

	public Vector<PetriObject> objects;
	
	public PetriXMLReader(String file)
	{
		this.file = file;
		this.objects = new Vector<PetriObject>();
	}
	
	/**
	 * Parses XML of net
	 * @return vector of found Petri's objects
	 */
	public Vector<PetriObject> parse()
	{
		DefaultHandler handler = new DefaultHandler()
		{

			@Override
			public void endElement(String uri, String localName, String qName) throws SAXException
			{
				// Atributes
				if(qName.equalsIgnoreCase("ID"))
				{
					aid = actual;
				}

				if(qName.equalsIgnoreCase("X"))
				{
					ax = actual;
				}

				if(qName.equalsIgnoreCase("Y"))
				{
					ay = actual;
				}

				if(qName.equalsIgnoreCase("ID1"))
				{
					aid1 = actual;
				}

				if(qName.equalsIgnoreCase("ID2"))
				{
					aid2 = actual;
				}

				if(qName.equalsIgnoreCase("numbers"))
				{
					numbers = actual;
				}

				if(qName.equalsIgnoreCase("equation"))
				{
					equation = actual;
				}

				if(qName.equalsIgnoreCase("outputEquation"))
				{
					outputEquation = actual;
				}

				if(qName.equalsIgnoreCase("name"))
				{
					name = actual;
				}
				// Objects
				if(qName.equals("place"))
				{
					Place obj = new Place();
					obj.setPosition(Integer.parseInt(ax), Integer.parseInt(ay));
					obj.setNumbers(numbers);
					objects.addElement(obj);
				}

				if(qName.equals("transition"))
				{
					Transition obj = new Transition();
					obj.setPosition(Integer.parseInt(ax), Integer.parseInt(ay));
					obj.setEquation(equation);
					obj.setOutputEquation(outputEquation);
					objects.addElement(obj);
				}

				if(qName.equals("arcs"))
				{
					if((Integer.parseInt(aid1) >= 0 && Integer.parseInt(aid1) < objects.size()) &&
						(Integer.parseInt(aid2) >= 0 && Integer.parseInt(aid2) < objects.size()))
					{
						Arcs obj = new Arcs();
						obj.setObjects(objects.elementAt(Integer.parseInt(aid1)), objects.elementAt(Integer.parseInt(aid2)));
						obj.setName(name);
						objects.addElement(obj);
					}
				}
			}

			@Override
			public void characters(char ch[], int start, int length) throws SAXException
			{
				actual = new String(ch, start, length);
			}
		};

		XMLReader read = new XMLReader(this.file);
		read.setHandler(handler);
		read.startParse();

		return this.objects;
	}
	
}
