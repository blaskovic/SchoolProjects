package ija.petri.magic.petrinet;

import ija.petri.magic.petrinet.PetriXMLGenerator;
import java.util.HashSet;
import java.util.Vector;

//import com.thoughtworks.xstream.*;
//import com.thoughtworks.xstream.io.xml.DomDriver;

/**
 * Arcs class
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class Arcs extends PetriObject
{
	public PetriObject startObject;
	public PetriObject endObject;
	private String name;
	public Vector<HashSet> arcsVector;
	public String selection;

	public Arcs()
	{
		//System.out.println("Arcs");
		setName(this.DEFAULT_VALUE);
		Vector<HashSet> arcsVector = new Vector<HashSet>();
	}

	/**
	 * Sets input and output object of arc.
	 * @param start
	 * @param end 
	 */
	public void setObjects(PetriObject start, PetriObject end)
	{
		this.startObject = start;
		this.endObject = end;
	}

	/**
	 * Sets name of arc.
	 * @param txt Name of arc
	 */
	public void setName(String txt)
	{
		this.name = txt;
	}

	/**
	 * 
	 * @return name of arc
	 */
	@Override
	public String getName()
	{
		return this.name;
	}
	
	/**
	 * 
	 * @return true if name of arc is default
	 */
	public boolean isDefaultName()
	{
		if(this.name.equals(this.DEFAULT_VALUE)) return true;
		else return false;
	}

	/**
	 * 
	 * @return reference to end object of arc
	 */
	PetriObject getEndObject()
	{
		return this.endObject;
	}

	/**
	 * 
	 * @return reference to start object of arc
	 */
	PetriObject getStartObject()
	{
		return this.startObject;
	}
}
