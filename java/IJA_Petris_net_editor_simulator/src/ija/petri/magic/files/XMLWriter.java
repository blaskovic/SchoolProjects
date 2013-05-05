package ija.petri.magic.files;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;

/**
 * Main class for writing XML files.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class XMLWriter
{
	private String output;
	private String fileName;
	
	public XMLWriter()
	{
		this.output = "";
		this.fileName = "";
	}

	/**
	 * Creates element by provided parameters.
	 * @param elemName
	 * @param text
	 * @return String of element
	 */
	public String createElement(String elemName, String text)
	{
		return createElement(elemName, text, false, true);
	}

	/**
	 * Creates element with defined line ending and cdata escape sequence.
	 * @param elemName
	 * @param text
	 * @param lineEnding
	 * @param cdata
	 * @return 
	 */
	public String createElement(String elemName, String text, boolean lineEnding, boolean cdata)
	{
		String out = "";
		out += "<" + elemName + ">";
		if(lineEnding) out += "\n";
		if(cdata && !text.equals("")) text = "<![CDATA[" + text + "]]>";
		out += text;
		if(lineEnding) out += "\n";
		out += "</" + elemName + ">\n";

		return out;
	}

	/**
	 * Appends output with string.
	 * @param text 
	 */
	public void appendOutput(String text)
	{
		this.output += text;
	}

	/**
	 * Sets filename for output.
	 * @param fileName 
	 */
	public void setOutputFile(String fileName)
	{
		this.fileName = fileName;
	}

	/**
	 * Prints output to file or standard output.
	 * @param rootElem 
	 */
	public void echoOutput(String rootElem)
	{
		if(!"".equals(rootElem))
		{
			this.output = "<" + rootElem + ">\n" + this.output + "\n" + "</" + rootElem + ">\n";
		}

		if("".equals(this.fileName))
		{
			System.out.println(this.output);
		}
		else
		{
			try {
				BufferedWriter out = new BufferedWriter(new FileWriter(this.fileName));
				out.write(this.output);
				out.close();
			}
			catch (IOException e)
			{
				System.out.println("File problem");
			}
		}
	}
}
