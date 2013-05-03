package pplive.skin.command
{
	import org.puremvc.as3.interfaces.INotification;
	
	import pplive.ui.mediator.SkinVIPMediator;
	
	public class SkinVIPMediatorCommand extends SkinMediatorCommand
	{
		public function SkinVIPMediatorCommand()
		{
			super();
		}
		public override function execute(noti:INotification):void
		{
			var md:SkinVIPMediator = new SkinVIPMediator(noti.getBody())
			facade.registerMediator(md);
		}
		
	}
}