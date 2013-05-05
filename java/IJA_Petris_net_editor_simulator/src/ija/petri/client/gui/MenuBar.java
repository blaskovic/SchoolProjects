package ija.petri.client.gui;

import ija.petri.magic.connection.Communication;
import ija.petri.magic.petrinet.PetriObject;
import ija.petri.magic.petrinet.PetriXMLGenerator;
import ija.petri.magic.petrinet.PetriXMLReader;
import ija.petri.magic.petrinet.PetrinetMain;
import ija.petri.server.simulation.Simulation;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.io.File;
import java.util.Vector;
import javax.swing.*;

/**
 * Top menu bar of client's application.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class MenuBar extends JPanel {

	private Vector<PetriObject> objects;

	private Editor mainClass;
	private JMenuItem connectionRegister;
	private JMenuItem connectionLogin;
	private JMenuItem connectionSave;
	private JMenuItem connectionBrowser;
	private String helpText =
		"<html>MENU<br/>" +
		"&nbsp;* File<br/>" +
		"&nbsp;&nbsp;&nbsp;&nbsp;* Open XML<br/>" +
		"&nbsp;&nbsp;&nbsp;&nbsp;* Save XML<br/>" +
		"&nbsp;* Server<br/>" +
		"&nbsp;&nbsp;&nbsp;&nbsp;*&nbsp;Login - You must be logged in for simulation. Fill address of server, port and your login name and password. If you are not registered, please do it now.<br/>" +
		"&nbsp;&nbsp;&nbsp;&nbsp;*&nbsp;Register - Enter new login name and password. If you are already registered, please log in.<br/>" +
		"&nbsp;&nbsp;&nbsp;&nbsp;*&nbsp;Save on server - Fill name of net and description.<br/>" +
		"&nbsp;&nbsp;&nbsp;&nbsp;*&nbsp;Server browser - You can search and load versions or history of simulation.<br/>" +
		"&nbsp;* Settings<br/>" +
		"&nbsp;&nbsp;&nbsp;&nbsp;*&nbsp;Appearance - You can set the color theme. You can export existing settings or create the new one.<br/>" +
		"&nbsp;* Help<br/>" +
		"&nbsp;&nbsp;&nbsp;&nbsp;*&nbsp;Help<br/>" +
		"&nbsp;&nbsp;&nbsp;&nbsp;*&nbsp;About<br/>" +
		"BUTTONS<br/>" +
		"&nbsp;* Select - Click on object. You can modify it or move it.<br/>" +
		"&nbsp;* Remove<br/>" +
		"&nbsp;* Add place<br/>" +
		"&nbsp;* Add transition<br/>" +
		"&nbsp;* Add arcs - Click on input object and then on output object.<br/>" +
		"&nbsp;* Clear window<br/>" +
		"&nbsp;* Step - One step of simulation.<br/>" +
		"&nbsp;* Simulate - Run simulation.<br/>" +
		"&nbsp;* New tab - Create new tab.<br/>" +
		"&nbsp;* Close tab - Close active tab.<br/>" +
		"OBJECTS<br/>\n" +
		"&nbsp;* Place - You can set only numbers separate of comma. Confirm with Save button.<br/>" +
		"&nbsp;* Transition<br/>" +
		"&nbsp;&nbsp;&nbsp;&nbsp;*&nbsp;Condition - Operations: <, >, <=, >=, == and !=. You can use variables or numbers. Confirm with Save button.<br/>" +
		"&nbsp;&nbsp;&nbsp;&nbsp;*&nbsp;Output - Output equation. Only + and - operations are allowed. You can set one variable using assigning (f.e. a = b + c - d). Confirm with Save button.<br/>" +
		"&nbsp;* Arcs - You can set only variable or number as name of arc. Confirm with Save button.</html>";

	public MenuBar(Editor parent)
	{
		this.mainClass = parent;
	}

	/**
	 * Set reference to objects on drawing pane
	 * @param objects 
	 */
	public void setObjects(Vector<PetriObject> objects)
	{
		this.objects = objects;
	}

	/**
	 * Get reference for menu bar.
	 * @return reference for JMenuBar
	 */
	public JMenuBar getMenuBar() {
		JMenuBar menuBar = new JMenuBar();

		// File
		JMenu fileMenu = new JMenu("File");
		fileMenu.setMnemonic(KeyEvent.VK_F);
		menuBar.add(fileMenu);
		// File - Save
		JMenuItem fileSave = new JMenuItem("Save XML", KeyEvent.VK_S);
		fileSave.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e)
			{
				JFileChooser fc = new JFileChooser();
				try
				{
					File f = new File(new File("examples").getCanonicalPath());
					fc.setCurrentDirectory(f);	
				}
				catch (Exception ex)
				{
					try
					{
						File f = new File(new File(".").getCanonicalPath());
						fc.setCurrentDirectory(f);
					} catch(Exception ey) {}
				}

				int returnVal = fc.showSaveDialog(MenuBar.this);

				if (returnVal == JFileChooser.APPROVE_OPTION)
				{
					PetriXMLGenerator xmlPrinter = new PetriXMLGenerator(mainClass.objects);
					File file = fc.getSelectedFile();
					xmlPrinter.generateXML(file.getAbsolutePath());
				}
			}
		});
		fileMenu.add(fileSave);
		// File - Open XML
		JMenuItem fileOpen = new JMenuItem("Open XML", KeyEvent.VK_O);
		fileOpen.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e)
			{
				JFileChooser fc = new JFileChooser();
				try
				{
					File f = new File(new File("examples").getCanonicalPath());
					fc.setCurrentDirectory(f);	
				}
				catch (Exception ex)
				{
					try
					{
						File f = new File(new File(".").getCanonicalPath());
						fc.setCurrentDirectory(f);
					} catch(Exception ey) {}
				}
				
				int returnVal = fc.showOpenDialog(MenuBar.this);
				if (returnVal == JFileChooser.APPROVE_OPTION)
				{
					File file = fc.getSelectedFile();
					PetriXMLReader reader = new PetriXMLReader(file.getAbsolutePath());
					Vector<PetriObject> newObjects = reader.parse();

					PetrinetMain petriNet = new PetrinetMain();
					petriNet.setName("Local file");
					mainClass.petrinetMainVect.addElement(petriNet);
					mainClass.objectsVect.addElement(newObjects);

					mainClass.tabs.addTab(petriNet.getName(), new JLabel("loading ..."));
					mainClass.tabs.setSelectedIndex(mainClass.tabs.getTabCount() - 1);
				}
			}
		});

		fileMenu.add(fileOpen);
		fileMenu.add(fileSave);
		
		// Connection
		JMenu connectionMenu = new JMenu("Server");
		fileMenu.setMnemonic(KeyEvent.VK_C);
		menuBar.add(connectionMenu);
		
		// Connection - Login
		this.connectionLogin = new JMenuItem("Login");
		this.connectionLogin.setEnabled(true);
		this.connectionLogin.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e)
			{
				PopUpWindow login = new PopUpWindow(mainClass, "Login to server");
				login.showLoginWindow();
			}
		});
		connectionMenu.add(this.connectionLogin);
		
		// Connection - Register
		this.connectionRegister = new JMenuItem("Register");
		this.connectionRegister.setEnabled(false);
		this.connectionRegister.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e)
			{
				PopUpWindow login = new PopUpWindow(mainClass, "Register");
				login.showRegisterWindow();
			}
		});
		connectionMenu.add(this.connectionRegister);

		// Connection - Save
		this.connectionSave = new JMenuItem("Save on server");
		this.connectionSave.setEnabled(false);
		this.connectionSave.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e)
			{
				PopUpWindow save = new PopUpWindow(mainClass, "Save on server");
				save.showSaveOnServerWindow();
			}
		});
		connectionMenu.add(this.connectionSave);

		// Connection - Browser
		this.connectionBrowser = new JMenuItem("Server browser");
		this.connectionBrowser.setEnabled(false);
		this.connectionBrowser.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e)
			{
				// Get files from server
				mainClass.connection.sendObject(new Communication("getFiles"));
				Vector<PetrinetMain> files = (Vector<PetrinetMain>)mainClass.connection.getObject();

				mainClass.connection.sendObject(new Communication("getHistory"));
				Vector<Simulation> simulations = (Vector<Simulation>)mainClass.connection.getObject();
				
				ServerBrowser browser = new ServerBrowser(mainClass, files, simulations);
				browser.showServerBrowser();
			}
		});
		connectionMenu.add(this.connectionBrowser);

		// Settings
		JMenu settingsMenu = new JMenu("Settings");
		menuBar.add(settingsMenu);
		
		// Settings - Appearance
		JMenuItem appearanceMenu = new JMenuItem("Appearance");
		appearanceMenu.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent ae)
			{
				Appearance appearance = new Appearance(mainClass);
				appearance.showAppearanceWindow();
			}
		});
		settingsMenu.add(appearanceMenu);

		// Help
		JMenu helpMenu = new JMenu("Help");
		menuBar.add(helpMenu);
		
		// Help - help
		JMenuItem helpHelpMenu = new JMenuItem("Help");
		helpHelpMenu.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent ae)
			{
				// Frame
				JFrame helpFrame = new JFrame("Help");

				// Panel
				JPanel helpPanel = new JPanel();
				helpPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
				JLabel helpTextLabel = new JLabel(helpText);
				helpPanel.add(helpTextLabel);
				
				// Show frame
				helpFrame.add(helpTextLabel, BorderLayout.CENTER);
				helpFrame.pack();
				helpFrame.setVisible(true);
				
				// Center window
				Dimension dim = Toolkit.getDefaultToolkit().getScreenSize();
				int w = helpFrame.getSize().width;
				int h = helpFrame.getSize().height;
				int x = (dim.width-w)/2;
				int y = (dim.height-h)/2;
				
				// Move the window
				helpFrame.setLocation(x, y);
			}
		});
		helpMenu.add(helpHelpMenu);
		
		// Help - about
		JMenuItem aboutMenu = new JMenuItem("About");
		aboutMenu.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent ae)
			{
				JOptionPane.showMessageDialog(null, "Petri's nets v 0.2.1\n\nCreated by\n  - Pavlina Bartikova\n  - Branislav Blaskovic\n\n2012 FIT VUT Brno", "About Petri's nets", JOptionPane.INFORMATION_MESSAGE);
			}
		});
		helpMenu.add(aboutMenu);
		

		return menuBar;
	}

	/**
	 * Toggle server menu bar options visibility.
	 * @param value 
	 */
	public void toggleServerOperations(Boolean value)
	{
		this.connectionRegister.setEnabled(value);
		this.connectionSave.setEnabled(value);
		this.connectionBrowser.setEnabled(value);
	}
}
