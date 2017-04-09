package se.eit.d1_pkg;

import se.eit.web_package.WordReader;

public interface D1Interface {

	public D1Interface getParent();
	public int getId();
	public D1GlobalConfig getGlobalConfig();
	public D1Interface findObjectByNameIndexOrId(WordReader wr);
	
}
