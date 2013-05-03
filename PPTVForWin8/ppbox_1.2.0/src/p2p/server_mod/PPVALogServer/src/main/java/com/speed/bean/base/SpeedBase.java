package com.speed.bean.base;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import org.json.JSONObject;
public abstract class  SpeedBase{
    private Map<String,Object> dirtyProperties=new HashMap<String,Object>();
       //ID
       private int   id=0;
       //
       private String   speedKey="";
       //鍒涘缓鏃堕棿
       private Date   createTime=null;
       //
       private String   resultID="";
       //
       private int   resultValue=0;
       //
       private Date   completeTime=null;
       //
       private String   note="";
       //
       private String   userIp="";
       //鎿嶄綔浜�
       private String   operator="";
       public int   getId(){
              return this.id;
      }
       public void   setId(int id){
              this.id=id;
      }
       public String   getSpeedKey(){
              return this.speedKey;
      }
       public void   setSpeedKey(String speedKey){
              this.speedKey=speedKey;
              dirtyProperties.put("speedKey",speedKey);
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
       public String   getResultID(){
              return this.resultID;
      }
       public void   setResultID(String resultID){
              this.resultID=resultID;
              dirtyProperties.put("resultID",resultID);
      }
       public int   getResultValue(){
              return this.resultValue;
      }
       public void   setResultValue(int resultValue){
              this.resultValue=resultValue;
              dirtyProperties.put("resultValue",resultValue);
      }
       public Date   getCompleteTime(){
              return this.completeTime;
      }
      public String   getCompleteTime_ToDateString(){
              if(completeTime==null)   return "";
              SimpleDateFormat   formatter   =   new SimpleDateFormat("yyyy-MM-dd");
              return formatter.format(this.completeTime);
      }
      public String   getCompleteTime_ToTimeString(){
              if(completeTime==null)   return "";
              SimpleDateFormat   formatter   =   new SimpleDateFormat("HH:mm:ss");
              return formatter.format(this.completeTime);
      }
    public String   getCompleteTime_ToDateTimeString(){
              if(completeTime==null)   return "";
             SimpleDateFormat   formatter   =   new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
              return formatter.format(this.completeTime);
      }
       public void   setCompleteTime(Date completeTime){
              this.completeTime=completeTime;
              dirtyProperties.put("completeTime",completeTime);
      }
       public String   getNote(){
              return this.note;
      }
       public void   setNote(String note){
              this.note=note;
              dirtyProperties.put("note",note);
      }
       public String   getUserIp(){
              return this.userIp;
      }
       public void   setUserIp(String userIp){
              this.userIp=userIp;
              dirtyProperties.put("userIp",userIp);
      }
       public String   getOperator(){
              return this.operator;
      }
       public void   setOperator(String operator){
              this.operator=operator;
              dirtyProperties.put("operator",operator);
      }
     public Map<String,Object>getDirtyProperties(){
            return this.dirtyProperties;
     }
     public JSONObject toJSON()  throws Exception{
        JSONObject json=new JSONObject();
        SimpleDateFormat   formatter   =   new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        json.put("id", this.getId());
        if(speedKey!=null){
           json.put("speedKey",this.getSpeedKey());
        }
        if(createTime!=null){
             json.put("createTime",formatter.format(this.getCreateTime()));
        }
        if(resultID!=null){
           json.put("resultID",this.getResultID());
        }
        json.put("resultValue", this.getResultValue());
        if(completeTime!=null){
             json.put("completeTime",formatter.format(this.getCompleteTime()));
        }
        if(note!=null){
           json.put("note",this.getNote());
        }
        if(userIp!=null){
           json.put("userIp",this.getUserIp());
        }
        if(operator!=null){
           json.put("operator",this.getOperator());
        }
        return json;
    }
    public void fromJSON(JSONObject json) throws Exception{
       if(json==null) return;
       SimpleDateFormat   formatter   =   new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        try{
           if(json.get("id")!=null){
                 this.setId(Integer.parseInt(json.get("id")+""));
            }
       }catch(Exception ex){}
        try{
            if(json.get("speedKey")!=null){
                 this.setSpeedKey(json.get("speedKey")+"");
            }
       }catch(Exception ex){}
        try{
            if(json.get("createTime")!=null){
                 this.setCreateTime(formatter.parse(json.get("createTime")+""));
            }
       }catch(Exception ex){}
        try{
            if(json.get("resultID")!=null){
                 this.setResultID(json.get("resultID")+"");
            }
       }catch(Exception ex){}
        try{
           if(json.get("resultValue")!=null){
                 this.setResultValue(Integer.parseInt(json.get("resultValue")+""));
            }
       }catch(Exception ex){}
        try{
            if(json.get("completeTime")!=null){
                 this.setCompleteTime(formatter.parse(json.get("completeTime")+""));
            }
       }catch(Exception ex){}
        try{
            if(json.get("note")!=null){
                 this.setNote(json.get("note")+"");
            }
       }catch(Exception ex){}
        try{
            if(json.get("userIp")!=null){
                 this.setUserIp(json.get("userIp")+"");
            }
       }catch(Exception ex){}
        try{
            if(json.get("operator")!=null){
                 this.setOperator(json.get("operator")+"");
            }
       }catch(Exception ex){}
    }
    public void fromMap(Map<String,String> json) throws Exception{
       if(json==null) return;
       SimpleDateFormat   formatter   =   new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        try{
           if(json.containsKey("id".toLowerCase())){
                 this.setId(Integer.parseInt(json.get("id".toLowerCase())+""));
            }
       }catch(Exception ex){}
        try{
            if(json.containsKey("speedKey".toLowerCase())){
                 this.setSpeedKey(json.get("speedKey".toLowerCase())+"");
            }
       }catch(Exception ex){}
        try{
            if(json.containsKey("createTime".toLowerCase())){
                 this.setCreateTime(formatter.parse(json.get("createTime".toLowerCase())+""));
            }
       }catch(Exception ex){}
        try{
            if(json.containsKey("resultID".toLowerCase())){
                 this.setResultID(json.get("resultID".toLowerCase())+"");
            }
       }catch(Exception ex){}
        try{
           if(json.containsKey("resultValue".toLowerCase())){
                 this.setResultValue(Integer.parseInt(json.get("resultValue".toLowerCase())+""));
            }
       }catch(Exception ex){}
        try{
            if(json.containsKey("completeTime".toLowerCase())){
                 this.setCompleteTime(formatter.parse(json.get("completeTime".toLowerCase())+""));
            }
       }catch(Exception ex){}
        try{
            if(json.containsKey("note".toLowerCase())){
                 this.setNote(json.get("note".toLowerCase())+"");
            }
       }catch(Exception ex){}
        try{
            if(json.containsKey("userIp".toLowerCase())){
                 this.setUserIp(json.get("userIp".toLowerCase())+"");
            }
       }catch(Exception ex){}
        try{
            if(json.containsKey("operator".toLowerCase())){
                 this.setOperator(json.get("operator".toLowerCase())+"");
            }
       }catch(Exception ex){}
    }
}
