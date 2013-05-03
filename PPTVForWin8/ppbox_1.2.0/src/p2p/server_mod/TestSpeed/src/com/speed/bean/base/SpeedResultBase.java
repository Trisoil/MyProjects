package com.speed.bean.base;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import org.json.JSONObject;
public abstract class  SpeedResultBase{
    private Map<String,Object> dirtyProperties=new HashMap<String,Object>();
       //ID
       private int   id=0;
       //
       private String   speedKey="";
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
    }
}
