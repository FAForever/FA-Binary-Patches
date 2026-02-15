package main;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import javax.swing.JPanel;
import javax.swing.JFrame;
import java.util.ArrayList;
import java.util.concurrent.ThreadLocalRandom;

class CircleRasterer extends JPanel implements MouseListener, MouseWheelListener, Runnable {
	static final int width = 100;
	static final int height = 60;
	static final int startTick = 0;
	static final Strategy testStrategy = Strategy.Exp2;
	static final Strategy moveStrategy = Strategy.ExpMove2;
	static final boolean reportDiscrepancies = true;
	static final boolean noMove = false; // useless when `testStrategy` doesn't have a mover
	static final boolean useTestGrid = true;
	static final boolean useMoveGrid = true    && (moveStrategy != null && ! noMove);
	static final boolean proof = true;
	static final int proofTicks = 1_000_000;
	static final int provers = 10;
	static final int proofs = 10_000;
	static final int proofCheckInInterval = 5_000;

	static final int cellSize = 10;
	static final int frameUpdateInterval = 5;
	static final boolean hasMouse = false    && (! proof);
	static final int startMouseX = 208, startMouseY = 104;
	static final double startMouseR = 10.0;
	static final boolean pauseOnDiscrepancy = true;
	static final boolean drawGrid = true;
	static final boolean drawBounds = false;
	static final boolean drawExact = true;
	static final boolean drawOverlap = true;
	static final Color[][] gridColors = {
		{new Color(0, 0, 255, 127), new Color(127, 127, 0, 127)},
		{new Color(0, 255, 0, 127), new Color(127, 0, 127, 127)},
		{new Color(255, 0, 0, 127), new Color(0, 127, 127, 127)}
	};
	static final Color boundColor = new Color(127, 127, 127, 127);
	static final int buttonSize = 60;

	static Thread provingThread;
	public static void main(String[] args) {
		if (! proof) {
			var p = new Parameter(0.0, 0.0, 1.0316283548683414,0.8546104211644724,0.8906900997810884);
			//var p = new Parameter(30.0, 20.0, 10.0, (1 + Math.sqrt(2)) / 2, (1 + Math.sqrt(5)) / 2);
			new CircleRasterer(p).start();
		} else {
			provingThread = new Thread(() -> {
				var c = new CircleRasterer[provers];
				for (int k = 0; k < c.length; ++k) {
					c[k] = newProof();
				}
				monitorThreads(c);
				for (int k = 0; k < c.length; ++k) {
					if (c[k] != null) {
						c[k].stop();
					}
				}
			});
			provingThread.start();
		}
	}
	
	static int totalProofs = 0;
	static CircleRasterer newProof() {
		if (totalProofs >= proofs) {
			return null;
		}
		double r, dx, dy;
		if (Math.random() < 0.5) {
			r = ThreadLocalRandom.current().nextDouble(100.0);
		} else {
			r = ThreadLocalRandom.current().nextInt(1, 100);
		}
		dx = ThreadLocalRandom.current().nextDouble(-2.0, 2.0);
		dy = ThreadLocalRandom.current().nextDouble(-2.0, 2.0);
		var p = new Parameter(0.0, 0.0, r, dx, dy);
		var c = new CircleRasterer(p, true);
		c.start();
		++totalProofs;
		return c;
	}
	
	static void monitorThreads(CircleRasterer[] c) {
		int[] lastTicks = new int[c.length];
		for (int k = 0; k < c.length; ++k) {
			lastTicks[k] = startTick - 1;
		}
		Outer:
		while (true) {
			long bedtime = System.currentTimeMillis();
			long amt = proofCheckInInterval;
			do {
				try {
					Thread.sleep(amt);
				} catch (InterruptedException ex) {
					for (int k = 0; k < c.length; ++k) {
						CircleRasterer r = c[k];
						if (r == null) {
							continue;
						}
						int tick = r.tick;
						if (tick == proofTicks) {
							r = newProof();
							if (r != null) {
								c[k] = r;
								lastTicks[k] = startTick - 1;
							}
							continue;
						}
					}
				}
				long curTime = System.currentTimeMillis();
				amt -= curTime - bedtime;
				bedtime = curTime;
			} while (amt > 10);
			int furthestTick = Integer.MIN_VALUE;
			int slowestTick = Integer.MAX_VALUE;
			int remaining = 0;
			for (int k = 0; k < c.length; ++k) {
				CircleRasterer r = c[k];
				if (r == null) {
					continue;
				}
				int tick = r.tick;
				if (tick == proofTicks) {
					r = newProof();
					if (r != null) {
						c[k] = r;
						lastTicks[k] = startTick - 1;
						++remaining;
					}
					continue;
				}
				if (lastTicks[k] == startTick - 1) {
					lastTicks[k] = startTick; // sometimes, the thread has just started. ignore it
				} else if (lastTicks[k] == tick) {
					System.err.println("Thread " + r + " stopped responding on tick " + tick + "!");
					break Outer;
				}
				lastTicks[k] = tick;
				if (tick < slowestTick) {
					slowestTick = tick;
				}
				if (tick > furthestTick) {
					furthestTick = tick;
				}
				++remaining;
			}
			String left;
			if (totalProofs == proofs) {
				left = " (" + remaining + " remaining)";
			} else {
				left = " (" + (proofs - totalProofs) + " left)";
			}
			println(formatInt(slowestTick) + " to " + formatInt(furthestTick) + left);
			if (remaining == 0) {
				break;
			}
		}
	}

	static String formatTime(long t) {
		StringBuilder time = new StringBuilder(Long.toString(t % 60_000));
		if (time.length() < 5) {
			time.insert(0, "0000".substring(time.length() - 1));
		}
		time.insert(time.length() - 3, ".");
		int min = (int) ((t / 60_000) % 60);
		time.insert(0, min + ":");
		if (min < 10) {
			time.insert(0, "0");
		}
		time.insert(0, (t / 3_600_000) + ":");
		return time.toString();
	}
	static String formatInt(int v) {
		StringBuilder b = new StringBuilder(Integer.toString(v));
		for (int k = b.length() - 3; k > 0; k -= 3) {
			b.insert(k, "'");
		}
		return b.toString();
	}
	static final long startTime = System.currentTimeMillis();
	static void print(String msg) {
		long t = System.currentTimeMillis() - startTime;
		System.out.print(formatTime(t) + ": " + msg);
	}
	static void println(String msg) {
		print(msg + "\n");
	}


	static void rasterCircle(Grid g, Circle c, boolean dir) {
		double r2 = c.r*c.r;
		Bound b = c.bounds(g);
		for (int j = b.y0; j < b.y1; ++j) {
			double dy = j - c.y;
			double cmp = r2 - dy*dy;
			for (int i = b.x0; i < b.x1; ++i) {
				double dx = i - c.x;
				if (dx*dx <= cmp) {
					g.chng(i, j, dir);
				}
			}
		}
	}
	static void moveRasterCircle(Grid g, Circle p, Circle c) {
		double pr2 = p.r*p.r;
		double cr2 = c.r*c.r;
		Bound b = p.bounds(g).union(c.bounds(g));
		for (int j = b.y0; j < b.y1; ++j) {
			double pdy = j - p.y;
			double cdy = j - c.y;
			double pcmp = pr2 - pdy*pdy;
			double ccmp = cr2 - cdy*cdy;
			for (int i = b.x0; i < b.x1; ++i) {
				double pdx = i - p.x;
				double cdx = i - c.x;
				boolean isIn = cdx*cdx <= ccmp;
				if ((pdx*pdx <= pcmp) != isIn) {
					g.chng(i, j, isIn);
				}
			}
		}
	}

	static void rasterHalfCircleExp0(Grid g, Circle c, boolean dir, boolean upper) {
		Bound b = c.bounds(g);
		int x0 = b.x0;
		int x1 = b.x1 - 1;
		int y = (int) c.y + (upper ? 0 : 1);
		double dy = c.y - y;
		double cmp = c.r*c.r - dy*dy;
		Outer:
		while (upper ? (y >= b.y0) : (y < b.y1)) {
			double dx = c.x - x0;
			while (dx*dx > cmp) {
				dx = c.x - ++x0;
			}
			dx = x1 - c.x;
			while (dx*dx > cmp) {
				dx = --x1 - c.x;
			}
			for (int i = x0; i <= x1; ++i) {
				g.chng(i, y, dir);
			}
			y += upper ? -1 : 1;
			dy = c.y - y;
			cmp = c.r*c.r - dy*dy;
			if (cmp < 1) {
				if (cmp < 0) {
					return;
				}
				while (dx > 0) {
					if (dx*dx <= cmp) {
						continue Outer;
					}
					dx = --x1 - c.x;
				}
				if (dx*dx <= cmp) {
					continue Outer;
				}
				return;
			}
		}
	}
	static void rasterCircleExp0(Grid g, Circle c, boolean dir) {
		rasterHalfCircleExp0(g, c, dir, true);
		rasterHalfCircleExp0(g, c, dir, false);
	}
	
	static void rasterHalfCircleExp1(Grid g, Circle c, boolean dir, int y0, int y1, boolean upper) {
		double r2 = c.r * c.r;

		Bound b = c.bounds(g);
		Line mid = new Line(b.x0, b.x1 - 1, y0);
		Line top = new Line((int) Math.floor(c.x), (int) Math.ceil(c.x), y1);
		double dx = c.x - top.l;
		double dy = c.y - top.y;
		double cmp = r2 - dy*dy;
		if (dx*dx > cmp) {
			dx = c.x - top.r;
			if (dx*dx > cmp) {
				top.y += upper ? 1 : -1;
			} else {
				top.l = top.r;
			}
		} else {
			dx = c.x - top.r;
			if (dx*dx > cmp) {
				top.r = top.l;
			}
		}
		if (upper) {
			if (mid.y < top.y) {
				return;
			}
		} else {
			if (mid.y > top.y) {
				return;
			}
		}
		while (true) {
			dy = c.y - mid.y;
			dx = c.x - mid.l;
			if (dx*dx + dy*dy > r2) {
				++mid.l;
			}
			dx = mid.r - c.x;
			if (dx*dx + dy*dy > r2) {
				--mid.r;
			}

			dx = c.x - (top.l - 1);
			while (dx*dx + dy*dy <= r2) {
				dx = c.x - (--top.l - 1);
			}
			dx = top.r + 1 - c.x;
			while (dx*dx + dy*dy <= r2) {
				dx = ++top.r + 1 - c.x;
			}

			for (int i = mid.l; i <= mid.r; ++i) {
				g.chng(i, mid.y, dir);
			}
			if (mid.y == top.y) {
				break;
			}
			mid.y += upper ? -1 : 1;
			for (int i = Math.max(top.l, 0); i <= Math.min(top.r, g.width - 1); ++i) {
				g.chng(i, top.y, dir);
			}
			if (top.y == mid.y) {
				break;
			}
			top.y += upper ? 1 : -1;
		}
	}
	static void rasterCircleExp1(Grid g, Circle c, boolean dir) {
		Bound b = c.bounds(g);
		rasterHalfCircleExp1(g, c, dir, (int) c.y, b.y0, true);
		rasterHalfCircleExp1(g, c, dir, (int) c.y + 1, b.y1 - 1, false);
	}
	
	static void rasterCircleExp2(Grid g, Circle c, boolean dir) {
		if (c.r < 2.0) {
			rasterCircle(g, c, dir);
			return;
		}
		double r2 = c.r*c.r;
		Bound b = c.bounds(g);
		Line l = new Line((int) Math.floor(c.x), (int) Math.ceil(c.x), b.y0);
		boolean out = c.y > l.y + 0.5;
		double dx = c.x - l.l;
		double dy = c.y - l.y;
		double cmp = r2 - dy*dy;
		if (dx*dx > cmp) {
			dx = c.x - l.r;
			if (dx*dx > cmp) {
				dy = c.y - ++l.y;
				cmp = r2 - dy*dy;
			} else {
				l.l = l.r;
			}
		} else {
			dx = c.x - l.r;
			if (dx*dx > cmp) {
				l.r = l.l;
			}
		}
		if (cmp < 0) {
			return;
		}
		if (! out) {
			dx = c.x - (l.l - 1);
			while (dx*dx <= cmp) {
				dx = c.x - (--l.l - 1);
			}
			dx = l.r + 1 - c.x;
			while (dx*dx <= cmp) {
				dx = ++l.r + 1 - c.x;
			}
		}
		Outer:
		while (true) {
			if (out) {
				dx = c.x - (l.l - 1);
				while (dx*dx <= cmp) {
					dx = c.x - (--l.l - 1);
				}
				dx = l.r + 1 - c.x;
				while (dx*dx <= cmp) {
					dx = ++l.r + 1 - c.x;
				}
			} else {
				dx = c.x - l.l;
				while (dx*dx > cmp) {
					dx = c.x - ++l.l;
				}
				dx = l.r - c.x;
				while (dx*dx > cmp) {
					dx = --l.r - c.x;
				}
			}
			if (l.y >= g.height) {
				return;
			}
			for (int i = Math.max(l.l, 0); i <= Math.min(l.r, g.width - 1); ++i) {
				g.chng(i, l.y, dir);
			}
			dy = c.y - ++l.y;
			cmp = r2 - dy*dy;
			if (out) {
				if (dy < 0 && dx*dx > cmp) {
					dx = c.x - (l.l - 1);
					if (dx*dx > cmp) {
						out = false;
					}
				}
			} else {
				if (cmp < 1) {
					if (cmp < 0) {
						return;
					}
					while (dx > 0) {
						if (dx*dx <= cmp) {
							continue Outer;
						}
						dx = --l.r - c.x;
					}
					if (dx*dx <= cmp) {
						continue Outer;
					}
					return;
				}
			}
		}
	}
	enum LineState { Early, Out, In, Done }
	static void moveRasterCircleExp2(Grid g, Circle p, Circle c) {
		boolean useDef = false;
		if (c.r < 2.0) {
			if (p.r < 2.0) {
				moveRasterCircle(g, p, c);
				return;
			} else {
				useDef = true;
			}
		} else if (p.r < 2.0) {
			useDef = true;
		} else if (p.r != c.r) {
			useDef = true;
		} else {
			double dx = p.x - c.x, dy = p.y - c.y;
			if (dx*dx + dy*dy > (p.r + c.r) * 0.5) {
				useDef = true;
			}
		}
		if (useDef) {
			rasterCircleExp2(g, p, false);
			rasterCircleExp2(g, c, true);
			return;
		}
		double r2 = c.r*c.r;

		Bound cb = c.bounds(g);
		Line cl = new Line((int) Math.floor(c.x), (int) Math.ceil(c.x), cb.y0);
		LineState cout = c.y > cl.y + 0.5 ? LineState.Out : LineState.In;
		double cdx = c.x - cl.l;
		double cdy = c.y - cl.y;
		double ccmp = r2 - cdy*cdy;
		if (cdx*cdx > ccmp) {
			cdx = c.x - cl.r;
			if (cdx*cdx > ccmp) {
				cdy = c.y - ++cl.y;
				ccmp = r2 - cdy*cdy;
			} else {
				cl.l = cl.r;
			}
		} else {
			cdx = c.x - cl.r;
			if (cdx*cdx > ccmp) {
				cl.r = cl.l;
			}
		}
		if (ccmp < 0) {
			cout = LineState.Done;
		}
		if (cout == LineState.In) {
			cdx = c.x - (cl.l - 1);
			while (cdx*cdx <= ccmp) {
				cdx = c.x - (--cl.l - 1);
			}
			cdx = cl.r + 1 - c.x;
			while (cdx*cdx <= ccmp) {
				cdx = ++cl.r + 1 - c.x;
			}
		}

		Bound pb = p.bounds(g);
		Line pl = new Line((int) Math.floor(p.x), (int) Math.ceil(p.x), pb.y0);
		LineState pout = p.y > pl.y + 0.5 ? LineState.Out : LineState.In;
		double pdx = p.x - pl.l;
		double pdy = p.y - pl.y;
		double pcmp = r2 - pdy*pdy;
		if (pdx*pdx > pcmp) {
			pdx = p.x - pl.r;
			if (pdx*pdx > pcmp) {
				pdy = p.y - ++pl.y;
				pcmp = r2 - pdy*pdy;
			} else {
				pl.l = pl.r;
			}
		} else {
			pdx = p.x - pl.r;
			if (pdx*pdx > pcmp) {
				pl.r = pl.l;
			}
		}
		if (pcmp < 0) {
			pout = LineState.Done;
		}
		if (pout == LineState.In) {
			pdx = p.x - (pl.l - 1);
			while (pdx*pdx <= pcmp) {
				pdx = p.x - (--pl.l - 1);
			}
			pdx = pl.r + 1 - p.x;
			while (pdx*pdx <= pcmp) {
				pdx = ++pl.r + 1 - p.x;
			}
		}
		if (cl.y < pl.y) {
			if (pout != LineState.Done) {
				pout = LineState.Early;
			}
		} else if (cl.y > pl.y) {
			if (cout != LineState.Done) {
				cout = LineState.Early;
			}
		}
	
		while (cout != LineState.Done || pout != LineState.Done) {
			if (cout == LineState.Out) {
				cdx = c.x - (cl.l - 1);
				while (cdx*cdx <= ccmp) {
					cdx = c.x - (--cl.l - 1);
				}
				cdx = cl.r + 1 - c.x;
				while (cdx*cdx <= ccmp) {
					cdx = ++cl.r + 1 - c.x;
				}
			} else if (cout == LineState.In) {
				cdx = c.x - cl.l;
				while (cdx*cdx > ccmp) {
					cdx = c.x - ++cl.l;
				}
				cdx = cl.r - c.x;
				while (cdx*cdx > ccmp) {
					cdx = --cl.r - c.x;
				}
			}
			if (pout == LineState.Out) {
				pdx = p.x - (pl.l - 1);
				while (pdx*pdx <= pcmp) {
					pdx = p.x - (--pl.l - 1);
				}
				pdx = pl.r + 1 - p.x;
				while (pdx*pdx <= pcmp) {
					pdx = ++pl.r + 1 - p.x;
				}
			} else if (pout == LineState.In) {
				pdx = p.x - pl.l;
				while (pdx*pdx > pcmp) {
					pdx = p.x - ++pl.l;
				}
				pdx = pl.r - p.x;
				while (pdx*pdx > pcmp) {
					pdx = --pl.r - p.x;
				}
			}
			
			if ((cl.y >= g.height || cout == LineState.Done) && (pl.y >= g.height || pout == LineState.Done)) {
				return;
			}
			
			if (cout == LineState.Early || cout == LineState.Done) {
				for (int i = Math.max(pl.l, 0); i <= Math.min(pl.r, g.width - 1); ++i) {
					g.chng(i, pl.y, false);
				}
			} else if (pout == LineState.Early || pout == LineState.Done) {
				for (int i = Math.max(cl.l, 0); i <= Math.min(cl.r, g.width - 1); ++i) {
					g.chng(i, cl.y, true);
				}
			} else {
				if (cl.l < pl.l) {
					for (int i = Math.max(cl.l, 0); i < Math.min(pl.l, g.width); ++i) {
						g.chng(i, cl.y, true);
					}
				} else {
					for (int i = Math.max(pl.l, 0); i < Math.min(cl.l, g.width); ++i) {
						g.chng(i, cl.y, false);
					}
				}
				if (cl.r < pl.r) {
					for (int i = Math.max(cl.r + 1, 0); i <= Math.min(pl.r, g.width - 1); ++i) {
						g.chng(i, cl.y, false);
					}
				} else {
					for (int i = Math.max(pl.r + 1, 0); i <= Math.min(cl.r, g.width - 1); ++i) {
						g.chng(i, cl.y, true);
					}
				}
			}
			if (cout == LineState.Early) {
				if (pl.y + 1 == cl.y) {
					cout = LineState.Out;
				}
			} else if (cout != LineState.Done) {
				cdy = c.y - ++cl.y;
				ccmp = r2 - cdy*cdy;
				if (cout == LineState.Out) {
					if (cdy < 0 && cdx*cdx > ccmp) {
						cdx = c.x - cl.l + 1;
						if (cdx*cdx > ccmp) {
							cout = LineState.In;
						}
					}
				} else if (ccmp < 1) {
					if (ccmp < 0) {
						cout = LineState.Done;
					} else {
						boolean found = false;
						while (cdx > 0) {
							if (cdx*cdx <= ccmp) {
								found = true;
								break;
							}
							cdx = --cl.r - c.x;
						}
						if (cdx*cdx <= ccmp) {
							found = true;
						}
						if (! found) {
							cout = LineState.Done;
						}
					}
				}
			}

			if (pout == LineState.Early) {
				if (pl.y == cl.y) {
					pout = LineState.Out;
				}
			} else if (pout != LineState.Done) {
				pdy = p.y - ++pl.y;
				pcmp = r2 - pdy*pdy;
				if (pout == LineState.Out) {
					if (pdy < 0 && pdx*pdx > pcmp) {
						pdx = p.x - pl.l + 1;
						if (pdx*pdx > pcmp) {
							pout = LineState.In;
						}
					}
				} else if (pcmp < 1) {
					if (pcmp < 0) {
						pout = LineState.Done;
					} else {
						boolean found = false;
						while (pdx > 0) {
							if (pdx*pdx <= pcmp) {
								found = true;
								break;
							}
							pdx = --pl.r - p.x;
						}
						if (pdx*pdx <= pcmp) {
							found = true;
						}
						if (! found) {
							pout = LineState.Done;
						}
					}
				}
			}
		}
	}
	
	static void rasterCircleExp3(Grid g, Circle c, boolean dir) {
		double r2 = c.r*c.r;
		
		Bound b = c.bounds(g);
		Line l = new Line((int) Math.floor(c.x), (int) Math.ceil(c.x), b.y0);
		double dx = c.x - l.l;
		double dy = c.y - l.y;
		double cmp = r2 - dy*dy;
		if (dx*dx > cmp) {
			dx = c.x - l.r;
			if (dx*dx > cmp) {
				dy = c.y - ++l.y;
				cmp = r2 - dy*dy;
			} else {
				l.l = l.r;
			}
		} else {
			dx = c.x - l.r;
			if (dx*dx > cmp) {
				l.r = l.l;
			}
		}
		if (cmp < 0) {
			return;
		}

		int endy = b.y1 - 1;
		dx = c.x - (int) Math.floor(c.x);
		dy = endy - c.y;
		double cmp2 = r2 - dy*dy;
		if (dx*dx > cmp2) {
			dx = c.x - (int) Math.ceil(c.x);
			if (dx*dx > cmp2) {
				--endy;
			}
		}
		while (true) {
			if (l.y >= g.height) {
				return;
			}

			dx = c.x - (l.l - 1);
			while (dx*dx <= cmp) {
				dx = c.x - (--l.l - 1);
			}
			dx = l.r + 1 - c.x;
			while (dx*dx <= cmp) {
				dx = ++l.r + 1 - c.x;
			}

			for (int i = Math.max(l.l, 0); i <= Math.min(l.r, g.width - 1); ++i) {
				g.chng(i, l.y, dir);
			}

			dy = c.y - ++l.y;
			cmp = r2 - dy*dy;
			if (dy < 0 && dx*dx > cmp) {
				dx = c.x - (l.l - 1);
				if (dx*dx > cmp) {
					if (l.y > endy) {
						return;
					}
					break;
				}
			}
		}
		while (true) {
			if (l.y >= g.height) {
				return;
			}

			dx = c.x - l.l;
			while (dx*dx > cmp) {
				dx = c.x - ++l.l;
			}
			dx = l.r - c.x;
			while (dx*dx > cmp) {
				dx = --l.r - c.x;
			}

			for (int i = Math.max(l.l, 0); i <= Math.min(l.r, g.width - 1); ++i) {
				g.chng(i, l.y, dir);
			}

			if (l.y == endy) {
				break;
			}
			dy = c.y - ++l.y;
			cmp = r2 - dy*dy;
		}
	}
	static void moveRasterCircleExp3(Grid g, Circle p, Circle c) {
		boolean useDef = false;
		if (p.r != c.r) {
			useDef = true;
		}
		double dx = p.x - c.x, dy = p.y - c.y;
		if (dx*dx + dy*dy > (p.r + c.r) * 0.5) {
			useDef = true;
		}
		if (useDef) {
			rasterCircleExp3(g, p, false);
			rasterCircleExp3(g, c, true);
			return;
		}

		double r2 = c.r*c.r;
		
		Bound pb = p.bounds(g);
		Line pl = new Line((int) Math.floor(p.x), (int) Math.ceil(p.x), pb.y0);
		double pdx = p.x - pl.l;
		double pdy = p.y - pl.y;
		double pcmp = r2 - pdy*pdy;
		if (pdx*pdx > pcmp) {
			pdx = p.x - pl.r;
			if (pdx*pdx > pcmp) {
				pdy = p.y - ++pl.y;
				pcmp = r2 - pdy*pdy;
			} else {
				pl.l = pl.r;
			}
		} else {
			pdx = p.x - pl.r;
			if (pdx*pdx > pcmp) {
				pl.r = pl.l;
			}
		}
		if (pcmp < 0) {
			return;
		}

		int pendy = pb.y1 - 1;
		pdx = p.x - (int) Math.floor(p.x);
		pdy = pendy - p.y;
		double pcmp2 = r2 - pdy*pdy;
		if (pdx*pdx > pcmp2) {
			pdx = p.x - (int) Math.ceil(p.x);
			if (pdx*pdx > pcmp2) {
				--pendy;
			}
		}
		

		
		Bound cb = c.bounds(g);
		Line cl = new Line((int) Math.floor(c.x), (int) Math.ceil(c.x), cb.y0);
		double cdx = c.x - cl.l;
		double cdy = c.y - cl.y;
		double ccmp = r2 - cdy*cdy;
		if (cdx*cdx > ccmp) {
			cdx = c.x - cl.r;
			if (cdx*cdx > ccmp) {
				cdy = c.y - ++cl.y;
				ccmp = r2 - cdy*cdy;
			} else {
				cl.l = cl.r;
			}
		} else {
			cdx = c.x - cl.r;
			if (cdx*cdx > ccmp) {
				cl.r = cl.l;
			}
		}
		if (ccmp < 0) {
			return;
		}

		int cendy = cb.y1 - 1;
		cdx = c.x - (int) Math.floor(c.x);
		cdy = cendy - c.y;
		double ccmp2 = r2 - cdy*cdy;
		if (cdx*cdx > ccmp2) {
			cdx = c.x - (int) Math.ceil(c.x);
			if (cdx*cdx > ccmp2) {
				--cendy;
			}
		}


		while (cl.y < pl.y) {
			if (cl.y >= g.height) {
				return;
			}

			cdx = c.x - (cl.l - 1);
			while (cdx*cdx <= ccmp) {
				cdx = c.x - (--cl.l - 1);
			}
			cdx = cl.r + 1 - c.x;
			while (cdx*cdx <= ccmp) {
				cdx = ++cl.r + 1 - c.x;
			}

			for (int i = Math.max(cl.l, 0); i <= Math.min(cl.r, g.width - 1); ++i) {
				g.chng(i, cl.y, true);
			}

			cdy = c.y - ++cl.y;
			ccmp = r2 - cdy*cdy;
		}
		while (pl.y < cl.y) {
			if (pl.y >= g.height) {
				return;
			}

			pdx = p.x - (pl.l - 1);
			while (pdx*pdx <= pcmp) {
				pdx = p.x - (--pl.l - 1);
			}
			pdx = pl.r + 1 - p.x;
			while (pdx*pdx <= pcmp) {
				pdx = ++pl.r + 1 - p.x;
			}

			for (int i = Math.max(pl.l, 0); i <= Math.min(pl.r, g.width - 1); ++i) {
				g.chng(i, pl.y, false);
			}

			pdy = p.y - ++pl.y;
			pcmp = r2 - pdy*pdy;
		}
		
		
		if (c.y > p.y) {
			if (cl.y > cendy) {
				return;
			}

			while (true) {
				if (pl.y >= g.height) {
					return;
				}

				pdx = p.x - (pl.l - 1);
				while (pdx*pdx <= pcmp) {
					pdx = p.x - (--pl.l - 1);
				}
				pdx = pl.r + 1 - p.x;
				while (pdx*pdx <= pcmp) {
					pdx = ++pl.r + 1 - p.x;
				}

				cdx = c.x - (cl.l - 1);
				while (cdx*cdx <= ccmp) {
					cdx = c.x - (--cl.l - 1);
				}
				cdx = cl.r + 1 - c.x;
				while (cdx*cdx <= ccmp) {
					cdx = ++cl.r + 1 - c.x;
				}

				if (pl.l < cl.l) {
					for (int i = Math.max(pl.l, 0); i < Math.min(cl.l, g.width); ++i) {
						g.chng(i, pl.y, false);
					}
				} else {
					for (int i = Math.max(cl.l, 0); i < Math.min(pl.l, g.width); ++i) {
						g.chng(i, pl.y, true);
					}
				}
				if (pl.r < cl.r) {
					for (int i = Math.max(pl.r + 1, 0); i <= Math.min(cl.r, g.width - 1); ++i) {
						g.chng(i, pl.y, true);
					}
				} else {
					for (int i = Math.max(cl.r + 1, 0); i <= Math.min(pl.r, g.width - 1); ++i) {
						g.chng(i, pl.y, false);
					}
				}

				cdy = c.y - ++cl.y;
				ccmp = r2 - cdy*cdy;

				pdy = p.y - ++pl.y;
				pcmp = r2 - pdy*pdy;
				if (pdy < 0 && pdx*pdx > pcmp) {
					pdx = p.x - (pl.l - 1);
					if (pdx*pdx > pcmp) {
						break;
					}
				}

			}
			
			while (true) {
				if (pl.y >= g.height) {
					return;
				}
	
				pdx = p.x - pl.l;
				while (pdx*pdx > pcmp) {
					pdx = p.x - ++pl.l;
				}
				pdx = pl.r - p.x;
				while (pdx*pdx > pcmp) {
					pdx = --pl.r - p.x;
				}

				cdx = c.x - (cl.l - 1);
				while (cdx*cdx <= ccmp) {
					cdx = c.x - (--cl.l - 1);
				}
				cdx = cl.r + 1 - c.x;
				while (cdx*cdx <= ccmp) {
					cdx = ++cl.r + 1 - c.x;
				}

				if (pl.l < cl.l) {
					for (int i = Math.max(pl.l, 0); i < Math.min(cl.l, g.width); ++i) {
						g.chng(i, pl.y, false);
					}
				} else {
					for (int i = Math.max(cl.l, 0); i < Math.min(pl.l, g.width); ++i) {
						g.chng(i, pl.y, true);
					}
				}
				if (pl.r < cl.r) {
					for (int i = Math.max(pl.r + 1, 0); i <= Math.min(cl.r, g.width - 1); ++i) {
						g.chng(i, pl.y, true);
					}
				} else {
					for (int i = Math.max(cl.r + 1, 0); i <= Math.min(pl.r, g.width - 1); ++i) {
						g.chng(i, pl.y, false);
					}
				}
	
				pdy = p.y - ++pl.y;
				pcmp = r2 - pdy*pdy;

				cdy = c.y - ++cl.y;
				ccmp = r2 - cdy*cdy;
				if (cdy < 0 && cdx*cdx > ccmp) {
					cdx = c.x - (cl.l - 1);
					if (cdx*cdx > ccmp) {
						break;
					}
				}
			}

			while (true) {
				if (pl.y >= g.height) {
					return;
				}
	
				pdx = p.x - pl.l;
				while (pdx*pdx > pcmp) {
					pdx = p.x - ++pl.l;
				}
				pdx = pl.r - p.x;
				while (pdx*pdx > pcmp) {
					pdx = --pl.r - p.x;
				}

				cdx = c.x - cl.l;
				while (cdx*cdx > ccmp) {
					cdx = c.x - ++cl.l;
				}
				cdx = cl.r - c.x;
				while (cdx*cdx > ccmp) {
					cdx = --cl.r - c.x;
				}
	

				if (pl.l < cl.l) {
					for (int i = Math.max(pl.l, 0); i < Math.min(cl.l, g.width); ++i) {
						g.chng(i, pl.y, false);
					}
				} else {
					for (int i = Math.max(cl.l, 0); i < Math.min(pl.l, g.width); ++i) {
						g.chng(i, pl.y, true);
					}
				}
				if (pl.r < cl.r) {
					for (int i = Math.max(pl.r + 1, 0); i <= Math.min(cl.r, g.width - 1); ++i) {
						g.chng(i, pl.y, true);
					}
				} else {
					for (int i = Math.max(cl.r + 1, 0); i <= Math.min(pl.r, g.width - 1); ++i) {
						g.chng(i, pl.y, false);
					}
				}
	
				pdy = p.y - ++pl.y;
				pcmp = r2 - pdy*pdy;

				cdy = c.y - ++cl.y;
				ccmp = r2 - cdy*cdy;
				if (pl.y > pendy) {
					break;
				}
			}
			
			while (true) {
				if (cl.y >= g.height) {
					return;
				}
	
				cdx = c.x - cl.l;
				while (cdx*cdx > ccmp) {
					cdx = c.x - ++cl.l;
				}
				cdx = cl.r - c.x;
				while (cdx*cdx > ccmp) {
					cdx = --cl.r - c.x;
				}
	
				for (int i = Math.max(cl.l, 0); i <= Math.min(cl.r, g.width - 1); ++i) {
					g.chng(i, cl.y, true);
				}
	
				if (cl.y == cendy) {
					break;
				}
				cdy = c.y - ++cl.y;
				ccmp = r2 - cdy*cdy;
			}

		} else {//if (pendy > cendy) {
			if (pl.y > pendy) {
				return;
			}

			while (true) {
				if (cl.y >= g.height) {
					return;
				}

				cdx = c.x - (cl.l - 1);
				while (cdx*cdx <= ccmp) {
					cdx = c.x - (--cl.l - 1);
				}
				cdx = cl.r + 1 - c.x;
				while (cdx*cdx <= ccmp) {
					cdx = ++cl.r + 1 - c.x;
				}

				pdx = p.x - (pl.l - 1);
				while (pdx*pdx <= pcmp) {
					pdx = p.x - (--pl.l - 1);
				}
				pdx = pl.r + 1 - p.x;
				while (pdx*pdx <= pcmp) {
					pdx = ++pl.r + 1 - p.x;
				}

				if (pl.l < cl.l) {
					for (int i = Math.max(pl.l, 0); i < Math.min(cl.l, g.width); ++i) {
						g.chng(i, pl.y, false);
					}
				} else {
					for (int i = Math.max(cl.l, 0); i < Math.min(pl.l, g.width); ++i) {
						g.chng(i, pl.y, true);
					}
				}
				if (pl.r < cl.r) {
					for (int i = Math.max(pl.r + 1, 0); i <= Math.min(cl.r, g.width - 1); ++i) {
						g.chng(i, pl.y, true);
					}
				} else {
					for (int i = Math.max(cl.r + 1, 0); i <= Math.min(pl.r, g.width - 1); ++i) {
						g.chng(i, pl.y, false);
					}
				}

				pdy = p.y - ++pl.y;
				pcmp = r2 - pdy*pdy;

				cdy = c.y - ++cl.y;
				ccmp = r2 - cdy*cdy;
				if (cdy < 0 && cdx*cdx > ccmp) {
					cdx = c.x - (cl.l - 1);
					if (cdx*cdx > ccmp) {
						break;
					}
				}

			}
			

			while (true) {
				if (cl.y >= g.height) {
					return;
				}
	
				cdx = c.x - cl.l;
				while (cdx*cdx > ccmp) {
					cdx = c.x - ++cl.l;
				}
				cdx = cl.r - c.x;
				while (cdx*cdx > ccmp) {
					cdx = --cl.r - c.x;
				}

				pdx = p.x - (pl.l - 1);
				while (pdx*pdx <= pcmp) {
					pdx = p.x - (--pl.l - 1);
				}
				pdx = pl.r + 1 - p.x;
				while (pdx*pdx <= pcmp) {
					pdx = ++pl.r + 1 - p.x;
				}

				if (pl.l < cl.l) {
					for (int i = Math.max(pl.l, 0); i < Math.min(cl.l, g.width); ++i) {
						g.chng(i, pl.y, false);
					}
				} else {
					for (int i = Math.max(cl.l, 0); i < Math.min(pl.l, g.width); ++i) {
						g.chng(i, pl.y, true);
					}
				}
				if (pl.r < cl.r) {
					for (int i = Math.max(pl.r + 1, 0); i <= Math.min(cl.r, g.width - 1); ++i) {
						g.chng(i, pl.y, true);
					}
				} else {
					for (int i = Math.max(cl.r + 1, 0); i <= Math.min(pl.r, g.width - 1); ++i) {
						g.chng(i, pl.y, false);
					}
				}
	
				cdy = c.y - ++cl.y;
				ccmp = r2 - cdy*cdy;

				pdy = p.y - ++pl.y;
				pcmp = r2 - pdy*pdy;
				if (pdy < 0 && pdx*pdx > pcmp) {
					pdx = p.x - (pl.l - 1);
					if (pdx*pdx > pcmp) {
						break;
					}
				}
			}

			while (true) {
				if (cl.y >= g.height) {
					return;
				}
	
				cdx = c.x - cl.l;
				while (cdx*cdx > ccmp) {
					cdx = c.x - ++cl.l;
				}
				cdx = cl.r - c.x;
				while (cdx*cdx > ccmp) {
					cdx = --cl.r - c.x;
				}

				pdx = p.x - pl.l;
				while (pdx*pdx > pcmp) {
					pdx = p.x - ++pl.l;
				}
				pdx = pl.r - p.x;
				while (pdx*pdx > pcmp) {
					pdx = --pl.r - p.x;
				}
	

				if (pl.l < cl.l) {
					for (int i = Math.max(pl.l, 0); i < Math.min(cl.l, g.width); ++i) {
						g.chng(i, pl.y, false);
					}
				} else {
					for (int i = Math.max(cl.l, 0); i < Math.min(pl.l, g.width); ++i) {
						g.chng(i, pl.y, true);
					}
				}
				if (pl.r < cl.r) {
					for (int i = Math.max(pl.r + 1, 0); i <= Math.min(cl.r, g.width - 1); ++i) {
						g.chng(i, pl.y, true);
					}
				} else {
					for (int i = Math.max(cl.r + 1, 0); i <= Math.min(pl.r, g.width - 1); ++i) {
						g.chng(i, pl.y, false);
					}
				}
	
				cdy = c.y - ++cl.y;
				ccmp = r2 - cdy*cdy;

				pdy = p.y - ++pl.y;
				pcmp = r2 - pdy*pdy;
				if (cl.y > cendy) {
					break;
				}
			}
			
			while (true) {
				if (pl.y >= g.height) {
					return;
				}
	
				pdx = p.x - pl.l;
				while (pdx*pdx > pcmp) {
					pdx = p.x - ++pl.l;
				}
				pdx = pl.r - p.x;
				while (pdx*pdx > pcmp) {
					pdx = --pl.r - p.x;
				}
	
				for (int i = Math.max(pl.l, 0); i <= Math.min(pl.r, g.width - 1); ++i) {
					g.chng(i, pl.y, false);
				}
	
				if (pl.y == pendy) {
					break;
				}
				pdy = p.y - ++pl.y;
				pcmp = r2 - pdy*pdy;
			}
		}
	}
	
	enum State {
		Pause,
		Running,
		Step,
	}

	enum Showing {
		All(true, true, true),
		TestMove(false, true, true),
		Move(false, false, true),
		RefMove(true, false, true),
		Ref(true, false, false),
		RefTest(true, true, false),
		Test(false, true, false),
		;
		final boolean ref;
		final boolean test;
		final boolean move;
		Showing(boolean ref, boolean test, boolean move) {
			this.ref = ref;
			this.test = useTestGrid && test;
			this.move = useMoveGrid && move;
		}
	}

	enum Strategy implements Raster, Move {
		Ref(CircleRasterer::rasterCircle),
		Exp0(CircleRasterer::rasterCircleExp0),
		Exp1(CircleRasterer::rasterCircleExp1),
		Exp2(CircleRasterer::rasterCircleExp2),
		ExpMove2(CircleRasterer::rasterCircleExp2, CircleRasterer::moveRasterCircleExp2),
		Exp3(CircleRasterer::rasterCircleExp3),
		ExpMove3(CircleRasterer::rasterCircleExp3, CircleRasterer::moveRasterCircleExp3),
		;
		final Raster r;
		final Move m;

		<T extends Raster, U extends Move>Strategy(T r, U m) {
			this.r = r;
			this.m = noMove ? null : m;
		}
		<T extends Raster>Strategy(T r) {
			this(r, null);
		}
		@Override
		public void raster(Grid g, Circle c, boolean dir) {
			r.raster(g, c, dir);
		}
		@Override
		public void move(Grid g, Circle p, Circle c) {
			g.moveInBounds(c);
			if (m == null) {
				r.raster(g, p, false);
				r.raster(g, c, true);
			} else {
				m.move(g, p, c);
			}
		}
	};
	
	interface Raster {
		abstract void raster(Grid g, Circle c, boolean dir);
	}
	interface Move {
		abstract void move(Grid g, Circle p, Circle c);
	}

	static class Parameter {
		double x, y, r, dx, dy;
		
		Parameter(double x, double y, double r, double dx, double dy) {
			this.x = x;
			this.y = y;
			this.r = r;
			this.dx = dx;
			this.dy = dy;
		}
		Parameter(double dx, double dy) {
			this(0.0, 0.0, 10.0, dx, dy);
		}
		@Override
		public String toString() {
			return "(" + x + "," + y + " " + r + "+" + dx + "," + dy + ")";
		}
	}
	static class Env {
		Graphics2D g;
		int x, y, s;
	
		Env(Graphics2D g, int x, int y, int s) {
			this.g = g;
			this.x = x;
			this.y = y;
			this.s = s;
		}
		int x(double x) { return (int) (this.x + x * s); }
		int y(double y) { return (int) (this.y + y * s); }
		int w(double w) { return (int) (w * s); }
		int h(double h) { return (int) (h * s); }
	}

	static class Bound {
		int x0, x1, y0, y1;
	
		Bound(int x0, int x1, int y0, int y1) {
			this.x0 = x0;
			this.x1 = x1;
			this.y0 = y0;
			this.y1 = y1;
		}
		Bound union(Bound that) {
			return new Bound(
				Math.min(this.x0, that.x0),
				Math.max(this.x1, that.x1),
				Math.min(this.y0, that.y0),
				Math.max(this.y1, that.y1)
			);
		}
	}
	static class Circle {
		double x, y, r;
		
		Circle(Circle c) {
			this(c.x, c.y, c.r);
		}
		Circle(double x, double y, double r) {
			this.x = x;
			this.y = y;
			this.r = r;
		}
		void set(Circle that) {
			this.x = that.x;
			this.y = that.y;
			this.r = that.r;
		}
		
		Bound bounds(Grid g) {
			return new Bound(
				(int) Math.max(Math.ceil(x - r), 0),
				(int) Math.min(Math.floor(x + r) + 1, g.width),
				(int) Math.max(Math.ceil(y - r), 0),
				(int) Math.min(Math.floor(y + r) + 1, g.height)
			);
		}
	}
	
	static class Source extends Circle {
		Strategy s;

		Source(Strategy s, Circle c) {
			super(c);
			this.s = s;
		}
		void moveBy(Grid g, Circle c) {
			c = new Circle(c);
			c.r += r;
			move(g, c);
		}
		void moveBy(Grid g, double dx, double dy) {
			move(g, new Circle(x + dx, y + dy, r));
		}
		void move(Grid g, Circle cur) {
			s.move(g, this, cur);
			set(cur);
		}
		void raster(Grid g, boolean dir) {
			s.raster(g, this, dir);
		}
	}
	static class Grid {
		ArrayList<Source> srcs;
		int[][] grid;
		final int width, height;
		final Color positive, negative;
		
		Grid(int w, int h, Color[] cols) {
			this.srcs = new ArrayList<>();
			this.grid = new int[w][h];
			this.width = w;
			this.height = h;
			this.positive = cols[0];
			this.negative = cols[1];
		}
		
		void add(Source s) {
			srcs.add(s);
		}
		void clear() {
			for (int i = 0; i < width; ++i) {
				for (int j = 0; j < height; ++j) {
					grid[i][j] = 0;
				}
			}
			for (Source s : srcs) {
				s.raster(this, true);
			}
		}
		void chng(int x, int y, boolean dir) {
			grid[x][y] += dir ? 1 : -1;
		}
		void moveInBounds(Circle c) {
			while (c.x - c.r > width) {
				c.x -= width + 2 * c.r;
			}
			while (c.x + c.r < 0) {
				c.x += width + 2 * c.r;
			}
			while (c.y - c.r > height) {
				c.y -= height + 2 * c.r;
			}
			while (c.y + c.r < 0) {
				c.y += height + 2 * c.r;
			}
		}
		
		void step(Circle mouse, double dx, double dy) {
			if (mouse != null) {
				srcs.get(0).moveBy(this, mouse);
			} else {
				srcs.get(0).moveBy(this, dx, dy);
			}
			for (int k = 1; k < srcs.size(); ++k) {
				srcs.get(k).moveBy(this, dx, dy);
			}
		}
		
		void drawGrid(Env e) {
			e.g.setPaint(Color.BLACK);
			for (int i = 0; i < width; ++i) {
				for (int j = 0; j < height; ++j) {
					e.g.drawRect(e.x(i + 0.5) - 1, e.y(j + 0.5) - 1, 0, 0);
				}
			}
		}
		void drawBounds(Env e) {
			for (Source s : srcs) {
				Bound b = s.bounds(this);
				e.g.fillRect(e.x(b.x0), e.y(b.y0), e.w(b.x1 - b.x0), e.h(b.y1 - b.y0));
			}
		}
		enum Last { Pos, Neg, None }
		void drawCells(Env e) {
			for (int j = 0; j < height; ++j) {
				int lastX = 0;
				Last last = Last.None;
				for (int i = 0; i < width; ++i) {
					if (grid[i][j] == 0) {
						if (last != Last.None) {
							e.g.setPaint(last == Last.Pos ? positive : negative);
							e.g.fillRect(e.x(lastX), e.y(j), e.w(i - lastX), e.h(1));
							last = Last.None;
						}
					} else if (drawOverlap ? (grid[i][j] == 1) : (grid[i][j] >= 1)) {
						if (last == Last.Neg) {
							e.g.setPaint(negative);
							e.g.fillRect(e.x(lastX), e.y(j), e.w(i - lastX), e.h(1));
						}
						if (last != Last.Pos) {
							last = Last.Pos;
							lastX = i;
						}
					} else {
						if (last == Last.Pos) {
							e.g.setPaint(positive);
							e.g.fillRect(e.x(lastX), e.y(j), e.w(i - lastX), e.h(1));
						}
						if (last != Last.Neg) {
							last = Last.Neg;
							lastX = i;
						}
					}
				}
				if (last != Last.None) {
					e.g.setPaint(last == Last.Pos ? positive : negative);
					e.g.fillRect(e.x(lastX), e.y(j), e.w(width - lastX), e.h(1));
				}
			}
		}
		void drawExact(Env e) {
			for (Source s : srcs) {
				e.g.drawOval(e.x(s.x - s.r + 0.5f), e.y(s.y - s.r + 0.5f), e.w(2 * s.r), e.h(2 * s.r));
				e.g.drawRect(e.x(s.x + 0.5f) - 1, e.y(s.y + 0.5f) - 1, 0, 0);
			}
		}
	}

	static class Line {
		int l, r, y;

		Line(int l, int r, int y) {
			this.l = l;
			this.r = r;
			this.y = y;
		}
	}
	
	Grid ref, grid, move;
	volatile int tick; // monitor thread accesses this
	Parameter param;
	Thread thread;
	JFrame frame;
	State state;
	Showing showing;
	double clicks;
	
	public CircleRasterer(Parameter p) {
		init(p);
		var s = new Dimension(width * cellSize + 22, height * cellSize + 44);
		this.frame = new JFrame();
		this.frame.add(this);
		this.frame.setSize(s);
		this.frame.setPreferredSize(s);
		this.frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.state = State.Pause;
		this.showing = Showing.All;
		addMouseListener(this);
		if (hasMouse) {
			this.clicks = 0.0;
			addMouseWheelListener(this);
		}
		setup();
		this.frame.setVisible(true);
	}
	public CircleRasterer(Parameter p, boolean flag) {
		init(p);
		setup();
	}
	void init(Parameter p) {
		int w = width, h = height;
		if (p.r * 2 > w) {
			w = (int) (p.r * 2);
		}
		if (p.r * 2 > h) {
			h = (int) (p.r * 2);
		}
		ref = new Grid(w, h, gridColors[0]);
		if (useTestGrid) grid = new Grid(w, h, gridColors[1]);
		if (useMoveGrid)  move = new Grid(w, h, gridColors[2]);
		tick = startTick;
		param = p;
		thread = new Thread(this);
	}
	void setup() {
		boolean saveMoveTick = ! noMove && tick > 0;
		if (saveMoveTick) {
			--tick; // move the last tick instead of rastering it
		}
		Circle m = null;
		if (hasMouse) {
			m = new Circle((double) startMouseX / cellSize, (double) startMouseY / cellSize, startMouseR);
			ref.add(new Source(Strategy.Ref, m));
			if (grid != null) grid.add(new Source(testStrategy, m));
			if (move != null) move.add(new Source(moveStrategy, m));
		}
		var c = new Circle(param.x, param.y, param.r);
		for (int k = 0; k < tick; ++k) {
			c.x += param.dx;
			c.y += param.dy;
			ref.moveInBounds(c);
		}
		ref.add(new Source(Strategy.Ref, c));
		if (grid != null) grid.add(new Source(testStrategy, c));
		if (move != null) move.add(new Source(moveStrategy, c));

		refresh();
		if (saveMoveTick) {
			++tick;
			if (m != null) {
				m.r = 0.0;
			}
			ref.step(m, param.dx, param.dy);
			if (grid != null) grid.step(m, param.dx, param.dy);
			if (move != null) move.step(m, param.dx, param.dy);
		}
		compare();
	}
	void compare() {
		if (! (reportDiscrepancies || proof || pauseOnDiscrepancy)) return;
		int k = 0, m = 0;
		StringBuilder gb = new StringBuilder();
		StringBuilder mb = new StringBuilder();
		Outer:
		for (int j = 0; j < ref.height; ++j) {
			for (int i = 0; i < ref.width; ++i) {
				if (grid != null && grid.grid[i][j] != ref.grid[i][j]) {
					++k;
					if (reportDiscrepancies) gb.append(" " + i + ", " + j + " = " + grid.grid[i][j] + "\n");
					else break Outer;
				}
				if (move != null && move.grid[i][j] != ref.grid[i][j]) {
					++m;
					if (reportDiscrepancies) mb.append(" " + i + ", " + j + " = " + move.grid[i][j] + "\n");
					else break Outer;
				}
			}
		}
		String appendage = "";
		if (reportDiscrepancies && hasMouse) {
			Source s = ref.srcs.get(0);
			appendage = ", mouse at " + s.x + ", " + s.y + ":" + s.r;
		}
		if (k != 0) {
			if (reportDiscrepancies) print(param + " " + k + " cells mismatch on tick " + tick + appendage + ": \n" + gb);
			if (proof) System.exit(0);
			if (pauseOnDiscrepancy) state = State.Pause;
		}
		if (m != 0) {
			if (reportDiscrepancies) print(param + " " + m + " moved cells mismatch on tick " + tick + appendage + ": \n" + mb);
			if (proof) System.exit(0);
			if (pauseOnDiscrepancy) state = State.Pause;
		}
	}
	void refresh() {
		ref.clear();
		if (grid != null) grid.clear();
		if (move != null) move.clear();
	}
	void start() {
		thread.start();
	}
	void stop() {
		thread.stop();
	}
	void step() {
		++tick;
		Circle c = null;
		if (hasMouse) {
			Point m = getMousePosition(false);
			if (m != null) {
				c = new Circle((double) m.x / cellSize, (double) m.y / cellSize, clicks / cellSize);
				clicks = 0.0;
			}
		}
		ref.step(c, param.dx, param.dy);
		if (grid != null) grid.step(c, param.dx, param.dy);
		if (move != null) move.step(c, param.dx, param.dy);
		compare();
	}
	void windowStep() {
		if (state == State.Running) {
			step();
		} else if (state == State.Step) {
			step();
			state = State.Pause;
		}
		repaint();
	}
	
	boolean clickedButton(int b, MouseEvent e) {
		return b * buttonSize < e.getX() && e.getX() < (b + 1) * buttonSize && e.getY() < buttonSize;
	}
	void drawButton(Graphics2D g, int b, String text) {
		g.setPaint(Color.WHITE);
		g.fillRect(b * buttonSize, 0, buttonSize - 1, buttonSize - 1);
		g.setPaint(Color.BLACK);
		g.drawRect(b * buttonSize, 0, buttonSize - 1, buttonSize - 1);
		g.drawString(text, b * buttonSize + 5, buttonSize / 2);
	}
	@Override
	public void paintComponent(Graphics graphics) {
		Graphics2D g = (Graphics2D) graphics;
		var e = new Env(g, 3, 3, cellSize);
		g.setPaint(Color.WHITE);
		g.fillRect(0, 0, getWidth(), getHeight());
	
		if (drawGrid) ref.drawGrid(e);
		
		if (drawBounds) {
			g.setPaint(boundColor);
			if (showing.ref)       ref.drawBounds(e);
			else if (showing.test) grid.drawBounds(e);
			else if (showing.move) move.drawBounds(e);
		}

		if (showing.ref)  ref.drawCells(e);
		if (showing.test) grid.drawCells(e);
		if (showing.move) move.drawCells(e);
		
		if (drawExact) {
			g.setPaint(Color.BLACK);
			if (showing.ref)       ref.drawExact(e);
			else if (showing.test) grid.drawExact(e);
			else if (showing.move) move.drawExact(e);
		}

		g.setPaint(Color.BLACK);
		String s = Integer.toString(tick);
		int w = g.getFontMetrics().stringWidth(s);
		int h = g.getFontMetrics().getAscent();
		g.drawString(s, getWidth() - w, h);
		Point m = getMousePosition(false);
		if (m == null || ! (m.x > getWidth() - w && m.y < h)) {
			drawButton(g, 0, "Refresh");
			if (state == State.Running) {
				drawButton(g, 1, "Pause");
			} else {
				drawButton(g, 1, "Resume");
				drawButton(g, 2, "Step");
			}
			drawButton(g, 3, showing.toString());
		}
	}
	@Override
	public String toString() {
		return param.toString();
	}

	@Override
	public void run() {
		if (proof) {
			runProof();
		} else {
			runWindow();
		}
	}
	void runProof() {
		while (tick < proofTicks) {
			step();
		}
		provingThread.interrupt();
	}
	void runWindow() {
		while (true) {
			try {
				Thread.sleep(frameUpdateInterval);
			} catch (InterruptedException ex) {
				ex.printStackTrace();
			}
			windowStep();
		}
	}
	@Override
	public void mouseWheelMoved(MouseWheelEvent e) {
		clicks += e.getPreciseWheelRotation();
	}
	@Override
	public void mouseClicked(MouseEvent e) {
		if (clickedButton(0, e)) {
			refresh();
		}
		if (clickedButton(1, e)) {
			state = state == State.Running ? State.Pause : State.Running;
		}
		if (clickedButton(2, e) && state == State.Pause) {
			state = State.Step;
		}
		if (clickedButton(3, e)) {
			showing = Showing.values()[(showing.ordinal() + 1) % Showing.values().length];
		}
	}
	@Override public void mousePressed(MouseEvent e) {}
	@Override public void mouseReleased(MouseEvent e) {}
	@Override public void mouseEntered(MouseEvent e) {}
	@Override public void mouseExited(MouseEvent e) {}
}
