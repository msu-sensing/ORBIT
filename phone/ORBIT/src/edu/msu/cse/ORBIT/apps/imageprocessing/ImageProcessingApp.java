package edu.msu.cse.ORBIT.apps.imageprocessing;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.util.Vector;

import edu.msu.cse.ORBIT.omp.CloudCommunicationActivity;
import mpi.cbg.fly.Feature;
import mpi.cbg.fly.SIFT;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

public class ImageProcessingApp extends Activity {
int TAKE_PHOTO_CODE = 0;
public static int count=0;

private static final int PICTURE_RESULT = 9;

private static final int OK = 0;
private static final int MEMORY_ERROR = 1;
private static final int ERROR = 2;

private Bitmap mPicture;
private ImageView mView;
private final String dir = Environment.getExternalStorageDirectory() + "/ORBIT/pics/"; 

private ProgressDialog mProgressDialog;
private String tag = this.getClass().getSimpleName();

/** Called when the activity is first created. */
@Override
public void onCreate(Bundle savedInstanceState)
{
    super.onCreate(savedInstanceState);
    setContentView(R.layout.main_takepicture);

    //making a folder to store pics taken by the camera
       
        File newdir = new File(dir); 
        newdir.mkdirs();
       
	    Button capture = (Button) findViewById(R.id.btnCapture);
	    capture.setOnClickListener(new View.OnClickListener() {
	        public void onClick(View v) {
	
	            // here,counter will be incremented each time,and the picture taken by camera will be stored as 1.jpg,2.jpg and likewise.
	            count++;
	            String file = dir+count+".jpg";
	            File newfile = new File(file);
	            try {
	                newfile.createNewFile();
	            } catch (IOException e) {}       
	
	            Uri outputFileUri = Uri.fromFile(newfile);
	
	            Intent cameraIntent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE); 
	            cameraIntent.putExtra(MediaStore.EXTRA_OUTPUT, outputFileUri);
	            
	            startActivityForResult(cameraIntent, TAKE_PHOTO_CODE);
	        }
	    });
    
	mView = (ImageView) findViewById(R.id.view);
	
}

private void load_and_process(String path) {
	// Free the data of the last picture
	
	if(mPicture != null)
		mPicture.recycle();
	
	boolean from_memory = true;
	
	if (from_memory) {
        File imgFile = new File(path);
        if(imgFile.exists())
        {
        	//Decode with inSampleSize
            BitmapFactory.Options option = new BitmapFactory.Options();
            option.inSampleSize=4;  // downsampling size should be power of 2
            
            Bitmap myBitmap = BitmapFactory.decodeFile(imgFile.getAbsolutePath(), option);  
            mPicture = myBitmap.copy(myBitmap.getConfig(), true);
			myBitmap.recycle();
			
            mView.setImageBitmap(mPicture);
        }
        else                    
            Log.e("SIFT" , "no IMAGE IS PRESENT'");
	}
	
	// process SIFT algorithm on the picture
	if(mPicture.getHeight()*mPicture.getWidth() > 1000*700)
		sendToCloud();
	else
		processAndDrawSIFT();
	
	
	Log.d(tag , "size = " + mPicture.getHeight() + "*" + mPicture.getWidth());
}

private void processAndDrawSIFT() {
	// show the dialog
	mProgressDialog = ProgressDialog.show(this, "Please wait",
			"Processing of SIFT Algorithm...");

	new Thread(new Runnable() {

		@Override
		public void run() {
			Message msg = null;

			try {
				Log.e("SIFT", ": conveting : " + System.currentTimeMillis());
				
				// convert bitmap to pixels table
				int pixels[] = toPixelsTab(mPicture);
				
				Log.e("SIFT", ": getting features : " + System.currentTimeMillis());
				// get the features detected into a vector
				Vector<Feature> features = SIFT.getFeatures(mPicture.getWidth(), mPicture.getHeight(), pixels);
				
				Log.e("SIFT", ": drawing : " + System.currentTimeMillis());
				// draw features on bitmap
				Canvas c = new Canvas(mPicture);
				for (Feature f : features) {
					drawFeature(c, f.location[0], f.location[1], f.scale,
							f.orientation);
				}
				Log.e("SIFT", ": done : " + System.currentTimeMillis());
				
				msg = mHandler.obtainMessage(OK);
			} catch (Exception e) {
				e.printStackTrace();
				msg = mHandler.obtainMessage(ERROR);
			} catch (OutOfMemoryError e) {
				msg = mHandler.obtainMessage(MEMORY_ERROR);
			} finally {
				// send the message
				if (msg == null) {
					msg = new Message();
					Bundle data = new Bundle();
					data.putCharSequence("msg", "empty message");
					msg.setData(data);
				}
				
				mHandler.sendMessage(msg);
			}
		}

	}).start();
}

@Override
protected void onActivityResult(int requestCode, int resultCode, Intent data) {
    super.onActivityResult(requestCode, resultCode, data);

    if (requestCode == TAKE_PHOTO_CODE && resultCode == RESULT_OK) {
//    	Bundle bundle = data.getExtras();
//    	Uri savedFileUri = (Uri) bundle.get(MediaStore.EXTRA_OUTPUT);
    	
        Log.d("CameraDemo", "Pic saved "); 
        
    	String filePath = dir+count+".jpg";
    	load_and_process(filePath);
    }
}

private int[] toPixelsTab(Bitmap picture) {
	int width = picture.getWidth();
	int height = picture.getHeight();
	
	int[] pixels = new int[width * height];
	// copy pixels of picture into the tab
	picture.getPixels(pixels, 0, picture.getWidth(), 0, 0, width, height);

	// On Android, Color are coded in 4 bytes (argb),
	// whereas SIFT needs color coded in 3 bytes (rgb)

	for (int i = 0; i < (width * height); i++)
		pixels[i] &= 0x00ffffff;

	return pixels;
}

public void drawFeature(Canvas c, float x, float y, double scale,
		double orientation) {
	Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
	
	// line too small...
	scale *= 6.;

	double sin = Math.sin(orientation);
	double cos = Math.cos(orientation);

	paint.setStrokeWidth(2f);
	paint.setColor(Color.GREEN);
	c.drawLine((float) x, (float) y, (float) (x - (sin - cos) * scale),
			(float) (y + (sin + cos) * scale), paint);

	paint.setStrokeWidth(4f);
	paint.setColor(Color.YELLOW);
	c.drawPoint(x, y, paint);
}

private final Handler mHandler = new Handler() {

	@Override
	public void handleMessage(Message msg) {
		AlertDialog.Builder builder;

		switch (msg.what) {
		case OK:
			// set the picture with features drawed
			mView.setImageBitmap(mPicture);
			break;
		case MEMORY_ERROR:
			builder = new AlertDialog.Builder(ImageProcessingApp.this);
			builder.setMessage("Picture too big \n Will send it to cloud");
			builder.setPositiveButton("Ok", null);
			builder.show();
			
			Log.d("msg handler", "sending to cloud ...");
			sendToCloud();
			break;
		case ERROR:
			builder = new AlertDialog.Builder(ImageProcessingApp.this);
			builder.setMessage("Error during the process.");
			builder.setPositiveButton("Ok", null);
			builder.show();
			break;
		}
		mProgressDialog.dismiss();
//		Log.d("msg handler 2", "sending to cloud ...");
//		sendToCloud();
	}

};

private void sendToCloud() {
	Intent intent = new Intent(this, CloudCommunicationActivity.class);
	
	ByteArrayOutputStream stream = new ByteArrayOutputStream();
	mPicture.compress(Bitmap.CompressFormat.JPEG, 20, stream);	 //compress to be able to cross the binder btw activities
	byte[] byteArray = stream.toByteArray();
	//mPicture.recycle();
	Log.d(tag, "bytearray size=" + byteArray.length);
	intent.putExtra("image", byteArray);
	startActivity(intent);
}

public void onDestroy() {
	super.onDestroy();
	if (mPicture != null) {
		mPicture.recycle();
	}
}


}