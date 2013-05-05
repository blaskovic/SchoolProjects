package ija.petri.client.gui;

import ija.petri.magic.petrinet.Arcs;
import ija.petri.magic.petrinet.PetriObject;
import ija.petri.magic.petrinet.Place;
import ija.petri.magic.petrinet.Transition;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.*;
import javax.swing.border.EmptyBorder;

/**
 * Side bar in editor, which allow to change object's properties.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class Settings extends JPanel
{
	private JLabel coordinates;
	private JTextField input;
	private JTextField output;
	public JButton btnSave;

	private Transition actualTransition;
	private Place actualPlace;
	private Object actualObject;
	private Arcs actualArcs;
	private final JLabel label1;
	private final JLabel label2;

	/**
	 * Prepare sidebar for editing.
	 */
	public Settings()
	{
		setBorder(new EmptyBorder(15, 10, 10, 10));

		BoxLayout boxLay = new BoxLayout(this, BoxLayout.Y_AXIS);
		
		setLayout(boxLay);
		setPreferredSize(new Dimension(150, getPreferredSize().height));
		// Labels
		this.coordinates = new JLabel("Coords: ");
		this.label1 = new JLabel("");
		this.label2 = new JLabel("");
		
		this.input = new JTextField("NA", 15);
		this.input.setMaximumSize(new Dimension(Integer.MAX_VALUE, this.input.getPreferredSize().height) );
		this.output = new JTextField("NA", 15);
		this.output.setMaximumSize(new Dimension(Integer.MAX_VALUE, this.output.getPreferredSize().height) );
		this.btnSave = new JButton("Save");

		// Hide verything
		this.label1.setVisible(false);
		this.label2.setVisible(false);
		this.input.setVisible(false);
		this.output.setVisible(false);
		this.btnSave.setVisible(false);

		// Listeners
		ActionListener btnListeners = new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				if(e.getSource() == btnSave)
				{
					if(actualTransition != null)
					{
						actualTransition.setEquation(input.getText());
						actualTransition.setOutputEquation(output.getText());
					}
					if(actualPlace != null)
					{
						actualPlace.setNumbers(input.getText());
					}
					if(actualArcs != null)
					{
						actualArcs.setName(input.getText());
					}
				}
			}
		};
		this.btnSave.addActionListener(btnListeners);

		// Addings stuff
		add(new JLabel("Settings"));

		add(Box.createVerticalStrut(15));
		add(this.label1);
		add(this.input);

		add(Box.createVerticalStrut(15));
		add(this.label2);
		add(this.output);

		add(Box.createVerticalStrut(15));
		add(this.btnSave);
	}

	/**
	 * Update sidebar for relevant object.
	 */
	public void updateInfo()
	{
		if(this.actualObject instanceof Place)
		{
			Place pl = (Place)this.actualObject;
			this.label1.setText("Numbers:");
			this.input.setText(pl.getNumbers());
			
			this.label1.setVisible(true);
			this.label2.setVisible(false);
			this.input.setVisible(true);
			this.output.setVisible(false);
			this.btnSave.setVisible(true);
			
			this.coordinates.setText("Coords: " + pl.getPosX() + " x " + pl.getPosY());
		}
		else if(this.actualObject instanceof Transition)
		{
			Transition tr = (Transition)this.actualObject;
			this.label1.setText("Condition");
			this.label2.setText("Output");
			this.input.setText(tr.getEquation());
			this.output.setText(tr.getOutputEquation());

			this.label1.setVisible(true);
			this.label2.setVisible(true);
			this.input.setVisible(true);
			this.output.setVisible(true);
			this.btnSave.setVisible(true);
			
			this.coordinates.setText("Coords: " + tr.getPosX() + " x " + tr.getPosY());
		}
		else if(this.actualObject instanceof Arcs)
		{
			Arcs ar = (Arcs)this.actualObject;
			this.label1.setText("Name");
			this.input.setText(ar.getName());
			this.coordinates.setText("Coords: N/A");

		
			this.label1.setVisible(true);
			this.label2.setVisible(false);
			this.input.setVisible(true);
			this.btnSave.setVisible(true);
			this.output.setVisible(false);
		}
	}

	/**
	 * Returns true if object is actual.
	 * @param object
	 * @return true, if object is actual
	 */
	public boolean isActualObject(PetriObject object)
	{
		if(this.actualObject == null || object == null) return false;

		if(this.actualObject.equals(object))
			return true;
		else
			return false;
	}

	/**
	 * Sets actual object to object from param.
	 * @param object 
	 */
	public void setActualObject(PetriObject object)
	{
		this.actualObject = object;
		if(this.actualObject instanceof Place)
		{
			this.actualPlace = (Place)this.actualObject;
			this.actualTransition = null;
			this.actualArcs = null;
		}
		else if(this.actualObject instanceof Transition)
		{
			this.actualTransition = (Transition)this.actualObject;
			this.actualPlace = null;
			this.actualArcs = null;
		}
		else if(this.actualObject instanceof Arcs)
		{
			this.actualArcs = (Arcs)this.actualObject;
			this.actualPlace = null;
			this.actualTransition = null;
		}
		updateInfo();
	}

}
