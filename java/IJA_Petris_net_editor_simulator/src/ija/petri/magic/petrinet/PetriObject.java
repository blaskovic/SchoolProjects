package ija.petri.magic.petrinet;

import java.io.Serializable;
import java.util.Vector;

/**
 * All objects in Petri nets
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class PetriObject implements Serializable
{
	private int posX;
	private int posY;
	
	protected static final String DEFAULT_VALUE = "??";

	public void setPosition(int x, int y)
	{
		this.posX = x;
		this.posY = y;
	}
	
	public int getPosX()
	{
		return this.posX;
	}

	public int getPosY()
	{
		return this.posY;
	}

	// Need to be overrided
	public String getNumbers()
	{
		return "";
	};

	public String getEquation()
	{
		return "";
	}

	public String getOutputEquation()
	{
		return "";
	}

	public String getName()
	{
		return "";
	}

	public void setEquation(String eq)
	{
	}
}
