package
{
	import flash.errors.IllegalOperationError;
	import flash.events.DRMAuthenticationCompleteEvent;
	import flash.events.DRMAuthenticationErrorEvent;
	import flash.events.DRMErrorEvent;
	import flash.events.DRMStatusEvent;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.events.SecurityErrorEvent;
	import flash.events.StatusEvent;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	import flash.net.URLStream;
	import flash.net.drm.AuthenticationMethod;
	import flash.net.drm.DRMContentData;
	import flash.net.drm.DRMManager;
	import flash.net.drm.DRMVoucher;
	import flash.net.drm.LoadVoucherSetting;
	import flash.net.drm.VoucherAccessInfo;
	import flash.system.SystemUpdater;
	import flash.system.SystemUpdaterType;
	import flash.utils.ByteArray;

	public class DRMService extends EventDispatcher
	{
		
		private var _drmContentData:DRMContentData = null;
		private var _drmManager:DRMManager;
		private var _drmState:String = DRMState.UNINITIALIZED;
		private var voucher:DRMVoucher;
		private var lastToken:ByteArray;
		private static const DRM_AUTHENTICATION_FAILED:int	= 3301;
		private static const DRM_NEEDS_AUTHENTICATION:int	= 3330;
		private static const DRM_CONTENT_NOT_YET_VALID:int	= 3331;
		
		private static var updater:SystemUpdater;
		
		public function DRMService()
		{
			_drmManager = DRMManager.getDRMManager();
		}
		
		public function get drmState():String
		{
			return _drmState;
		}
		
		public function get drmDone():Boolean
		{
			return (_drmState == DRMState.AUTHENTICATION_COMPLETE);
		}
				
		public function set drmMetaData(value:Object):void
		{
			if (value is DRMContentData)
			{
				_drmContentData = value as DRMContentData;
				retrieveVoucher();	
			}
			else
			{	
				try
				{
					trace("start to produce drmcontentdata");
					_drmContentData = new DRMContentData(value as ByteArray);
					retrieveVoucher();	
				}
				catch (argError:ArgumentError)  // DRM ContentData is invalid
				{		
					trace("drmcontentdata: argument error");
					updateDRMState		
					( 	DRMState.AUTHENTICATION_ERROR,
						new MediaError(argError.errorID, "DRMContentData invalid")
					);
					
				}
				catch (error:IllegalOperationError)
				{					
					function onComplete(event:Event):void
					{						
						updater.removeEventListener(Event.COMPLETE, onComplete);
						drmMetaData = value;						
					}			
					trace("updating flash's drm module");
					update(SystemUpdaterType.DRM);
					updater.addEventListener(Event.COMPLETE, onComplete);
				}
			}
		}
		
		public function get drmMetadata():Object
		{		
			return _drmContentData;
		}
		
		private function retrieveVoucher():void
		{				
			updateDRMState(DRMState.AUTHENTICATING);
			
			_drmManager.addEventListener(DRMErrorEvent.DRM_ERROR, onDRMError);
			_drmManager.addEventListener(DRMStatusEvent.DRM_STATUS, onVoucherLoaded);
			//this.addEventListener(DRMErrorEvent.DRM_ERROR, _drmManager.onDRMError);
			
			_drmManager.loadVoucher(_drmContentData, LoadVoucherSetting.ALLOW_SERVER);					
		}
		
		public function updateDRMState(newState:String,  error:MediaError = null,  start:Date = null, end:Date = null, period:Number = 0 , token:Object=null, prompt:String = null ):void
		{
			_drmState = newState;
			dispatchEvent
			( new DRMEvent
				( DRMEvent.DRM_STATE_CHANGE,
					newState,
					false,
					false,
					start,
					end,
					period,
					prompt,
					token,
					error
				)
			);
		}
		
		public function authenticate(username:String = null, password:String = null):void
		{			
			if (_drmContentData == null)
			{
				throw new IllegalOperationError(SysString.getString(SysString.DRM_METADATA_NOT_SET));
			}
			
			_drmManager.addEventListener(DRMAuthenticationErrorEvent.AUTHENTICATION_ERROR, authError);			
			_drmManager.addEventListener(DRMAuthenticationCompleteEvent.AUTHENTICATION_COMPLETE, authComplete);		
			
			if (password == null && username == null)
			{
				retrieveVoucher();
			}	
			else
			{
				_drmManager.authenticate(_drmContentData.serverURL, _drmContentData.domain, username, password);
			}
		}
		
		public function authenticateWithToken(token:Object):void
		{
			if (_drmContentData == null)
			{
				throw new IllegalOperationError(SysString.getString(SysString.DRM_METADATA_NOT_SET));
			}
			
			_drmManager.setAuthenticationToken(_drmContentData.serverURL, _drmContentData.domain, token as ByteArray);
			retrieveVoucher();
		}
		
		private function forceRefreshVoucher():void
		{
			_drmManager.loadVoucher(_drmContentData, LoadVoucherSetting.FORCE_REFRESH);
		}
		
		public function onVoucherLoaded(event:DRMStatusEvent):void
		{	
			trace("onVoucherLoaded, DRM status changed");
			if (event.contentData == _drmContentData)
			{
				var now:Date = new Date();	
				if (event.voucher && 
					(
						(   event.voucher.voucherEndDate == null
							|| event.voucher.voucherEndDate.time >= now.time)
						&& (   event.voucher.voucherStartDate == null
							|| event.voucher.voucherStartDate.time <= now.time)
					)
				)
				{
					this.voucher = event.voucher;		
					removeEventListeners();
					
					if (voucher.playbackTimeWindow == null)
					{					
						updateDRMState(DRMState.AUTHENTICATION_COMPLETE, null, voucher.voucherStartDate, voucher.voucherEndDate, period, lastToken);
					} 
					else
					{
						updateDRMState(DRMState.AUTHENTICATION_COMPLETE, null, voucher.playbackTimeWindow.startDate, voucher.playbackTimeWindow.endDate, voucher.playbackTimeWindow.period, lastToken);
					}								
				}
				else  //Only force refresh if voucher was good, and has expired (local voucher).
				{
					forceRefreshVoucher();
				}	
			}	
		}
		
		/**
		 * @private
		 * 
		 * Signals failures from the DRMsubsystem not captured though the 
		 * DRMServices class.
		 *  
		 *  @langversion 3.0
		 *  @playerversion Flash 10.1
		 *  @playerversion AIR 1.5
		 *  @productversion OSMF 1.0
		 */	
		public function inlineDRMFailed(error:MediaError):void
		{
			updateDRMState(DRMState.AUTHENTICATION_ERROR, error);
		}
		
		/**
		 * @private
		 * Signals DRM is available, taken from the inline netstream APIs.
		 * Assumes the voucher is available.
		 * 	
		 *  @langversion 3.0
		 *  @playerversion Flash 10.1
		 *  @playerversion AIR 1.5
		 *  @productversion OSMF 1.0
		 */	
		public function inlineOnVoucher(event:DRMStatusEvent):void
		{
			_drmContentData = event.contentData;
			onVoucherLoaded(event);
		}
		
		public function onDRMError(event:DRMErrorEvent):void
		{
			trace("catched DRMError event");
			if (event.contentData == _drmContentData) //Ensure this event is for our data.
			{
				trace("error id " + event.errorID);
				switch(event.errorID)
				{
					case DRM_CONTENT_NOT_YET_VALID:
						forceRefreshVoucher();
						break;
					case DRM_NEEDS_AUTHENTICATION:
						updateDRMState(DRMState.AUTHENTICATION_NEEDED, null, null, null, 0, null, event.contentData.serverURL );
						break;
					default:
						removeEventListeners();							
						updateDRMState(DRMState.AUTHENTICATION_ERROR,  new MediaError(event.errorID, event.text));	
						break;
				}
			}	
		}
		
		private function authComplete(event:DRMAuthenticationCompleteEvent):void
		{
			trace("authComplete");
			_drmManager.removeEventListener(DRMAuthenticationErrorEvent.AUTHENTICATION_ERROR, authError);
			_drmManager.removeEventListener(DRMAuthenticationCompleteEvent.AUTHENTICATION_COMPLETE, authComplete);
			lastToken = event.token;
			retrieveVoucher();
		}			
		
		private function authError(event:DRMAuthenticationErrorEvent):void
		{
			trace("authError");
			_drmManager.removeEventListener(DRMAuthenticationErrorEvent.AUTHENTICATION_ERROR, authError);
			_drmManager.removeEventListener(DRMAuthenticationCompleteEvent.AUTHENTICATION_COMPLETE, authComplete);
			
			updateDRMState(DRMState.AUTHENTICATION_ERROR, new MediaError(event.errorID, event.toString()));
		}
		
		public function get period():Number
		{
			if (voucher != null)
			{
				return voucher.playbackTimeWindow ? voucher.playbackTimeWindow.period : (voucher.voucherEndDate && voucher.voucherStartDate) ? (voucher.voucherEndDate.time - voucher.voucherStartDate.time)/1000 : 0;	
			}
			else
			{
				return NaN;
			}		
		}
		
		public function get startDate():Date
		{
			if (voucher != null)
			{
				return voucher.playbackTimeWindow ? voucher.playbackTimeWindow.startDate : voucher.voucherStartDate;	
			}
			else
			{
				return null;
			}			
		}
		
		public function get endDate():Date
		{
			if (voucher != null)
			{
				return voucher.playbackTimeWindow ? voucher.playbackTimeWindow.endDate : voucher.voucherEndDate;	
			}
			else
			{
				return null;
			}			
		}
		
		private function removeEventListeners():void
		{
			_drmManager.removeEventListener(DRMErrorEvent.DRM_ERROR, onDRMError);
			_drmManager.removeEventListener(DRMStatusEvent.DRM_STATUS, onVoucherLoaded);
		}
		
		public function update(type:String):SystemUpdater
		{
			updateDRMState(DRMState.DRM_SYSTEM_UPDATING);		
			if (updater == null) //An update hasn't been triggered
			{
				updater = new SystemUpdater();
				//If there is an update already happening, just wait for it to finish.			
				toggleErrorListeners(updater, true);	
				updater.update(type);
			}
			else
			{
				//If there is an update already happening, just wait for it to finish.			
				toggleErrorListeners(updater, true);	
			}
			
			return updater;		
		}
		
		private function toggleErrorListeners(updater:SystemUpdater, on:Boolean):void
		{
			if (on)
			{
				updater.addEventListener(Event.COMPLETE, onUpdateComplete);
				updater.addEventListener(Event.CANCEL, onUpdateComplete);
				updater.addEventListener(IOErrorEvent.IO_ERROR, onUpdateError);
				updater.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onUpdateError);
				updater.addEventListener(StatusEvent.STATUS, onUpdateError);
			}
			else
			{
				updater.removeEventListener(Event.COMPLETE, onUpdateComplete);
				updater.removeEventListener(Event.CANCEL, onUpdateComplete);
				updater.removeEventListener(IOErrorEvent.IO_ERROR, onUpdateError);
				updater.removeEventListener(SecurityErrorEvent.SECURITY_ERROR, onUpdateError);
				updater.removeEventListener(StatusEvent.STATUS, onUpdateError);
			}
		}	
		
		private function onUpdateComplete(event:Event):void
		{
			toggleErrorListeners(updater, false);
		}
		
		private function onUpdateError(event:Event):void
		{
			toggleErrorListeners(updater, false);
			updateDRMState(DRMState.AUTHENTICATION_ERROR, new MediaError(MediaErrorCodes.DRM_SYSTEM_UPDATE_ERROR, event.toString()));
		}
		
	}
	
}