package pplive.skin.command
{
	import org.puremvc.as3.interfaces.INotification;
	
	import pplive.ui.mediator.SkinHDMediator;
	
	public class SkinHDMediatorCommand extends SkinMediatorCommand
	{
		public function SkinHDMediatorCommand()
		{
			super();
		}
		public override function execute(noti:INotification):void
		{
			var md:SkinHDMediator = new SkinHDMediator(noti.getBody())
			facade.registerMediator(md); 
		}
		
	}
}