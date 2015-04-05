package com.example.augmentedreality.ar;

public interface State {
	public final int beforeFirst = 100;
	public final int gottenFirst = 200;
	public final int beforeSecond = 300;
	public final int initFailed = 400;
	public final int initSuccess = 500;
	public final int beforeTrack = 600;
	public final int trackFailed = 700;
	public final int trackSuccess = 800;
	public final int triangulateFailed = 900;
	public final int triangulateSuccess = 1000;

}
