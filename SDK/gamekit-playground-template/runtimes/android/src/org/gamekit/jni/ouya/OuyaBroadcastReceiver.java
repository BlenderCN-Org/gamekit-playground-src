package org.gamekit.jni.ouya;

import org.gamekit.jni.OgreActivityJNI;

import tv.ouya.console.api.OuyaIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

public class OuyaBroadcastReceiver extends BroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
        if(intent.getAction().equals(OuyaIntent.ACTION_MENUAPPEARING)) {
        	OgreActivityJNI.pause();
        }
    }
}
