#ifndef TELEOP_PAD_H
#define TELEOP_PAD_H
//© VK control   
//Made by Lincoln

//所需要包含的头文件
#include <ros/ros.h>
#include <ros/console.h>
#include <rviz/panel.h>   //plugin基类的头文件
#include <QListView>

//addbylincoln
/*
#include "../include/simplemotion.h"
#include "../include/hexapodserviceAction.h"
#include <actionlib/server/simple_action_server.h>
#include "../include/legs.h"
#include "../include/legjoints.h"
#include "../include/define.h"
#include "../include/hexapodserviceGoal.h"
*/

#include <actionlib/client/simple_action_client.h>
#include "../include/hexapodserviceAction.h"
#include <std_msgs/Int32.h>
#include "../include/define.h"
#include "../include/hexapodserviceResult.h"


class QTabWidget;   //tab_window 类需要用到
class QPushButton;
class QLineEdit;
class QListView;


namespace rviz_teleop_commander
{
// 所有的plugin都必须是rviz::Panel的子类
    
class TeleopPanel: public rviz::Panel
{
// 后边需要用到Qt的信号和槽，都是QObject的子类，所以需要声明Q_OBJECT宏
Q_OBJECT
public:
  // 构造函数，在类中会用到QWidget的实例来实现GUI界面，这里先初始化为0即可
  TeleopPanel( QWidget* parent = 0 );

  
  
  // 重载rviz::Panel积累中的函数，用于保存、加载配置文件中的数据，在这个plugin
  // 中，数据就是topic的名称
  virtual void load( const rviz::Config& config );
  virtual void save( rviz::Config config ) const;

  // 公共槽.
public Q_SLOTS:
  // 当用户输入topic的命名并按下回车后，回调用此槽来创建一个相应名称的topic publisher
  void setTopic( const QString& topic );
  //void set_bus_topic(const QString& new_bus_topic);
  
  static void doneCb(const actionlib::SimpleClientGoalState& state, const hexapodservice::hexapodserviceResultConstPtr& result);
  
  // 内部槽.
protected Q_SLOTS:
  void sendVel();                 // 发布当前的速度值
  void update_Linear_Velocity();  // 根据用户的输入更新线速度值
  void update_Angular_Velocity(); // 根据用户的输入更新角速度值
  void updateTopic();             // 根据用户的输入更新topic name
 // void updateBusTopic();	  //根据用户输入更新bus topic name
  void stopVelocity();		  //停止运动
 

  
  
  //总线函数槽
  void OPENBUSButtonFuc();
  void CLOSEBUSButtonFuc();
  void ENABLEMOTORButonFuc();
  void DISABLEMOTORButtonFuc();
  void GETSTATUSButtonFuc();
  void CLEARFAULTButtonFuc();
  void ALLLEGRESETButtonFuc();
  
  void list_msg_show_in_listview();
  void list_msg_show_in_listview_giveup();
  //void list_msg_show_of_donecb();
  // 内部变量.
protected:
   // 建立一个tab的页面
   // QTabWidget tab_window_pages_;
    //QString tab_window_;
    
    //第一个界面的元件
    
  // topic name输入框
  QLineEdit* output_topic_editor_;
  QString output_topic_;
  
  /* 第二个界面的topic name输入框
  QLineEdit* output_topic_editor_tab2_;
  QString output_topic_tab2_;
  */
  
  
  // 线速度值输入框
  
  QLineEdit* output_topic_editor_linear_x;
  QString output_topic_linear_x;
  QLineEdit* output_topic_editor_linear_y;
  QString output_topic_linear_y;
  // 角速度值输入框
  QLineEdit* output_topic_editor_angular_z;
  QString output_topic_angular_z;
  
  //第一个界面的pushbutton
  QPushButton *runButton;
  QPushButton *stopButton;
  QPushButton *closeButton;
 
 
  // ROS的publisher，用来发布速度topic
  ros::Publisher velocity_publisher_;
  ros::Publisher bus_topic_publisher_;
  

  // The ROS node handle.
  ros::NodeHandle nh_;

  //addbylincoln
  actionlib::SimpleActionClient<hexapodservice::hexapodserviceAction> smcontrol_client;
  hexapodservice::hexapodserviceGoal goal;

  
  
  // 当前保存的线速度和角速度值
  float linear_velocity_x;
  float linear_velocity_y;
  float angular_velocity_;
  
     //第二个界面的pushbutton
 QPushButton  *OPENBUSButton;
 QPushButton  *CLOSEBUSButton;
 QPushButton  *ENABLEMOTORButon;
 QPushButton  *DISABLEMOTORButton;
 QPushButton  *GETSTATUSButton;
 QPushButton  *CLEARFAULTButton;
 QPushButton  *ALLLEGRESETButton;
 
 QListView    *listView_in_tab2_ ;

};

} // end namespace rviz_teleop_commander



#endif 

// TELEOP_PANEL_H
