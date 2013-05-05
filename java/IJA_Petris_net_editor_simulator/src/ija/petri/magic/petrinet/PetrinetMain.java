package ija.petri.magic.petrinet;

import java.io.Serializable;
import java.util.HashSet;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Vector;

/**
 * Main class of logic Petri nets
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class PetrinetMain implements Serializable, Cloneable
{
	protected Vector<PetriObject> objects;
	protected String newEquationStr;
	private String[] nums_array;
	private String[] transArray;
	private String[] transArraySpace;
	private String placeNum;
	private String placeNum2;
	private String newPlaceNumber;
	private Vector<Arcs> allInArcs;
	private Vector<Arcs> allOutArcs;
	private Vector<Vector<String>> vectorTrans;
	private Vector<String> vectorOutTrans;

	private boolean endSimulation = false;

	protected int netId = 0;
	protected String netName = "";
	protected String netDescription = "";
	protected String netAuthor = "";
	protected int netVersion = 0;
	protected int netDate = 0;

	@Override
	public Object clone()
	{
		try
		{
			return super.clone();
		} catch (CloneNotSupportedException e)
		{
			return null;
		}
	}

	public PetrinetMain(Vector<PetriObject> objects)
	{
		this.objects = objects;
	}

	public PetrinetMain()
	{
		this.objects = new Vector<PetriObject>();
	}

	public void setInfo(int id, String name, String description, String author, int version, int date)
	{
		this.netId = id;
		this.netName = name;
		this.netDescription = description;
		this.netAuthor = author;
		this.netVersion = version;
		this.netDate = date;
	}

	public void setName(String name)
	{
		this.netName = name;
	}

	public void setDescription(String desc)
	{
		this.netDescription = desc;
	}

	public void setAuthor(String userName)
	{
		this.netAuthor = userName;
	}

	public int getId()
	{
		return this.netId;
	}

	public String getName()
	{
		return this.netName;
	}

	public String getDescription()
	{
		return this.netDescription;
	}

	public String getAuthor()
	{
		return this.netAuthor;
	}

	public int getVersion()
	{
		return this.netVersion;
	}

	public int getDate()
	{
		return this.netDate;
	}

	public String getDateFormat()
	{
		Date date = new Date((long)this.netDate*1000);
		SimpleDateFormat sdf = new SimpleDateFormat("d.MM.yyyy H:mm");
		String formattedDate = sdf.format(date);
		return formattedDate;
	}

	public void setObjects(Vector<PetriObject> objects)
	{
		this.objects = objects;
	}
	
	public Vector<PetriObject> getObjects()
	{
		return this.objects;
	}

	/**
	 * Calculate intersection of two HashSets
	 * @param x 1. HashSet
	 * @param y 2. HashSet
	 * @return intersection in HashSet
	 */
	public static HashSet intersection(HashSet x, HashSet y)
	{
		HashSet t = new HashSet(x);
		t.retainAll(y);
		return t;
	}

	/**
	 * Run simulation
	 */
	public void simulate()
	{
		int count = 0;
		endSimulation = false;

		while(endSimulation == false && count < 100)
		{
			stepForward();
			count++;
		}
	}

	/**
	 * One step in simulation
	 */
	public void stepForward()
	{
		if(allInArcs != null) allInArcs.clear();
		if(allOutArcs != null) allOutArcs.clear();
		if(vectorTrans != null) vectorTrans.clear();
		if(vectorOutTrans != null) vectorOutTrans.clear();

		boolean transEmpty = false;

		for(int i = 0; i < this.objects.size(); i++)
		{
			if(this.objects.elementAt(i) instanceof Transition)
			{
				Transition trans = (Transition)objects.elementAt(i);
				
				allInArcs = getInArcs(trans);
				vectorTrans = getVectorTrans(trans.getEquation());

				allOutArcs = getOutArcs(trans);
				vectorOutTrans = getVectorOutTrans(trans.getOutputEquation());

				for(int t = 0; t < allOutArcs.size(); t++)
				{
					if(vectorOutTrans.indexOf(allOutArcs.elementAt(t).getName()) == -1 &&
					(!(allOutArcs.elementAt(t).getName().matches("[0-9]+"))))
					{
						endSimulation = true;
						return;
					}
				}

				// no condition
				if(trans.getEquation() == null || !trans.getEquation().matches(".*[a-zA-Z0-9].*"))
				{
					transEmpty = true;
					for(int a = 0; a < allInArcs.size(); a++)
					{
						Place tmpPlace = (Place)allInArcs.elementAt(a).startObject;
						tmpPlace.removeElement(allInArcs.elementAt(a).getName());
						if(tmpPlace.removed == false)
						{
							// all numbers in places are correct, choose first
							allInArcs.elementAt(a).selection = tmpPlace.removeFirstNumber();
							if(tmpPlace.removed == false)
							{
								endSimulation = true;
								return;
							}
						}
						tmpPlace.removed = false;
					}
				}
				
				if(transEmpty == false)
				{
					// constant on arc
					for(int o = 0; o < allInArcs.size(); o++)
					{
						if(allInArcs.elementAt(o).getName().matches("[0-9]+"))
						{
							Place tmpPlace = (Place)allInArcs.elementAt(o).startObject;
							tmpPlace.isElement(allInArcs.elementAt(o).getName());
							if(tmpPlace.removed == false)
							{
								endSimulation = true;
								return;
							}
							tmpPlace.removed = false;
						}
					}

					// create HashSet of numbers, which are correct
					for(int k = 0; k < vectorTrans.size(); k++)
					{
						setPlaceNumber(allInArcs, vectorTrans.elementAt(k));
					}

					// make intersection of HashSets and choose one correct number
					for(int l = 0; l < allInArcs.size(); l++)
					{
						if(!(allInArcs.elementAt(l).arcsVector.isEmpty()))
						{
							if(allInArcs.elementAt(l).arcsVector.size() > 1)
							{
								for(int m = 0; m < allInArcs.elementAt(l).arcsVector.size()-1; m++)
								{
									if(allInArcs.elementAt(l).arcsVector.elementAt(m).isEmpty()) continue;
									if(!(allInArcs.elementAt(l).arcsVector.elementAt(m+1).isEmpty()))
									{
										HashSet intersect = intersection(allInArcs.elementAt(l).arcsVector.elementAt(m), allInArcs.elementAt(l).arcsVector.elementAt(m+1));
										if(intersect.isEmpty()) continue;
										allInArcs.elementAt(l).selection = intersect.iterator().next().toString();
									}
								}
							}
							if(allInArcs.elementAt(l).arcsVector.size() == 1)
							{
								if(!(allInArcs.elementAt(l).arcsVector.firstElement().isEmpty()))
								{
									allInArcs.elementAt(l).selection = allInArcs.elementAt(l).arcsVector.firstElement().iterator().next().toString();
								}
							}
						}
					}

					// every place has to have number, which is correct
					for(int n = 0; n < allInArcs.size(); n++)
					{
						if(allInArcs.elementAt(n).selection != null)
						{
							if(allInArcs.elementAt(n).selection.isEmpty())
							{
								endSimulation = true;
								return;
							}
							Place tmpPlace = (Place)allInArcs.elementAt(n).startObject;
							tmpPlace.isElement(allInArcs.elementAt(n).selection);
							if(tmpPlace.removed == false)
							{
								endSimulation = true;
								return;
							}
							tmpPlace.removed = false;
						}
						else 
						{
							endSimulation = true;
							return;
						}
					}
				}

				// remove used numbers from places
				for(int s = 0; s < allInArcs.size(); s++)
				{
					if(allInArcs.elementAt(s).selection != null)
					{
						if(allInArcs.elementAt(s).selection.isEmpty()) 
						{
							endSimulation = true;
							return;
						}
						Place tmpPlace = (Place)allInArcs.elementAt(s).startObject;
						tmpPlace.removeElement(allInArcs.elementAt(s).selection);
					}
				}

				// constant on arc
				for(int o = 0; o < allInArcs.size(); o++)
				{
					if(allInArcs.elementAt(o).getName().matches("[0-9]+"))
					{
						Place tmpPlace = (Place)allInArcs.elementAt(o).startObject;
						tmpPlace.removeElement(allInArcs.elementAt(o).getName());
						if(tmpPlace.removed == false) 
						{
							endSimulation = true;
							return;
						}
						tmpPlace.removed = false;
					}
				}
				
				// equation
				calculate(vectorOutTrans, allInArcs, allOutArcs);

				// insert numbers to output places
				for(int v = 0; v < allOutArcs.size(); v++)
				{
					Place tmpPlace = (Place)allOutArcs.elementAt(v).endObject;
					if(allOutArcs.elementAt(v).getName().matches("[0-9]+"))
					{
						allOutArcs.elementAt(v).selection = allOutArcs.elementAt(v).getName();
					}
					if(allOutArcs.elementAt(v).selection == null)
					{
						endSimulation = true;
						return;
					}
					if(allOutArcs.elementAt(v).endObject.getNumbers().isEmpty())
					{
						tmpPlace.numbers = allOutArcs.elementAt(v).selection;
					}
					else
					{
						if(allOutArcs.elementAt(v).endObject.getNumbers().equals("??"))
						{
							tmpPlace.numbers = allOutArcs.elementAt(v).selection;
							continue;
						}
						tmpPlace.numbers += ",";
						tmpPlace.numbers += allOutArcs.elementAt(v).selection;
					}
				}
			}
		}
	}

	/**
	 * Set output arcs
	 * @param trans equation
	 * @param inArcs input arcs
	 * @param outArcs output arcs
	 */
	public void calculate(Vector<String> trans, Vector<Arcs> inArcs, Vector<Arcs> outArcs)
	{
		for(int c = 0; c < trans.size(); c++)
		{
			if(trans.elementAt(c).matches("[a-zA-Z_]+"))
			{
				for(int d = 0; d < inArcs.size(); d++)
				{
					if(inArcs.elementAt(d).getName().equals(trans.elementAt(c)))
					{
						trans.set(c, inArcs.elementAt(d).selection);
					}
				}
			}
		}
		for(int e = 0; e < outArcs.size(); e++)
		{
			if(!(trans.isEmpty()))
			{
				if(trans.firstElement().equals(outArcs.elementAt(e).getName()))
				{
					outArcs.elementAt(e).selection = getResult(trans);
				}
			}
		}
	}

	/**
	 * Calculate equation
	 * @param trans equation
	 * @return result of calculation
	 */
	public String getResult(Vector<String> trans)
	{
		int result = 0;

		if(trans.size() < 3)
		{
			return null;
		}

		// only assign
		result = Integer.parseInt(trans.elementAt(2));

		int tmp;
		boolean wasPlus = false;
		boolean wasMinus = false;
		if(trans.size() > 3)
		{
			for(int i = 3; i < trans.size(); i++)
			{
				if("+".equals(trans.elementAt(i)))
				{
					wasPlus = true;
					wasMinus = false;
				}
				else if("-".equals(trans.elementAt(i)))
				{
					wasPlus = false;
					wasMinus = true;
				}
				else
				{
					try
					{
						tmp = Integer.parseInt(trans.elementAt(i));
						if(wasMinus) result -= tmp;
						else if(wasPlus) result += tmp;
						else return null;
					}
					catch (Exception e)
					{
						return null;
					}
				}
			}
		}
		return Integer.toString(result);
	}

	/**
	 * Analyze condition
	 * @param exprVector condition
	 * @return true or false
	 */
	public boolean parseExpression(Vector<String> exprVector)
	{
		boolean result = false;;
		for(int i = 0; i < exprVector.size(); i++)
		{
			try
			{
				if(exprVector.elementAt(i).equals("<="))
				{
					if(Integer.parseInt(exprVector.elementAt(i-1)) <= Integer.parseInt(exprVector.elementAt(i+1)))
						result = true;
					else result = false;
				}

				if(exprVector.elementAt(i).equals(">="))
				{
					if(Integer.parseInt(exprVector.elementAt(i-1)) >= Integer.parseInt(exprVector.elementAt(i+1)))
						result = true;
					else result = false;
				}

				if(exprVector.elementAt(i).equals("!="))
				{
					if(Integer.parseInt(exprVector.elementAt(i-1)) != Integer.parseInt(exprVector.elementAt(i+1)))
						result = true;
					else result = false;
				}

				if(exprVector.elementAt(i).equals("=="))
				{
					if(Integer.parseInt(exprVector.elementAt(i-1)) == Integer.parseInt(exprVector.elementAt(i+1)))
						result = true;
					else result = false;
				}

				if(exprVector.elementAt(i).equals("<"))
				{
					if(Integer.parseInt(exprVector.elementAt(i-1)) < Integer.parseInt(exprVector.elementAt(i+1)))
						result = true;
					else result = false;
				}

				if(exprVector.elementAt(i).equals(">"))
				{
					if(Integer.parseInt(exprVector.elementAt(i-1)) > Integer.parseInt(exprVector.elementAt(i+1)))
						result = true;
					else result = false;
				}
			}
			catch(Exception e)
			{
				result = false;
			}
		}
		if(result == true) return true;
		else return false;
	}

	/**
	 * Parse conditions and save it to the vector
	 * @param trans condition
	 * @return final vector of vectors of strings
	 */
	public Vector<Vector<String>> getVectorTrans(String trans)
	{
		Vector<Vector<String>> transVectorVector = new Vector<Vector<String>>();
		trans = trans.replace("&&", " && ");
		trans = trans.replace("!=", " != ");
		trans = trans.replace("==", " == ");
		trans = trans.replace("<=", " <= ");
		trans = trans.replace(">=", " >= ");
		trans = trans.replace("<[^=]", " < ");
		trans = trans.replace(">[^=]", " > ");
		transArray = trans.trim().split("&&");
		for(int i = 0; i < transArray.length; i++)
		{
			Vector<String> transVector = new Vector<String>();
			transVectorVector.addElement(transVector);
			transArraySpace = transArray[i].trim().split(" ");
			for(int j = 0; j < transArraySpace.length; j++)
			{
				if(transArraySpace[j].matches("[\000-\040]+") || transArraySpace[j].equals("")) continue;
				transVectorVector.elementAt(i).addElement(transArraySpace[j].trim());
			}
		}
		return transVectorVector;
	}

	/**
	 * Parse condition to vector of strings
	 * @param trans condition
	 * @return final vector of strings
	 */
	public Vector<String> getVectorOutTrans(String trans)
	{
		Vector<String> transOutVector = new Vector<String>();
		trans = trans.replace("=", " = ");
		trans = trans.replace("+", " + ");
		trans = trans.replace("-", " - ");
		transArray = trans.trim().split(" ");
		for(int i = 0; i < transArray.length; i++)
		{
			transArray[i] = transArray[i].trim();
			if(transArray[i].equals(" ") || transArray[i].equals("")) continue;
			transOutVector.addElement(transArray[i]);
		}
		return transOutVector;
	}

	/**
	 * @param trans transition
	 * @return vector of input arcs
	 */
	public Vector<Arcs> getInArcs(Transition trans)
	{
		Vector<Arcs> newArcs = new Vector<Arcs>();
		for(int i = 0; i < this.objects.size(); i++)
		{
			if(this.objects.elementAt(i) instanceof Arcs)
			{
				Arcs arc = (Arcs)this.objects.elementAt(i);
				if(arc.getEndObject().equals(trans))
				{
					newArcs.addElement(arc);
					arc.arcsVector = new Vector<HashSet>();
				}
			}
		}
		return newArcs;
	}

	/**
	 * @param trans transition
	 * @return vector of output arcs
	 */
	public Vector<Arcs> getOutArcs(Transition trans)
	{
		Vector<Arcs> newOutArcs = new Vector<Arcs>();
		for(int i = 0; i < this.objects.size(); i++)
		{
			if(this.objects.elementAt(i) instanceof Arcs)
			{
				Arcs arc = (Arcs)this.objects.elementAt(i);
				if(arc.getStartObject().equals(trans))
				{
					newOutArcs.addElement(arc);
				}
			}
		}
		return newOutArcs;
	}

	/**
	 * Create HashSets of possible numbers in places
	 * @param inArcs input arcs
	 * @param trans	condition
	 */
	public void setPlaceNumber(Vector<Arcs> inArcs, Vector<String> trans)
	{
		for(int i = 0; i < inArcs.size(); i++)
		{
			Arcs arc = (Arcs)inArcs.elementAt(i);
			HashSet<Object> setNum = new HashSet<Object>();
			if(trans.indexOf(arc.getName()) != -1)
			{
				for(int j = 0; j < arc.getStartObject().getNumbers().split(",").length; j++)
				{
					placeNum = arc.getStartObject().getNumbers().split(",")[j];
					if(!(placeNum.equals("")))
					{
						trans.setElementAt(placeNum, trans.indexOf(arc.getName()));

						///
						for(int h = 0; h < inArcs.size(); h++)
						{
							Arcs arc2 = (Arcs)inArcs.elementAt(h);
							HashSet<Object> setNum2 = new HashSet<Object>();
							if(trans.indexOf(arc2.getName()) != -1)
							{
								for(int g = 0; g < arc2.getStartObject().getNumbers().split(",").length; g++)
								{
									placeNum2 = arc2.getStartObject().getNumbers().split(",")[g];
									if(!(placeNum2.equals("")))
									{
										trans.setElementAt(placeNum2, trans.indexOf(arc2.getName()));

										if(parseExpression(trans) == true)
										{
											setNum2.add(placeNum2);
										}
										trans.setElementAt(arc2.getName(), trans.indexOf(placeNum2));
									}
								}
								if(!(setNum2.isEmpty())) arc2.arcsVector.addElement(setNum2);
							}
							else
							{
								boolean tmp2 = true;
								for(int u = 0; u < trans.size(); u++)
								{
									if(trans.elementAt(u).matches("[a-zA-Z_]+")) tmp2 = false;
								}
								if(tmp2 == true)
								{
									for(int e = 0; e < arc2.getStartObject().getNumbers().split(",").length; e++)
									{
										placeNum2 = arc2.getStartObject().getNumbers().split(",")[e];
										if(!(placeNum2.equals("")))
										{

											if(parseExpression(trans) == true)
											{
												setNum2.add(placeNum2);
											}
										}
									}
									if(!(setNum2.isEmpty())) arc2.arcsVector.addElement(setNum2);
								}
							}
						}
						///
;
						if(parseExpression(trans) == true)
						{
							if(!(placeNum.isEmpty())) setNum.add(placeNum);
						}
						trans.setElementAt(arc.getName(), trans.indexOf(placeNum));
					}
				}
				if(!(setNum.isEmpty())) arc.arcsVector.addElement(setNum);
			}
			else
			{
				boolean tmp = true;
				for(int t = 0; t < trans.size(); t++)
				{
					if(trans.elementAt(t).matches("[a-zA-Z_]+")) tmp = false;
				}
				if(tmp == true)
				{
					for(int k = 0; k < arc.getStartObject().getNumbers().split(",").length; k++)
					{
						placeNum = arc.getStartObject().getNumbers().split(",")[k];
						if(!(placeNum.equals("")))
						{

							if(parseExpression(trans) == true)
							{
								setNum.add(placeNum);
							}
						}
					}
					if(!(setNum.isEmpty())) arc.arcsVector.addElement(setNum);
				}
			}
		}
	}
}
