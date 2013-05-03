package com.speed.bean.base;
import java.util.*;
import java.text.SimpleDateFormat;
public abstract class  SysRoleBase{
    private Map<String,Object> dirtyProperties=new HashMap<String,Object>();
       //
       private int   id=0;
       //
       private String   roleName="";
       //
       private String   permission="";
       //
       private Date   createTime=null;
       public int   getId(){
              return this.id;
      }
       public void   setId(int id){
              this.id=id;
      }
       public String   getRoleName(){
              return this.roleName;
      }
       public void   setRoleName(String roleName){
              this.roleName=roleName;
              dirtyProperties.put("roleName",roleName);
      }
       public String   getPermission(){
              return this.permission;
      }
       public void   setPermission(String permission){
              this.permission=permission;
              dirtyProperties.put("permission",permission);
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
