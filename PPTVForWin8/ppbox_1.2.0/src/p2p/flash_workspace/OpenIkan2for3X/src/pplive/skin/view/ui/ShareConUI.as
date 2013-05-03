package pplive.skin.view.ui{
	
	import flash.display.Loader;
	import flash.display.MovieClip;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.MouseEvent;
	import flash.net.URLRequest;
	import flash.system.System;
	import pplive.skin.view.utils.UtilsMethod;
	import pplive.skin.view.events.SkinEvent;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class ShareUI extends MovieClip {
		
		public var Name:String = "share";
		private var _recommend_mc:MovieClip;
		private var _tip_mc:MovieClip;
		private var _error_mc:MovieClip;
		private var _jsonObj:Object;
		private var _groupArr:Array;
		private var _share_mc:MovieClip;
		
		public function ShareUI() {
			init();
		}
		
		private function init():void {
			this.visible = false;
			_share_mc = this.getChildByName("share_mc") as MovieClip;
			_share_mc["videoAddressCopy"].addEventListener(MouseEvent.CLICK, onClickHandler);
			_share_mc["flashAddressCopy"].addEventListener(MouseEvent.CLICK, onClickHandler);
			_share_mc["htmlAddressCopy"].addEventListener(MouseEvent.CLICK, onClickHandler);
			_share_mc["sharefiximg"].buttonMode = true;
			_share_mc["sharefiximg"].mouseChildren = false;
			_share_mc["sharefiximg"].addEventListener(MouseEvent.CLICK, onShareFixHandler);
			_share_mc["shareReplay"].addEventListener(MouseEvent.CLICK, onReplayHandler);
		}
		
		private function onReplayHandler(e:MouseEvent):void {
			this.visible = false;
			this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_PLAY));
		}
		
		private function onShareFixHandler(e:MouseEvent):void {
			UtilsMethod.getURL(UtilsMethod.curr_link);
		}
		
		private function onClickHandler(e:MouseEvent):void {
			//_share_mc["copyTip"].visible = true;
			var str:String = "";
			switch (e.currentTarget) {
				case _share_mc["videoAddressCopy"]:
					System.setClipboard(_share_mc["videoAddress"].text);
					str = "视频地址已复制成功,您可以通过E-mail/MSN/QQ等通讯工具分享。";
					break;
				case _share_mc["flashAddressCopy"]:
					System.setClipboard(_share_mc["flashAddress"].text);
					str = "flash地址已复制成功，您可以通过支持插入flash的博客或论坛分享。";
					break;
				case _share_mc["htmlAddressCopy"]:
					System.setClipboard(_share_mc["htmlAddress"].text);
					str = "html代码已复制成功，您可以通过支持html编辑的网页、论坛或博客分享。";
					break;
			}
			UtilsMethod.skin.showError( { "title":str } );
		}
		
		public function showIcon(arr:Array):void {
			for (var i:int = 0; i < arr.length; i++) {
				var _mc_:MovieClip = new MovieClip();
				_share_mc["icon_mc"].addChild(_mc_);
				_mc_.x = i * 24;
				_mc_._link_ = arr[i]["link"];
				_mc_._name_ = arr[i]["name"];
				var _loader:Loader = new Loader();
				_mc_.addChild(_loader);
				_loader.load(new URLRequest(arr[i]["icon"]));
				_mc_.buttonMode = true;
				_mc_.addEventListener(MouseEvent.CLICK, onLinkHandler);
				_mc_.addEventListener(MouseEvent.MOUSE_OVER, onOverHandler);
				_mc_.addEventListener(MouseEvent.MOUSE_OUT, onOutHandler);
			}
		}
		
		private function onOverHandler(e:MouseEvent):void {
			e.target["parent"].addChild(UtilsMethod.tip);
			UtilsMethod.showTip(e.currentTarget._name_, this);
		}
		
		private function onOutHandler(e:MouseEvent):void {
			UtilsMethod.hideTip();
		}
		
		private function onLinkHandler(e:MouseEvent):void {
			UtilsMethod.getURL(e.currentTarget._link_.replace("[LINK]", UtilsMethod.curr_link).replace("[TITLE]", UtilsMethod.curr_title));
		}
		
		public function showAddress(videoAddre:String, flashAddre:String):void {
			_share_mc["videoAddress"].text = videoAddre;
			_share_mc["flashAddress"].text = flashAddre;
			_share_mc["htmlAddress"].text = '<embed src="' + flashAddre + '" quality="high" width="480" height="390" align="middle" allowScriptAccess="always" allownetworking="all" allowfullscreen="true" type="application/x-shockwave-flash" wmode="window"></embed>';
		}
		
		public function showTitle(txt:String):void {
			_share_mc["sharetitle"].text = txt;
		}
		
		public function showImage(url:String):void {
			var _loader:Loader = new Loader();
			_loader.contentLoaderInfo.addEventListener(Event.COMPLETE, onCompleteHandler);
			_loader.load(new URLRequest(url));
		}
		
		private function onCompleteHandler(e:Event):void {
			var _this_loader:Loader = e.target.loader as Loader;
			_this_loader.width = 160;
			_this_loader.height = 120;
			_share_mc["sharefiximg"]["sharefiximgBg"].addChild(_this_loader);
		}
		
		public function isVisible(bool:Boolean = true):void {		
			this.visible = bool;
			resize();
		}
		
		public function resize():void {
			if (this.visible) {
				this.graphics.clear();
				this.graphics.beginFill(0x000000);
				this.graphics.drawRect(0, 32, this.stage.stageWidth, this.stage.stageHeight - 62);
				this.graphics.endFill();				
				_share_mc.x = (this.stage.stageWidth - _share_mc.width) / 2;
				_share_mc.y = 32 + (this.stage.stageHeight - 62 - _share_mc.height) / 2;
			} else {
				this.graphics.clear();
			}
		}
		
	}

}