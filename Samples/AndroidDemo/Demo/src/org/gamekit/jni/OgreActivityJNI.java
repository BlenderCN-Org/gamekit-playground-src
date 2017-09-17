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

import android.content.res.AssetManager;
import android.util.Pair;
import android.view.Surface;

public class OgreActivityJNI {	
	private static MainActivity mainActivity=null;
	
	public static void setActivity(MainActivity act) {
		mainActivity = act;
	}
	
	public static HashMap<Character, Pair<Integer, Integer>> char2code;
	
	public native static void create(AssetManager assetManager, String sdcardPath);	
	public native static void destroy();	
	public native static void initWindow(Surface surface);
	public native static void termWindow();
	public native static void renderOneFrame();
    public native static boolean multitouchEvent(int action, int numInputs, float data[], int dataStride);
    public native static boolean keyEvent(int action, int modifierState, int keyCode);
    public native static void nativeCommitText(String text, int newCursorPosition);
    
    public native static void setOffset(int x,int y);
    public native static void pause();
    public native static void resume();
    public native static void sendMessage(String from,String to,String subject,String body);
    
    public native static void onControllerJoystickMove(int player, int joystick, float x, float y);
    public native static void onControllerButtonDown(int player, int button);
    public native static void onControllerButtonUp(int player, int button);

    public native static void sendAcceleratorVector(float x,float y,float z);
    public native static void sendOrientationVector(float x,float y,float z);
    
    // 0 = plain android 1=ouya 2=gamestick
    public static int getPlatformType()
    {
    	return mainActivity.platform;
    }

    
    public static void onMessage(String from,String to,String subject,String body) {
    	//System.out.println("Got Message from gk from:"+from+" from:"+from+" to:"+to+" subject:"+subject+" body:"+body );
    	if (subject.equals("__showkeyboard")) {
    		if (body.equalsIgnoreCase("true")) {
    			System.out.println("Show keyboard-msg!");
    			mainActivity.showVirtualKeyboard(true);
    		} else {
    			System.out.println("Remove virtualkeyboard");
    			mainActivity.showVirtualKeyboard(false);
    		}
    	} 
    	else if (subject.equalsIgnoreCase("start_sensor")) {
			if (body!=null && body.length()!=0) {
				mainActivity.startSensor(body);
			}
    	}
    	else if (subject.equalsIgnoreCase("stop_sensor")) {
			if (body!=null && body.length()!=0) {
				mainActivity.stopSensor(body);
			}
    	}
    	else if (subject.equalsIgnoreCase("finish")) {
    		mainActivity.finish();
    	}
    	else if (subject.equalsIgnoreCase("gk_up"))
    	{
    		if (mainActivity.is_ouya) {
        		OgreActivityJNI.sendMessage("", "internal", "subsystem", "ouya");
    		}
    	}
    } 
    
	static {
		char2code = new HashMap<Character, Pair<Integer, Integer>>();
		char2code.put('0', new Pair<Integer,Integer>(7,0));
		char2code.put('1', new Pair<Integer,Integer>(8,0));
		char2code.put('2', new Pair<Integer,Integer>(9,0));
		char2code.put('3', new Pair<Integer,Integer>(10,0));
		char2code.put('4', new Pair<Integer,Integer>(11,0));
		char2code.put('5', new Pair<Integer,Integer>(12,0));
		char2code.put('6', new Pair<Integer,Integer>(13,0));
		char2code.put('7', new Pair<Integer,Integer>(14,0));
		char2code.put('8', new Pair<Integer,Integer>(15,0));
		char2code.put('9', new Pair<Integer,Integer>(16,0));
		char2code.put('a', new Pair<Integer,Integer>(29,0));
		char2code.put('b', new Pair<Integer,Integer>(30,0));
		char2code.put('c', new Pair<Integer,Integer>(31,0));
		char2code.put('d', new Pair<Integer,Integer>(32,0));
		char2code.put('e', new Pair<Integer,Integer>(33,0));
		char2code.put('f', new Pair<Integer,Integer>(34,0));
		char2code.put('g', new Pair<Integer,Integer>(35,0));
		char2code.put('h', new Pair<Integer,Integer>(36,0));
		char2code.put('i', new Pair<Integer,Integer>(37,0));
		char2code.put('j', new Pair<Integer,Integer>(38,0));
		char2code.put('k', new Pair<Integer,Integer>(39,0));
		char2code.put('l', new Pair<Integer,Integer>(40,0));
		char2code.put('m', new Pair<Integer,Integer>(41,0));
		char2code.put('n', new Pair<Integer,Integer>(42,0));
		char2code.put('o', new Pair<Integer,Integer>(43,0));
		char2code.put('p', new Pair<Integer,Integer>(44,0));
		char2code.put('q', new Pair<Integer,Integer>(45,0));
		char2code.put('r', new Pair<Integer,Integer>(46,0));
		char2code.put('s', new Pair<Integer,Integer>(47,0));
		char2code.put('t', new Pair<Integer,Integer>(48,0));
		char2code.put('u', new Pair<Integer,Integer>(49,0));
		char2code.put('v', new Pair<Integer,Integer>(50,0));
		char2code.put('w', new Pair<Integer,Integer>(51,0));
		char2code.put('x', new Pair<Integer,Integer>(52,0));
		char2code.put('y', new Pair<Integer,Integer>(53,0));
		char2code.put('z', new Pair<Integer,Integer>(54,0));
		char2code.put(')', new Pair<Integer,Integer>(7,1));
		char2code.put('!', new Pair<Integer,Integer>(8,1));
		char2code.put('@', new Pair<Integer,Integer>(9,1));
		char2code.put('#', new Pair<Integer,Integer>(10,1));
		char2code.put('$', new Pair<Integer,Integer>(11,1));
		char2code.put('%', new Pair<Integer,Integer>(12,1));
		char2code.put('&', new Pair<Integer,Integer>(14,1));
		char2code.put('*', new Pair<Integer,Integer>(15,1));
		char2code.put('(', new Pair<Integer,Integer>(16,1));
		char2code.put('_', new Pair<Integer,Integer>(69,1));
		char2code.put('-', new Pair<Integer,Integer>(69,0));
		char2code.put('+', new Pair<Integer,Integer>(70,1));
		char2code.put('=', new Pair<Integer,Integer>(70,0));
		char2code.put('\"', new Pair<Integer,Integer>(75,1));
		char2code.put('\'', new Pair<Integer,Integer>(75,0));
		char2code.put('{', new Pair<Integer,Integer>(71,1));
		char2code.put('[', new Pair<Integer,Integer>(71,0));
		char2code.put('}', new Pair<Integer,Integer>(72,1));
		char2code.put(']', new Pair<Integer,Integer>(72,0));
		char2code.put(';', new Pair<Integer,Integer>(74,0));
		char2code.put(',', new Pair<Integer,Integer>(55,0));
		char2code.put('.', new Pair<Integer,Integer>(56,0));
		char2code.put('?', new Pair<Integer,Integer>(76,1));
		char2code.put('<', new Pair<Integer,Integer>(55,1));
		char2code.put('>', new Pair<Integer,Integer>(56,1));
		char2code.put('/', new Pair<Integer,Integer>(76,0));
		char2code.put(' ', new Pair<Integer,Integer>(62,0));
		
	}


}
