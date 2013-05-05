package ija.petri.client.gui;

import ija.petri.magic.files.XMLReader;
import ija.petri.magic.files.XMLWriter;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import javax.swing.*;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

/**
 * Settings appearance window.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class Appearance extends JFrame
{
	public Editor mainClass;

	protected int xmlActual = 0;
	protected int xmlFontColor = 0;
	protected int xmlPlaceBg = 0;
	protected int xmlPlaceFg = 0;
	protected int xmlTransBg = 0;
	protected int xmlTransFg = 0;
	protected int xmlArcsLine = 0;
	protected int xmlHighlightColor = 0;
	protected int xmlBackground = 0;
	
	public Appearance(Editor editor)
	{
		this.mainClass = editor;
	}

	/**
	 * Create and show popup appearance window.
	 */
	public void showAppearanceWindow()
	{
		JPanel panel = new JPanel(new GridLayout(0, 2, 10, 10));

		// Colors
		String[] colors = {"default", "blue", "green", "red", "black", "cyan", "white", "pink", "light gray", "dark gray"};
		
		// Import, export
		JButton btnExport = new JButton("Export");
		JButton btnImport = new JButton("Import");
		panel.add(btnExport);
		panel.add(btnImport);

		// Place
		panel.add(new JLabel("Place background color:"));
		final JComboBox placeBg = new JComboBox(colors);
		placeBg.setSelectedIndex(mainClass.placeBgIndex);
		panel.add(placeBg);
	
		panel.add(new JLabel("Place foreground color:"));
		final JComboBox placeFg = new JComboBox(colors);
		placeFg.setSelectedIndex(mainClass.placeFgIndex);
		panel.add(placeFg);

		// Transition
		panel.add(new JLabel("Transition background color:"));
		final JComboBox transBg = new JComboBox(colors);
		transBg.setSelectedIndex(mainClass.transBgIndex);
		panel.add(transBg);
	
		panel.add(new JLabel("Transition foreground color:"));
		final JComboBox transFg = new JComboBox(colors);
		transFg.setSelectedIndex(mainClass.transFgIndex);
		panel.add(transFg);

		// Arcs
		panel.add(new JLabel("Arcs line color:"));
		final JComboBox arcsLine = new JComboBox(colors);
		arcsLine.setSelectedIndex(mainClass.arcsLineIndex);
		panel.add(arcsLine);

		// Highlight
		panel.add(new JLabel("Highlight color:"));
		final JComboBox highlight = new JComboBox(colors);
		highlight.setSelectedIndex(mainClass.highlightColorIndex);
		panel.add(highlight);

		// Font
		panel.add(new JLabel("Font color:"));
		final JComboBox font = new JComboBox(colors);
		font.setSelectedIndex(mainClass.fontColorIndex);
		panel.add(font);

		// Background
		panel.add(new JLabel("Background color:"));
		final JComboBox background = new JComboBox(colors);
		background.setSelectedIndex(mainClass.fontColorIndex);
		panel.add(background);

		// Buttons
		JButton btnSave = new JButton("Save");
		panel.add(btnSave);

		final JButton btnClose = new JButton("Save & Close");
		panel.add(btnClose);
		
		// Close and save
		ActionListener al = new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent ae)
			{
				mainClass.setAppearanceColor(
						placeBg.getSelectedIndex(),
						placeFg.getSelectedIndex(),
						transBg.getSelectedIndex(),
						transFg.getSelectedIndex(),
						arcsLine.getSelectedIndex(),
						highlight.getSelectedIndex(),
						font.getSelectedIndex(),
						background.getSelectedIndex()
				);
				mainClass.updateDrawingPane();
				
				export(
					mainClass.appearanceConfig,
					placeBg.getSelectedIndex(),
					placeFg.getSelectedIndex(),
					transBg.getSelectedIndex(),
					transFg.getSelectedIndex(),
					arcsLine.getSelectedIndex(),
					highlight.getSelectedIndex(),
					font.getSelectedIndex(),
					background.getSelectedIndex()
				);

				if(ae.getSource() == btnClose)
					setVisible(false);
			}
		};
		
		btnClose.addActionListener(al);
		btnSave.addActionListener(al);

		// Export & Import
		btnImport.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent ae)
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
				
				int returnVal = fc.showOpenDialog(null);
				if (returnVal == JFileChooser.APPROVE_OPTION)
				{
					File file = fc.getSelectedFile();
					importSettings(file.getAbsolutePath());
				}
			}
		});
		btnExport.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent ae)
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
				
				int returnVal = fc.showSaveDialog(null);
				if (returnVal == JFileChooser.APPROVE_OPTION)
				{
					File file = fc.getSelectedFile();
					export(
						file.getAbsolutePath(),
						placeBg.getSelectedIndex(),
						placeFg.getSelectedIndex(),
						transBg.getSelectedIndex(),
						transFg.getSelectedIndex(),
						arcsLine.getSelectedIndex(),
						highlight.getSelectedIndex(),
						font.getSelectedIndex(),
						background.getSelectedIndex()
					);
				}
			}
		});
	
		add(panel, BorderLayout.CENTER);

		setTitle("Appearance");
		pack();
		setVisible(true);
		centerWindow();
	}

	/**
	 * Create XML file with saved appearance settings.
	 * @param filename
	 * @param placeBg
	 * @param placeFg
	 * @param transBg
	 * @param transFg
	 * @param arcsLine
	 * @param highlightColor
	 * @param fontColor 
	 */
	public void export(String filename, int placeBg, int placeFg, int transBg, int transFg, int arcsLine, int highlightColor, int fontColor, int background)
	{
		XMLWriter writer = new XMLWriter();
		writer.setOutputFile(filename);

		writer.appendOutput(
			writer.createElement("placeBg", String.format("%d", placeBg), false, false) +
			writer.createElement("placeFg", String.format("%d", placeFg), false, false) +
			writer.createElement("transBg", String.format("%d", transBg), false, false) +
			writer.createElement("transFg", String.format("%d", transFg), false, false) +
			writer.createElement("arcsLine", String.format("%d", arcsLine), false, false) +
			writer.createElement("highlightColor", String.format("%d", highlightColor), false, false) +
			writer.createElement("fontColor", String.format("%d", fontColor), false, false) + 
			writer.createElement("background", String.format("%d", background), false, false)
		);

		writer.echoOutput("root");
	}

	/**
	 * Open XML appearance file and apply settings.
	 * @param filename 
	 */
	public void importSettings(String filename)
	{
		DefaultHandler handler = new DefaultHandler()
		{

			@Override
			public void endElement(String uri, String localName, String qName) throws SAXException
			{
				// Atributes
				if(qName.equalsIgnoreCase("placeBg"))
				{
					xmlPlaceBg = xmlActual;
				}

				if(qName.equalsIgnoreCase("placeFg"))
				{
					xmlPlaceFg = xmlActual;
				}

				if(qName.equalsIgnoreCase("transBg"))
				{
					xmlTransBg = xmlActual;
				}

				if(qName.equalsIgnoreCase("transFg"))
				{
					xmlTransFg = xmlActual;
				}

				if(qName.equalsIgnoreCase("arcsLine"))
				{
					xmlArcsLine = xmlActual;
				}

				if(qName.equalsIgnoreCase("highlightColor"))
				{
					xmlHighlightColor = xmlActual;
				}

				if(qName.equalsIgnoreCase("fontColor"))
				{
					xmlFontColor = xmlActual;
				}

				if(qName.equalsIgnoreCase("background"))
				{
					xmlBackground = xmlActual;
				}
				
				// Apply it!
				if(qName.equalsIgnoreCase("root"))
				{
					mainClass.setAppearanceColor(
							xmlPlaceBg,
							xmlPlaceFg,
							xmlTransBg,
							xmlTransFg,
							xmlArcsLine,
							xmlHighlightColor,
							xmlFontColor,
							xmlBackground
					);
					mainClass.updateDrawingPane();
					setVisible(false);
				}
			}

			@Override
			public void characters(char ch[], int start, int length) throws SAXException
			{
				try
				{
					String str = new String(ch, start, length);
					xmlActual = Integer.parseInt(str);
				}
				catch(Exception e)
				{
					xmlActual = 0;	
				}
			}
		};

		try
		{
			XMLReader read = new XMLReader(filename);
			read.setHandler(handler);
			read.startParse();
		}
		catch(Exception e)
		{
			System.err.println("Error: loadUsers(): " + e);
		}
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
}
