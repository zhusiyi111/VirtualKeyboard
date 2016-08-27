

#include "opencv2/opencv.hpp"  

#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include<Solution.h>

using namespace cv;  
using namespace std;  



void main()
{

	
	



	VideoCapture cap(1);   
	Mat img; 
	while(cvWaitKey()!=122)
	{

		cap>>img;
		imshow("当前img视频",img); 
		waitKey(10);
	}
	cvDestroyWindow("当前img视频");
	Solution solution;
	solution.Four_Point_init(img);
	solution.Four_Point_Past_init();
	vector<Point> a = solution.get_Four_Point_Past();
	Mat aa = solution.calculate_warp_mat(a,img);
	solution.XY_warp_init(img,aa);
	solution.area_of_key_init(img);
	char temp[101];
	solution.Get_Key_Value(temp);





	int flag=0,enter=0;
	int keynumber=0,keysave=0;
	int skinnumber=0,skinsave=0;


	while(1)
	{

		cap>>img;
		double t = (double)getTickCount();

		Mat skin(img.rows,img.cols,CV_8UC1);
		imshow("img",img);
		solution.GetSkin(img,skin);
		
		
		solution.transform_skin_warp(skin);
		solution.draw_biankuang(skin);
		skinnumber = solution.tongji_skin(skin);

		if(skinnumber!=-1)
		{
			if(skinnumber==skinsave)
			{
				flag++;
				if(flag>=2 && flag<=10)
				{
					enter=1;
				}
				else
				{
					enter=0;
				}
			}

			else
			{
				if(enter==1)
				{
					if( skinsave==14 )
					{
						printf("%c ",temp[skinsave]);

					}

					printf("%c",temp[skinsave]);

				}
				else
				{

				}
				flag=0;
				enter=0;
			}
			skinsave=skinnumber;
		}
		if(skinnumber==-1	&&	 enter==1 )
		{
			if( skinsave==14 )
			{
				printf("%c ",temp[skinsave]);

			}
			printf("%c",temp[skinsave]);
			flag=0;
			enter=0;
		}
		
		imshow("skin",skin);
		waitKey(1);
	} 




}















