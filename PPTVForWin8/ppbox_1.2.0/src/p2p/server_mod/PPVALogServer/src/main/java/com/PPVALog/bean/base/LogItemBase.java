package com.PPVALog.bean.base;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import org.json.JSONObject;
public abstract class  LogItemBase{
    private Map<String,Object> dirtyProperties=new HashMap<String,Object>();
       //
       private int   id=0;
       //
       private String   address="";
       //
       private String   peer_version="";
       //
       private String   peer_guid="";
       //
       private Date   time=null;
       //
       private String   log_file_md5="";
       //
       private String   resource_id="";
       //
       private String   resource_name="";
       //
       private String   statistics_id="";
       public int   getId(){
              return this.id;
      }
       public void   setId(int id){
              this.id=id;
      }
       public String   getAddress(){
              return this.address;
      }
       public void   setAddress(String address){
              this.address=address;
              dirtyProperties.put("address",address);
      }
       public String   getPeer_version(){
              return this.peer_version;
      }
       public void   setPeer_version(String peer_version){
              this.peer_version=peer_version;
              dirtyProperties.put("peer_version",peer_version);
      }
       public String   getPeer_guid(){
              return this.peer_guid;
      }
       public void   setPeer_guid(String peer_guid){
              this.peer_guid=peer_guid;
              dirtyProperties.put("peer_guid",peer_guid);
      }
       public Date   getTime(){
              return this.time;
      }
      public String   getTime_ToDateString(){
              if(time==null)   return "";
              SimpleDateFormat   formatter   =   new SimpleDateFormat("yyyy-MM-dd");
              return formatter.format(this.time);
      }
      public String   getTime_ToTimeString(){
              if(time==null)   return "";
              SimpleDateFormat   formatter   =   new SimpleDateFormat("HH:mm:ss");
              return formatter.format(this.time);
      }
    public String   getTime_ToDateTimeString(){
              if(time==null)   return "";
             SimpleDateFormat   formatter   =   new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
              return formatter.format(this.time);
      }
       public void   setTime(Date time){
              this.time=time;
              dirtyProperties.put("time",time);
      }
       public String   getLog_file_md5(){
              return this.log_file_md5;
      }
       public void   setLog_file_md5(String log_file_md5){
              this.log_file_md5=log_file_md5;
              dirtyProperties.put("log_file_md5",log_file_md5);
      }
       public String   getResource_id(){
              return this.resource_id;
      }
       public void   setResource_id(String resource_id){
              this.resource_id=resource_id;
              dirtyProperties.put("resource_id",resource_id);
      }
       public String   getResource_name(){
              return this.resource_name;
      }
       public void   setResource_name(String resource_name){
              this.resource_name=resource_name;
              dirtyProperties.put("resource_name",resource_name);
      }
       public String   getStatistics_id(){
              return this.statistics_id;
      }
       public void   setStatistics_id(String statistics_id){
              this.statistics_id=statistics_id;
              dirtyProperties.put("statistics_id",statistics_id);
      }
     public Map<String,Object>getDirtyProperties(){
            return this.dirtyProperties;
     }
     public JSONObject toJSON()  throws Exception{
        JSONObject json=new JSONObject();
        SimpleDateFormat   formatter   =   new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        json.put("id", this.getId());
        if(address!=null){
           json.put("address",this.getAddress());
        }
        if(peer_version!=null){
           json.put("peer_version",this.getPeer_version());
        }
        if(peer_guid!=null){
           json.put("peer_guid",this.getPeer_guid());
        }
        if(time!=null){
             json.put("time",formatter.format(this.getTime()));
        }
        if(log_file_md5!=null){
           json.put("log_file_md5",this.getLog_file_md5());
        }
        if(resource_id!=null){
           json.put("resource_id",this.getResource_id());
        }
        if(resource_name!=null){
           json.put("resource_name",this.getResource_name());
        }
        if(statistics_id!=null){
           json.put("statistics_id",this.getStatistics_id());
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
            if(json.get("address")!=null){
                 this.setAddress(json.get("address")+"");
            }
       }catch(Exception ex){}
        try{
            if(json.get("peer_version")!=null){
                 this.setPeer_version(json.get("peer_version")+"");
            }
       }catch(Exception ex){}
        try{
            if(json.get("peer_guid")!=null){
                 this.setPeer_guid(json.get("peer_guid")+"");
            }
       }catch(Exception ex){}
        try{
            if(json.get("time")!=null){
                 this.setTime(formatter.parse(json.get("time")+""));
            }
       }catch(Exception ex){}
        try{
            if(json.get("log_file_md5")!=null){
                 this.setLog_file_md5(json.get("log_file_md5")+"");
            }
       }catch(Exception ex){}
        try{
            if(json.get("resource_id")!=null){
                 this.setResource_id(json.get("resource_id")+"");
            }
       }catch(Exception ex){}
        try{
            if(json.get("resource_name")!=null){
                 this.setResource_name(json.get("resource_name")+"");
            }
       }catch(Exception ex){}
        try{
            if(json.get("statistics_id")!=null){
                 this.setStatistics_id(json.get("statistics_id")+"");
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
            if(json.containsKey("address".toLowerCase())){
                 this.setAddress(json.get("address".toLowerCase())+"");
            }
       }catch(Exception ex){}
        try{
            if(json.containsKey("peer_version".toLowerCase())){
                 this.setPeer_version(json.get("peer_version".toLowerCase())+"");
            }
       }catch(Exception ex){}
        try{
            if(json.containsKey("peer_guid".toLowerCase())){
                 this.setPeer_guid(json.get("peer_guid".toLowerCase())+"");
            }
       }catch(Exception ex){}
        try{
            if(json.containsKey("time".toLowerCase())){
                 this.setTime(formatter.parse(json.get("time".toLowerCase())+""));
            }
       }catch(Exception ex){}
        try{
            if(json.containsKey("log_file_md5".toLowerCase())){
                 this.setLog_file_md5(json.get("log_file_md5".toLowerCase())+"");
            }
       }catch(Exception ex){}
        try{
            if(json.containsKey("resource_id".toLowerCase())){
                 this.setResource_id(json.get("resource_id".toLowerCase())+"");
            }
       }catch(Exception ex){}
        try{
            if(json.containsKey("resource_name".toLowerCase())){
                 this.setResource_name(json.get("resource_name".toLowerCase())+"");
            }
       }catch(Exception ex){}
        try{
            if(json.containsKey("statistics_id".toLowerCase())){
                 this.setStatistics_id(json.get("statistics_id".toLowerCase())+"");
            }
       }catch(Exception ex){}
    }
}
