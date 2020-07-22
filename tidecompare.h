#ifndef TIDECOMPARE_H
#define TIDECOMPARE_H
#include<iostream>
#include<Eigen>
#define _USE_MATH_DEFINES
#include<math.h>
#include <QMainWindow>
#include <memory>
#include<map>
#include<fstream>
#include"qcustomplot.h"
using namespace std;


namespace Ui {
class tideCompare;
}


class tideCompare : public QMainWindow
{
    Q_OBJECT

public:
    explicit tideCompare(QWidget *parent = 0);
    ~tideCompare();

    int *aa=0;

   //keyParameters的数据类型
    typedef double keyParametersDataTypeD;
    typedef float  keyParametersDataTypeF;
    typedef int    keyParametersDataTypeInt;

    double static meanSeaSurface;

    //月份的枚举，判断闰年与否的年积日
    enum Month{January=1,February,March,April,May,June,July,
               August,September,October,November,December};

    //enum tideType{K1="k1",K2="K2"};
//    union month
//    {
//        int months[12];
//        int currentMonth;
//    };

public:

    //输入分潮doodson码
    void iniDoodSon();




    template <typename dataType>
    struct keyParameters
    {

        keyParameters(int U0,int U1,int U2,int U3,int U4,int U5,int U6):
                  u0(U0),u1(U1),u2(U2),u3(U3),u4(U4),u5(U5),u6(U6)
         {
            s1=13.17639/24;
            h1=0.98565/24;
            p1=0.1114/24;
            N1=0.05295/24;
            p_1=0.00005/24;
            tom1=15-s1+h1;//导数
            V_to=0;
            sigma=0;


          }

        //角速度
        inline dataType getSigma(){sigma=(u1*tom1+u2*s1+u3*h1+u4*p1+u5*N1+u6*p_1);return sigma;}
        //相位角
        inline dataType getV_to(){V_to=u1*tom+u2*s+u3*h+u4*p+u5*N+u6*p_+u0*90;return V_to;}

        //对时间信息赋值
        void setKeyTimes(dataType IL,dataType IY,dataType ID,dataType IH)
        {
             interval_LeapYear=IL;
             interval_Year=IY;
             interval_Day=ID;
             interval_hour=IH;
        }
        //Doodson编码
        void setDoodSonCode(int U0,int U1,int U2,int U3,int U4,int U5,int U6)
        {
            u0=U0;
            u1=U1;
            u2=U2;
            u3=U3;
            u4=U4;
            u5=U5;
            u6=U6;
        }


        //计算天文参数
        void calculateKeyValues()
        {

           //********计算天文参数
            h=(280.1895-0.23872*115+0.98565*(interval_LeapYear+interval_Day+interval_hour/24));
            h=(h-(int)h)+((int)h%360);

            s=(277.0247+129.38481*interval_Year+13.17639*(interval_LeapYear+interval_Day+interval_hour/24));
            s=(s-(int)s)+((int)s%360);

            p=334.3853+40.66249*interval_Year+0.1114*(interval_LeapYear+interval_Day+interval_hour/24);
            p=(p-(int)p)+((int)p%360);

            N=100.8432+19.32818*interval_Year+0.05295*(interval_LeapYear+interval_Day+interval_hour/24);
            N=(N-(int)N)+((int)N%360);

            p_=281.2209+0.017192*interval_Year+0.00005*(interval_LeapYear+interval_Day+interval_hour/24);
            p_=(p_-(int)p_)+((int)p_%360);

            tom=15*interval_hour-s+h;



        }



        //****写入相位角
        //---cos(sigma*t)+sin(sigma*t)---
        void writePhaseAngleArray(int hours)//时间间隔  13个分潮
        {

            //输入时间和doodson数
            double vTide=getV_to();//单位是度
            vTide=(vTide-(int)vTide)+((int)vTide%360);
            double sigmaTide=getSigma();//每个潮位的角速率---单位是度

            //map<pair<dataType,dataType>> singleTideCos_Sin;
            singleTideData singleTideCos_Sin;//记录时间推移的相位角

            V0_Sigma.push_back(pair<double,double>(vTide,sigmaTide));
            for(int i=0;i<hours;i++)
            {
                //存储相位值
//               singleTideCos_Sin.push_back(pair<dataType,dataType>//存角速度的
//                                 (cos(sigmaTide*i/180*M_PI),sin(sigmaTide*i/180*M_PI)));
                singleTideCos_Sin.push_back(pair<dataType,dataType>//存角速度的
                                  (cos((sigmaTide*i+vTide)/180*M_PI),sin((sigmaTide*i+vTide)/180*M_PI)));


            }


            allTidesCos_Sin.push_back(pair<dataType,vector<cos_sin_Sigma_t>>(vTide,singleTideCos_Sin));
            singleTideCos_Sin.clear();
        }



         //(cos(sigma*t)+sin(sigma*t))
         typedef pair<dataType,dataType> cos_sin_Sigma_t;

         //每个分潮的(cos(sigma*t)+sin(sigma*t))
         typedef vector<pair<dataType, dataType>>  singleTideData;

         //由外向内顺序为 13个分潮--->(初始相角+分潮相角变化）--->(cos(sigma*t)+sin(sigma*t))
         typedef vector<pair<dataType,vector<pair<dataType, dataType>>>>  allTidesData;




        allTidesData allTidesCos_Sin;//记录所有分潮的每个角速率相位角的正余弦值

        //存储初相角与角速率
        vector<pair<double,double>> V0_Sigma;

        //******天文要素
        dataType h;
        dataType s;
        dataType p;
        dataType N;
        dataType p_;
        dataType tom;
        dataType V_to;

        //******天文要素导数
        dataType tom1;
        dataType s1;
        dataType h1;
        dataType p1;
        dataType N1;
        dataType p_1;
        dataType sigma;//角速率

        //*****潮汐周期
        int u0,u1,u2,u3,u4,u5,u6;

        //*****时间要素
        dataType interval_LeapYear;//闰年数
        dataType interval_Year;//间隔年
        dataType interval_Day;//年积日
        dataType interval_hour;
    };

    struct time_Data//潮位文件的参数
    {
      int year,month,day;
      int hour,minute;
      double second;
      double tideHeight;
    };


public slots:

    void getTime(const QDateTime &dateTime);//计算特定时刻的年积日-闰年数-时长

    void getTideFiles();//读取潮位文件

    //位置潮面 bool isPlot;
     void plotTides();

private slots:
    void on_pushButton_calVo_clicked();

public:

    //潮位全局时间
    QDateTime timedate;

    vector<time_Data> vecTimeData;//存储文件读取的数据 按月存储
   // QScopedPointer<keyParameters> prepareKey;//备用
    shared_ptr<keyParameters<double>> keyP;
    //vector<std::shared_ptr<keyParameters<double>>> vecTides;

    //map<pair<QString,Qstring>>
    vector<pair<QString,QString>> vectorDoodson;//记录十三个分潮的doodson数+u0


    //存储分潮的振幅和相角
    vector<pair<double,double>> H_g;//H_g

    vector<pair<double,double>> f_u;//交点因子
public:


     Eigen::MatrixXd leastSquareTides()
    {
        int row,col;

        if(keyP->allTidesCos_Sin.size()>0)
          {
            col=keyP->allTidesCos_Sin.size();//13个分潮的 cos/sin
            row=keyP->allTidesCos_Sin[0].second.size();//所有大小
          }
         //最小二乘   2m+1?
        Eigen::MatrixXd B(row,col*2+1),l(row,1),x(col*2+1,1);// B_TPBx-B_TPl=0

       // Eigen::MatrixXd P=Eigen::MatrixXd::Identity(row,row);
          {
            for(int j=0;j<col;j++)
             {

                 for(int i=0;i<row;i++)
                 {
                     //vector<pair<dataType,vector<pair<dataType, dataType>>>>
                     double cos1=keyP->allTidesCos_Sin[j].second[i].first;
                     double sin1=keyP->allTidesCos_Sin[j].second[i].second;
                     B(i,j*2)=cos1;

                     B(i,j*2+1)=sin1;


                     if(j<1)
                     {
                         l(i)=vecTimeData[i].tideHeight;
                          B(i,2*col)=1;
                     }

                 }


             }
          }




      Eigen::MatrixXd  B_TPB=B.transpose()*B;
      Eigen::MatrixXd B_TPl=B.transpose()*l;
      x=(B_TPB.inverse())*B_TPl;


//      ofstream txt("C:\\Users\\gps\\Desktop\\temp_X.txt");
//     // for(int i=0;i<col;i++)
//        {

//         for(int j=0;j<col;j++)
//          {

//            //txt<<keyP->allTidesCos_Sin[i].first;
//             txt<<x(2*j)<<" "<<x(2*j+1)<<endl;
//             txt<<endl;
//           }
//            txt<<x(2*col);


//        }

//      txt.close();


        return x;

    }

     double getR_Sita(Eigen::MatrixXd &x)
     {
         int tideSize=vectorDoodson.size();
         double seaMeanSurface=x(2*tideSize);
         for(int i=0;i<tideSize;i++)
         {
             double A=x(2*i),B=x(2*i+1);
             //-----//----//----//----//
             //----R=sqrt(A*A+B*B)  sita=arctan(B/A)
             //H=R  ------ g=(V0+sita)
             //-----//----//----//----//
             /*H_g.push_back(pair<double,double>(sqrt(pow(A,2)+pow(B,2)),
                           atan(B/A)*180/M_PI+keyP->V0_Sigma[i].first));*///获取v0 keyP->allTidesCos_Sin[i].first

             double sita=atan(B/A)*180/M_PI;
             if(sita<0)
                 sita+=360;
             if(sita>360)
                 sita-=360;
             double H=sqrt(pow(A,2)+pow(B,2));
             H_g.push_back(pair<double,double>(H,sita));
         }


         return seaMeanSurface;
     }


private:
    Ui::tideCompare *ui;
};

#endif // TIDECOMPARE_H
