package ija.petri.client;

import ija.petri.client.connection.ClientConnection;
import ija.petri.client.gui.Editor;
import ija.petri.client.gui.StatusBar;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import javax.swing.JFrame;

/**
 * Main class for client side
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
class ClientMain
{
	/**
	 * Main method for creating GUI
	 */
	private static void createAndShowGUI()
	{
		// Create and set up the window.
		JFrame frame = new JFrame("Petri Net");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		// Prepare connection
		ClientConnection mainConnection = new ClientConnection();
		
		// Create and set up the content pane.
		Editor editor = new Editor();

		// Status bar
		StatusBar statusBar = new StatusBar(editor);
		
		editor.setStatusBar(statusBar);
		editor.setConnection(mainConnection);
		editor.setOpaque(true);
		frame.setContentPane(editor);

		mainConnection.setEditor(editor);

		// Display Status Bar
		frame.getContentPane().add(statusBar, java.awt.BorderLayout.SOUTH);

		// Pack menubar
		frame.setJMenuBar(editor.getMenuBar());

		// Close window action listener
		frame.addWindowListener(new WindowAdapter() {
			@Override
			public void windowClosing(WindowEvent e)
			{
				// Here will be connection close :)
				System.exit(0);
			}
		});
		
		frame.pack();
		frame.setVisible(true);

		// Center window
		Dimension dim = Toolkit.getDefaultToolkit().getScreenSize();
		int w = frame.getSize().width;
		int h = frame.getSize().height;
		int x = (dim.width-w)/2;
		int y = (dim.height-h)/2;
		
		// Move the window
		frame.setLocation(x, y);
    }
	
	/**
	 * Main method for client side application.
	 * This is starting the gui.
	 * @param argv 
	 */
	public static void main(String argv[])
	{
		// Main loop
		javax.swing.SwingUtilities.invokeLater(new Runnable()
		{
			@Override
            public void run() {
                createAndShowGUI();
            }
        });
	}
}
