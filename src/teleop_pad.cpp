#include <stdio.h>
#include <QPainter>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QTimer>
#include <QButtonGroup>
#include <geometry_msgs/Twist.h>
#include <QDebug>
#include <QPushButton>
#include "teleop_pad.h"
#include <QTabWidget>
#include <QListView>
#include <QStringListModel>

namespace rviz_teleop_commander
{

// 构造函数，初始化变量
TeleopPanel::TeleopPanel( QWidget* parent )
  : rviz::Panel( parent )
  , linear_velocity_x( 0 )
  , linear_velocity_y( 0 )//当前速度初始化
  , angular_velocity_( 0 )
  , smcontrol_client("hexapodservice", true)

  
  
  
  //tab界面
{
    
 //*************************创建第一个tab界面*************************
    
 QTabWidget *tabWidget = new QTabWidget;
 QWidget *first_widget = new QWidget();
 first_widget->setMinimumSize(0,0);
 first_widget->setMaximumSize(1000,1000);
 
  // 创建一个输入topic命名的窗口
  QVBoxLayout* topic_layout = new QVBoxLayout;
  topic_layout->addWidget( new QLabel( "Topic:" ));
  output_topic_editor_ = new QLineEdit;
  topic_layout->addWidget( output_topic_editor_ );

  // 创建一个输入线速度的窗口 x y
  topic_layout->addWidget( new QLabel( "Linear velocity of x:" ));
  output_topic_editor_linear_x = new QLineEdit;
  topic_layout->addWidget( output_topic_editor_linear_x );
  
    topic_layout->addWidget( new QLabel( "Linear velocity of y:" ));
  output_topic_editor_linear_y = new QLineEdit;
  topic_layout->addWidget( output_topic_editor_linear_y );
  

  // 创建一个输入角速度的窗口
  topic_layout->addWidget( new QLabel( "Angular velocity z:" ));
  output_topic_editor_angular_z = new QLineEdit;
  topic_layout->addWidget( output_topic_editor_angular_z );
  
  //增加点击才运行的按钮run
  runButton = new QPushButton( tr("&Run") );
  topic_layout->addWidget( runButton );
  runButton->setDefault(true);
  runButton->setEnabled(true);
  
   stopButton = new QPushButton( tr("&Stop") );
  topic_layout->addWidget( stopButton );
  stopButton->setDefault(true);
  stopButton->setEnabled(false);
  
  // hv界面给了first_widget
  first_widget->setLayout(topic_layout);
   
  QHBoxLayout* layout1 = new QHBoxLayout;
  layout1->addLayout( topic_layout );
  //setLayout( layout1 );
  //第一个页面结束, topic_layout是一个垂直布局的界面
 
  //*************************新建第二个tab页面*************************
 QWidget *second_widget= new QWidget();
 
  //左边界面
 QVBoxLayout* bus_control_layout = new QVBoxLayout;  
 OPENBUSButton = new QPushButton( tr("&Open Bus") );
 CLOSEBUSButton = new QPushButton( tr("&Close Bus") );
 ENABLEMOTORButon = new QPushButton(tr("&Enable Motor") ) ;
 DISABLEMOTORButton = new QPushButton(tr("&Disable Motor") ) ;
 GETSTATUSButton = new QPushButton(tr("&Get status"));
 CLEARFAULTButton = new QPushButton(tr("&Clear fault"));
 ALLLEGRESETButton = new QPushButton(tr("&All leg reset"));

 //全部pushbutton添加到 左边的qv界面中
 
 bus_control_layout->addWidget(OPENBUSButton);
 bus_control_layout->addWidget(CLOSEBUSButton);
 bus_control_layout->addWidget(ENABLEMOTORButon);
 bus_control_layout->addWidget(DISABLEMOTORButton);
 bus_control_layout->addWidget(GETSTATUSButton);
 bus_control_layout->addWidget(CLEARFAULTButton);
 bus_control_layout->addWidget(ALLLEGRESETButton);
 
// qv界面给了second_widget
 
 //右边界面
 
 listView_in_tab2_ = new QListView;
 QVBoxLayout *listView_layout_in_tab2_ = new QVBoxLayout;
 listView_layout_in_tab2_->addWidget(listView_in_tab2_);
 
 //主界面
 QGridLayout *mainLayout_in_tab2 = new QGridLayout;
 mainLayout_in_tab2->addLayout(bus_control_layout,0,0);
 mainLayout_in_tab2->addLayout(listView_layout_in_tab2_,0,1);
 mainLayout_in_tab2->setColumnStretch(1,4); //列宽比为1比3
 
  second_widget->setLayout(mainLayout_in_tab2);
  
  QHBoxLayout* layout2 = new QHBoxLayout;
  layout2->addLayout( mainLayout_in_tab2); 
  
  //setLayout( layout2 );
 
//*************************新建第三个tab页面*************************
QPushButton *pushButton3 = new QPushButton("©VK Control");
  
  
  
  //*************************界面加入tab*************************
   tabWidget->addTab(first_widget,  "VelCtrl");
   tabWidget->setMaximumSize(350,400);
   tabWidget->setMinimumSize(350,400);
   tabWidget->adjustSize();
   tabWidget->addTab(second_widget,  "BusCtrl");
   tabWidget->addTab(pushButton3, "...");
   
   //保证 tab 都可以将其他 界面显示完全
layout1->addWidget(tabWidget);
this->setLayout(layout1);
//this->resize(900, 100);
//this->setWindowTitle("QTabWidgetDemo1");

layout2->addWidget(tabWidget);
this->setLayout(layout2);
//this->resize(500, 100);
//this->setWindowTitle("QTabWidgetDemo2");
  


  // 创建一个定时器，用来定时发布消息
  QTimer* output_timer = new QTimer( this );

  // 设置信号与槽的连接
  connect( output_topic_editor_, SIGNAL( editingFinished() ), this, SLOT( updateTopic() ));             // 输入topic命名，回车后，调用updateTopic()
 // connect( output_topic_editor_linear_x, SIGNAL( editingFinished() ), this, SLOT( update_Linear_Velocity() )); // 输入线速度值，回车后，调用update_Linear_Velocity()
  //connect( output_topic_editor_linear_y, SIGNAL( editingFinished() ), this, SLOT( update_Linear_Velocity() ));  //改变y
  //connect( output_topic_editor_angular_z, SIGNAL( editingFinished() ), this, SLOT( update_Angular_Velocity() ));// 输入角速度值，回车后，调用update_Angular_Velocity()
  
  connect( runButton,SIGNAL(clicked(bool)),this,SLOT( update_Linear_Velocity() ) );
  connect( runButton,SIGNAL(clicked(bool)),this,SLOT( update_Angular_Velocity() ) );
  connect(stopButton,SIGNAL(clicked(bool)),this,SLOT(stopVelocity()) );
  
  connect(OPENBUSButton,SIGNAL(clicked(bool)),this,SLOT(OPENBUSButtonFuc()));
  connect(CLOSEBUSButton,SIGNAL(clicked(bool)),this,SLOT(CLOSEBUSButtonFuc()));
  connect(ENABLEMOTORButon,SIGNAL(clicked(bool)),this,SLOT(ENABLEMOTORButonFuc()));
  connect(DISABLEMOTORButton,SIGNAL(clicked(bool)),this,SLOT(DISABLEMOTORButtonFuc()));
  connect(GETSTATUSButton,SIGNAL(clicked(bool)),this,SLOT(GETSTATUSButtonFuc()));
  connect(CLEARFAULTButton,SIGNAL(clicked(bool)),this,SLOT(CLEARFAULTButtonFuc()));
  connect(ALLLEGRESETButton,SIGNAL(clicked(bool)),this,SLOT(ALLLEGRESETButtonFuc()) );
  
  
  // 设置定时器的回调函数，按周期调用sendVel()
  connect( output_timer, SIGNAL( timeout() ), this, SLOT( sendVel() ));

  // 设置定时器的周期，100ms
  output_timer->start( 100 );
}

//stop
void TeleopPanel::stopVelocity()
{
    linear_velocity_x = 0;
    linear_velocity_y = 0;
    angular_velocity_ = 0;
    stopButton->setEnabled(false);
}



// 更新线速度值
void TeleopPanel::update_Linear_Velocity()
{
    // 获取输入框内的数据
    QString temp_stringx = output_topic_editor_linear_x->text();
	QString temp_stringy = output_topic_editor_linear_y->text();
	// 将字符串转换成浮点数
    float linex = temp_stringx.toFloat();  
	float liney = temp_stringy.toFloat();
    
	// 保存当前的输入值
    linear_velocity_x = linex;
    linear_velocity_y = liney;
    stopButton->setEnabled(true);
}

// 更新角速度值
void TeleopPanel::update_Angular_Velocity()
{
    QString temp_stringz = output_topic_editor_angular_z->text();
    float ang = temp_stringz.toFloat() ;  
    angular_velocity_ = ang;
    stopButton->setEnabled(true);
}

// 更新topic命名
void TeleopPanel::updateTopic()
{
  setTopic( output_topic_editor_->text() );
}

/*
void TeleopPanel::updateBusTopic()
{
  setTopic( output_topic_editor_tab2_->text() );
}
*/

// 设置topic命名
void TeleopPanel::setTopic( const QString& new_topic )
{
  // 检查topic是否发生改变.
  if( new_topic != output_topic_ )
  {
    output_topic_ = new_topic;
	
    // 如果命名为空，不发布任何信息void TeleopPanel::updateTopic()
{
  setTopic( output_topic_editor_->text() );
}
    if( output_topic_ == "" )
    {
      velocity_publisher_.shutdown();
    }
	// 否则，初始化publisher
    else
    {
      velocity_publisher_ = nh_.advertise<geometry_msgs::Twist>( output_topic_.toStdString(), 1 );
    }

    Q_EMIT configChanged();
  }
}

/*
//tab2中设置topic命名
void TeleopPanel::set_bus_topic(const QString& new_bus_topic){
  
   if( new_bus_topic != output_topic_tab2_ )
  {
    output_topic_tab2_ = new_bus_topic;
	
    // 如果命名为空，不发布任何信息
    if( output_topic_ == "" )
    {
      bus_topic_publisher_.shutdown();
    }
	// 否则，初始化publisher
    else
    {
      //初始化bustopic 的 publisher
      bus_topic_publisher_ = nh_.advertise<geometry_msgs::Twist>( output_topic_tab2_.toStdString(), 1 );
    }

    Q_EMIT configChanged();
  }
}
*/



// 发布消息
void TeleopPanel::sendVel()
{
  if( ros::ok() && velocity_publisher_ )
  {
    geometry_msgs::Twist msg;
    msg.linear.x = linear_velocity_x;
    msg.linear.y = linear_velocity_y;
    msg.linear.z = 0;
    msg.angular.x = 0;
    msg.angular.y = 0;
    msg.angular.z = angular_velocity_;
    velocity_publisher_.publish( msg );
  }
}

// 重载父类的功能
void TeleopPanel::save( rviz::Config config ) const
{
  rviz::Panel::save( config );
  config.mapSetValue( "Topic", output_topic_ );
}

// 重载父类的功能，加载配置数据
void TeleopPanel::load( const rviz::Config& config )
{
  rviz::Panel::load( config );
  QString topic;
  if( config.mapGetString( "Topic", &topic ))
  {
    output_topic_editor_->setText( topic );                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
    updateTopic();
  }
}

} // end namespace rviz_teleop_commander

//全局变量 
QStringList list_msg_;
QStringListModel *model = new QStringListModel();

//两个显示信息的函数
void rviz_teleop_commander::TeleopPanel::list_msg_show_in_listview()
{
   list_msg_<<QString("Could not connect to hexapod server,")<<QString("Retrying...");
    //显示到listview里
    model->setStringList(list_msg_);
    listView_in_tab2_->setModel(model);
}

void rviz_teleop_commander::TeleopPanel::list_msg_show_in_listview_giveup()
{
  list_msg_<<QString("giving up waiting on result");
  model->setStringList(list_msg_);
   listView_in_tab2_->setModel(model);
}


//bus function
void rviz_teleop_commander::TeleopPanel::OPENBUSButtonFuc()
{
  goal.MODE = SIMPLEMOTION_CONTROL;
  goal.SIMPLEMOTION_MODE = OPENBUS;
  bool server_exists = smcontrol_client.waitForServer(ros::Duration(5.0));
  if (!server_exists){
    
    ROS_WARN("Could not connect to hexapod server, retrying...");
    list_msg_show_in_listview();
    //server_exists = smcontrol_client.waitForServer(ros::Duration(5.0));
    return;
  }
  
  smcontrol_client.sendGoal(goal, &TeleopPanel::doneCb);
  bool finished = smcontrol_client.waitForResult(ros::Duration(5.0));
  
  if(!finished){
    ROS_WARN("giving up waiting on result");
    list_msg_show_in_listview_giveup();
  }
}
    //system("gnome-terminal -x bash -c 'source /opt/ros/indigo/setup.bash;  rostopic pub /simplemotion_mode std_msgs/Int32 '{data: 1}'   '&");
    //system("gnome-terminal -x bash -c 'rostopic pub /simplemotion_mode std_msgs/Int32 '{data: 1}'   '&"); 
  

void rviz_teleop_commander::TeleopPanel::CLOSEBUSButtonFuc()
{
  goal.MODE = SIMPLEMOTION_CONTROL;
  goal.SIMPLEMOTION_MODE = CLOSEBUS;
  bool server_exists = smcontrol_client.waitForServer(ros::Duration(5.0));
  if (!server_exists){
    ROS_WARN("Could not connect to hexapod server, retrying...");
    list_msg_show_in_listview();
    //server_exists = smcontrol_client.waitForServer(ros::Duration(5.0));
    return;
  }
  
  smcontrol_client.sendGoal(goal, &TeleopPanel::doneCb);
  bool finished = smcontrol_client.waitForResult(ros::Duration(5.0));
  
  if(!finished){
    ROS_WARN("giving up waiting on result");
    list_msg_show_in_listview_giveup();
  }
}
 
void rviz_teleop_commander::TeleopPanel::ENABLEMOTORButonFuc()
{
  goal.MODE = SIMPLEMOTION_CONTROL;
  goal.SIMPLEMOTION_MODE = ENABLEMOTOR;
  bool server_exists = smcontrol_client.waitForServer(ros::Duration(5.0));
  if (!server_exists){
    ROS_WARN("Could not connect to hexapod server, retrying...");
    list_msg_show_in_listview();
    //server_exists = smcontrol_client.waitForServer(ros::Duration(5.0));
    return;
  }
  
  smcontrol_client.sendGoal(goal, &TeleopPanel::doneCb);
  bool finished = smcontrol_client.waitForResult(ros::Duration(5.0));
  
  if(!finished){
    ROS_WARN("giving up waiting on result");
    list_msg_show_in_listview_giveup();
  }
}

void rviz_teleop_commander::TeleopPanel::DISABLEMOTORButtonFuc()
{
  goal.MODE = SIMPLEMOTION_CONTROL;
  goal.SIMPLEMOTION_MODE = DISABLEMOTOR;
  bool server_exists = smcontrol_client.waitForServer(ros::Duration(5.0));
  if (!server_exists){
    ROS_WARN("Could not connect to hexapod server, retrying...");
    list_msg_show_in_listview();
    //server_exists = smcontrol_client.waitForServer(ros::Duration(5.0));
    return;
  }
  
  smcontrol_client.sendGoal(goal, &TeleopPanel::doneCb);
  bool finished = smcontrol_client.waitForResult(ros::Duration(5.0));
  
  if(!finished){
    ROS_WARN("giving up waiting on result");
    list_msg_show_in_listview_giveup();
  }
}

void rviz_teleop_commander::TeleopPanel::GETSTATUSButtonFuc()
{
  goal.MODE = SIMPLEMOTION_CONTROL;
  goal.SIMPLEMOTION_MODE = GETSTATUS;
  bool server_exists = smcontrol_client.waitForServer(ros::Duration(5.0));
  if (!server_exists){
    ROS_WARN("Could not connect to hexapod server, retrying...");
    list_msg_show_in_listview();
    //server_exists = smcontrol_client.waitForServer(ros::Duration(5.0));
    return;
  }
  
  smcontrol_client.sendGoal(goal, &TeleopPanel::doneCb);
  bool finished = smcontrol_client.waitForResult(ros::Duration(5.0));
  
  if(!finished){
    ROS_WARN("giving up waiting on result");
    list_msg_show_in_listview_giveup();
  }
}

void rviz_teleop_commander::TeleopPanel::CLEARFAULTButtonFuc()
{
  goal.MODE = SIMPLEMOTION_CONTROL;
  goal.SIMPLEMOTION_MODE = CLEARFAULT;
  bool server_exists = smcontrol_client.waitForServer(ros::Duration(5.0));
  if (!server_exists){
    ROS_WARN("Could not connect to hexapod server, retrying...");
    list_msg_show_in_listview();
    //server_exists = smcontrol_client.waitForServer(ros::Duration(5.0));
    return;
  }
  
  smcontrol_client.sendGoal(goal, &TeleopPanel::doneCb);
  bool finished = smcontrol_client.waitForResult(ros::Duration(5.0));
  
  if(!finished){
    ROS_WARN("giving up waiting on result");
    list_msg_show_in_listview_giveup();
  }
}

void rviz_teleop_commander::TeleopPanel::ALLLEGRESETButtonFuc()
{
  goal.MODE = SIMPLEMOTION_CONTROL;
  goal.SIMPLEMOTION_MODE = ALLLEGRESET;
  bool server_exists = smcontrol_client.waitForServer(ros::Duration(5.0));
  if (!server_exists){
    ROS_WARN("Could not connect to hexapod server, retrying...");
    list_msg_show_in_listview();
    //server_exists = smcontrol_client.waitForServer(ros::Duration(5.0));
    return;
  }
  
  smcontrol_client.sendGoal(goal, &TeleopPanel::doneCb);
  bool finished = smcontrol_client.waitForResult(ros::Duration(5.0));
  
  if(!finished){
    ROS_WARN("giving up waiting on result");
    list_msg_show_in_listview_giveup();
  }
}


 
void rviz_teleop_commander::TeleopPanel::doneCb(const actionlib::SimpleClientGoalState& state, const hexapodservice::hexapodserviceResultConstPtr& result)
{
  ROS_INFO("server responded with state[%s]", state.toString().c_str());
  const char* r = result->result.data();
  ROS_INFO("%s", r);
  ROS_INFO("simplemotion status is: %d", result->status);
}

// 声明此类是一个rviz的插件
#include <pluginlib/class_list_macros.h>
PLUGINLIB_EXPORT_CLASS(rviz_teleop_commander::TeleopPanel,rviz::Panel )

// END_TUTORIAL
