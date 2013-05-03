package com.speed.bean.base;
import java.util.*;
import java.text.SimpleDateFormat;
public abstract class  SysModuleBase{
    private Map<String,Object> dirtyProperties=new HashMap<String,Object>();
       //
       private int   id=0;
       //
       private String   moduleID="";
       //
       private String   moduleName="";
       //
       private String   parentID="";
       //
       private String   link="";
       //
       private String   linkClass="";
       //
       private int   status=0;
       //
       private String   memo="";
       //
       private Date   createTime=null;
       public int   getId(){
              return this.id;
      }
       public void   setId(int id){
              this.id=id;
      }
       public String   getModuleID(){
              return this.moduleID;
      }
       public void   setModuleID(String moduleID){
              this.moduleID=moduleID;
              dirtyProperties.put("moduleID",moduleID);
      }
       public String   getModuleName(){
              return this.moduleName;
      }
       public void   setModuleName(String moduleName){
              this.moduleName=moduleName;
              dirtyProperties.put("moduleName",moduleName);
      }
       public String   getParentID(){
              return this.parentID;
      }
       public void   setParentID(String parentID){
              this.parentID=parentID;
              dirtyProperties.put("parentID",parentID);
      }
       public String   getLink(){
              return this.link;
      }
       public void   setLink(String link){
              this.link=link;
              dirtyProperties.put("link",link);
      }
       public String   getLinkClass(){
              return this.linkClass;
      }
       public void   setLinkClass(String linkClass){
              this.linkClass=linkClass;
              dirtyProperties.put("linkClass",linkClass);
      }
       public int   getStatus(){
              return this.status;
      }
       public void   setStatus(int status){
              this.status=status;
              dirtyProperties.put("status",status);
      }
       public String   getMemo(){
              return this.memo;
      }
       public void   setMemo(String memo){
              this.memo=memo;
              dirtyProperties.put("memo",memo);
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
