package ija.petri.client.gui;

import ija.petri.magic.connection.Communication;
import ija.petri.magic.petrinet.PetrinetMain;
import ija.petri.server.simulation.Simulation;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.Vector;
import javax.swing.*;

/**
 * Client's window for browsing nets on remote server.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class ServerBrowser extends JFrame
{
	Editor mainClass;
	Vector<PetrinetMain> files;
	Vector<Simulation> simulations;
	JTextField searchText;

	public ServerBrowser(Editor mainClass, Vector<PetrinetMain> files, Vector<Simulation> simulations)
	{
		this.mainClass = mainClass;
		this.files = files;
		this.simulations = simulations;
	}

	/**
	 * Fill panel with all found nets on server.
	 * @param panel 
	 */
	public void fillPanel(JPanel panel)
	{
		int row = 0;
		panel.removeAll();
		GridBagConstraints c = new GridBagConstraints();
		//System.out.println(this.files.size());
		for(int i = this.files.size()-1; i >= 0 ; i--)
		{
			// Only first
			if(this.files.elementAt(i).getVersion() != 1)
				continue;

			// Searching!
			if(!this.searchText.getText().equals(""))
			{
				String search = this.searchText.getText();
				if(
					this.files.elementAt(i).getName().toLowerCase().indexOf(search.toLowerCase()) == -1 &&
					this.files.elementAt(i).getDescription().toLowerCase().indexOf(search.toLowerCase()) == -1 &&
					this.files.elementAt(i).getAuthor().toLowerCase().indexOf(search.toLowerCase()) == -1
				)
					continue;
			}
			
			final int netId = this.files.elementAt(i).getId();
			final int lastVersion = getNumVersions(this.files.elementAt(i).getId());
			
			String name = this.files.elementAt(i).getId() + "# " + this.files.elementAt(i).getName();
			JLabel description = new JLabel(this.files.elementAt(i).getDescription());
			JLabel version = new JLabel("Versions: " +  getNumVersions(this.files.elementAt(i).getId()));
			JLabel author = new JLabel("Author: " + this.files.elementAt(i).getAuthor());
			JLabel date = new JLabel("Created: " + this.files.elementAt(i).getDateFormat());

			description.setForeground(Color.BLUE);
			
			// Panel for file
			GridBagLayout smallLayout = new GridBagLayout();
			JPanel smallPanel = new JPanel(smallLayout);
			smallPanel.setBorder(BorderFactory.createTitledBorder(name));
			GridBagConstraints sc = new GridBagConstraints();

			sc.insets = new Insets(5, 5, 5, 5);
			// Description
			sc.gridx = 0; sc.gridy = 0;
			sc.gridwidth = 3;
			sc.weightx = 0;
			smallPanel.add(description, sc);

			sc.gridwidth = 1;
			
			// Author
			sc.gridx = 0; sc.gridy = 1;
			sc.weightx = 1;
			smallPanel.add(author, sc);
			// Date
			sc.gridx = 1; sc.gridy = 1;
			sc.weightx = 1;
			smallPanel.add(date, sc);
			// Version
			sc.gridx = 2; sc.gridy = 1;
			sc.weightx = 1;
			smallPanel.add(version, sc);

			// Buttons
			JButton btnVersions = new JButton("versions");
			JButton btnLoad = new JButton("load");
			JButton btnSimulations = new JButton("sim history");

			sc.gridx = 0; sc.gridy = 2;
			smallPanel.add(btnVersions, sc);
			sc.gridx = 1; sc.gridy = 2;
			smallPanel.add(btnSimulations, sc);
			sc.gridx = 2; sc.gridy = 2;
			smallPanel.add(btnLoad, sc);

			// Versions
			final JPanel versionsPanel = getVersions(this.files.elementAt(i).getId());
			sc.gridx = 0; sc.gridy = 3;
			sc.gridwidth = 3;
			smallPanel.add(versionsPanel, sc);

			// History simulations
			final JPanel historyPanel = getHistory(this.files.elementAt(i).getId());
			sc.gridx = 0; sc.gridy = 3;
			sc.gridwidth = 3;
			smallPanel.add(historyPanel, sc);
			
			// Whole panel
			c.gridx = 0; c.gridy = row;
			c.fill = GridBagConstraints.HORIZONTAL;
			c.anchor = GridBagConstraints.NORTH;
			c.weighty = 0.1;
			panel.add(smallPanel, c);

			// Action listeners
			// Get petrinet!
			btnLoad.addActionListener(new ActionListener() {

				@Override
				public void actionPerformed(ActionEvent ae)
				{
					callForNet(netId, lastVersion);
				}
			});
			// Show versions
			btnVersions.addActionListener(new ActionListener() {

				@Override
				public void actionPerformed(ActionEvent ae)
				{
					// Get index
					if(versionsPanel.isVisible())
					{
						versionsPanel.setVisible(false);
					}
					else
					{
						historyPanel.setVisible(false);
						versionsPanel.setVisible(true);
					}
				}
			});
			// Show history
			btnSimulations.addActionListener(new ActionListener() {

				@Override
				public void actionPerformed(ActionEvent ae)
				{
					// Get index
					if(historyPanel.isVisible())
					{
						historyPanel.setVisible(false);
					}
					else
					{
						versionsPanel.setVisible(false);
						historyPanel.setVisible(true);
					}
				}
			});
			
			row++;
		}	

		panel.updateUI();
	}

	/**
	 * Show server browser popup window.
	 */
	public void showServerBrowser()
	{
		if(!this.mainClass.connection.isConnected())
		{
			this.mainClass.statusBar.setMessage("Not connected to server");
			//return;
		}
		setTitle("Server browser");
		
		GridBagLayout layout = new GridBagLayout();
		final JPanel panel = new JPanel(layout);
		panel.setBorder(BorderFactory.createTitledBorder("Files on server"));

		this.searchText = new JTextField("", 20);

		// Scrolling
		JScrollPane scroll = new JScrollPane(panel);
		scroll.getVerticalScrollBar().setUnitIncrement(16);
		setPreferredSize(new Dimension(400, 600));
		GridBagConstraints c = new GridBagConstraints();

		c.insets = new Insets(5, 5, 5, 5);
		
		// Loop through files
		if(this.files != null && this.files.size() > 0)
		{
			fillPanel(panel);
		}

		// Search
		JPanel search = new JPanel();
		search.setBorder(BorderFactory.createTitledBorder("Search"));
		search.add(this.searchText, BorderLayout.CENTER);
		this.searchText.addKeyListener(new KeyListener() {

			@Override
			public void keyTyped(KeyEvent ke){}
			@Override
			public void keyPressed(KeyEvent ke){}
			@Override
			public void keyReleased(KeyEvent ke)
			{
				// Refresh panel
				if(files != null && files.size() > 0)
				{
					fillPanel(panel);
				}
			}
		});
		
		add(search, BorderLayout.NORTH);
		add(scroll, BorderLayout.CENTER);
		pack();
		setVisible(true);
		centerWindow();
	}

	/**
	 * Gets net from server by id and version.
	 * @param id
	 * @param version 
	 */
	public void callForNet(int id, int version)
	{
		mainClass.connection.sendObject(new Communication("getPetrinet"));
		mainClass.connection.sendObject(new Integer(id));
		mainClass.connection.sendObject(new Integer(version));
		// Received
		PetrinetMain received = (PetrinetMain)mainClass.connection.getObject();
		if(received != null)
		{
			if(mainClass.objects == null) System.err.println("Error: callForNet(): null");

			System.out.println(received.getName());

			mainClass.petrinetMainVect.addElement(received);
			mainClass.objectsVect.addElement(received.getObjects());

			mainClass.tabs.addTab(received.getName(), new JLabel("loading ..."));
			mainClass.tabs.setSelectedIndex(mainClass.tabs.getTabCount() - 1);
			
			mainClass.statusBar.setMessage("Petri's net loaded");
		}
		else
		{
			mainClass.statusBar.setMessage("Received null object");
		}
	}

	/**
	 * Returns number of versions for id of net.
	 * @param id
	 * @return amount of versions.
	 */
	private int getNumVersions(int id)
	{
		int count = 0;
		for(int i = 0; i < this.files.size(); i++)
		{
			if(this.files.elementAt(i).getId() != id)
				continue;

			count++;
		}
		return count;
	}

	/**
	 * Fill panel with history data for nets
	 * @param id
	 * @return JPanel reference for filled history panel.
	 */
	private JPanel getHistory(int id)
	{
		GridBagLayout layout = new GridBagLayout();
		JPanel panel = new JPanel(layout);
		GridBagConstraints c = new GridBagConstraints();
		
		panel.setBorder(BorderFactory.createTitledBorder("History"));
	
		c.insets = new Insets(5, 5, 5, 5);
		
		int row = 0;
		for(int i = 0; i < this.simulations.size(); i++)
		{
			if(this.simulations.elementAt(i).getId() != id)
				continue;

			final int netId = this.simulations.elementAt(i).getId();
			final int lastVersion = this.simulations.elementAt(i).getVersion();
			
			JLabel name = new JLabel("v" + this.simulations.elementAt(i).getVersion() + " - " + getNameByNet(this.simulations.elementAt(i).getId(), this.simulations.elementAt(i).getVersion()));
			JLabel author = new JLabel("Author: " + this.simulations.elementAt(i).getAuthor());
			
			JLabel date = new JLabel("Date: " + this.simulations.elementAt(i).getDateFormat());

			final JButton btnLoad = new JButton("load");

			
			name.setForeground(Color.BLUE);
			
			// Name
			c.gridx = 0; c.gridy = row;
			c.gridwidth = 3;
			c.weightx = 0;
			panel.add(name, c);

			c.gridwidth = 1;
			
			// Author
			c.gridx = 0; c.gridy = row + 1;
			c.weightx = 1;
			panel.add(author, c);
			// Date
			c.gridx = 1; c.gridy = row + 1;
			c.weightx = 1;
			panel.add(date, c);
			// Load
			c.gridx = 2; c.gridy = row + 1;
			c.weightx = 1;
			panel.add(btnLoad, c);

			// Action listeners
			btnLoad.addActionListener(new ActionListener() {

				@Override
				public void actionPerformed(ActionEvent ae)
				{
					callForNet(netId, lastVersion);
				}
			});
			
			row += 2;
		}

		panel.setVisible(false);
		
		return panel;
	}

	/**
	 * Get file versions panel
	 * @param id
	 * @return reference to JPanel versions panel.
	 */
	private JPanel getVersions(int id)
	{
		GridBagLayout layout = new GridBagLayout();
		JPanel panel = new JPanel(layout);
		GridBagConstraints c = new GridBagConstraints();
		
		panel.setBorder(BorderFactory.createTitledBorder("Versions"));
	
		c.insets = new Insets(5, 5, 5, 5);
		
		int row = 0;
		for(int i = 0; i < this.files.size(); i++)
		{
			if(this.files.elementAt(i).getId() != id)
				continue;

			final int netId = this.files.elementAt(i).getId();
			final int lastVersion = this.files.elementAt(i).getVersion();
			
			JLabel name = new JLabel(this.files.elementAt(i).getVersion() + "# " + this.files.elementAt(i).getName());
			JLabel version = new JLabel("Version: " + this.files.elementAt(i).getVersion());
			JLabel author = new JLabel("Author: " + this.files.elementAt(i).getAuthor());
			JLabel date = new JLabel("Created: " + this.files.elementAt(i).getDateFormat());

			final JButton btnLoad = new JButton("load");

			name.setForeground(Color.BLUE);

			// Name
			c.gridx = 0; c.gridy = row;
			c.gridwidth = 3;
			c.weightx = 0;
			panel.add(name, c);

			c.gridwidth = 1;
			
			// Author
			c.gridx = 0; c.gridy = row + 1;
			c.weightx = 1;
			panel.add(author, c);
			// Date
			c.gridx = 1; c.gridy = row + 1;
			c.weightx = 1;
			panel.add(date, c);
			// Load
			c.gridx = 2; c.gridy = row + 1;
			c.weightx = 1;
			panel.add(btnLoad, c);

			// Action listeners
			btnLoad.addActionListener(new ActionListener() {

				@Override
				public void actionPerformed(ActionEvent ae)
				{
					callForNet(netId, lastVersion);
				}
			});
			
			row += 2;
		}

		panel.setVisible(false);
		
		return panel;
	}

	private void centerWindow()
	{
		// Center window
		Dimension dim = Toolkit.getDefaultToolkit().getScreenSize();
		int w = getSize().width;
		int h = getSize().height;
		int x = (dim.width-w)/2;
		int y = (dim.height-h)/2;
		
		// Move the window
		setLocation(x, y);
	}

	/**
	 * Get name of net by ID and Version.
	 * @param id
	 * @param version
	 * @return name of net
	 */
	private String getNameByNet(int id, int version)
	{
		for(int i = 0; i < this.files.size(); i++)
		{
			if(this.files.elementAt(i).getId() == id && this.files.elementAt(i).getVersion() == version)			
			{
				return this.files.elementAt(i).getName();
			}
		}

		return "";
	}
}
