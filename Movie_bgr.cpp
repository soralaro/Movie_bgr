#include<iostream>
#include<highgui.h>
#include<fstream>
#include<string>
#include <sys/stat.h> 　
#include <sys/types.h> 
#include <unistd.h>  
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;
//#define SAVE_BGR_FILE 
#define SHOW_WINDOW   
#define SAVE_YUV_FILE
void write_raw_rgb(Mat bgr_Mat);
void write_raw_yuv420(Mat bgr_Mat);
void CopyYUVToImage(uchar * dst ,uint8_t *pY, uint8_t *pU , uint8_t *pV,int width, int height);
void CopyImageToYUV(uint8_t *pY, uint8_t *pU, uint8_t *pV, uchar * src ,int width, int height);
void write_raw_yuv420sp(uchar *pdata,int cols,int rows);
static void yuv420_yuv420sp(uchar *img_src,uchar *img_dst,int width, int height)
{
    int i;
    uchar *y_src,*u_src,*v_src;
    y_src=img_src;
    u_src=img_src+width*height;
    v_src=u_src+width*height/4;
    memcpy(img_dst,img_src,width*height);
    uchar *p=img_dst+width*height;
    int size=width*height/4;
    for(i=0;i<size;i++)
    {
        *p=*u_src;
        p++;
        u_src++;
        *p=*v_src;
        p++;
        v_src++;
    }
}

/**
 * @brief 视频文件转为BGR或者YUV文件
 * @param argv[1] 视频文件路径名称
 * @param argv[2] 最大输出的文件数量
 * 输出文件在当前目录output_data
 */
int main(int argc, char ** argv)
{

    int maximum_img=-1;
    int i;
    if(argc<3)
    {
        printf("please input filename of video  and maximum of output images!\n ");
        return 0;
    }
    printf("The video to decode is ");
    printf(argv[1]);
    printf("\n");
    maximum_img=atoi(argv[2]);
    if(maximum_img<0||maximum_img>100000)
    {
        printf("Error:input a invalid maximum argv!");
        return 0;
    }
    printf("maximum of output images is %d!\n",maximum_img);
    int isaccess=access("output_data", F_OK);
    if(isaccess<0)
    {
        int isCreate = mkdir("output_data",S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
        if( !isCreate )
            printf("create path:output_data .\n");
        else
        {
            printf("create output_data path failed! error code : %sd \n",isCreate);
            return 0;
        }
    }
    isaccess=access("output_data", W_OK );
    if(isaccess<0)
    {
        printf("output_data path dont not permit to write!");
        return 0;
    }

    VideoCapture capture(argv[1]);

   
    if(!capture.isOpened())
    {
        cout<<"Movie open Error"<<endl;
        return -1;
    }

    //获取视频帧频
    double rate=capture.get(CV_CAP_PROP_FPS);
    cout<<"帧率为:"<<" "<<rate<<endl;
    cout<<"总帧数为:"<<" "<<capture.get(CV_CAP_PROP_FRAME_COUNT)<<endl;//输出帧总数
    Mat frame;
#ifdef SHOW_WINDOW
    namedWindow("Movie Player");
#endif

    double position=0.0;
    //设置播放到哪一帧，这里设置为第0帧
    capture.set(CV_CAP_PROP_POS_FRAMES,position);
    i=0;
    uchar * yuv420spImg=NULL;
    while(i<maximum_img)
    {
        //读取视频帧
        if(!capture.read(frame))
            break;
        cv::Mat dstYuvImage;
        cv::cvtColor(frame, dstYuvImage, CV_RGB2YUV_I420); //，CV_BGR2YUV_I420
        printf("frame_id=%dw=%d,h=%d ,total=%d,dims=%d ,step1=%d\n ",i,dstYuvImage.cols,dstYuvImage.rows,dstYuvImage.total(),dstYuvImage.dims,dstYuvImage.step1());
        i++;
#ifdef SAVE_BGR_FILE      
        write_raw_rgb(frame);
#endif
#ifdef SAVE_YUV_FILE
        yuv420spImg=(uchar *)malloc(frame.rows*frame.cols*3/2);
        yuv420_yuv420sp(dstYuvImage.data,yuv420spImg,dstYuvImage.cols, frame.rows);
        write_raw_yuv420sp(yuv420spImg,frame.cols, frame.rows);
#endif
#ifdef SHOW_WINDOW
        imshow("Movie Player",frame);
        //获取按键值
        char c=waitKey(1);
        if(c==27)
            break;
#endif
    }
    if(yuv420spImg!=NULL)
        free(yuv420spImg);
    capture.release();
#ifdef SHOW_WINDOW
    destroyWindow("Movie Player");
#endif
    return 0;
}

void write_raw_yuv420sp(uchar *pdata,int cols,int rows)
{
    static int i;
    char file_name[100];
    i++;
    sprintf(file_name, "output_data/yuv420sp%dx%d_%d.yuv",cols,rows,i);  
    printf("sn:%d w=%d,h=%d\n ",i,cols,rows);
    ofstream out; 
    out.open(file_name,ios::out|ios::binary);
    out.write((char *)pdata,cols*rows*3/2);
    out.close();
}

void write_raw_rgb(Mat bgr_Mat)
{
    static int i;
    char file_name[100];
    i++;
    sprintf(file_name, "output_data/%06d.bin", i);  
    printf("w=%d,h=%d ,total=%d,dims=%d ,step1=%d\n ",bgr_Mat.cols,bgr_Mat.rows,bgr_Mat.total(),bgr_Mat.dims,bgr_Mat.step1());
    ofstream out; 
    printf(file_name);
    printf("\n");
    out.open(file_name,ios::out|ios::binary);
    out.write((char *)bgr_Mat.data,bgr_Mat.rows*bgr_Mat.step1());
    out.close();
}

void write_raw_yuv420(Mat bgr_Mat)
{
    static int i;
    char file_name[100];
    i++;
    sprintf(file_name, "output_data/yuv420_%dX%d_%d.raw",bgr_Mat.cols,bgr_Mat.rows,i);  
    printf("sn:%d w=%d,h=%d ,total=%d,dims=%d ,step1=%d\n ",i,bgr_Mat.cols,bgr_Mat.rows,bgr_Mat.total(),bgr_Mat.dims,bgr_Mat.step1());
    ofstream out; 
    out.open(file_name,ios::out|ios::binary);
    out.write((char *)bgr_Mat.data,bgr_Mat.rows*bgr_Mat.step1());
    out.close();
}

void CopyYUVToImage(uchar * dst ,uint8_t *pY, uint8_t *pU , uint8_t *pV,int width, int height)
{
   int size = width * height;
   memcpy(dst, pY, size);
   memcpy(dst + size, pU, size /4);
   memcpy(dst + size + size /4, pV, size / 4);
} 
 
void CopyImageToYUV(uint8_t *pY, uint8_t *pU, uint8_t *pV, uchar * src ,int width, int height)
{
   int size = width * height;
   memcpy(pY, src, size);
   memcpy(pU, src + size, size / 4);
   memcpy(pV, src + size + size / 4, size / 4);
}