package se.eit.flameengine_package;

import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;

import se.eit.d1_pkg.*;
import se.eit.web_package.*;


public class FlameRoom extends FlameEntity{
	
	
	public FlameRoom doors[] = new FlameRoom[6];
	public String door_name[] = new String[6];
	
	// Inner size of room
	public int xSectors=64;
	public int ySectors=64;
	
	// Outer size of room
	public int outerX=2; // TODO: Rename to outerSizeX
	public int outerY=1; // TODO: Rename to outerSizeY
	
	
	
	public int[][] map = new int[xSectors][ySectors];
	
	public BufferedImage image=null; /*ImageComponent2D*/
	public WebFileData fileData=null;
	
	
	// Size of each block in this room
	public final int gridSizeX=16; 
	public final int gridSizeY=16;

	
	public FlameRoom(D1Container parent, String name, int size) 
	{
		super(parent, name);	

	    // TODO: vad mer behövs här?	
		xSectors=size;
		ySectors=size;
		
	}
	
	public FlameRoom()
	{	
		super();
	}

	
	
	@Override
	public void readSelf(WordReader wr)	
	{		
		super.readSelf(wr);
		outerX=wr.readInt();
		outerY=wr.readInt();
		xSectors=wr.readInt();
		ySectors=wr.readInt();
		for(int x=0;x<xSectors;x++)
		{	
			for(int y=0;y<ySectors;y++)
			{
				map[x][y] = wr.readInt();
			}
		}
		image=null;
	}
	
	// Move entity into room.
	@Override
	public boolean OnCollision (FlameEntity collider)
	{
		collider.position.x = 0;
		collider.position.y = 0;
		
		collider.relinkSelf(this);;
		((D1NotificationSender)getDbSubRoot()).notifySubscribers(-2);
		return true;
	}
	
	@Override
	public void writeSelf(WordWriter ww)
	{		
		super.writeSelf(ww);
		ww.writeInt(outerX);
		ww.writeInt(outerY);
		ww.writeInt(xSectors);
		ww.writeInt(ySectors);
		for(int x=0;x<xSectors;x++)
		{	
			for(int y=0;y<ySectors;y++)
			{
				ww.writeInt(map[x][y]);
			}
		}		
	}
	public int getTile (int x, int y)
	{
		if ( ((x<xSectors) && (y<ySectors)) && ((x>-1)&&(y>-1)) ) 
        {
	      return map[x][y];
        }
		return -1;
	}
	
	public int getTile (float x, float y)
	{
		return getTile((int)Math.floor(x), (int)Math.floor(y));
	}
	public void changeTile(int x, int y, int id)
	{
        if ( ((x<xSectors) && (y<ySectors)) && ((x>-1)&&(y>-1)) ) 
        {
	       map[x][y] = id;
        }

        
        /*DbSubRoot dr = getDbSubRoot();
        NotificationSender ns = (NotificationSender)dr;
        ns.notifySubscribers(this.getId());*/
        
     	this.setUpdateCounter();

        image=null;        
	}
	
	public void changeTile (float x, float y, int id)
	{
		changeTile ((int)Math.floor(x), (int)Math.floor(y), id);
	}
	/*
	public int list(PlayerCommandInterpreter pci)
	{
	
	    // list doors
	    listDoors();

	    // list objects in this room
	    listObjects();
	}
	*/
	
	/*
	public int listDoors(PlayerCommandInterpreter pci)
	{
	
	    // list doors
		int n=0;
		pci.println("From this room you can go:");
		for(int i=0;i<6;i++)
		{		
			if (doors[i]!=null)
			{
				pci.println("  "+ door_name[i] + " to " + doors[i].name);
				n++;
			}
		}	
	    if (n==0) 
	    {
	    	pci.println("nowhere");
	    }
	}

	public int listObjects(PlayerCommandInterpreter pci)
	{
	
	    // list doors
	    n=0;
	    pci.println("In this room there is:");
		for(int i=0;i<64;i++)
		{
			if (listOfsubGameObj[i]!=null)
			{
				pci.println("  "+ listOfsubGameObj[i].name);
				n++;
			}
		}
	    if (n==0) 
	    {
	    	pci.println("nothing");
	    }
		return n;
	}
	*/
	
	
	public int connect(FlameRoom other_room, String door_name)
	{
		for(int i=0;i<6;i++)
		{
			if (doors[i]==null)
			{
				doors[i]= other_room;
				this.door_name[i] = door_name;
				return 0;
			}
		}
		System.out.println("Room is full.");
		return -1;		
	}
	
	
	public final FlameRoom findDoor(String name)
	{
		for(int i=0;i<6;i++)
		{
			if (doors[i]!=null)
			{
				if (this.door_name[i].equals(name))
				{
				    return doors[i];
				}
			}
		}
		return null;
	}
	
	
	public int translateLocalGridToPixelX(int gridPosX)
	{
		return gridPosX*16;
	}

	public int translateLocalGridToPixelY(int gridPosY)
	{
		return gridPosY*16;
	}
	
	// TODO Is this still needed? The client also capable of doing this so it can do this instead. See hmeg_room.js function refreshSelf.
	public BufferedImage remakeImage(D1GlobalConfig config)
	{
		final int imageSizeX=xSectors*gridSizeX;
		final int imageSizeY=ySectors*gridSizeY;
		
		BufferedImage image = new BufferedImage(imageSizeX, imageSizeY, BufferedImage.TYPE_4BYTE_ABGR); // Or is TYPE_INT_ARGB better?
		
		// http://stackoverflow.com/questions/2318020/merging-two-images
		Graphics g = image.getGraphics();

		for(int x=0;x<xSectors;x++)
		{	
			for(int y=0;y<ySectors;y++)
			{
				int sx=translateLocalGridToPixelX(x);
				int sy=translateLocalGridToPixelY(y);
				//if ((sx>=0) && (sx<imageSizeX) && (sy>=0) && (sy<imageSizeY))
				{				
					final int blockId=map[x][y]; 
					if (blockId!=0)
					{
						String blockImageName=FlameTile.getBlockTexture(blockId);
						BufferedImage blockImage=null;
						File file=null;
						try
						{
							final String filename=config.httpRootDir+"/"+blockImageName+".png";
							file=new File(filename);
							blockImage=ImageIO.read(file);
							
							
							g.drawImage(blockImage, sx, sy, gridSizeX, gridSizeY, null);
							
							
						}
						catch (IOException e)
						{
							WordWriter.safeError("failed to read image "+file.getAbsolutePath());
						}

						//stc.writeLine("AddImg "+CityPvpBlock.getBlockTexture(blockId)+" "+sx*xSize+" "+sy*ySize+" "+xSize+" "+ ySize);
						
					}
				}
			}
		}	
		
		return image;
	}
    
	public String getImageName()
	{
		//final String n="tmp"+this.getIndexPath("/");
        D1SubRoot dr = getDbSubRoot();
		final String n="tmp"+dr.getIndexPath("/")+"/~"+this.getId();
		return n;
	}
	
	public String prepareImageAndGetName(D1GlobalConfig config)
	{
		getImage(config);
		return getImageName();
	}
	
	
	public BufferedImage getImage(D1GlobalConfig config)
	{
		if (image==null)
		{
			image=remakeImage(config);
			fileData=null;
		}
		return image;
	}
	
	public WebFileData getFile(D1GlobalConfig config)
	{		
		if (image==null)
		{
			image=remakeImage(config);
			fileData=null;
		}
		
		if (fileData==null)
		{
			if (image!=null)
			{
				debug("getFile");
				fileData = new WebFileData();

				ByteArrayOutputStream baos = new ByteArrayOutputStream();
		        try {
					ImageIO.write(image, "png", baos);
			        baos.flush();
			        fileData.data = baos.toByteArray();
					fileData.lastModified = System.currentTimeMillis();
					fileData.maxTimeS=1;
			        baos.close();
				} 
		        catch (IOException e) {
					e.printStackTrace();
				}
				catch (NullPointerException e) {
					debug("Why does this happen");
					e.printStackTrace();
				}
			}		
		}
		return fileData;
	}
	
	public boolean isControlPanel(float x, float y)
	{
		
	   if (getTile(x, y)==FlameTile.controlPanel)
	   {
		   return true;
	   }
	   return false;
	}
	
	// Commented out since this is not yet used. But perhaps it will be needed.
	/*
	// Translates coordinates within this room to the coordinates of the room that this room is in.
	public int translateToParentCoordinateX(int beforeX)
	{	
		int newX = this.x+beforeX/(this.xSectors/this.outerX);

		CityPvpRoom p = (CityPvpRoom)this.getParent();
		
		if (newX<0)
		{
			newX=0;
		}
		if (newX>=p.xSectors)
		{
			newX=p.xSectors-1;
		}
		
		return newX; 
	}

	public int translateToParentCoordinateY(int beforeY)
	{
		int newY = this.y+beforeY/(this.ySectors/this.outerY);

		CityPvpRoom p = (CityPvpRoom)this.getParent();
		
		if (newY<0)
		{
			newY=0;
		}
		if (newY>=p.ySectors)
		{
			newY=p.ySectors-1;
		}
		
		return newY;
	}
	

	// Translates from the coordinates of the room that this room is in to coordinates of this room.
	public int translateFromParentCoordinateX(int beforeX)
	{
		int newX=(beforeX-this.x) * (this.xSectors/this.outerX);
		if (newX<0)
		{
			newX=0;
		}
		if (newX>=this.xSectors)
		{
			newX=this.xSectors-1;
		}
		return newX;
		
	}

	
	public int translateFromParentCoordinateY(int beforeY)
	{
		int newY=(beforeY-this.y) * (this.ySectors/this.outerY);
		if (newY<0)
		{
			newY=0;
		}
		if (newY>=this.ySectors)
		{
			newY=this.ySectors-1;
		}
		return newY;
	}
	*/


	// This is called to send an ascii message to this object.	
	@Override
	public void postMessageToThis(String msg)
	{

		// Default action is to pass the message on to those in the room.
		if (this.listOfStoredObjects!=null)
		{
			for(D1Storable ds : this.listOfStoredObjects)
			{
				ds.postMessageToThis(msg);
			}
		}
	}
}
