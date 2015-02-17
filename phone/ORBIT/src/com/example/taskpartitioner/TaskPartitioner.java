package com.example.taskpartitioner;

import Jama.*;

public class TaskPartitioner {
	
	double[][] Ab_array = {{0,0,0,0,0,0,},{0,0,0,0,0,1,},{0,0,0,0,1,0,},{0,0,0,0,1,1,},{0,0,0,1,0,0,},{0,0,0,1,0,1,},{0,0,0,1,1,0,},{0,0,0,1,1,1,},{0,0,1,0,0,0,},{0,0,1,0,0,1,},
			{0,0,1,0,1,0,},{0,0,1,0,1,1,},{0,0,1,1,0,0,},{0,0,1,1,0,1,},{0,0,1,1,1,0,},{0,0,1,1,1,1,},{0,1,0,0,0,0,},{0,1,0,0,0,1,},{0,1,0,0,1,0,},{0,1,0,0,1,1,},
			{0,1,0,1,0,0,},{0,1,0,1,0,1,},{0,1,0,1,1,0,},{0,1,0,1,1,1,},{0,1,1,0,0,0,},{0,1,1,0,0,1,},{0,1,1,0,1,0,},{0,1,1,0,1,1,},{0,1,1,1,0,0,},{0,1,1,1,0,1,},
			{0,1,1,1,1,0,},{0,1,1,1,1,1,},{1,0,0,0,0,0,},{1,0,0,0,0,1,},{1,0,0,0,1,0,},{1,0,0,0,1,1,},{1,0,0,1,0,0,},{1,0,0,1,0,1,},{1,0,0,1,1,0,},{1,0,0,1,1,1,},
			{1,0,1,0,0,0,},{1,0,1,0,0,1,},{1,0,1,0,1,0,},{1,0,1,0,1,1,},{1,0,1,1,0,0,},{1,0,1,1,0,1,},{1,0,1,1,1,0,},{1,0,1,1,1,1,},{1,1,0,0,0,0,},{1,1,0,0,0,1,},
			{1,1,0,0,1,0,},{1,1,0,0,1,1,},{1,1,0,1,0,0,},{1,1,0,1,0,1,},{1,1,0,1,1,0,},{1,1,0,1,1,1,},{1,1,1,0,0,0,},{1,1,1,0,0,1,},{1,1,1,0,1,0,},{1,1,1,0,1,1,},
			{1,1,1,1,0,0,},{1,1,1,1,0,1,},{1,1,1,1,1,0,},{1,1,1,1,1,1,}};
			
	double [] tb_array = { 22.624, 369.93, 1384.5, 1367.336, 93.01, 92.48};
	double [] tph_nexus_array = { 87 , 124.336 , 385.33 , 1089.23 , 15 , 14.95};
	double [] tph_LG_array = {211.7, 989.28, 871.29, 2372.46, 50, 51};
	double [] tph_GLX_array = { 96.4 , 338.38 , 296.31 , 719.6, 12.2, 13.5};
	double [] M_array = {1,1,2,1,2,2,2,1,2,2,3,2,2,2,2,1,2,2,3,2,3,3,3,2,2,2,3,2,2,2,2,1,1,1,2,1,2,2,2,1,2,2,3,2,2,2,2,1,1,1,2,1,2,2,2,1,1,1,2,1,1,1,1,0};
	/*
	 * Arduino
	 */
	double PsBoard = 296, 
			PpBoard = 296,
			Tcopy = 5;

	/*
	 *  Nexus
	 */
	double PsNexus = 6.66, //mW 6.66 = 1.8*3.7
			PpNexus = 1076.7,

			EcopyNexus = 5383.5,
			EwNexus = 271,
			EtNexus = 67,
			TwNexus = 752,
			TtNexus = 250,
			EwastedNexus = EwNexus + EtNexus + EcopyNexus;

	/*
	 * LG
	 */
	double PsLG = 1.1*3.7, //mW
			PpLG = 121*3.7,
	
			EcopyLG = PpLG*Tcopy,
			EwLG = 75.7,
			EtLG = 59.9,
			TwLG = 1100,
			TtLG = 50,
			EwastedLG = EwLG + EtLG + EcopyLG;

	/*
	 * GLX
	 */
	double PsGLX = 14*3.7,  //mW
			PpGLX = 211*3.7,

			EcopyGLX = PpGLX*Tcopy,
			EwGLX = 18.75,  				//???????????????????
			EtGLX = 6.47,  					//????????????????? 
			TwGLX = 1200,
			TtGLX = 10,
			EwastedGLX = EwGLX + EtGLX + EcopyGLX;

	/**
	 * General constants
	 */
	double PsPhone = PsNexus,
			PpPhone = PpNexus,
			Ewasted  = EwastedNexus;
	double [] tph_array = tph_nexus_array;
	//double Ps_all = [PsNexus PsLG PsGLX];
	//Pp_all = [PpNexus PpLG PpGLX];
	//Ewasted_all = [EwastedNexus EwastedLG EwastedGLX];	public Matrix test() {
		
	public Matrix test() {
	double[][] array = {{1.,2.,3},{4.,5.,6.},{7.,8.,10.}};
		Matrix A = new Matrix(array);
		Matrix b = Matrix.random(3,1);
		Matrix x = A.solve(b);
		Matrix Residual = A.times(x).minus(b);
		double rnorm = Residual.normInf();
		
		return Residual;
	}
	
	public double solvePartitioning() {
		Matrix Ab = new Matrix(Ab_array);
		
		Matrix Ones = new Matrix(Ab.getRowDimension(), Ab.getColumnDimension(), 1);
		Matrix Aph = Ones.minus(Ab);
		Matrix Eb = new Matrix(Ab.getRowDimension(),1);
		Matrix Eph = new Matrix(Ab.getRowDimension(),1);
		Matrix tb = new Matrix( tb_array,1);
		Matrix tph = new Matrix(tph_array, 1);
		Matrix M = new Matrix(M_array,1);
		
		Eb = Ab.times(tb.transpose()).times(PsPhone);
		Eph = Aph.times(tph.transpose()).times(PpPhone);
	
		Matrix E = Eb.plus(Eph).plus(M.times(Ewasted).transpose());
		E.timesEquals(0.001);   // mJ --> J
		
		Matrix T =  Ab.times(tb.transpose()).plus( M.transpose().times(Tcopy));
		
		/** apply constraint and find minimum energy **/
		double minE = E.get(0, 0);
		int minIdx = 0;
		for(int i=0;  i<E.getRowDimension(); i++ ) {
			if (T.get(i, 0) > 1000)
				E.set(i, 0, 500000000);
			
			if (E.get(i, 0) < minE ) {
				minE = E.get(i, 0);
				minIdx = i;
			}
		}
		
		
		return minE;
	}
	
	public String print_matrix (Matrix M) {
		
		return "Col: " + M.getColumnDimension() + " Row: "+ M.getRowDimension();
		
		
	}
	
	public double solvePartitioning_sclable(int numOfTasks) {
		Matrix Ab = Matrix.random(2^numOfTasks, numOfTasks);
		Matrix Ones = new Matrix(Ab.getRowDimension(), Ab.getColumnDimension(), 1);
		Matrix Aph = Ones.minus(Ab);
		Matrix Eb = new Matrix(Ab.getRowDimension(),1);
		Matrix Eph = new Matrix(Ab.getRowDimension(),1);
		Matrix tb = Matrix.random(1, numOfTasks);
		Matrix tph = Matrix.random(1, numOfTasks );
		Matrix M = new Matrix(1, 2^numOfTasks);
		
		Eb = Ab.times(tb.transpose()).times(PsPhone);
		Eph = Aph.times(tph.transpose()).times(PpPhone);
	
		Matrix E = Eb.plus(Eph).plus(M.times(Ewasted).transpose());
		E.timesEquals(0.001);   // mJ --> J
		
		Matrix T =  Ab.times(tb.transpose()).plus( M.transpose().times(Tcopy));
		
		/** apply constraint and find minimum energy **/
		double minE = E.get(0, 0);
		int minIdx = 0;
		for(int i=0;  i<E.getRowDimension(); i++ ) {
			if (T.get(i, 0) > 1000)
				E.set(i, 0, 500000000);
			
			if (E.get(i, 0) < minE ) {
				minE = E.get(i, 0);
				minIdx = i;
			}
		}
		
		
		return minE;
	}


}

