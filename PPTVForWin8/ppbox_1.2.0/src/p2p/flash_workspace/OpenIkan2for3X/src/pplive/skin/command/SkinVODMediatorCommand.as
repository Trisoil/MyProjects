package pplive.skin.command
{
	import org.puremvc.as3.interfaces.INotification;
	
	import pplive.ui.mediator.SkinVODMediator;
	
	public class SkinVODMediatorCommand extends SkinMediatorCommand
	{
		public function SkinVODMediatorCommand()
		{
			super();
		}
		public override function execute(noti:INotification):void
		{
			var md:SkinVODMediator = new SkinVODMediator(noti.getBody())
			facade.registerMediator(md);
		}
		
	}
}