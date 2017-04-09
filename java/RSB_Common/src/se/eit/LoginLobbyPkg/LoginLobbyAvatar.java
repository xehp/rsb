package se.eit.LoginLobbyPkg;

import se.eit.d1_pkg.D1ThreadSafe;
import se.eit.rsb_package.AvatarInterface;
import se.eit.web_package.WordReader;
import se.eit.web_package.WordWriter;

public class LoginLobbyAvatar extends D1ThreadSafe implements AvatarInterface{

	//public int pinCode=0;
	public int team;
	public String ip;
	
	
	// http://stackoverflow.com/questions/936684/getting-the-class-name-from-a-static-method-in-java		
	public static String className()
	{	
		return LoginLobbyAvatar.class.getSimpleName();	
	}
	
	public LoginLobbyAvatar()
	{	
		super();
		
		// No the below can't be done here, object must be linked first so do it in linkSelf instead.
		//WorldBase worldBase = (WorldBase)this.getDbSubRoot();
		//pinCode=worldBase.random.nextInt();
	}

	
	@Override
	public void readSelf(WordReader wr)
	{
		super.readSelf(wr);
						
		//pinCode=wr.readInt();
		team=wr.readInt();
		ip=wr.readString();
	}
	
	@Override
	public void writeSelf(WordWriter ww)
	{		
		super.writeSelf(ww);

  		//ww.writeInt(pinCode);  			
  		ww.writeInt(team);  			
  		ww.writeString(ip);  			
	}
	
	@Override
	public void listInfo(WordWriter pw)
	{
		super.listInfo(pw);
		
		//pw.println("pinCode"+" "+pinCode);
		pw.println("team"+" "+team);
		pw.println("ip"+" "+ip);
		
	}
	
	@Override
	public int getInfo(WordWriter ww, String parameterName, WordReader wr)
	{
		if (parameterName.equals("team"))
		{
			ww.writeInt(team);
			return 1;
		}
		if (parameterName.equals("ip"))
		{
			ww.writeString(ip);
			return 1;
		}
		
		return super.getInfo(ww, parameterName, wr);
	}	

	
	@Override
	public int setInfo(WordReader wr, String infoName)
	{
		if (infoName.equals("team"))
		{
			team=wr.readInt();
			setUpdateCounter();

			return 1;
		}
		return super.setInfo(wr, infoName);
	}

	
	@Override
	public void setTeam(int team) {
		this.team=team;
	}


	public void setIp(String ip) {
		this.ip=ip;
	}
	
	public String getIp() {
		return this.ip;
	}

/*	
	@Override
	public void linkSelf(D1Container parentObj)
	{
		super.linkSelf(parentObj);
		WorldBase worldBase = (WorldBase)this.getDbSubRoot();
		pinCode=worldBase.random.nextInt();
	}
	*/
}
