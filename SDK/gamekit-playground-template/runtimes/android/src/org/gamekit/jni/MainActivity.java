/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2013 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

package org.gamekit.jni;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import tv.ouya.console.api.OuyaController;
import tv.ouya.console.api.OuyaFacade;
import android.app.Activity;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.util.Pair;
import android.view.GestureDetector;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import android.view.View;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputMethodManager;
import android.widget.AbsoluteLayout;
import android.widget.TextView;

public class MainActivity extends Activity implements SensorEventListener {
	
	protected Handler handler = null;
	protected SurfaceView surfaceView = null;
	protected Surface lastSurface = null;

	public static MainActivity mActivity;
	public static final MainActivity getContext() { return mActivity; }
	private Runnable renderer = null;
	private boolean paused = false;
	private boolean initOGRE = false;
	private long startMilli = System.currentTimeMillis();
    private boolean ret=false;

    protected static final int MULTI_DATA_STRIDE = 5;
    protected static final int MULTI_MAX_INPUTS = 10;
    protected float multiData[];
       
    public static final int OIS_Shift = 1;
    public static final int OIS_Ctrl  = 16;
    public static final int OIS_Alt   = 256;

    // additional added to for the android-ois-modifier-handling
    private static final int OIS_LONGPRESS = 2;
    private static final int OIS_IS_CANCELED = 4;
    private static final int OIS_IS_PRINTINGKEY = 8;
    private static final int OIS_SYM_KEY = 32;
	
    boolean isOpened = false;
    boolean keyboardSetByUser = false;

    private SensorManager sm = null;
    
    private Map<String, Integer> sensorMapping;
    private List<String> activeSensors;
	private TextView fakeEditField;
	private AbsoluteLayout mLayout;
    
    
    public static int platform = 0;
    public static int PLATFORM_ANDROID_PLAIN = 0;
    public static int PLATFORM_OUYA = 1;
    public static int PLATFORM_GAMESTICK = 2;
    
    public static boolean is_ouya = false;
    
    private static final int SWIPE_MIN_DISTANCE = 120;
    private static final int SWIPE_THRESHOLD_VELOCITY = 200;
	GestureDetector myG;
	
	static class Gesture extends GestureDetector.SimpleOnGestureListener{
        @Override
        public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
  	      	//OgreActivityJNI.sendMessage("mobile", "__internal", "fling", Float.toString(velocityX)+"|"+Float.toString(velocityY));

        	if(e1.getX() - e2.getX() > SWIPE_MIN_DISTANCE && Math.abs(velocityX) > SWIPE_THRESHOLD_VELOCITY) {
                return false; // Right to left
            }  else if (e2.getX() - e1.getX() > SWIPE_MIN_DISTANCE && Math.abs(velocityX) > SWIPE_THRESHOLD_VELOCITY) {
                return false; // Left to right
            }

            if(e1.getY() - e2.getY() > SWIPE_MIN_DISTANCE && Math.abs(velocityY) > SWIPE_THRESHOLD_VELOCITY) {
                return false; // Bottom to top
            }  else if (e2.getY() - e1.getY() > SWIPE_MIN_DISTANCE && Math.abs(velocityY) > SWIPE_THRESHOLD_VELOCITY) {
                return false; // Top to bottom
            }
            return false;
        }
		}

	private ScaleGestureDetector SGD;
	private float scale = 1f;
	
	private class ScaleListener extends ScaleGestureDetector.
	   SimpleOnScaleGestureListener {
	   @Override
	   public boolean onScale(ScaleGestureDetector detector) {
	      scale *= detector.getScaleFactor();
	      scale = Math.max(0.1f, Math.min(scale, 5.0f));
	      OgreActivityJNI.sendMessage("mobile", "__internal", "scale", Float.toString(scale));
	      System.out.println("SCALE:"+scale);
	      return true;
	   }
	}
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		

		mActivity = this;

		if (OuyaFacade.getInstance().isRunningOnOUYAHardware())
		{
			platform = PLATFORM_OUYA;
			is_ouya = true;
			System.out.println("OUYA");
			OuyaController.init(this);
		}
		
		SGD = new ScaleGestureDetector(this,new ScaleListener());
		
		OgreActivityJNI.setActivity(this);
		
		multiData = new float[MULTI_DATA_STRIDE*MULTI_MAX_INPUTS]; 
		handler = new Handler();
		
		sm = (SensorManager) getSystemService(SENSOR_SERVICE);
		
		sysInit();
		
		activeSensors = new LinkedList<String>();




		myG = new GestureDetector(this,new Gesture());

		

	}

	public Integer getSensor(String name) {
		if (sensorMapping == null) {
			 sensorMapping = new HashMap<String, Integer>();
			 sensorMapping.put("orient",Sensor.TYPE_ORIENTATION);
			 sensorMapping.put("accel",Sensor.TYPE_ACCELEROMETER);
		}
		return sensorMapping.get(name);
	}
	
	
	@Override
	protected void onStop() {
		super.onStop();
		stopSensors();
	}



	@Override
	protected void onPause() {
		super.onPause();
		handler.removeCallbacks(renderer);
		paused = true;
		OgreActivityJNI.pause();
		stopSensors();
	}

	@Override
	protected void onResume() {
		super.onResume();
		
		// since I seem not to be able to keep the list from pause->resume I leave it up to the application
		// to restart the sensor! 
		// TOOD: Check to make it happen automatically
		//resumeSensors();
		paused = false;
		handler.post(renderer);
		OgreActivityJNI.resume();
	}
	
	
	public void stopSensors() {
		sm.unregisterListener(this);
	}
	
	public void stopSensor(String sensorName) {
		if (activeSensors.remove(sensorName)) {
			stopSensors();
			resumeSensors();
		}
	}
	
	private void resumeSensors() {
		if (activeSensors!=null && activeSensors.size()>0) {
			for (String sensorName : activeSensors) {
				startSensor(sensorName);
			}
		} else {
//			System.out.println("TRIED TO RESUME SENSORS, BUT THE SENSOR LIST WAS NULL! MAYBE WE NEED TO PERSIST BETTER!!");
		}
	}
	
	public void startSensor(String name) {
		if (activeSensors.contains(name))
			return;
		
		Integer sensor = getSensor(name);
		if (sensor == null) {
			System.out.println("Unknown sensor:"+name);
			return;
		}
		
		List<Sensor> typedSensors =
				sm.getSensorList(sensor);
		
		if (typedSensors != null && typedSensors.size()>0) {
			sm.registerListener(this, typedSensors.get(0), SensorManager.SENSOR_DELAY_GAME);
			activeSensors.add(name);
		}
	}
	
	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		int screenHeight = this.getWindowManager().getDefaultDisplay().getHeight();
		int viewHeight = surfaceView.getHeight();
		
		// Use the difference as the cursor offset
		OgreActivityJNI.setOffset(0, viewHeight - screenHeight);
		
		super.onWindowFocusChanged(hasFocus);
	}	

	public void showVirtualKeyboard(boolean showit) {
//    	
//    	System.out.println("show");
//    	if (showit && !isOpened){
//    		imm.showSoftInput(fakeEditField, InputMethodManager.SHOW_FORCED);
//    		
//    		imm.toggleSoftInput(InputMethodManager.SHOW_FORCED,0);
//    		isOpened = true;
//    		keyboardSetByUser = true;
//    	} else if (!showit && isOpened) {
//    		imm.toggleSoftInput(InputMethodManager.HIDE_IMPLICIT_ONLY,0);
//    		isOpened = false;
//    		keyboardSetByUser = true;
//    	}	
		if (showit) {
			showTextInput(0, 0, 800, 300);
		} else {
	    	InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
			imm.toggleSoftInput(InputMethodManager.HIDE_IMPLICIT_ONLY,0);
		}
		
	}
	
	private void sysInit() {
		final Runnable initRunnable = new Runnable() {
			public void run() {
				if (!initOGRE) {
					initOGRE = true;

//					for (int i=0;i<40000;i++)
//					{
//						System.out.println("Waiter:"+i);
//					}
					 
//						OgreActivityJNI.create(getAssets(),Environment.getExternalStorageDirectory().getAbsolutePath());
					String externalCacheDir = getExternalCacheDir().getAbsolutePath();
					String cacheDir = getCacheDir().getAbsolutePath();
					String extFilesDir = getExternalFilesDir("tomaga").getAbsolutePath();
					String filesDir = getFilesDir().getAbsolutePath();
						
						OgreActivityJNI.create(getAssets(),extFilesDir);

					renderer = new Runnable() {
						public void run() {
							
							if (paused)
							{
								return;
							}
	
							
							if (!wndCreate && lastSurface != null && System.currentTimeMillis()-startMilli > 2500) {
								System.out.println("INIT");
								wndCreate = true;
								OgreActivityJNI.initWindow(lastSurface);
								System.out.println("AFTERINIT");
								int screenHeight = MainActivity.this.getWindowManager().getDefaultDisplay().getHeight();
								int viewHeight = surfaceView.getHeight();
								OgreActivityJNI.setOffset(0,viewHeight - screenHeight);
								handler.post(this);
								
								OgreActivityJNI.sendMessage("android", "", "orient", "bla");
								OgreActivityJNI.sendMessage("android","","platformtype",OgreActivityJNI.getPlatformType()+"");

								//DataCollector.startContactCollection(MainActivity.this);
								
								return;
							}

							
							if (initOGRE && wndCreate)
								OgreActivityJNI.renderOneFrame();

							handler.post(this);
						}
					};

					handler.post(renderer);
				}
			}

		};

		SurfaceView view = new SurfaceView(this){
		    @Override
		    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
		    	if (!keyboardSetByUser && isOpened) {
		    		isOpened = false;
		    		System.out.println("on keyboard close");
		    	}
		    	
		    	keyboardSetByUser = false;
		    	
		    	super.onMeasure(widthMeasureSpec, heightMeasureSpec);
		    }
		};
		SurfaceHolder holder = view.getHolder();
		// holder.setType(SurfaceHolder.SURFACE_TYPE_GPU);
		surfaceView = view;

		holder.addCallback(new Callback() {
			public void surfaceCreated(SurfaceHolder holder) {
				if (holder.getSurface() != null
						&& holder.getSurface().isValid()) {
					lastSurface = holder.getSurface();
					handler.post(initRunnable);
				}
			}
			
			public void surfaceDestroyed(SurfaceHolder holder) {
				if (initOGRE && wndCreate) {
					wndCreate = false;
					lastSurface = null;
					handler.post(new Runnable() {
						public void run() {
							OgreActivityJNI.termWindow();
						}
					});
				}
			}

			public void surfaceChanged(SurfaceHolder holder, int format,
					int width, int height) {
				System.out.println("SURFACE CHANGED! DO SOMETHING!!");
			}
		});
		mLayout = new AbsoluteLayout(this);
		mLayout.addView(surfaceView);
		
		setContentView(mLayout);
	}

	boolean wndCreate = false;

	public void onAccuracyChanged(Sensor sensor, int accuracy) {

	}

	public void onSensorChanged(SensorEvent event) {
		if (!(initOGRE && wndCreate)) 
			return;
		
		if (event.sensor.getType() == Sensor.TYPE_ORIENTATION) {
//			System.out.println("ORIENT:"+event.values[0]+" : "+event.values[1]+" : "+event.values[2]);
//			OgreActivityJNI.sendMessage("android", "", "orient", event.values[0]+":"+event.values[1]+":"+event.values[2]);
			OgreActivityJNI.sendOrientationVector(event.values[0], event.values[1], event.values[2]);
		}
		else if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
//			System.out.println("Accel:"+event.values[0]+" : "+event.values[1]+" : "+event.values[2]);
//			OgreActivityJNI.sendMessage("android", "", "accel", event.values[0]+":"+event.values[1]+":"+event.values[2]);
			OgreActivityJNI.sendAcceleratorVector(event.values[0], event.values[1], event.values[2]);
		}
		
	}

	
	
	@Override
	public boolean onTouchEvent(MotionEvent event) {
    	//System.out.println("OnTouch");
    	
    	//myG.onTouchEvent(event);
    	
		if (initOGRE) {
        	int numEvents = event.getPointerCount();
        	for (int i=0; i<numEvents; i++)
        	{
        		int j = i*MULTI_DATA_STRIDE;
        		// put x and y FIRST, so if people just want that data, there's nothing else
        		// to jump over...
        		multiData[j + 0] = (float)event.getX(i);
        		multiData[j + 1] = (float)event.getY(i);
        		multiData[j + 2] = (float)event.getPointerId(i);
        		multiData[j + 3] = (float)event.getSize(i);
        		multiData[j + 4] = (float)event.getPressure(i);
        	}
            ret = OgreActivityJNI.multitouchEvent(event.getAction(), numEvents, multiData, MULTI_DATA_STRIDE);
		}

    	SGD.onTouchEvent(event);

		if (ret) // consumed event?
			return true;
		else
			return super.onTouchEvent(event);
	}

    private int getModifierValue(KeyEvent event)
    {
    	int modifiers = 0;
    	if (event.isShiftPressed())
    		modifiers |= OIS_Shift;
    	if (event.isAltPressed())
    		modifiers |= OIS_Alt;

    	if (event.isLongPress())
    		modifiers |= OIS_LONGPRESS;
    	if (event.isCanceled())
    		modifiers |= OIS_IS_CANCELED;
    	if (event.isPrintingKey())
    		modifiers |= OIS_IS_PRINTINGKEY;
    	if (event.isSymPressed())
    		modifiers |= OIS_SYM_KEY;
    	return modifiers;
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
    	System.out.println(System.currentTimeMillis()+" - Onkeydown:"+keyCode);
    	
    	boolean result = handleSpecialKeys(keyCode,event);
    	
    	if (result)
    		return true;

    	if (platform == PLATFORM_ANDROID_PLAIN)
    	{
        	boolean ret = super.onKeyDown(keyCode, event);
            if (!ret){
            	int modifiers = getModifierValue(event);
            	
//            	Unicodes are not passed at the moment since I need the space for the modifiers
//            	int unicode = event.getUnicodeChar();

                ret = OgreActivityJNI.keyEvent(event.getAction(), modifiers, event.getKeyCode());
            }
            return ret;
    	}
    	else if (platform == PLATFORM_OUYA)
    	{
    		int devId = event.getDeviceId();
			int player = OuyaController.getPlayerNumByDeviceId(devId);
			OuyaController con = OuyaController.getControllerByDeviceId(devId);
			if (true)
				return true;
			
			if (player >= 0) {
				OgreActivityJNI.onControllerButtonDown(player, keyCode);
			}
			else
			{
				int modifiers = getModifierValue(event);
				boolean ret = OgreActivityJNI.keyEvent(event.getAction(), modifiers , event.getKeyCode());
			}
	    	return true;    		
    	}
    	else {
        	System.out.println("PLATFORM:"+platform+" not supported yet!");
        	return false;
    	}
    }
    
    private boolean handleSpecialKeys(int keyCode, KeyEvent event)
    {
        if (keyCode == KeyEvent.KEYCODE_VOLUME_UP) {
        	// tell the engine
        	OgreActivityJNI.sendMessage("internal", "internal", "__volup", "");
        	System.out.println("VOLUP");
        	return true;
        }
        else if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN) {
        	// tell the engine
        	OgreActivityJNI.sendMessage("internal", "internal", "__voldown", "");
        	System.out.println("VOLDOWN");
        	return true;
        }
        else if (keyCode == KeyEvent.KEYCODE_BACK)
        {
        	System.out.println("BACK");
        	OgreActivityJNI.sendMessage("internal", "internal", "__back", "");
        	return true;
        }
        else if (keyCode == OuyaController.BUTTON_A)
        {
        	return true;
        	
        }
        else if (keyCode == OuyaController.BUTTON_Y)
        {
        	return true;
        	
        }
        else if (keyCode == OuyaController.BUTTON_O)
        {
        	return true;
        	
        }
        else if (keyCode == OuyaController.BUTTON_U)
        {
        	return true;
        	
        }
        else if (keyCode == OuyaController.BUTTON_A)
        {
        	return true;
        	
        }
        
        return false;
    }
    
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event)
    {
    	System.out.println(System.currentTimeMillis()+" - Onkeyup:"+keyCode);

    	if (platform == PLATFORM_ANDROID_PLAIN)
    	{
            boolean ret = super.onKeyUp(keyCode, event);
            
            if (!ret){

            	int modifiers = getModifierValue(event);
//     			Unicodes are not passed at the moment since I need the space for the modifiers
//            	int unicode = event.getUnicodeChar();
            	
                ret = OgreActivityJNI.keyEvent(event.getAction(), modifiers , event.getKeyCode());
         
            } else {
            	System.out.println("KEY ALREADY CONSUMED!?");
            }
            return ret;
    	}
    	else if (platform == PLATFORM_OUYA)
    	{
    		int player = OuyaController.getPlayerNumByDeviceId(event.getDeviceId());
    		OgreActivityJNI.onControllerButtonUp(player, keyCode);
    	    return true;    		
    	}
    	else
    	{
    		System.out.println("KeyUP Platform:"+platform+" not supported yet!");
    		return false;
    	}
    }

    @Override public boolean onGenericMotionEvent(MotionEvent event)
    {
    	if (platform == PLATFORM_ANDROID_PLAIN){
    		return false;
    	}
    		
	int player = OuyaController.getPlayerNumByDeviceId(event.getDeviceId());
		
	// Joysticks
	if((event.getSource() & InputDevice.SOURCE_CLASS_JOYSTICK) != 0)
	{
	    switch(event.getAction())
	    {
	        case MotionEvent.ACTION_MOVE:
		     float x0, y0;
		     float x1, y1;
		     for (int i = 0; i < event.getHistorySize(); i++)
		     {
		         x0 = event.getHistoricalAxisValue(MotionEvent.AXIS_X, i);
			 y0 = event.getHistoricalAxisValue(MotionEvent.AXIS_Y, i);
	    		 OgreActivityJNI.onControllerJoystickMove(player, 0, x0, y0);

			 x1 = event.getHistoricalAxisValue(MotionEvent.AXIS_Z, i);
		    	 y1 = event.getHistoricalAxisValue(MotionEvent.AXIS_RZ, i);
		    	 OgreActivityJNI.onControllerJoystickMove(player, 1, x1, y1);
		     }
			    
		     x0 = event.getAxisValue(MotionEvent.AXIS_X); 
		     y0 = event.getAxisValue(MotionEvent.AXIS_Y);
		     OgreActivityJNI.onControllerJoystickMove(player, 0, x0, y0);
				    
		     x1 = event.getAxisValue(MotionEvent.AXIS_Z);
		     y1 = event.getAxisValue(MotionEvent.AXIS_RZ);
		     OgreActivityJNI.onControllerJoystickMove(player, 1, x1, y1);
                     return true;
	    }
	}

        return super.onGenericMotionEvent(event);
    }

    
	static {
//		System.loadLibrary("crypto");
//		System.loadLibrary("ssl");
		System.loadLibrary("openal");
		System.loadLibrary("ogrekit");
	}


//----------------- KEYBOARD STUFF ----------------------------------------------
	
    static class TTShowTextInputTask implements Runnable {
        /*
         * This is used to regulate the pan&scan method to have some offset from
         * the bottom edge of the input region and the top edge of an input
         * method (soft keyboard)
         */
        static final int HEIGHT_PADDING = 15;

        public int x, y, w, h;

		private TTDummyEdit mTextEdit;

        public TTShowTextInputTask(int x, int y, int w, int h) {
            this.x = x;
            this.y = y;
            this.w = w;
            this.h = h;
        }

        @Override
        public void run() {
            AbsoluteLayout.LayoutParams params = new AbsoluteLayout.LayoutParams(
                    w, h + HEIGHT_PADDING, x, y);

            if (mTextEdit == null) {
                mTextEdit = new TTDummyEdit(getContext());

                mActivity.mLayout.addView(mTextEdit, params);
            } else {
                mTextEdit.setLayoutParams(params);
            }

            mTextEdit.setVisibility(View.VISIBLE);
            mTextEdit.requestFocus();

            InputMethodManager imm = (InputMethodManager) mActivity.getSystemService(Context.INPUT_METHOD_SERVICE);
            imm.showSoftInput(mTextEdit, 0);
        }
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static boolean showTextInput(int x, int y, int w, int h) {
        // Transfer the task to the main thread as a Runnable
        return mActivity.handler.post(new TTShowTextInputTask(x, y, w, h));
    }
	
}
class TTDummyEdit extends View implements View.OnKeyListener {
    InputConnection ic;

    public TTDummyEdit(Context context) {
        super(context);
        setFocusableInTouchMode(true);
        setFocusable(true);
        setOnKeyListener(this);
    }

    @Override
    public boolean onCheckIsTextEditor() {
        return true;
    }

    @Override
    public boolean onKey(View v, int keyCode, KeyEvent event) {

        // This handles the hardware keyboard input
        if (event.isPrintingKey()) {
            if (event.getAction() == KeyEvent.ACTION_DOWN) {
                ic.commitText(String.valueOf((char) event.getUnicodeChar()), 1);
            }
            return true;
        }

        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            MainActivity.getContext().onKeyDown(keyCode, event);
//            SDLActivity.onNativeKeyDown(keyCode);
            return true;
        } else if (event.getAction() == KeyEvent.ACTION_UP) {
            MainActivity.getContext().onKeyUp(keyCode, event);
//            SDLActivity.onNativeKeyUp(keyCode);
            return true;
        }

        return false;
    }
        
    //
    @Override
    public boolean onKeyPreIme (int keyCode, KeyEvent event) {
        // As seen on StackOverflow: http://stackoverflow.com/questions/7634346/keyboard-hide-event
        // FIXME: Discussion at http://bugzilla.libsdl.org/show_bug.cgi?id=1639
        // FIXME: This is not a 100% effective solution to the problem of detecting if the keyboard is showing or not
        // FIXME: A more effective solution would be to change our Layout from AbsoluteLayout to Relative or Linear
        // FIXME: And determine the keyboard presence doing this: http://stackoverflow.com/questions/2150078/how-to-check-visibility-of-software-keyboard-in-android
        // FIXME: An even more effective way would be if Android provided this out of the box, but where would the fun be in that :)
        if (event.getAction()==KeyEvent.ACTION_UP && keyCode == KeyEvent.KEYCODE_BACK) {
            if (SDLActivity.mTextEdit != null && SDLActivity.mTextEdit.getVisibility() == View.VISIBLE) {
            	System.out.println("KeyboardFocusLost: TODO");
            	//                SDLActivity.onNativeKeyboardFocusLost();
            }
        }
        return super.onKeyPreIme(keyCode, event);
    }

    @Override
    public InputConnection onCreateInputConnection(EditorInfo outAttrs) {
        ic = new TTSDLInputConnection(this, true);

        outAttrs.imeOptions = EditorInfo.IME_FLAG_NO_EXTRACT_UI
                | 33554432 /* API 11: EditorInfo.IME_FLAG_NO_FULLSCREEN */;

        return ic;
    }
}

class TTSDLInputConnection extends BaseInputConnection {

    public TTSDLInputConnection(View targetView, boolean fullEditor) {
        super(targetView, fullEditor);

    }

    @Override
    public boolean sendKeyEvent(KeyEvent event) {

        /*
         * This handles the keycodes from soft keyboard (and IME-translated
         * input from hardkeyboard)
         */
        int keyCode = event.getKeyCode();
        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            if (event.isPrintingKey()) {
                commitText(String.valueOf((char) event.getUnicodeChar()), 1);
            }
            MainActivity.getContext().onKeyDown(keyCode, event);
//            SDLActivity.onNativeKeyDown(keyCode);
            return true;
        } else if (event.getAction() == KeyEvent.ACTION_UP) {
            MainActivity.getContext().onKeyUp(keyCode, event);

//            SDLActivity.onNativeKeyUp(keyCode);
            return true;
        }
        return super.sendKeyEvent(event);
    }

    @Override
    public boolean commitText(CharSequence text, int newCursorPosition) {
    	System.out.println("Commit Text:"+text+" CursorPos:"+newCursorPosition);
//    	OgreActivityJNI.nativeCommitText(text.toString(), newCursorPosition);
    	char c = text.charAt(0);
    	Pair<Integer,Integer> keycode = OgreActivityJNI.char2code.get(Character.toLowerCase(c));
    	if (keycode != null) {
    		int modifier = Character.isUpperCase(c)?MainActivity.OIS_Shift:keycode.second;
        	OgreActivityJNI.keyEvent(0, modifier, keycode.first);
    	}
        return super.commitText(text, newCursorPosition);
    }

    @Override
    public boolean setComposingText(CharSequence text, int newCursorPosition) {
    	System.out.println("SetCompText:"+text+" new c:"+newCursorPosition);
//        nativeSetComposingText(text.toString(), newCursorPosition);
//    	for (int i=0;i<text.length();i++){
//    		commitText(Character.toString(text.charAt(i)), 1);
//    	}
    	
        return super.setComposingText(text, newCursorPosition);
    }

    @Override
    public boolean deleteSurroundingText(int beforeLength, int afterLength) {       
        // Workaround to capture backspace key. Ref: http://stackoverflow.com/questions/14560344/android-backspace-in-webview-baseinputconnection
        if (beforeLength == 1 && afterLength == 0) {
            // backspace
            return super.sendKeyEvent(new KeyEvent(KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_DEL))
                && super.sendKeyEvent(new KeyEvent(KeyEvent.ACTION_UP, KeyEvent.KEYCODE_DEL));
        }

        return super.deleteSurroundingText(beforeLength, afterLength);
    }
}	
