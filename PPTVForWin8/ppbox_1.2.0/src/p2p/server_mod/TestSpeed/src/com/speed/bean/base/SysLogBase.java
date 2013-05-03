package com.speed.bean.base;
import java.util.*;
import java.text.SimpleDateFormat;
public abstract class  SysLogBase{
    private Map<String,Object> dirtyProperties=new HashMap<String,Object>();
       //ID
       private int   id=0;
       //用户名
       private String   userName="";
       //操作名称
       private String   operate="";
       //内容(执行的SQL语句等)
       private String   content="";
       //IP地址
       private String   iP="";
       //创建时间
       private Date   createTime=null;
       public int   getId(){
              return this.id;
      }
       public void   setId(int id){
              this.id=id;
      }
       public String   getUserName(){
              return this.userName;
      }
       public void   setUserName(String userName){
              this.userName=userName;
              dirtyProperties.put("userName",userName);
      }
       public String   getOperate(){
              return this.operate;
      }
       public void   setOperate(String operate){
              this.operate=operate;
              dirtyProperties.put("operate",operate);
      }
       public String   getContent(){
              return this.content;
      }
       public void   setContent(String content){
              this.content=content;
              dirtyProperties.put("content",content);
      }
       public String   getIP(){
              return this.iP;
      }
       public void   setIP(String iP){
              this.iP=iP;
              dirtyProperties.put("iP",iP);
      }
       public Date   getCreateTime(){
              return this.createTime;
      }
      public String   getCreateTime_ToDateString(){
              if(createTime==null)   return "";
              SimpleDateFormat   formatter   =   new SimpleDateFormat("yyyy-MM-dd");
              return formatter.format(this.createTime);
      }
      public String   getCreateTime_ToTimeString(){
              if(createTime==null)   return "";
              SimpleDateFormat   formatter   =   new SimpleDateFormat("HH:mm:ss");
              return formatter.format(this.createTime);
      }
    public String   getCreateTime_ToDateTimeString(){
              if(createTime==null)   return "";
             SimpleDateFormat   formatter   =   new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
              return formatter.format(this.createTime);
      }
       public void   setCreateTime(Date createTime){
              this.createTime=createTime;
              dirtyProperties.put("createTime",createTime);
      }
     public Map<String,Object>getDirtyProperties(){
            return this.dirtyProperties;
     }
}
