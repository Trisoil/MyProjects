package pplive.skin.command
{
	import org.puremvc.as3.interfaces.ICommand;
	import org.puremvc.as3.interfaces.INotification;
	import org.puremvc.as3.patterns.command.SimpleCommand;
	
	import pplive.ui.mediator.SkinMediator;

	public class SkinMediatorCommand extends SimpleCommand implements ICommand
	{
		public function SkinMediatorCommand()
		{
			super();
		}	
		public override function execute(noti:INotification):void
		{
			facade.registerMediator(new SkinMediator(noti.getBody()));
		}
		
	}
}