package ija.petri.magic.petrinet;

/**
 * Transition class
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class Transition extends PetriObject
{
	private String equation;
	private String outputEquation;

	public Transition()
	{
		this.setEquation(this.DEFAULT_VALUE);
		this.setOutputEquation(this.DEFAULT_VALUE);
	}

	/**
	 * Sets equation for transition.
	 * @param eq 
	 */
	@Override
	public void setEquation(String eq)
	{
		this.equation = eq;
	}

	/**
	 * 
	 * @return equation in String
	 */
	@Override
	public String getEquation()
	{
		return this.equation;
	}

	/**
	 * Sets output equation for transition.
	 * @param out 
	 */
	public void setOutputEquation(String out)
	{
		this.outputEquation = out;
	}

	/**
	 * 
	 * @return output equation in String
	 */
	@Override
	public String getOutputEquation()
	{
		return this.outputEquation;
	}

	/**
	 * 
	 * @return true if equation is default
	 */
	public boolean isDefaultName()
	{
		if(getEquation().equals(this.DEFAULT_VALUE)) return true;
		else return false;
	}
	
	/**
	 * 
	 * @return true if output equation is default
	 */
	public boolean isDefaultName2()
	{
		if(getOutputEquation().equals(this.DEFAULT_VALUE)) return true;
		else return false;
	}
}
