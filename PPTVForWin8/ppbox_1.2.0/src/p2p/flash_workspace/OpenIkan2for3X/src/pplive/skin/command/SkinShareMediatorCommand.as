package pplive.skin.command
{
	import pplive.ui.mediator.SkinShareMediator;
	import org.puremvc.as3.interfaces.INotification;
	
	public class SkinShareMediatorCommand extends SkinMediatorCommand
	{
		public function SkinShareMediatorCommand()
		{
			super();
		}
		public override function execute(noti:INotification):void
		{
			facade.registerMediator(new SkinShareMediator(noti.getBody()));
		}
		
	}
}