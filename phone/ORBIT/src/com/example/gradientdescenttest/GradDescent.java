package com.example.gradientdescenttest;

/**
 * Implementation of gradient descent alg.
 * Ref: AI-Class.com, Week 3, Topic 5.31.
 *
 * By MichaelFromGalway, Oct 2011.
 * Further details: see http://MichaelFromGalway.wordpress.com.
 *
 * This code is may be used freely without restriction,
 * though attribution of my authorship would be appreciated.
 *
 */
 
// This program uses JMathPlot, a package for producing Matlab-style graphs
//   Get it at http://code.google.com/p/jmathplot/
//   or just delete the blocks of code that use it.
//import org.math.plot.*;

import android.util.Log;
 
public class GradDescent
{
   // data taken from one of the worked examples
   // Note: for this data, correct answers are w0=0.5, w1=0.9.
   static double[] x = {2, 4, 6, 8, 10 , 12, 14};
   static double[] y = {2, 5, 5, 8 , -1 , 3 , 7};
 
   static int trendline; // handle used for adding/removing trendline
 
   // parameters
   public static double w0;
   public static double w1;
 
   public double[] run_wo_plot(double alpha, double tol, int maxiter, int dispiter)
   {
	   boolean default_setting = false;
      // Algorithm settings
	   if (default_setting) 
	   {
	       alpha = 0.01;  // learning rate
	       tol = 1e-11;   // tolerance to determine convergence
	       maxiter = 5000;   // maximum number of iterations (in case convergence is not reached)
	       dispiter = 100;   // interval for displaying results during iterations
	   }
 
      // Other parameters
      double delta0, delta1;
      int iters = 0;
 
      // initial guesses for parameters
      w0 = 0;
      w1 = 0;
 
      // keep track of results so I can plot convergence
      double[] w0plot = new double[maxiter+1];
      double[] w1plot = new double[maxiter+1];
      double[] tplot = new double[maxiter+1];

 
      do {
         delta1 = alpha * dLdw1();
         delta0 = alpha * dLdw0();
 
         // Store data for plotting
         tplot[iters] = iters;
         w0plot[iters] = w0;
         w1plot[iters] = w1;
 
         iters++;
         w1 -= delta1;
         w0 -= delta0;
 
         // display progress
         if (iters % dispiter == 0) {
            System.out.println("Iteration " + iters + ": w0=" + w0 + " - " + delta0 + ", w1=" + w1 + " - "+ delta1);
         }
 
         if (iters > maxiter) break;
      } while (Math.abs(delta1) > tol || Math.abs(delta0) > tol);
 
      Log.d("Gradient without plot ", "\nConvergence after " + iters + " iterations: w0=" + w0 + ", w1=" + w1);
      return w1plot;
   }
   
//   public void run_w_plot()
//   {
//      // Algorithm settings
//      double alpha = 0.01;  // learning rate
//      double tol = 1e-11;   // tolerance to determine convergence
//      int maxiter = 9000;   // maximum number of iterations (in case convergence is not reached)
//      int dispiter = 100;   // interval for displaying results during iterations
// 
//      // Other parameters
//      double delta0, delta1;
//      int iters = 0;
// 
//      // initial guesses for parameters
//      w0 = 0;
//      w1 = 0;
// 
//      // keep track of results so I can plot convergence
//      double[] w0plot = new double[maxiter+1];
//      double[] w1plot = new double[maxiter+1];
//      double[] tplot = new double[maxiter+1];
// 
//      // plot the data
//      // create a PlotPanel
////      Plot2DPanel plot = new Plot2DPanel();
// 
//      // add a line plot to the PlotPanel
////      double[][] xy = {x,y};
////      plot.addLinePlot("X-Y", x, y);
////      plot.addLinePlot("X-Y", xy);
// 
//      // show the trendline
////      addTrendline(plot, false);
// 
//      // put the PlotPanel in a JFrame, as a JPanel
////      JFrame frame = new JFrame("Original X-Y Data");
////      frame.setContentPane(plot);
////      frame.setSize(600, 600);
////      frame.setVisible(true);
// 
//      do {
//         delta1 = alpha * dLdw1();
//         delta0 = alpha * dLdw0();
// 
//         // Store data for plotting
//         tplot[iters] = iters;
//         w0plot[iters] = w0;
//         w1plot[iters] = w1;
// 
//         iters++;
//         w1 -= delta1;
//         w0 -= delta0;
// 
//         // display progress
//         if (iters % dispiter == 0) {
//            System.out.println("Iteration " + iters + ": w0=" + w0 + " - " + delta0 + ", w1=" + w1 + " - "+ delta1);
////            addTrendline(plot, true);
//         }
// 
//         if (iters > maxiter) break;
//      } while (Math.abs(delta1) > tol || Math.abs(delta0) > tol);
// 
//      System.out.println("\nConvergence after " + iters + " iterations: w0=" + w0 + ", w1=" + w1);
// 
////      addTrendline(plot, false);
// 
//      // Before plotting the data, extract an array of the right size from it
//      double[] w0plot2 = new double[iters];
//      double[] w1plot2 = new double[iters];
//      double[] tplot2 = new double[iters];
//      System.arraycopy(w0plot, 0, w0plot2, 0, iters);
//      System.arraycopy(w1plot, 0, w1plot2, 0, iters);
//      System.arraycopy(tplot, 0, tplot2, 0, iters);
// 
//      // Plot the convergence of data
//      Plot2DPanel convPlot = new Plot2DPanel();
// 
//      // add a line plot to the PlotPanel
//      double[][] tw0 = {tplot2, w0plot2};
//
//      double[][] tw1 = {tplot2, w1plot2};
//      
//      convPlot.addLinePlot("w0", tw0);
//      convPlot.addLinePlot("w1", tw1);
//      
////      convPlot.addLinePlot("w0", tplot2, w0plot2);
////      convPlot.addLinePlot("w1", tplot2, w1plot2);
// 
//      // put the PlotPanel in a JFrame, as a JPanel
////      JFrame frame2 = new JFrame("Convergence of parameters over time");
////      frame2.setContentPane(convPlot);
////      frame2.setSize(600, 600);
////      frame2.setVisible(true);
// 
//      // Commented out System.exit() so that plots don't vanish
//      // System.exit(0);
//   }
 
   public static double dLdw1()
   {
      double sum = 0;
 
      for (int j=0; j<x.length; j++) {
         sum += (y[j] - f(x[j])) * x[j];
      }
      return -2 * sum / x.length;
   }
 
   public static double dLdw0()
   {
      double sum = 0;
 
      for (int j=0; j<x.length; j++) {
         sum += y[j] - f(x[j]);
      }
      return -2 * sum / x.length;
   }
 
   public static double f(double x)
   {
      return w1*x + w0;
   }
 
//   public static void addTrendline(Plot2DPanel plot, boolean removePrev)
//   {
//      if (removePrev)
//      plot.removePlot(trendline);
// 
//      double[] yEnd = new double[x.length];
//      for (int i=0; i<x.length; i++)
//      yEnd[i] = f(x[i]);
//      double[][] x_yEnd = {x, yEnd};
//      trendline = plot.addLinePlot("final",x_yEnd);
//   }
}