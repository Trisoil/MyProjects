package pplive.skin.command
{
	import org.puremvc.as3.interfaces.INotification;
	
	import pplive.ui.mediator.SkinB2BMediator;
	
	public class SkinB2BMediatorCommand extends SkinMediatorCommand
	{
		public function SkinB2BMediatorCommand()
		{
			super();
		}
		public override function execute(noti:INotification):void
		{
			var md:SkinB2BMediator = new SkinB2BMediator(noti.getBody())
			facade.registerMediator(md);
		}
		
	}
}