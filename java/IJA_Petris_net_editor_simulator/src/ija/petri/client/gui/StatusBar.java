package ija.petri.client.gui;

import java.awt.Dimension;
import javax.swing.JLabel;

/**
 * Status bar for editor.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class StatusBar extends JLabel
{
	Editor mainClass;
	
	public StatusBar(Editor mainClass)
	{
		super();
        super.setPreferredSize(new Dimension(100, 16));
		this.mainClass = mainClass;
        setMessage("Ready");
	}
	
	public void setMessage(String message)
	{
		String netInfo = "";
		if(this.mainClass.petrinetMain.getId() > 0)
		{
			netInfo = new String("#" + this.mainClass.petrinetMain.getId() + " " + this.mainClass.petrinetMain.getName() + " version " + this.mainClass.petrinetMain.getVersion() + " > ");
		}
		setText(netInfo + " " + message);        
	}
}
