package com.speed.bean.base;
import java.util.*;
import java.text.SimpleDateFormat;
public abstract class  SysUserBase{
    private Map<String,Object> dirtyProperties=new HashMap<String,Object>();
       //
       private int   id=0;
       //
       private String   userName="";
       //
       private String   password="";
       //
       private int   roleID=0;
       //
       private String   permission="";
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
       public String   getUserName(){
              return this.userName;
      }
       public void   setUserName(String userName){
              this.userName=userName;
              dirtyProperties.put("userName",userName);
      }
       public String   getPassword(){
              return this.password;
      }
       public void   setPassword(String password){
              this.password=password;
              dirtyProperties.put("password",password);
      }
       public int   getRoleID(){
              return this.roleID;
      }
       public void   setRoleID(int roleID){
              this.roleID=roleID;
              dirtyProperties.put("roleID",roleID);
      }
       public String   getPermission(){
              return this.permission;
      }
       public void   setPermission(String permission){
              this.permission=permission;
              dirtyProperties.put("permission",permission);
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
