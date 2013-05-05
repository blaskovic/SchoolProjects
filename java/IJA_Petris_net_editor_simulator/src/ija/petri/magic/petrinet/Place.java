package ija.petri.magic.petrinet;

import java.util.Vector;

/**
 * Place class
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class Place extends PetriObject
{
	public String numbers = this.DEFAULT_VALUE;
	private String[] arrayNum;
	public boolean removed = false;

	public Place()
	{
		
	}

	/**
	 * Sets numbers for place.
	 * @param nums 
	 */
	public void setNumbers(String nums)
	{
		nums = nums.replaceAll("[^0-9A-Za-z,]+", "");
		this.numbers = nums;
	}

	/**
	 * 
	 * @return numbers as String
	 */
	@Override
	public String getNumbers()
	{
		return this.numbers;
	}

	/**
	 * 
	 * @return true if place has default name
	 */
	public boolean isDefaultName()
	{
		if(this.numbers.equals(this.DEFAULT_VALUE)) return true;
		else return false;
	}
	
	/**
	 * If is element in place, set flag removed to true.
	 * @param el element which we look for
	 */
	public void isElement(String el)
	{
		Vector<String> VectorNumbers = new Vector<String>();
		arrayNum = this.numbers.split(",");
		for(int i = 0; i < arrayNum.length; i++)
		{
			VectorNumbers.addElement(arrayNum[i].trim());
		}
		if(VectorNumbers.contains(el)) removed = true;
	}

	/**
	 * Removed element from place.
	 * @param el element to remove
	 */
	public void removeElement(String el)
	{
		Vector<String> VectorNumbers = new Vector<String>();
		arrayNum = this.numbers.split(",");
		for(int i = 0; i < arrayNum.length; i++)
		{
			VectorNumbers.addElement(arrayNum[i].trim());
		}
		if(!(VectorNumbers.isEmpty()))
		{
			VectorNumbers.remove(el);
			this.numbers = "";
			if(!(VectorNumbers.isEmpty()))
			{
				this.numbers = VectorNumbers.firstElement();
				for(int j = 1; j < VectorNumbers.size(); j++)
				{
					this.numbers += ",";
					this.numbers += VectorNumbers.elementAt(j);
				}
			}
		}
	}

	/**
	 * Removes first number
	 * @return removed number
	 */
	public String removeFirstNumber()
	{
		String num = null;
		Vector<String> VectorNumbers = new Vector<String>();
		arrayNum = this.numbers.split(",");
		for(int i = 0; i < arrayNum.length; i++)
		{
			VectorNumbers.addElement(arrayNum[i].trim());
		}
		if(!(VectorNumbers.isEmpty()))
		{
			num = VectorNumbers.elementAt(0);
			VectorNumbers.removeElementAt(0);
			this.numbers = "";
			if(!(VectorNumbers.isEmpty()))
			{
				this.numbers = VectorNumbers.firstElement();
				for(int j = 1; j < VectorNumbers.size(); j++)
				{
					this.numbers += ",";
					this.numbers += VectorNumbers.elementAt(j);
				}
			}
			removed = true;
		}
		return num;
	}
}
