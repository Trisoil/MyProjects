package pplive.skin.command
{
	import org.puremvc.as3.interfaces.INotification;
	import pplive.ui.mediator.SkinNotMediator;

	public class SkinNOTMediatorCommand extends SkinMediatorCommand
	{
		public function SkinNOTMediatorCommand()
		{
			super();
		}
		public override function execute(noti:INotification):void
		{
			var md:SkinNotMediator = new SkinNotMediator(noti.getBody())
			facade.registerMediator(md);
		}
	}
}