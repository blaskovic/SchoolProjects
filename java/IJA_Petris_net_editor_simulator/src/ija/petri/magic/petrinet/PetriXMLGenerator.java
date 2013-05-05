package ija.petri.magic.petrinet;

import ija.petri.magic.files.XMLWriter;
import java.util.Vector;

/**
 * Generate XML code
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class PetriXMLGenerator
{
	private Vector<PetriObject> objects;

	/**
	 * Create class with objects.
	 * @param objects which objects we want to save into XML
	 */
	public PetriXMLGenerator(Vector<PetriObject> objects)
	{
		this.objects = objects;
	}
	
	/**
	 * Generated XML of Petri's net.
	 * @param fileName output file
	 */
	public void generateXML(String fileName)
	{
		XMLWriter writer = new XMLWriter();
		writer.setOutputFile(fileName);
		
		for (int i = 0; i < this.objects.size(); i++)
		{
			PetriObject obj = this.objects.elementAt(i);

			String line = "";

			if(obj instanceof Place)
			{
				line = writer.createElement(
						"place",
						writer.createElement("id", String.format("%d", i)) +
						writer.createElement("x", String.format("%d", obj.getPosX())) +
						writer.createElement("y", String.format("%d", obj.getPosY())) +
						writer.createElement("numbers", obj.getNumbers()),
						true,
						false
				);
			}
			else if(obj instanceof Transition)
			{
				line = writer.createElement(
						"transition",
						writer.createElement("id", String.format("%d", i)) +
						writer.createElement("x", String.format("%d", obj.getPosX())) +
						writer.createElement("y", String.format("%d", obj.getPosY())) +
						writer.createElement("equation", obj.getEquation()) +
						writer.createElement("outputEquation", obj.getOutputEquation()),
						true,
						false
				);
			}
			else if(obj instanceof Arcs)
			{
				Arcs ar = (Arcs)obj;
				int id1 = this.objects.indexOf(ar.startObject);
				int id2 = this.objects.indexOf(ar.endObject);
				line = writer.createElement(
						"arcs",
						writer.createElement("id1", String.format("%d", id1)) +
						writer.createElement("id2", String.format("%d", id2)) +
						writer.createElement("name", obj.getName()),
						true,
						false
				);
			}
			
			writer.appendOutput(line);
		}

		writer.echoOutput("root");
	}
}
