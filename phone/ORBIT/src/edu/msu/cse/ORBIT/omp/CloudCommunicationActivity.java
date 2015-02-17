package edu.msu.cse.ORBIT.omp;

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.ViewGroup.LayoutParams;

public class CloudCommunicationActivity extends Activity {
	
	private static final String TAG = "SIFTExampleActivity";
	
//	Preview mPreview; 
//	ResultView mResultView;
	private Context mContext = this;
	
	/** PLEASE PUT YOUR SERVER URL **/
	private final String SERVERURL = "";
	
	private final static String INPUT_IMG_FILENAME = "/temp.jpg"; //name for storing image captured by camera view
	
	//flag to check if camera is ready for capture
	private boolean mCameraReadyFlag = true;
		 
    // Called when the activity is first created. 
    @Override
    public void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);
        
        //make the screen full screen
//        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
//    			WindowManager.LayoutParams.FLAG_FULLSCREEN);
        //remove the title bar
        //requestWindowFeature(Window.FEATURE_NO_TITLE);
        
//        mResultView=new ResultView(this);
//        mPreview = new Preview(this);
        
        //set Content View as the preview
//        setContentView(mPreview);
        
        //add result view  to the content View
//        addContentView(mResultView,new LayoutParams(LayoutParams.WRAP_CONTENT,LayoutParams.WRAP_CONTENT));
        
        //set the orientation as landscape
//        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        
        Log.d(TAG, "oncreate");
		//** Send image and offload image processing task to server by starting async task ** 
        Intent intent = getIntent();
        Bundle bundle = intent.getExtras();
        byte[] dataBytes = bundle.getByteArray("image");
        CloudCommunication task = new CloudCommunication();
		task.execute( dataBytes);
		
    }
    
    // Async task to send to server
public class CloudCommunication  extends AsyncTask<byte[], Integer , Void>
{
	private String TAG = this.getClass().getSimpleName();
	public byte[] dataToServer;
	
	/** PLEASE PUT YOUR SERVER URL **/
	private final String SERVERURL = "";
			
	//Task state
	private final int UPLOADING_PHOTO_STATE  = 0;
	private final int SERVER_PROC_STATE  = 1;
	
	private ProgressDialog dialog;
	
	//upload photo to server
	
	HttpURLConnection uploadByteArray(byte[] databytes)
	{
		Log.d(TAG, "uploadByteArray");
		
		final String lineEnd = "\r\n";
		final String twoHyphens = "--";
		final String boundary = "*****";
		
		try
		{
			URL url = new URL(SERVERURL);
			// Open a HTTP connection to the URL
			final HttpURLConnection conn = (HttpURLConnection)url.openConnection();
			// Allow Inputs
			conn.setDoInput(true);				
			// Allow Outputs
			conn.setDoOutput(true);				
			// Don't use a cached copy.
			conn.setUseCaches(false);
			
			// Use a post method.
			conn.setRequestMethod("POST");
			conn.setRequestProperty("Connection", "Keep-Alive");
			conn.setRequestProperty("Content-Type", "multipart/form-data;boundary="+boundary);
			
			DataOutputStream dos = new DataOutputStream( conn.getOutputStream() );
			
			dos.writeBytes(twoHyphens + boundary + lineEnd);
//			dos.writeBytes("Content-Disposition: form-data; name=\"uploadedfile\";filename=\"" + serverFileName +"\"" + lineEnd);
			dos.writeBytes(lineEnd);

			// create a buffer of maximum size
			int bytesAvailable = databytes.length;
			int maxBufferSize = 1024;
			int bufferSize = Math.min(bytesAvailable, maxBufferSize);
						
			int offset =0;
			while(offset < databytes.length)
			{				
				dos.write(databytes, offset, bufferSize);
				offset +=bufferSize;
			}
			
			// send multipart form data after file data...
			dos.writeBytes(lineEnd);
			dos.writeBytes(twoHyphens + boundary + twoHyphens + lineEnd);
			publishProgress(SERVER_PROC_STATE);
			dos.flush();
			
			return conn;
		}
		catch (MalformedURLException ex){
			Log.e(TAG, "error: " + ex.getMessage(), ex);
			return null;
		}
		catch (IOException ioe){
			Log.e(TAG, "error: " + ioe.getMessage(), ioe);
			return null;
		}
		
	}
	
	HttpURLConnection uploadPhoto(FileInputStream fileInputStream)
	{
		
		final String serverFileName = "test"+ (int) Math.round(Math.random()*1000) + ".jpg";		
		final String lineEnd = "\r\n";
		final String twoHyphens = "--";
		final String boundary = "*****";
		
		try
		{
			URL url = new URL(SERVERURL);
			// Open a HTTP connection to the URL
			final HttpURLConnection conn = (HttpURLConnection)url.openConnection();
			// Allow Inputs
			conn.setDoInput(true);				
			// Allow Outputs
			conn.setDoOutput(true);				
			// Don't use a cached copy.
			conn.setUseCaches(false);
			
			// Use a post method.
			conn.setRequestMethod("POST");
			conn.setRequestProperty("Connection", "Keep-Alive");
			conn.setRequestProperty("Content-Type", "multipart/form-data;boundary="+boundary);
			
			DataOutputStream dos = new DataOutputStream( conn.getOutputStream() );
			
			dos.writeBytes(twoHyphens + boundary + lineEnd);
			dos.writeBytes("Content-Disposition: form-data; name=\"uploadedfile\";filename=\"" + serverFileName +"\"" + lineEnd);
			dos.writeBytes(lineEnd);

			// create a buffer of maximum size
			int bytesAvailable = fileInputStream.available();
			int maxBufferSize = 1024;
			int bufferSize = Math.min(bytesAvailable, maxBufferSize);
			byte[] buffer = new byte[bufferSize];
			
			// read file and write it into form...
			int bytesRead = fileInputStream.read(buffer, 0, bufferSize);
			
			while (bytesRead > 0)
			{
				dos.write(buffer, 0, bufferSize);
				bytesAvailable = fileInputStream.available();
				bufferSize = Math.min(bytesAvailable, maxBufferSize);
				bytesRead = fileInputStream.read(buffer, 0, bufferSize);
			}
			
			// send multipart form data after file data...
			dos.writeBytes(lineEnd);
			dos.writeBytes(twoHyphens + boundary + twoHyphens + lineEnd);
			publishProgress(SERVER_PROC_STATE);
			// close streams
			fileInputStream.close();
			dos.flush();
			
			return conn;
		}
		catch (MalformedURLException ex){
			Log.e(TAG, "error: " + ex.getMessage(), ex);
			return null;
		}
		catch (IOException ioe){
			Log.e(TAG, "error: " + ioe.getMessage(), ioe);
			return null;
		}
	}
	
    //get image result from server and display it in result view
	void getResultImage(HttpURLConnection conn){		
		// retrieve the response from server
		InputStream is;
		try {
			is = conn.getInputStream();
			//get result image from server
	        //mResultView.resultImage = BitmapFactory.decodeStream(is);
			Bitmap resultImage = BitmapFactory.decodeStream(is);
			if (resultImage != null)
				Log.d(TAG, "resulting image from server received, size= " 
							+ resultImage.getHeight() + "*" + resultImage.getWidth() );
	        is.close();		        
	        //mResultView.IsShowingResult = true;	        
		} catch (IOException e) {
			Log.e(TAG,e.toString());
			e.printStackTrace();
		}
	}
	
	//Main code for processing image algorithm on the server
	
	void uploadData(String inputDataPath){			
		publishProgress(UPLOADING_PHOTO_STATE);
		File inputFile = new File(inputDataPath);
		try {
			
			Log.d(TAG, " uploading file " + inputDataPath);
			//create file stream for captured image file
			FileInputStream fileInputStream  = new FileInputStream(inputFile);
	    	
			//upload photo
	    	final HttpURLConnection  conn = uploadPhoto(fileInputStream);
	    	
	    	//get processed photo from server
	    	if (conn != null){
	    	getResultImage(conn);}
			fileInputStream.close();
		}
        catch (FileNotFoundException ex){
        	Log.e(TAG, ex.toString());
        }
        catch (IOException ex){
        	Log.e(TAG, ex.toString());
        }
	}
	
    public CloudCommunication() {
        dialog = new ProgressDialog(mContext);
    }		
	
    protected void onPreExecute() {
        this.dialog.setMessage("data received");
        this.dialog.show();
    }
	@Override
	protected Void doInBackground(byte[]... params) {			//background operation 
        Log.d(TAG, "doInBackground");

		//String uploadFilePath = params[0];
		byte[] uploadFileData = params[0];
		//uploadData(uploadFilePath);
		if(!this.SERVERURL.equals(""))
			uploadByteArray(uploadFileData);
		//release camera when previous image is processed
//		mCameraReadyFlag = true; 
		return null;
	}		
	//progress update, display dialogs
	@Override
     protected void onProgressUpdate(Integer... progress) {
    	 if(progress[0] == UPLOADING_PHOTO_STATE){
    		 if(!this.SERVERURL.equals("")) 
    			 dialog.setMessage("Uploading");
    		 else
    			 dialog.setMessage("Set the URL to the server");
    		 dialog.show();
    	 }
    	 else if (progress[0] == SERVER_PROC_STATE){
	           if (dialog.isShowing()) {
	               dialog.dismiss();
	           }	    	 
    		 dialog.setMessage("Processing");
    		 dialog.show();
    	 }	         
     }		
       @Override
       protected void onPostExecute(Void param) {
           if (dialog.isShowing()) {
               dialog.dismiss();
           }
       }
}// end of asynchtask
}