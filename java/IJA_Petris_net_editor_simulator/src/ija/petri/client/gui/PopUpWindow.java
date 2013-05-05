package ija.petri.client.gui;

import ija.petri.magic.connection.Communication;
import ija.petri.magic.petrinet.PetrinetMain;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.*;

/**
 * Class for showing popup windows.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class PopUpWindow extends JFrame
{
	private Container contentPane;
	private Editor mainClass;
	
	public PopUpWindow(Editor editor, String title)
	{
		super(title);
		this.mainClass = editor;
		
		// Get contentPane
		this.contentPane = getContentPane();
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
	 * Shows login popup window.
	 */
	public void showLoginWindow()
	{
		JPanel panelCredentials = new JPanel();
		JPanel panelServer = new JPanel();
		panelCredentials.setBorder(BorderFactory.createTitledBorder("Credentials"));
		panelServer.setBorder(BorderFactory.createTitledBorder("Server"));
		
		final JTextField nick = new JTextField("", 10);
		final JTextField password = new JPasswordField("", 10);

		final JTextField address = new JTextField("localhost:2000", 21);
		address.setHorizontalAlignment(JTextField.CENTER);
		JButton btnLogin = new JButton("Login");
		
		panelCredentials.add(nick, BorderLayout.LINE_START);
		panelCredentials.add(password, BorderLayout.CENTER);
		panelServer.add(address);
		
		// Action listener
		btnLogin.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae) {
				
				// Parse address and port
				String serverAddress = "localhost";
				int port = 2000;
				if(address.getText().indexOf(":") > 0)
				{
					serverAddress = address.getText().split(":")[0];
					port = Integer.parseInt(address.getText().split(":")[1]);
				}
				// Connect to server
				if(!mainClass.connection.isConnected())
				{
					mainClass.connection.setServerDetails(serverAddress, port);
					mainClass.connection.connectToServer();
				}
				if(mainClass.connection.isConnected())
				{
					mainClass.statusBar.setMessage("Connection established");
					
					// Send credentials
					mainClass.connection.sendObject(new Communication("login"));
					mainClass.connection.sendObject(nick.getText());
					mainClass.connection.sendObject(mainClass.connection.hashIt(password.getText()));

					// Receive respond
					Boolean respond = (Boolean)mainClass.connection.getObject();
					if(respond == null || respond == false)
					{
						mainClass.statusBar.setMessage("Incorect nickname or password.");
					}
					else
					{
						mainClass.statusBar.setMessage("You have been successfuly logged in.");
						setVisible(false);
					}
				}
				else
				{
					mainClass.statusBar.setMessage("Connection refused (is server running?)");
				}
				
			}
		});
		
		// Add components to layout
		setLayout(new BorderLayout(10, 10));
        //this.contentPane.add(new JLabel("Credentials: "), BorderLayout.NORTH);
		this.contentPane.add(panelServer, BorderLayout.NORTH);
		this.contentPane.add(panelCredentials, BorderLayout.CENTER);
		this.contentPane.add(btnLogin, BorderLayout.SOUTH);
		
		pack();
		setVisible(true);
		
		centerWindow();
	}
	
	/**
	 * Shows register popup window.
	 */
	public void showRegisterWindow()
	{
		final JTextField nick = new JTextField("", 10);
		final JTextField password = new JPasswordField("", 10);
		JButton btnLogin = new JButton("Register");
		
		// Action listener
		btnLogin.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent ae) {
				// Send register data
				mainClass.connection.sendObject(new Communication("register"));
				mainClass.connection.sendObject(nick.getText());
				mainClass.connection.sendObject(mainClass.connection.hashIt(password.getText()));
				
				// Receive respond
				Communication respond = (Communication)mainClass.connection.getObject();
				if(respond == null)
				{
					mainClass.statusBar.setMessage("Null pointer received.");
				}
				else
				{
					mainClass.statusBar.setMessage(respond.getCommand());
					if(respond.success())
						setVisible(false);
				}
			}
		});
		
		// Add components to layout
		setLayout(new BorderLayout(10, 10));
        this.contentPane.add(new JLabel("Register data: "), BorderLayout.NORTH);
        this.contentPane.add(nick, BorderLayout.LINE_START);
		this.contentPane.add(password, BorderLayout.CENTER);
		this.contentPane.add(btnLogin, BorderLayout.SOUTH);
		
		pack();
		setVisible(true);
		
		centerWindow();
	}

	/**
	 * Show popup window for saving net on server.
	 */
	public void showSaveOnServerWindow()
	{
		GridBagLayout layout = new GridBagLayout();
		GridBagConstraints c = new GridBagConstraints();
		JPanel panel = new JPanel(layout);
	

		panel.setBorder(BorderFactory.createTitledBorder("Petri's net data"));
		
		final JTextField name = new JTextField(this.mainClass.petrinetMain.getName(), 10);
		final JTextField description = new JTextField(this.mainClass.petrinetMain.getDescription(), 10);
		
		c.insets = new Insets(5, 5, 5, 5);
		
		c.gridx = 0;c.gridy = 0;
		panel.add(new JLabel("Name"), c);
		c.gridx = 1;c.gridy = 0;
		panel.add(name, c);
		c.gridx = 0;c.gridy = 1;
		panel.add(new JLabel("Description"), c);
		c.gridx = 1;c.gridy = 1;
		panel.add(description, c);

		JButton btnSave = new JButton("Save");
		
		btnSave.addActionListener(new ActionListener() {

			public void actionPerformed(ActionEvent ae)
			{
				mainClass.petrinetMain.setName(name.getText());
				mainClass.petrinetMain.setDescription(description.getText());
				PetrinetMain petriToSend = (PetrinetMain) mainClass.petrinetMain.clone();
				
				mainClass.connection.sendObject(new Communication("save"));
				mainClass.connection.sendObject(petriToSend);
				
				// Receive respond
				String respond = (String)mainClass.connection.getObject();
				PetrinetMain receivedPetri = (PetrinetMain)mainClass.connection.getObject();
				mainClass.petrinetMain = receivedPetri;
				mainClass.tabs.setTitleAt(mainClass.tabsLastIndex, receivedPetri.getName());

				if(respond == null)
				{
					mainClass.statusBar.setMessage("Null pointer received.");
				}
				else
				{
					mainClass.statusBar.setMessage(respond);
				}
			}
		});
		
		setLayout(new BorderLayout(10, 10));
        this.contentPane.add(panel, BorderLayout.CENTER);
		this.contentPane.add(btnSave, BorderLayout.SOUTH);
		
		pack();
		setVisible(true);
		
		centerWindow();
	}
}

