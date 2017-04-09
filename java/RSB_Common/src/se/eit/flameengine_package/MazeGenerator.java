package se.eit.flameengine_package;

import java.util.Collections;
import java.util.Arrays;
 
/*
 * recursive backtracking algorithm
 * shamelessly borrowed from the ruby at
 * http://weblog.jamisbuck.org/2010/12/27/maze-generation-recursive-backtracking
 */
public class MazeGenerator {
	public final int x;
	public final int y;
	public final int[][] maze;
 
	public MazeGenerator(int x, int y) {
		this.x = x;
		this.y = y;
		maze = new int[this.x][this.y];
		generateMaze(0, 0);
	}
 
	public void insertMaze (FlameRoom fr)
	{
		// Add poles.
		for (int i = 0; i < fr.ySectors; i = i + 4)
		{
			for (int j = 0; j < fr.ySectors; j = j + 4)
			{
				//  Post blocks.
				fr.changeTile(j, i, FlameTile.tankstone);
			}
		}
		
		for (int i = 0; i < this.y; i++) {
					
			// draw the north edge
			for (int j = 0; j < this.x; j++) {
				
				if ((this.maze[j][i] & 1) == 0)
				{
					
					// vertical walls
					fr.changeTile(j*4+1, i*4, 1);
					fr.changeTile(j*4+2, i*4, 1);
					fr.changeTile(j*4+3, i*4, 1);
				}
				
				if((this.maze[j][i] & 8) == 0)
				{
					
					// horizontal walls
					fr.changeTile(j*4, i*4+1, 1);
					fr.changeTile(j*4, i*4+2, 1);
					fr.changeTile(j*4, i*4+3, 1);

				}
			}
		}
				
		// draw the bottom line
		for (int j = 0; j < fr.xSectors; j++) {
			fr.changeTile(j, fr.ySectors-1, 1);
		}
		
		// draw the bottom line
		for (int j = 0; j < fr.ySectors; j++) {
			fr.changeTile(fr.xSectors-1, j, 1);
		}
	}
	
	public void display() {
		for (int i = 0; i < y; i++) {
			// draw the north edge
			for (int j = 0; j < x; j++) {
				System.out.print((maze[j][i] & 1) == 0 ? "+---" : "+   ");
			}
			System.out.println("+");
			// draw the west edge
			for (int j = 0; j < x; j++) {
				System.out.print((maze[j][i] & 8) == 0 ? "|   " : "    ");
			}
			System.out.println("|");
		}
		// draw the bottom line
		for (int j = 0; j < x; j++) {
			System.out.print("+---");
		}
		System.out.println("+");
	}
 
	private void generateMaze(int cx, int cy) {
		DIR[] dirs = DIR.values();
		Collections.shuffle(Arrays.asList(dirs));
		for (DIR dir : dirs) {
			int nx = cx + dir.dx;
			int ny = cy + dir.dy;
			if (between(nx, x) && between(ny, y)
					&& (maze[nx][ny] == 0)) {
				maze[cx][cy] |= dir.bit;
				maze[nx][ny] |= dir.opposite.bit;
				generateMaze(nx, ny);
			}
		}
	}
 
	private static boolean between(int v, int upper) {
		return (v >= 0) && (v < upper);
	}
 
	private enum DIR {
		N(1, 0, -1), S(2, 0, 1), E(4, 1, 0), W(8, -1, 0);
		private final int bit;
		private final int dx;
		private final int dy;
		private DIR opposite;
 
		// use the static initializer to resolve forward references
		static {
			N.opposite = S;
			S.opposite = N;
			E.opposite = W;
			W.opposite = E;
		}
 
		private DIR(int bit, int dx, int dy) {
			this.bit = bit;
			this.dx = dx;
			this.dy = dy;
		}
	};
 
	/*public static void main(String[] args) {
		int x = args.length >= 1 ? (Integer.parseInt(args[0])) : 8;
		int y = args.length == 2 ? (Integer.parseInt(args[1])) : 8;
		MazeGenerator maze = new MazeGenerator(x, y);
		maze.display();
	}*/
 
}
