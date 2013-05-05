package ija.petri.client.gui;

import ija.petri.client.connection.ClientConnection;
import ija.petri.magic.connection.Communication;
import ija.petri.magic.petrinet.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.geom.AffineTransform;
import java.util.Vector;
import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

/**
 * Main GUI class for editor.
 * @author Pavlina Bartikova, Branislav Blaskovic
 */
public class Editor extends JPanel implements MouseListener, MouseMotionListener
{
	private Dimension area; 
	protected Vector<PetriObject> objects;
	protected Vector<Vector<PetriObject>> objectsVect;
	public PetrinetMain petrinetMain;
	public Vector<PetrinetMain> petrinetMainVect;
	private JPanel drawingPane;
	private boolean changed = false;

	protected String appearanceConfig = "examples/appearance_actual.xml";
	
	// Dimensions
	private final int W = 30;
	private final int H = 30;
	private final int WT = 10;
	private final int HT = 30;

	// Buttons
	private JButton btnStep;
	private JButton btnSimulate;
	private JButton btnSelect;
	private JButton btnAddPlace;
	private JButton btnAddTransition;
	private JButton btnAddArcs;
	private JButton btnClear;
	private JButton btnRemove;
	private JButton btnAddTab;
	private JButton btnCloseTab;
	// Actions
	private boolean actAddPlace;
	private boolean actAddTransition;
	private boolean actSelect;
	private boolean actRemove;
	private int actAddArcs;
	private int startArcs;
	private int endArcs;
	private Settings settings;
	private int dragIndex;
	public StatusBar statusBar;
	protected ClientConnection connection;
	public MenuBar menuBar;
	public JTabbedPane tabs;
	public int tabsLastIndex = 0;
	private boolean tabsWorking;
	public JScrollPane scroller;

	// Appearance (colors)
	private Color placeBg;
	private Color placeFg;
	private Color transBg;
	private Color transFg;
	private Color arcsLine;
	private Color highlightColor;
	private Color fontColor;
	private Color background;
	// Indexes
	public int placeBgIndex = 0;
	public int placeFgIndex = 0;
	public int transBgIndex = 0;
	public int transFgIndex = 0;
	public int arcsLineIndex = 0;
	public int highlightColorIndex = 0;
	public int fontColorIndex = 0;
	public int backgroundIndex = 0;
	
	/**
	 * Initialize all neccessery components.
	 */
	public Editor()
	{
		super(new BorderLayout());

		area = new Dimension(0,0);
		this.objects = new Vector<PetriObject>();
		this.petrinetMain = new PetrinetMain(this.objects);
		this.petrinetMain.setName("Local file");
		this.petrinetMain.setObjects(this.objects);
		this.objectsVect = new Vector<Vector<PetriObject>>();
		this.petrinetMainVect = new Vector<PetrinetMain>();

		this.objectsVect.addElement(this.objects);
		this.petrinetMainVect.addElement(this.petrinetMain);
		
		this.settings = new Settings();
		this.startArcs = -1;
		this.endArcs = -1;

		// Draw main pane
		drawingPane = new DrawingPane();
		drawingPane.setBackground(Color.white);
		drawingPane.addMouseListener(this);
		drawingPane.addMouseMotionListener(this);

		// Colors
		Appearance appearance = new Appearance(this);
		setAppearanceColor(0, 0, 0, 0, 0, 0, 0, 0);
		appearance.importSettings(this.appearanceConfig);
		
		// Buttons
		this.btnStep = new JButton("Step");
		this.btnSimulate = new JButton("Simulate");
		this.btnAddTab = new JButton("New tab");
		this.btnCloseTab = new JButton("Close tab");
		this.btnSelect = new JButton("Select");
		this.btnRemove = new JButton("Remove");
		this.btnAddPlace = new JButton("Add place");
		this.btnAddTransition = new JButton("Add transition");
		this.btnAddArcs = new JButton("Add arcs");
		this.btnClear = new JButton("Clear window");

		// Cursors
		this.btnCloseTab.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
		this.btnAddTab.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
		this.btnStep.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
		this.btnSimulate.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
		this.btnSelect.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
		this.btnRemove.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
		this.btnAddPlace.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
		this.btnAddTransition.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
		this.btnAddArcs.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
		this.btnClear.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));

		ActionListener btnListeners = new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				if(e.getSource() == btnAddTab)
				{
					PetrinetMain newPetri = new PetrinetMain();
					newPetri.setName("Local file");
					// Add to vectors
					petrinetMainVect.addElement(newPetri);
					objectsVect.addElement(newPetri.getObjects());
					// Add tab
					tabs.addTab(newPetri.getName(), new JLabel("loading ..."));
					tabs.setSelectedIndex(tabs.getTabCount() - 1);
				}
				if(e.getSource() == btnCloseTab)
				{
					if(tabs.getTabCount() > 1)
					{
						int index = tabs.getSelectedIndex();
						tabsLastIndex = index;
						// Remove tab
						int newIndex = index - 1;
						if(tabs.getSelectedIndex() == 0)
						{
							newIndex = 1;
						}
						tabs.setSelectedIndex(newIndex);
						
						// Remove from vectors
						tabs.removeTabAt(index);
						petrinetMainVect.removeElementAt(index);
						objectsVect.remove(index);
						try
						{
							tabs.setComponentAt(newIndex, scroller);
						}
						catch (Exception ex){}
					}
				}
				if(e.getSource() == btnStep)
				{
					if(connection.isConnected())
					{
						// Send actual
						connection.sendObject(new Communication("step"));
						PetrinetMain toSend = (PetrinetMain)petrinetMain.clone();
						connection.sendObject(toSend);
						// Receive
						PetrinetMain newPetri = (PetrinetMain)connection.getObject();
						petrinetMain = newPetri;
						objects = newPetri.getObjects();
						petrinetMainVect.setElementAt(newPetri, tabsLastIndex);
						objectsVect.setElementAt(newPetri.getObjects(), tabsLastIndex);
						
						updateDrawingPane();
						statusBar.setMessage("Step forward");
					}
					else
					{
						statusBar.setMessage("Not connected to server.");
					}
				}
				if(e.getSource() == btnSimulate)
				{
					if(connection.isConnected())
					{
						// Send actual
						connection.sendObject(new Communication("simulate"));
						PetrinetMain toSend = (PetrinetMain)petrinetMain.clone();
						connection.sendObject(toSend);
						// Receive
						PetrinetMain newPetri = (PetrinetMain)connection.getObject();
						petrinetMain = newPetri;
						objects = newPetri.getObjects();
						petrinetMainVect.setElementAt(newPetri, tabsLastIndex);
						objectsVect.setElementAt(newPetri.getObjects(), tabsLastIndex);
						
						updateDrawingPane();
						statusBar.setMessage("Simulation");
					}
					else
					{
						statusBar.setMessage("Not connected to server.");
					}
				}
				if(e.getSource() == btnSelect)
				{
					clearActions();
					actSelect = true;
					statusBar.setMessage("Select mode");
				}
				if(e.getSource() == btnRemove)
				{
					clearActions();
					actRemove = true;
					statusBar.setMessage("Remove mode");
				}
				if(e.getSource() == btnAddPlace)
				{
					clearActions();
					actAddPlace = true;
					statusBar.setMessage("Add place mode");
				}
				if(e.getSource() == btnAddTransition)
				{
					clearActions();
					actAddTransition = true;
					statusBar.setMessage("Add transition mode");
				}
				if(e.getSource() == btnAddArcs)
				{
					clearActions();
					actAddArcs = 1;
					statusBar.setMessage("Add arcs mode");
				}
				if(e.getSource() == btnClear)
				{
					clearActions();
					objects.removeAllElements();
					area.width=0;
					area.height=0;
					changed = true;
					updateDrawingPane();
					statusBar.setMessage("Workspace cleared");
				}
				if(e.getSource() == settings.btnSave)
				{
					updateDrawingPane();
				}
			}
		};
		// Listeners
		this.btnAddTab.addActionListener(btnListeners);
		this.btnCloseTab.addActionListener(btnListeners);
		this.btnStep.addActionListener(btnListeners);
		this.btnSimulate.addActionListener(btnListeners);
		this.btnSelect.addActionListener(btnListeners);
		this.btnRemove.addActionListener(btnListeners);
		this.btnAddPlace.addActionListener(btnListeners);
		this.btnAddTransition.addActionListener(btnListeners);
		this.btnAddArcs.addActionListener(btnListeners);
		this.btnClear.addActionListener(btnListeners);
		this.settings.btnSave.addActionListener(btnListeners);
		
		// Add all to panel!
		JPanel instructionPanel = new JPanel(new GridLayout(0,6));
		instructionPanel.setFocusable(true);
		instructionPanel.add(this.btnSelect);
		instructionPanel.add(this.btnRemove);
		instructionPanel.add(this.btnAddPlace);
		instructionPanel.add(this.btnAddTransition);
		instructionPanel.add(this.btnAddArcs);
		instructionPanel.add(this.btnClear);
		instructionPanel.add(this.btnStep);
		instructionPanel.add(this.btnSimulate);
		instructionPanel.add(new JButton(""));
		instructionPanel.add(new JButton(""));
		instructionPanel.add(this.btnAddTab);
		instructionPanel.add(this.btnCloseTab);

		// Scrolling
		this.scroller = new JScrollPane(drawingPane);
		this.scroller.setPreferredSize(new Dimension(200,500));

		// Tabs
		this.tabs = new JTabbedPane();
		this.tabs.addTab(this.petrinetMain.getName(), this.scroller);
		this.tabs.addChangeListener(new ChangeListener() {
			@Override
			public void stateChanged(ChangeEvent ce)
			{
				int index = tabs.getSelectedIndex();
				
				if(index == tabsLastIndex) return;
				if(index >= petrinetMainVect.size()) return;

				try
				{
					petrinetMain = petrinetMainVect.elementAt(index);
					objects = objectsVect.elementAt(index);
					tabs.setComponentAt(tabsLastIndex, new JLabel("not loaded"));
					tabs.setComponentAt(index, scroller);
				}
				catch (Exception e) {}
				
				updateDrawingPane();
				
				tabsLastIndex = index;
			}
		});

		// Add stuff to main window
		add(instructionPanel, BorderLayout.PAGE_START);
		add(settings, BorderLayout.LINE_START);
		add(this.tabs, BorderLayout.CENTER);
	}

	/**
	 * Set appearance of editor by parameters.
	 * @param placeBg
	 * @param placeFg
	 * @param transBg
	 * @param transFg
	 * @param arcsLine
	 * @param highlightColor
	 * @param fontColor 
	 */
	void setAppearanceColor(int placeBg, int placeFg, int transBg, int transFg, int arcsLine, int highlightColor, int fontColor, int background)
	{
		Color[] colors = {Color.BLACK, Color.BLUE, Color.GREEN, Color.RED, Color.BLACK, Color.CYAN, Color.WHITE, Color.PINK, Color.LIGHT_GRAY, Color.DARK_GRAY};
		
		this.placeBg = colors[placeBg];
		this.placeFg = colors[placeFg];
		this.transBg = colors[transBg];
		this.transFg = colors[transFg];
		this.arcsLine = colors[arcsLine];
		this.highlightColor = colors[highlightColor];
		this.fontColor = colors[fontColor];
		this.background = colors[background];

		this.placeBgIndex = placeBg;
		this.placeFgIndex = placeFg;
		this.transBgIndex = transBg;
		this.transFgIndex = transFg;
		this.arcsLineIndex = arcsLine;
		this.highlightColorIndex = highlightColor;
		this.fontColorIndex = fontColor;

		// Defaults
		if(placeBg == 0) this.placeBg = Color.LIGHT_GRAY;
		if(placeFg == 0) this.placeFg = Color.BLACK;
		if(transBg == 0) this.transBg = Color.LIGHT_GRAY;
		if(transFg == 0) this.transFg = Color.BLACK;
		if(arcsLine == 0) this.arcsLine = Color.BLACK;
		if(highlightColor == 0) this.highlightColor = Color.BLACK;
		if(fontColor == 0) this.fontColor = Color.DARK_GRAY;
		if(background == 0) this.background = Color.WHITE;

		drawingPane.setBackground(this.background);
		updateDrawingPane();
	}

	/**
	 * Clear all actions, which was triggered by buttons.
	 */
	void clearActions()
	{
		this.actAddPlace = false;
		this.actRemove = false;
		this.actAddTransition = false;
		this.actAddArcs = 0;
		this.actSelect = false;
		this.startArcs = -1;
		this.endArcs = -1;
		updateDrawingPane();
	}

	/**
	 * Create menu bar.
	 * @return menu bar reference.
	 */
	public JMenuBar getMenuBar()
	{
		this.menuBar = new MenuBar(this);
		return menuBar.getMenuBar();
	}

	/**
	 * Draw arrow to drawing pane.
	 * @param g1
	 * @param x1
	 * @param y1
	 * @param x2
	 * @param y2 
	 */
	void drawArrow(Graphics g1, int x1, int y1, int x2, int y2)
	{
		Graphics2D g = (Graphics2D) g1.create();
		int ARR_SIZE = 7;
		
		double dx = x2 - x1, dy = y2 - y1;
		double angle = Math.atan2(dy, dx);
		int len = (int) Math.sqrt(dx*dx + dy*dy) - 15;
		AffineTransform at = AffineTransform.getTranslateInstance(x1, y1);
		at.concatenate(AffineTransform.getRotateInstance(angle));
		g.transform(at);
		//g.drawLine(0, 0, len, 0);
		g.fillPolygon(new int[] {len, len-ARR_SIZE, len-ARR_SIZE, len},	new int[] {0, -ARR_SIZE, ARR_SIZE, 0}, 4);
	}

	/**
	 * Set reference to statusbar
	 * @param statusBar 
	 */
	public void setStatusBar(StatusBar statusBar) {
		this.statusBar = statusBar;
	}
	
	/**
	 * Set reference for main connection class
	 * @param connection 
	 */
	public void setConnection(ClientConnection connection)
	{
		this.connection = connection;
	}

	/**
	 * Drawing pane main class, which is drawing all objects for editor.
	 */
	public class DrawingPane extends JPanel
	{
		@Override
		protected void paintComponent(Graphics g1)
		{
			Graphics2D g = (Graphics2D) g1;
			super.paintComponent(g);

			// Antialiasing, stroke width
			g.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
			g.setStroke(new BasicStroke(1.5f));
			
			// PAINT OBJECTS!
			// Arcs
			for (int i = 0; i < objects.size(); i++)
			{
				if(objects.elementAt(i) instanceof Arcs)
				{
					int startX = 0, startY = 0;
					int endX = 0, endY = 0;
					int labX = 0, labY = 0;
					
					Arcs ar = (Arcs)objects.elementAt(i);
					startX = ar.startObject.getPosX();
					startY = ar.startObject.getPosY();
					endX = ar.endObject.getPosX();
					endY = ar.endObject.getPosY();

					labX = (startX + endX) / 2;
					labY = (startY + endY) / 2;
					if(ar.isDefaultName()) g.setColor(Color.red);
					else g.setColor(fontColor);
					g.drawString(ar.getName(), labX, labY);
					g.setColor(arcsLine);
					g.drawLine(startX, startY, endX, endY);
					g.setColor(arcsLine);
					drawArrow(g, startX, startY, endX, endY);
				}
			}
			// Transition & Places
			for (int i = 0; i < objects.size(); i++)
			{
				g.setStroke(new BasicStroke(1.5f));
				
				if(objects.elementAt(i) instanceof Transition || objects.elementAt(i) instanceof Place)
				{					
					if(objects.elementAt(i) instanceof Transition)
					{
						Transition tr = (Transition)objects.elementAt(i);
						if(tr.isDefaultName()) g.setColor(Color.red);
						else g.setColor(fontColor);
						g.drawString(tr.getEquation(), objects.elementAt(i).getPosX() - WT/2, objects.elementAt(i).getPosY() - HT/2 - 4);
					
						// Output action
						if(tr.isDefaultName2()) g.setColor(Color.red);
						else g.setColor(fontColor);
						g.drawString(tr.getOutputEquation(), objects.elementAt(i).getPosX() - WT/2, objects.elementAt(i).getPosY() + HT/2 + 14);

						g.setColor(transBg);
						g.fillRect(objects.elementAt(i).getPosX() - WT/2, objects.elementAt(i).getPosY() - HT/2, WT, HT);
						if(i == dragIndex)
							g.setColor(Color.blue);
						else if(i == startArcs)
							g.setColor(Color.red);
						else if(settings.isActualObject(objects.elementAt(i)))
						{
							g.setStroke(new BasicStroke(3.5f));
							g.setColor(highlightColor);
						}
						else
							g.setColor(transFg);
						g.drawRect(objects.elementAt(i).getPosX() - WT/2, objects.elementAt(i).getPosY() - HT/2, WT, HT);
					}
					// Place
					else if(objects.elementAt(i) instanceof Place)
					{
						Place pl = (Place)objects.elementAt(i);
						if(pl.isDefaultName()) g.setColor(Color.red);
						else g.setColor(fontColor);
						
						g.drawString(pl.getNumbers(), objects.elementAt(i).getPosX() - WT/2, objects.elementAt(i).getPosY() - HT/2 - 4);
						g.setColor(placeBg);
					
						g.fillOval(objects.elementAt(i).getPosX() - W/2, objects.elementAt(i).getPosY() - H/2, W, H);
						if(i == dragIndex)
							g.setColor(Color.blue);
						else if(i == startArcs)
							g.setColor(Color.red);
						else if(settings.isActualObject(objects.elementAt(i)))
						{
							g.setStroke(new BasicStroke(3.5f));
							g.setColor(highlightColor);
						}
						else
							g.setColor(placeFg);
						g.drawOval(objects.elementAt(i).getPosX() - W/2, objects.elementAt(i).getPosY() - H/2, W, H);
					}
				}
			}
		}
	}

	/**
	 * Try to find an object on given coordinates.
	 * @param x
	 * @param y
	 * @return object index in vector
	 */
	public int findObjectAtPos(int x, int y)
	{
		for(int i = 0; i < objects.size(); i++)
		{
			if(objects.elementAt(i) instanceof Place)
			{

				Place place = (Place)objects.elementAt(i);
				int posTopX = place.getPosX() - (W/2);
				int posTopY = place.getPosY() - (H/2);
				if(
					posTopX < x && (posTopX + W) > x &&
					posTopY < y && (posTopY + H) > y
				)
				{
					return i;
				}

			}
			else if(objects.elementAt(i) instanceof Transition)
			{
				Transition transition = (Transition)objects.elementAt(i);
				int posTopX = transition.getPosX() - (WT/2);
				int posTopY = transition.getPosY() - (HT/2);
				if(
					posTopX < x && (posTopX + WT) > x &&
					posTopY < y && (posTopY + HT) > y
				)
				{
					return i;
				}
			}
			else if(objects.elementAt(i) instanceof Arcs)
			{
				Arcs ar = (Arcs)objects.elementAt(i);
				int startX = 0, startY = 0;
				int endX = 0, endY = 0;
				int labX = 0, labY = 0;
				startX = ar.startObject.getPosX();
				startY = ar.startObject.getPosY();
				endX = ar.endObject.getPosX();
				endY = ar.endObject.getPosY();
				int posTopX = (startX + endX) / 2 - 20;
				int posTopY = (startY + endY) / 2 - 20;
				if(
					posTopX < x && (posTopX + 40) > x &&
					posTopY < y && (posTopY + 40) > y
				)
				{
					return i;
				}
			}
		}

		return -1;
	}

	/**
	 * Redraw drawing pane.
	 */
	public void updateDrawingPane()
	{
		this.petrinetMain.setObjects(this.objects);

		if (changed)
		{
			drawingPane.setPreferredSize(area);
			drawingPane.revalidate();
			changed = false;
		}
		drawingPane.repaint();
	}

	//Handle mouse events.
	@Override
	public void mouseClicked(MouseEvent e)
	{
		
		if (SwingUtilities.isRightMouseButton(e))
		{
			//This will clear the graphic objects.
			objects.removeAllElements();
			area.width=0;
			area.height=0;
			changed = true;
		}
		else
		{
			int width = W;
			int height = H;
			if(actAddTransition)
			{
				width = WT;
				height = HT;
			}

			int x = e.getX() - width/2;
			int y = e.getY() - height/2;
			if (x < 0) x = 0;
			if (y < 0) y = 0;

			Rectangle rect = new Rectangle(x, y, width, height);

			// SELECT
			int pos = -1;
			if(actAddArcs == 0 && !actRemove && (pos = findObjectAtPos(e.getX(), e.getY())) != -1)
			{
				settings.setActualObject(objects.elementAt(pos));
			}
			// ADDING
			else if(actAddPlace && findObjectAtPos(e.getX(), e.getY()) == -1)
			{
				Place pl = new Place();
				pl.setPosition(e.getX(), e.getY());
				objects.addElement(pl);
			}
			else if(actAddTransition && findObjectAtPos(e.getX(), e.getY()) == -1)
			{
				Transition trans = new Transition();
				trans.setPosition(e.getX(), e.getY());
				objects.addElement(trans);
			}
			// SELECTING
			else if(actSelect)
			{
				int index = findObjectAtPos(e.getX(), e.getY());
				if(index != -1)
				{
					
				}
			}
			// REMOVING
			else if(actRemove)
			{
				int index = findObjectAtPos(e.getX(), e.getY());

				if(index != -1)
				{
					PetriObject toRemove = objects.elementAt(index);
					// Remove arcs
					objects.remove(index);

					Vector<PetriObject> newObjects = new Vector<PetriObject>();

					for(int i = 0; i < objects.size(); i++)
					{
						if(objects.elementAt(i) instanceof Arcs)
						{
							Arcs ar = (Arcs)objects.elementAt(i);
							if(objects.indexOf(ar.startObject) == -1 || objects.indexOf(ar.endObject) == -1)
							{
								continue;
							}
							else
							{
								newObjects.add(objects.elementAt(i));
							}
						}
						else
						{
							newObjects.add(objects.elementAt(i));
						}
					}
					objects.removeAllElements();
					objects.addAll(newObjects);
				}
			}
			// JOINING
			else if(actAddArcs == 1)
			{
				// Start of arcs
				int index = findObjectAtPos(e.getX(), e.getY());
				if(index != -1)
				{
					startArcs = index;
					actAddArcs = 2;
				}
			}
			else if(actAddArcs == 2)
			{
				// End of arcs
				int index = findObjectAtPos(e.getX(), e.getY());
				if(index != -1)
				{
					endArcs = index;
					actAddArcs = 1;

					if(endArcs != startArcs)
					{
						Arcs ar = new Arcs();
						ar.setObjects(objects.elementAt(startArcs), objects.elementAt(endArcs));
						objects.addElement(ar);
						startArcs = -1;
						endArcs = -1;
					}
				}
			}

			drawingPane.scrollRectToVisible(rect);

			// Stretch
			int this_width = (x + W + 2);
			if (this_width > area.width)
			{
				area.width = this_width; changed=true;
			}

			int this_height = (y + H + 2);
			if (this_height > area.height)
			{
				area.height = this_height; changed=true;
			}
		}
		updateDrawingPane();
	}

	@Override
	public void mousePressed(MouseEvent e)
	{
		int pos = -1;
		if(actSelect && (pos = findObjectAtPos(e.getX(), e.getY())) != -1)
		{
			if(objects.elementAt(pos) instanceof Place || objects.elementAt(pos) instanceof Transition)
			{
				dragIndex = pos;
			}
		}
		else
		{
			dragIndex = -1;
		}

	}

	@Override
	public void mouseReleased(MouseEvent e)
	{
		/*if(dragIndex != -1)
		{
			objects.elementAt(dragIndex).setPosition(e.getX(), e.getY());
			updateDrawingPane();
		}*/
		dragIndex = -1;
		updateDrawingPane();
	}
	@Override
	public void mouseDragged(MouseEvent e)
	{
		if(dragIndex != -1)
		{
			objects.elementAt(dragIndex).setPosition(e.getX(), e.getY());
			updateDrawingPane();
		}
	}
	@Override
	public void mouseMoved(MouseEvent e) {}
	@Override
	public void mouseEntered(MouseEvent e){}
	@Override
	public void mouseExited(MouseEvent e){}
	
}
