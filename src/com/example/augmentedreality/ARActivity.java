package com.example.augmentedreality;


import com.example.augmentedreality.ar.SysApplication;

import android.app.Activity;
import android.os.Bundle;


public class ARActivity extends Activity {
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
        SysApplication.getInstance().addActivity(this); 
	}

}
