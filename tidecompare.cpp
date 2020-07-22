#include "tidecompare.h"
#include "ui_tidecompare.h"
//qt库
#include<QLabel>
#include <QPainter>
#include<QFileDialog>
#include<QFile>
#include<QTextStream>
#include<QDateTime>
#include<Eigen>
#pragma execution_character_set("utf-8")

double tideCompare::meanSeaSurface=0;

tideCompare::tideCompare(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::tideCompare)
{
    ui->setupUi(this);



   ui->dateTimeEdit->setDisplayFormat("yyyy/MM/dd HH:mm:ss");
   ui->dateTimeEdit->setCalendarPopup(true);
   keyP.reset(new keyParameters<double>(0,2,0,0,0,0,0));//M2

   iniDoodSon();//设置分潮的doodson码

   connect(ui->actionOpenFiles,SIGNAL(triggered()),this,SLOT(getTideFiles()));
   connect(ui->dateTimeEdit, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(getTime(QDateTime)));

   connect(ui->pushButtonPlot,SIGNAL(clicked()),this,SLOT(plotTides()));

}

tideCompare::~tideCompare()
{


    delete ui;
}

void tideCompare::getTime(const QDateTime &dateTime)
{

    QDateTime utcTime = dateTime.toUTC();
    QTime Time=utcTime.time();
    QDate Date=utcTime.date();
    //只将年、日、小时传入
    keyParametersDataTypeD currentYear=static_cast<keyParametersDataTypeD>(Date.year());
    keyParametersDataTypeD currentDay=static_cast<keyParametersDataTypeD>(Date.day());
    keyParametersDataTypeD currentMonth=static_cast<keyParametersDataTypeD>(Date.month());

    keyParametersDataTypeD currentHour=static_cast<keyParametersDataTypeD>(Time.hour());
    keyParametersDataTypeD currentMinute=static_cast<keyParametersDataTypeD>(Time.minute());
    keyParametersDataTypeD currentSecond=static_cast<keyParametersDataTypeD>(Time.second());


    int dates=0;
    int isLeapYear=0;
    int leapYearNum=0;
    if(currentYear<1900)
        return;
    else
    {
        //判断几个闰年，以及今年是不是闰年
        if(!(((int)currentYear-1900)%4))//如果是闰年，则为0
        {
            leapYearNum=((int)currentYear-1900)/4-1;//所计算的闰年数，是加上今年的闰年
            isLeapYear=1;
        }
        else
        {
            leapYearNum=floor(((int)currentYear-1900)/4);
            isLeapYear=0;
        }
    }

    int month=static_cast<int>(currentMonth);

    switch((Month)month)
    {
    case January:
          dates=0;
        break;
    case February:
          dates+=30;
        break;
    case March:
        if(!isLeapYear)
        {
            dates+=58;
        }
        else
        {
            dates+=59;
        }
        break;
    case April:
        if(!isLeapYear)
        {
            dates+=89;
        }
        else
        {
            dates+=90;
        }
        break;
    case May:
        if(!isLeapYear)
        {
            dates+=119;
        }
        else
        {
            dates+=120;
        }
        break;
    case June:

        if(!isLeapYear)
        {
            dates+=150;
        }
        else
        {
            dates+=151;
        }
        break;
      case July:
        if(!isLeapYear)
        {
            dates+=180;
        }
        else
        {
            dates+=181;
        }
        break;
    case August:
        if(!isLeapYear)
        {
            dates+=211;
        }
        else
        {
            dates+=212;
        }
        break;
    case September:
        if(!isLeapYear)
        {
            dates+=242;
        }
        else
        {
            dates+=243;
        }
        break;
    case October:
        if(!isLeapYear)
        {
            dates+=272;
        }
        else
        {
            dates+=273;
        }
        break;
    case November:
        if(!isLeapYear)
        {
            dates+=303;
        }
        else
        {
            dates+=304;
        }
        break;
    case December:
        if(!isLeapYear)
        {
            dates+=333;
        }
        else
        {
            dates+=334;
        }
        break;
    }

    //***加上这个月的天数
    dates+=currentDay;
    currentHour=currentHour+currentMinute/60+currentSecond/3600;

    int yearsNum=0;
    yearsNum=currentYear-1900;

    keyP->setKeyTimes(leapYearNum,currentYear-1900,dates,currentHour);//


}

void tideCompare::on_pushButton_calVo_clicked()
{

    if(aa)
    {
        int aaa=0;
    }
    else
    {
        aa=new int;
        delete aa;
        aa=0;

    }
    keyP->calculateKeyValues();//天文相角只与时间有关，先传入时间计算

    if(vecTimeData.size())
    {


        //获得每个分潮的doodson码

      for( vector<pair<QString,QString>>::iterator iter = vectorDoodson.begin(); iter != vectorDoodson.end() ; iter++)
        {


            QString doodsonCode=iter->second;
            QString tideType=iter->first;
            QStringList truncString=iter->second.split(" ");
            int u1=truncString[0].toInt();
            int u2=truncString[1].toInt();
            int u3=truncString[2].toInt();

            int u4=truncString[3].toInt();
            int u5=truncString[4].toInt();
            int u6=truncString[5].toInt();
            int u0=truncString[6].toInt();

            keyP->setDoodSonCode(u0,u1,u2,u3,u4,u5,u6);// 重新传入dooson
            keyP->writePhaseAngleArray(vecTimeData.size());//计算角速率变化时长



        }
        Eigen::MatrixXd x= leastSquareTides();
        meanSeaSurface=getR_Sita(x);//获得平均海平面信息


    }//如果从日历中选时间
    else
    {
        //keyP->setDoodSonCode();
        keyP->writePhaseAngleArray(23);
    }

}

void tideCompare::getTideFiles()
{
    QString dir =QFileDialog::getOpenFileName(this, tr("Open TideFiles"), ".", tr("Files(*.txt)"));
    QFile tideFile(dir);
    if(!tideFile.open(QIODevice::ReadOnly))
      return ;

    //int monthCount=1;

    while(!tideFile.atEnd())
    {
        QString line;
        line=tideFile.readLine();
        QStringList te=line.split(QRegExp("[-: \t]"));

        time_Data time;

        for(int i=0;i<te.size();i++)
        {
            if(i==0)
            {
                QString temp=te[i];
                time.year=temp.toInt();
            }
            if(i==1)
            {
                QString temp=te[i];
                time.month=temp.toInt();
            }
            if(i==2)
            {
                QString temp=te[i];
                time.day=temp.toInt();
            }
            if(i==3)
            {
                QString temp=te[i];
                time.hour=temp.toInt();
            }
            if(i==4)
            {
                QString temp=te[i];
                time.minute=temp.toInt();
            }
            if(i==5)
            {
                QString temp=te[i];
                time.tideHeight=temp.toDouble();
            }
                //time.year=QString::number(,10,3);
        }

        time.second=0;
        vecTimeData.push_back(time);

    }

    if(vecTimeData.size()>0)
    {

        QDate date;
        QTime time;
        time_Data timedata=vecTimeData[0];
        date.setDate(timedata.year,timedata.month,timedata.day);
        time.setHMS(timedata.hour,timedata.minute,timedata.second);
        timedate.setDate(date);
        timedate.setTime(time);

        getTime(timedate);//将起始时间传入

    }


}


// K1 O1 P1 Q1
    //K2 M2 N2 S2
      //M4 MS4 M6
        //a sa
void tideCompare::iniDoodSon()//为map的doodson赋值
{
     //日潮
     vectorDoodson.push_back(pair<QString,QString>("K1","1 1 0 0 0 0 1"));//doodson+u0
     vectorDoodson.push_back(pair<QString,QString>("O1","1 -1 0 0 0 0 -1"));
     vectorDoodson.push_back(pair<QString,QString>("P1","1 1 -2 0 0 0 -1"));
     vectorDoodson.push_back(pair<QString,QString>("Q1","1 -2 0 1 0 0 -1"));
     //半日潮
     vectorDoodson.push_back(pair<QString,QString>("K2","2 2 0 0 0 0 0"));
     vectorDoodson.push_back(pair<QString,QString>("M2","2 0 0 0 0 0 0"));
     vectorDoodson.push_back(pair<QString,QString>("N2","2 -1 0 1 0 0 0"));
     vectorDoodson.push_back(pair<QString,QString>("S2","2 2 -2 0 0 0 0"));
     //1/4 1/6潮
     vectorDoodson.push_back(pair<QString,QString>("M4","4 0 0 0 0 0 0"));
     vectorDoodson.push_back(pair<QString,QString>("MS4","4 2 -2 0 0 0 0"));
     vectorDoodson.push_back(pair<QString,QString>("M6","6 0 0 0 0 0 0"));
     //长周期
     vectorDoodson.push_back(pair<QString,QString>("Sa","0 0 1 0 0 0 0"));
     vectorDoodson.push_back(pair<QString,QString>("Ssa","0 0 2 0 0 0 0"));


}

void tideCompare::plotTides()
{
    ui->customPlot->clearGraphs();
    ui->customPlot->clearItems();
    ui->customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));


    double now = timedate.toTime_t();//QDateTime::currentDateTime().toTime_t();


    for (int gi=0; gi<1; ++gi)
    {
      ui->customPlot->addGraph();

      ui->customPlot->graph()->setLineStyle(QCPGraph::lsLine);
      ui->customPlot->graph()->setPen(QPen(QColor(30, 144, 255)));

    //  ui->customPlot->graph()->setBrush(QBrush(color));
      //设置潮位时间
      int dataSize= vecTimeData.size();

      QVector<QCPGraphData> timeData(dataSize);
      for (int i=0; i<dataSize; ++i)
      {
          timeData[i].key = now + 3600*i;

          timeData[i].value=vecTimeData[i].tideHeight;
      }
      ui->customPlot->graph()->data()->set(timeData);

    }

    //绘制分潮的合角
    for (int gi=0; gi<1; ++gi)
    {
      ui->customPlot->addGraph();

      ui->customPlot->graph()->setLineStyle(QCPGraph::lsLine);
      ui->customPlot->graph()->setPen(QPen(QColor(255, 144, 125)));

    //  ui->customPlot->graph()->setBrush(QBrush(color));
      //设置潮位时间
      int dataSize= vecTimeData.size();
      ofstream txt("C:\\Users\\Administrator\\Desktop\\temp_H_g.txt");

      for(int j=0;j<H_g.size();j++)
      {

          double A=H_g[j].first;
          double B=H_g[j].second;
          txt<<vectorDoodson[j].first.toStdString()<<" "<<A<<" "<<B<<endl;

//              timeData[i].value+=H*cos((sigma*i+v0-g)/180*M_PI)//Hcos(sigma*t+V0-g)
//                      +meanSeaSurface/H_g.size();
      }
      txt<<"meanSeaSurface "<<meanSeaSurface;

      QVector<QCPGraphData> timeData(dataSize);
      for (int i=0; i<dataSize; ++i)
      {
          for(int j=0;j<H_g.size();j++)
          {
              double sigma=keyP->V0_Sigma[j].second;
              double v0=keyP->V0_Sigma[j].first;
              double A=H_g[j].first;
              double B=H_g[j].second;
             // double
              timeData[i].key = now + 3600*i;//时间轴
              timeData[i].value+=A*cos((v0+sigma*i-B)/180*M_PI)//   A*cos((v0+sigma*i)/180*M_PI)+B*sin((v0+sigma*i/180*M_PI)
                     +meanSeaSurface/H_g.size();

//              timeData[i].value+=H*cos((sigma*i+v0-g)/180*M_PI)//Hcos(sigma*t+V0-g)
//                      +meanSeaSurface/H_g.size();
          }


      }
      txt.close();
      ui->customPlot->graph()->data()->set(timeData);

    }

    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("d. MMMM\nyyyy");
    ui->customPlot->xAxis->setTicker(dateTicker);
    //左边坐标轴名字
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    //textTicker->addTick(10, "a bit\nlow");
   // textTicker->addTick(50, "quite\nhigh");
   // ui->customPlot->yAxis->setTicker(textTicker);
    // 坐标轴字体
    ui->customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
    // 坐标轴文字
    ui->customPlot->xAxis->setLabel("日期");
    ui->customPlot->yAxis->setLabel("潮位");
    // 坐标轴刻度
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->xAxis2->setTicks(false);
    ui->customPlot->yAxis2->setTicks(false);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setTickLabels(false);
    //设置坐标轴范围
    ui->customPlot->xAxis->setRange(now, now+3600*vecTimeData.size());
    ui->customPlot->yAxis->setRange(0, 8);
    // legend的设置
    // ui->customPlot->legend->setObjectName("潮高");
    ui->customPlot->legend->setVisible(true);
    ui->customPlot->legend->setBrush(QColor(255, 255, 255, 150));
//QSharedPointer<QCPLegend> pp(new QCPLegend);

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->customPlot->axisRect()->setupFullAxesBox();
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    ui->customPlot->graph()->rescaleAxes();
    ui->customPlot->replot();
}
