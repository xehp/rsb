package se.eit.rsb_package;


public class SmallFifoBuffer {

	String a[]=new String[8];
	int head=0;
	int tail=0;
	
	public void put(String str)
	{
		final int nextTail=(tail+1)%8;
		if (nextTail==head)
		{
			// drop oldest in queue
			++head;
			head%=8;
		}
		a[tail]=str;
		tail=nextTail;
	}

	
	public String remove()
	{
		if (head!=tail)
		{
			String str=a[head];
			++head;
			head%=8;
			return str; 
		}
		else
		{
			return null;
		}
	}
}
