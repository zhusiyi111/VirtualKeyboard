#include "Solution.h"
#include<map>
int click_number=0,click_number_max=3;
float XX[10]={0};
float YY[10]={0};


Solution::Solution(void)
{
}


Solution::~Solution(void)
{
}






void on_mouse( int event, int x, int y, int flags, void* ustc)
{

	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, CV_AA);

	if( event == CV_EVENT_LBUTTONDOWN )
	{

		Point pt = cvPoint(x,y);
		printf("第%d个点x=%d,y=%d\n",click_number+1,pt.y,pt.x);
		YY[click_number]=pt.x;
		XX[click_number]=pt.y;
		click_number++;

	} 
	if(click_number>click_number_max)
	{
		cvDestroyWindow("请点击i个点");
	}
}

//init函数

void Solution::Four_Point_init(Mat img)
{
	Mat imGG=img.clone();
	imshow("请点击i个点",imGG);
	cvSetMouseCallback( "请点击i个点",on_mouse, 0 );	
	cvWaitKey(0); 
}



void Solution::Four_Point_Past_init()
{
	for(int i = 0 ; i <= click_number_max ; i++)
	{
		Point pt;
		pt.x = XX[i];
		pt.y = YY[i];
		Four_Point_Past.push_back(pt);
	}
}


int Y_warp[1000][1000]={0};
int X_warp[1000][1000]={0};
void Solution::XY_warp_init(Mat img,Mat warp_mat)
{


	int i,j;
	Mat C = (Mat_<double>(3,1) ); 
	for(i=0;i<img.rows;i++)
	{
		for(j=0;j<img.cols;j++)
		{
			C.at<double>(0,0)=j;
			C.at<double>(1,0)=i;
			C.at<double>(2,0)=1;
			C=warp_mat*C;
			Y_warp[i][j]=(C.at<double>(0,0))/(C.at<double>(2,0));
			X_warp[i][j]=(C.at<double>(1,0))/(C.at<double>(2,0));
			if(X_warp[i][j]<1){X_warp[i][j]=0;}
			if(X_warp[i][j]>img.rows-1){X_warp[i][j]=img.rows-1;}
			if(Y_warp[i][j]<1){Y_warp[i][j]=0;}
			if(Y_warp[i][j]>img.cols-1){Y_warp[i][j]=img.cols-1;}

		}
	}

	for(i=0;i<img.rows;i++)
	{
		vector<Point> A;
		for(j=0;j<img.cols;j++)
		{	
			Point pt(X_warp[i][j],Y_warp[i][j]);
			A.push_back(pt);
		}
		Solution::XY_warp.push_back(A);
	}
	
}











vector<Point> Solution::get_Four_Point_Past()
{
	return Four_Point_Past;
}




/*---------------------------------------

函数作用：计算img的畸变校正矩阵warp_mat
输入值：val是变换前四点的vector，img提供变换后的四点
输出值：透视变换矩阵warp_mat

------------------------------------------*/
Mat Solution::calculate_warp_mat(vector<Point> val,Mat img)
{
	vector<Point2f> corners(4);
	for(int i = 0 ; i < 4 ; i++ )
	{
		corners[i]=Point2f(val[i].y,val[i].x);
	}

	vector<Point2f> corners_trans(4);  
	corners_trans[0] = Point2f(0,0);  
	corners_trans[1] = Point2f(img.cols,0);  
	corners_trans[2] = Point2f(0,img.rows);  
	corners_trans[3] = Point2f(img.cols,img.rows);  

	Mat warp_mat= getPerspectiveTransform(corners,corners_trans);  
	return warp_mat;
}

void Solution::warp_transform(Mat img,Mat &src,Mat warp_mat,cv::Size _size)
{
	warpPerspective( img, src, warp_mat, img.size());
}




void Solution::GetSkin(Mat &img)
{
	cvtColor(img,img,CV_BGR2YCrCb);
	vector<Mat> YCRCB;
	split(img,YCRCB);

	
	uchar *CR,*CB; 
	for( int i = 0; i < img.rows; ++i)
	{

		CR = YCRCB[1].ptr<uchar>(i);
		CB = YCRCB[2].ptr<uchar>(i);

		for ( int j = 0; j < img.cols; ++j)
		{
			if(	 CR[j]>=130 && CR[j]<=173
				&& CB[j]>=77 && CB[j]<=127
				)
			{

				CR[j]=0;
				CB[j]=255;

			}



		}
	}
	merge(YCRCB,img);  
}

void Solution:: GetSkin(Mat img,Mat &src)
{
	
	cvtColor(img,img,CV_BGR2YCrCb);
	vector<Mat> YCRCB;
	split(img,YCRCB);
	uchar *CR,*CB,*SRC; 
	for( int i = 0; i < img.rows; ++i)
	{
		SRC = src.ptr<uchar>(i);
		CR = YCRCB[1].ptr<uchar>(i);
		CB = YCRCB[2].ptr<uchar>(i);

		for ( int j = 0; j < img.cols; ++j)
		{
			if(	 CR[j]>=130 && CR[j]<=173
				&& CB[j]>=77 && CB[j]<=127
				)
			{
				SRC[j] = 0;
			}
			else
			{
				SRC[j] = 255;
			}



		}
	}
	merge(YCRCB,img);  
}


void Solution::transform_skin_warp(Mat &img)
{
	uchar *p; 
	for( int i = 0; i < img.rows; ++i)
	{
		p = img.ptr<uchar>(i);

		for ( int j = 0; j < img.cols; ++j)
		{
			if(	 p[j]==0 )
			{
				uchar *aa;
				aa=img.ptr<uchar>(XY_warp[i][j].x);
				aa[XY_warp[i][j].y]=1;
				p[j]=255;
			}



		}
	} 
}





/*---------------------------------------

函数作用：计算键盘键位区域
输入值：img
输出值：函数内改变本文件公共数组Key_Range[]的值

------------------------------------------*/
void Solution::area_of_key_init(Mat img)
{
	int a=10;
	Key_Range_Begin.resize(58);											Key_Range_End.resize(58);
	Key_Range_Begin[0]=Point(0+a,img.rows/10+a);						Key_Range_End[0]=Point(img.cols/15-a,img.rows/5-a);		//`
	Key_Range_Begin[1]=Point(img.cols/15+a,img.rows/10+a);				Key_Range_End[1]=Point(img.cols*2/15-a,img.rows/5-a);		//1
	Key_Range_Begin[2]=Point(img.cols*2/15+a,img.rows/10+a);			Key_Range_End[2]=Point(img.cols*3/15-a,img.rows/5-a);		//2
	Key_Range_Begin[3]=Point(img.cols*3/15+a,img.rows/10+a);			Key_Range_End[3]=Point(img.cols*4/15-a,img.rows/5-a);		//3
	Key_Range_Begin[4]=Point(img.cols*4/15+a,img.rows/10+a);			Key_Range_End[4]=Point(img.cols*5/15-a,img.rows/5-a);		//4
	Key_Range_Begin[5]=Point(img.cols*5/15+a,img.rows/10+a);			Key_Range_End[5]=Point(img.cols*6/15-a,img.rows/5-a);		//5
	Key_Range_Begin[6]=Point(img.cols*6/15+a,img.rows/10+a);			Key_Range_End[6]=Point(img.cols*7/15-a,img.rows/5-a);		//6
	Key_Range_Begin[7]=Point(img.cols*7/15+a,img.rows/10+a);			Key_Range_End[7]=Point(img.cols*8/15-a,img.rows/5-a);		//7
	Key_Range_Begin[8]=Point(img.cols*8/15+a,img.rows/10+a);			Key_Range_End[8]=Point(img.cols*9/15-a,img.rows/5-a);		//8
	Key_Range_Begin[9]=Point(img.cols*9/15+a,img.rows/10+a);			Key_Range_End[9]=Point(img.cols*10/15-a,img.rows/5-a);		//9
	Key_Range_Begin[10]=Point(img.cols*10/15+a,img.rows/10+a);			Key_Range_End[10]=Point(img.cols*11/15-a,img.rows/5-a);		//0
	Key_Range_Begin[11]=Point(img.cols*11/15+a,img.rows/10+a);			Key_Range_End[11]=Point(img.cols*12/15-a,img.rows/5-a);		//-
	Key_Range_Begin[12]=Point(img.cols*12/15+a,img.rows/10+a);			Key_Range_End[12]=Point(img.cols*13/15-a,img.rows/5-a);		//=
	Key_Range_Begin[13]=Point(img.cols*13/15+a,img.rows/10+a);			Key_Range_End[13]=Point(img.cols*14/15-a,img.rows/5-a);		//\   
	Key_Range_Begin[14]=Point(img.cols*14/15+a,img.rows*1/10+a);		Key_Range_End[14]=Point(img.cols-a,img.rows*1/5-a);			//退格





	Key_Range_Begin[15]=Point(0+a,img.rows*3/10+a);						Key_Range_End[15]=Point(0.093*img.cols-a,img.rows*2/5-a);	//Tab （\t)
	Key_Range_Begin[16]=Point(0.093*img.cols+a,img.rows*3/10+a);		Key_Range_End[16]=Point(0.158*img.cols-a,img.rows*2/5-a);    //Q
	Key_Range_Begin[17]=Point(0.158*img.cols+a,img.rows*3/10+a);		Key_Range_End[17]=Point(0.230*img.cols-a,img.rows*2/5-a);	//W
	Key_Range_Begin[18]=Point(0.230*img.cols+a,img.rows*3/10+a);	    Key_Range_End[18]=Point(0.295*img.cols-a,img.rows*2/5-a);	//E
	Key_Range_Begin[19]=Point(0.295*img.cols+a,img.rows*3/10+a);		Key_Range_End[19]=Point(0.360*img.cols-a,img.rows*2/5-a);	//R
	Key_Range_Begin[20]=Point(0.360*img.cols+a,img.rows*3/10+a);		Key_Range_End[20]=Point(0.428*img.cols-a,img.rows*2/5-a);	//T
	Key_Range_Begin[21]=Point(0.428*img.cols+a,img.rows*3/10+a);		Key_Range_End[21]=Point(0.497*img.cols-a,img.rows*2/5-a);	//Y
	Key_Range_Begin[22]=Point(0.497*img.cols+a,img.rows*3/10+a);		Key_Range_End[22]=Point(0.560*img.cols-a,img.rows*2/5-a);	//U
	Key_Range_Begin[23]=Point(0.560*img.cols+a,img.rows*3/10+a);		Key_Range_End[23]=Point(0.628*img.cols-a,img.rows*2/5-a);	//I
	Key_Range_Begin[24]=Point(0.628*img.cols+a,img.rows*3/10+a);		Key_Range_End[24]=Point(0.694*img.cols-a,img.rows*2/5-a);	//O
	Key_Range_Begin[25]=Point(0.694*img.cols+a,img.rows*3/10+a);		Key_Range_End[25]=Point(0.762*img.cols-a,img.rows*2/5-a);	//P
	Key_Range_Begin[26]=Point(0.762*img.cols+a,img.rows*3/10+a);		Key_Range_End[26]=Point(0.827*img.cols-a,img.rows*2/5-a);	//[
	Key_Range_Begin[27]=Point(0.827*img.cols+a,img.rows*3/10+a);		Key_Range_End[27]=Point(0.9*img.cols-a,img.rows*2/5-a);		//]





	Key_Range_Begin[28]=Point(0+a,img.rows/2+a);						Key_Range_End[28]=Point(0.128*img.cols-a,img.rows*3/5-a);	//Caps Lock
	Key_Range_Begin[29]=Point(0.128*img.cols+a,img.rows/2+a);			Key_Range_End[29]=Point(0.196*img.cols-a,img.rows*3/5-a);	//A
	Key_Range_Begin[30]=Point(0.190*img.cols+a,img.rows/2+a);			Key_Range_End[30]=Point(0.260*img.cols-a,img.rows*3/5-a);   //S
	Key_Range_Begin[31]=Point(0.260*img.cols+a,img.rows/2+a);			Key_Range_End[31]=Point(0.330*img.cols-a,img.rows*3/5-a);   //D
	Key_Range_Begin[32]=Point(0.330*img.cols+a,img.rows/2+a);			Key_Range_End[32]=Point(0.396*img.cols-a,img.rows*3/5-a);   //F
	Key_Range_Begin[33]=Point(0.396*img.cols+a,img.rows/2+a);			Key_Range_End[33]=Point(0.460*img.cols-a,img.rows*3/5-a);   //G
	Key_Range_Begin[34]=Point(0.460*img.cols+a,img.rows/2+a);			Key_Range_End[34]=Point(0.527*img.cols-a,img.rows*3/5-a);   //H
	Key_Range_Begin[35]=Point(0.527*img.cols+a,img.rows/2+a);			Key_Range_End[35]=Point(0.596*img.cols-a,img.rows*3/5-a);   //J
	Key_Range_Begin[36]=Point(0.596*img.cols+a,img.rows/2+a);			Key_Range_End[36]=Point(0.660*img.cols-a,img.rows*3/5-a);   //K
	Key_Range_Begin[37]=Point(0.660*img.cols+a,img.rows/2+a);			Key_Range_End[37]=Point(0.727*img.cols-a,img.rows*3/5-a);   //L
	Key_Range_Begin[38]=Point(0.727*img.cols+a,img.rows/2+a);			Key_Range_End[38]=Point(0.792*img.cols-a,img.rows*3/5-a);	//;
	Key_Range_Begin[39]=Point(0.792*img.cols+a,img.rows/2+a);			Key_Range_End[39]=Point(0.866*img.cols-a,img.rows*3/5-a);	//,
	Key_Range_Begin[40]=Point(0.866*img.cols+a,img.rows/2+a);			Key_Range_End[40]=Point(img.cols-a,img.rows*3/5-a);			//回车





	Key_Range_Begin[41]=Point(0+a,img.rows*7/10+a);						Key_Range_End[41]=Point(0.162*img.cols-a,img.rows*4/5-a);	//shift
	Key_Range_Begin[42]=Point(0.162*img.cols+a,img.rows*7/10+a);		Key_Range_End[42]=Point(0.227*img.cols-a,img.rows*4/5-a);	//Z
	Key_Range_Begin[43]=Point(0.227*img.cols+a,img.rows*7/10+a);		Key_Range_End[43]=Point(0.295*img.cols-a,img.rows*4/5-a);	//X
	Key_Range_Begin[44]=Point(0.295*img.cols+a,img.rows*7/10+a);		Key_Range_End[44]=Point(0.360*img.cols-a,img.rows*4/5-a);	//C
	Key_Range_Begin[45]=Point(0.360*img.cols+a,img.rows*7/10+a);		Key_Range_End[45]=Point(0.426*img.cols-a,img.rows*4/5-a);	//V
	Key_Range_Begin[46]=Point(0.426*img.cols+a,img.rows*7/10+a);		Key_Range_End[46]=Point(0.497*img.cols-a,img.rows*4/5-a);	//B
	Key_Range_Begin[47]=Point(0.497*img.cols+a,img.rows*7/10+a);		Key_Range_End[47]=Point(0.560*img.cols-a,img.rows*4/5-a);	//N
	Key_Range_Begin[48]=Point(0.560*img.cols+a,img.rows*7/10+a);		Key_Range_End[48]=Point(0.626*img.cols-a,img.rows*4/5-a);	//M
	Key_Range_Begin[49]=Point(0.626*img.cols+a,img.rows*7/10+a);		Key_Range_End[49]=Point(0.695*img.cols-a,img.rows*4/5-a);	//,
	Key_Range_Begin[50]=Point(0.695*img.cols+a,img.rows*7/10+a);		Key_Range_End[50]=Point(0.762*img.cols-a,img.rows*4/5-a);	//.
	Key_Range_Begin[51]=Point(0.762*img.cols+a,img.rows*7/10+a);		Key_Range_End[51]=Point(0.831*img.cols-a,img.rows*4/5-a);	///







	Key_Range_Begin[52]=Point(0+a,img.rows*9/10+a);					Key_Range_End[52]=Point(0.093*img.cols-a,img.rows-a);		//Ctrl
	Key_Range_Begin[55]=Point(0.093*img.cols+a,img.rows*9/10+a);	Key_Range_End[55]=Point(0.191*img.cols-a,img.rows-a);		//Esc
	Key_Range_Begin[54]=Point(0.191*img.cols+a,img.rows*9/10+a);	Key_Range_End[54]=Point(0.293*img.cols-a,img.rows-a);		//Alt
	Key_Range_Begin[53]=Point(0.426*img.cols+a,img.rows*9/10+a);	Key_Range_End[53]=Point(0.497*img.cols-a,img.rows-a);		//空格
	Key_Range_Begin[56]=Point(0.693*img.cols+a,img.rows*9/10+a);	Key_Range_End[56]=Point(0.793*img.cols-a,img.rows-a);		//Alt
	Key_Range_Begin[57]=Point(0.896*img.cols+a,img.rows*9/10+a);	Key_Range_End[57]=Point(img.cols-a,img.rows-a);				//Ctrl







}



/*---------------------------------------

函数作用：画边框（黑色）
输入值：img
输出值：img

------------------------------------------*/
void Solution::draw_biankuang(Mat img)
{
	line(img,Point(0,0),Point(img.cols,0), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(0,img.rows/5),Point(img.cols,img.rows/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(0,img.rows*2/5),Point(img.cols*0.9,img.rows*2/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(0,img.rows*3/5),Point(img.cols,img.rows*3/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(0,img.rows*4/5),Point(img.cols,img.rows*4/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(0,img.rows),Point(img.cols,img.rows), Scalar( 0, 0, 0 ),6,8,0);


	line(img,Point(0,0),Point(0,img.rows), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols,0),Point(img.cols,img.rows), Scalar( 0, 0, 0 ),6,8,0);    //边框


	//第一行
	line(img,Point(img.cols/15,0),Point(img.cols/15,img.rows/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*2/15,0),Point(img.cols*2/15,img.rows/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols/5,0),Point(img.cols/5,img.rows/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*4/15,0),Point(img.cols*4/15,img.rows/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols/3,0),Point(img.cols/3,img.rows/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*2/5,0),Point(img.cols*2/5,img.rows/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*7/15,0),Point(img.cols*7/15,img.rows/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*8/15,0),Point(img.cols*8/15,img.rows/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*3/5,0),Point(img.cols*3/5,img.rows/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*2/3,0),Point(img.cols*2/3,img.rows/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*11/15,0),Point(img.cols*11/15,img.rows/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*4/5,0),Point(img.cols*4/5,img.rows/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*13/15,0),Point(img.cols*13/15,img.rows/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*14/15,0),Point(img.cols*14/15,img.rows/5), Scalar( 0, 0, 0 ),6,8,0);

	//第二行
	line(img,Point(img.cols*0.093,img.rows/5),Point(img.cols*0.093,img.rows*2/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.158,img.rows/5),Point(img.cols*0.158,img.rows*2/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.23,img.rows/5),Point(img.cols*0.23,img.rows*2/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.295,img.rows/5),Point(img.cols*0.295,img.rows*2/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.36,img.rows/5),Point(img.cols*0.36,img.rows*2/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.428,img.rows/5),Point(img.cols*0.428,img.rows*2/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.497,img.rows/5),Point(img.cols*0.497,img.rows*2/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.56,img.rows/5),Point(img.cols*0.56,img.rows*2/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.628,img.rows/5),Point(img.cols*0.628,img.rows*2/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.694,img.rows/5),Point(img.cols*0.694,img.rows*2/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.762,img.rows/5),Point(img.cols*0.762,img.rows*2/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.827,img.rows/5),Point(img.cols*0.827,img.rows*2/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.9,img.rows/5),Point(img.cols*0.9,img.rows*2/5), Scalar( 0, 0, 0 ),6,8,0);



	//第三行
	line(img,Point(img.cols*0.128,img.rows*2/5),Point(img.cols*0.128,img.rows*3/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.196,img.rows*2/5),Point(img.cols*0.196,img.rows*3/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.260,img.rows*2/5),Point(img.cols*0.260,img.rows*3/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.330,img.rows*2/5),Point(img.cols*0.330,img.rows*3/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.396,img.rows*2/5),Point(img.cols*0.396,img.rows*3/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.460,img.rows*2/5),Point(img.cols*0.460,img.rows*3/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.527,img.rows*2/5),Point(img.cols*0.527,img.rows*3/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.596,img.rows*2/5),Point(img.cols*0.596,img.rows*3/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.660,img.rows*2/5),Point(img.cols*0.660,img.rows*3/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.727,img.rows*2/5),Point(img.cols*0.727,img.rows*3/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.792,img.rows*2/5),Point(img.cols*0.792,img.rows*3/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.866,img.rows*2/5),Point(img.cols*0.866,img.rows*3/5), Scalar( 0, 0, 0 ),6,8,0);


	//第四行
	line(img,Point(img.cols*0.162,img.rows*3/5),Point(img.cols*0.162,img.rows*4/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.227,img.rows*3/5),Point(img.cols*0.227,img.rows*4/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.295,img.rows*3/5),Point(img.cols*0.295,img.rows*4/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.36,img.rows*3/5),Point(img.cols*0.36,img.rows*4/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.426,img.rows*3/5),Point(img.cols*0.426,img.rows*4/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.497,img.rows*3/5),Point(img.cols*0.497,img.rows*4/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.56,img.rows*3/5),Point(img.cols*0.56,img.rows*4/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.626,img.rows*3/5),Point(img.cols*0.626,img.rows*4/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.695,img.rows*3/5),Point(img.cols*0.695,img.rows*4/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.762,img.rows*3/5),Point(img.cols*0.762,img.rows*4/5), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.831,img.rows*3/5),Point(img.cols*0.831,img.rows*4/5), Scalar( 0, 0, 0 ),6,8,0);

	//第五行
	line(img,Point(img.cols*0.093,img.rows*4/5),Point(img.cols*0.093,img.rows), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.191,img.rows*4/5),Point(img.cols*0.191,img.rows), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.293,img.rows*4/5),Point(img.cols*0.293,img.rows), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.693,img.rows*4/5),Point(img.cols*0.693,img.rows), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.793,img.rows*4/5),Point(img.cols*0.793,img.rows), Scalar( 0, 0, 0 ),6,8,0);
	line(img,Point(img.cols*0.896,img.rows*4/5),Point(img.cols*0.896,img.rows), Scalar( 0, 0, 0 ),6,8,0);


}



int Solution::tongji_skin(Mat img)
{
	int x,y,i;
	char word[16];
	uchar *p;


	for(i=0;i<=57;i++)
	{
		if(i==28 || i==41 || i==52  ||i==54 ||i==56||i==57){continue;}
		int s=0;
		for(x=(2*Key_Range_Begin[i].y-Key_Range_End[i].y);x<Key_Range_End[i].y;x++)
		{
			p = img.ptr<uchar>(x);
			for(y=Key_Range_Begin[i].x;y<Key_Range_End[i].x;y++)
			{
				if(p[y]==1)
				{
					s++;
				}


			}
		}
		sprintf(word,"%d",s);  
		Point pre_pt = Point(Key_Range_Begin[i].x+img.cols/80,Key_Range_Begin[i].y); 
		putText(img,word,pre_pt,FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,255,0,255),1,8);//在窗口上显示坐标

		if(s>50 )
		{
			return(i);
			break;
		}


	}


	return(-1);





}

/*---------------------------------------

函数作用：_Get_Key_Value 将键的数字含义转成字母
输入值：
输出值：

------------------------------------------*/
void Solution::Get_Key_Value(char a[])
{
	a[0]='`';
	a[1]='1';
	a[2]='2';
	a[3]='3';
	a[4]='4';
	a[5]='5';
	a[6]='6';
	a[7]='7';
	a[8]='8';
	a[9]='9';
	a[10]='0';
	a[11]='-';
	a[12]='=';
	a[13]='\\';
	a[14]='\b';
	a[15]='\t';
	a[16]='q';
	a[17]='w';
	a[18]='e';
	a[19]='r';
	a[20]='t';
	a[21]='y';
	a[22]='u';
	a[23]='i';
	a[24]='o';
	a[25]='p';
	a[26]='[';
	a[27]=']';
	a[28]=' ';
	a[29]='a';
	a[30]='s';
	a[31]='d';
	a[32]='f';
	a[33]='g';
	a[34]='h';
	a[35]='j';
	a[36]='k';
	a[37]='l';
	a[38]=';';
	a[39]='\'';
	a[40]='\n';
	a[41]=' ';
	a[42]='z';
	a[43]='x';
	a[44]='c';
	a[45]='v';
	a[46]='b';
	a[47]='n';
	a[48]='m';
	a[49]=',';
	a[50]='.';
	a[51]='/';
	a[52]=' ';
	a[53]=' ';
	a[54]=' ';
	a[55]='\b';
	a[56]=' ';
	a[57]=' ';




}
