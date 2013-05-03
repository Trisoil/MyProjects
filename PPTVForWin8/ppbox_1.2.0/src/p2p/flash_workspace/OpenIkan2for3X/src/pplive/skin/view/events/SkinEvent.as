package pplive.skin.view.events {
	
	import flash.events.Event;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class SkinEvent extends Event {
		
		public static const MEDIA_PAUSE:String = "media_pause";
		public static const MEDIA_PLAY:String = "media_play";
		public static const MEDIA_STOP:String = "media_stop";
		
		public static const MEDIA_SEARCH:String = "media_search";
		
		public static const MEDIA_PREV:String = "media_prev";
		public static const MEDIA_PREV_OVER:String = "media_prev_over";
		public static const MEDIA_NEXT:String = "media_next";
		public static const MEDIA_NEXT_OVER:String = "media_next_over";
		
		public static const MEDIA_SOUND:String = "media_sound";
		public static const MEDIA_MUTE:String = "media_mute";
		public static const MEDIA_UNMUTE:String = "media_unmute";
		
		public static const MEDIA_CINEMA:String = "media_cinema";
		public static const MEDIA_SHARE:String = "media_share";
		
		public static const MEDIA_LIGHT:String = "media_light";
		public static const MEDIA_BROADCAST:String = "media_broadcast";
		public static const MEDIA_SHOW_BROADCAST:String = "media_show_broadcast";
		
		public static const MEDIA_DOWNLOAD:String = "media_download";
		public static const MEDIA_SNAPSHOTPIC:String = "media_snapshotpic";
		
		public static const MEDIA_SHOW_BRIGHTNESS:String = "media_show_Brightness";
		public static const MEDIA_HIDE_BRIGHTNESS:String = "media_hide_Brightness";
		public static const MEDIA_BRIGHTNESS:String = "media_Brightness";
		
		public static const MEDIA_SHOW_CHANGE:String = "media_show_change";
		public static const MEDIA_HIDE_CHANGE:String = "media_hide_change";
		public static const MEDIA_CHANGE:String = "media_change";
		
		public static const MEDIA_VOD_POSITION:String = "media_vod_position";
		public static const MEDIA_LIVE_POSITION:String = "media_live_position";
		
		public static const MEDIA_LOGO_CLICK:String = "media_logo_click";
		public static const LAYOUT_SUCCESS:String = "layout_success";
		public static const LAYOUT_FAILED:String = "layout_failed";
//		public static const SHOW_SHARE:String = "show_share";
		public static const SHOW_TITLE:String = "show_title";
		public static const CURR_STATE:String = "curr_state";
		public static const WINDOW_CHANGE:String = "window_change";
		
		public static const MEDIA_RESIZE:String = "media_resize";
		
		public static const MEDIA_LOGO:String = "media_logo";
		
		public static const MEDIA_POSITION:String = "media_position";//??
		
		public static const MEDIA_RECOMMEND:String = "media_recommend";
		
		public static const PLUGIN_VERSION:String = "plugin_version";
		public static const MEDIA_SKIP_PLAY:String = "media_skip_play";
		
		private var _currObj:Object = null;
		
		public function SkinEvent(type:String, currObj:Object = null, bubbles:Boolean = false, cancelable:Boolean = false) {
			super(type, bubbles, cancelable);
			_currObj = currObj;
		}
		
		public function get currObj():Object { return _currObj; }		
		public function set currObj(value:Object):void {
			_currObj = value;
		}	
		
	}

}