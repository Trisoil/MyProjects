package my.myspring.tool;

import org.springframework.beans.BeansException;
import org.springframework.context.ApplicationContext;


public class SpringBeanProxy  {

	private static ApplicationContext applicationContext;

	/**
	 * 实现ApplicationContextAware接口的回调方法，设置上下文环境
	 * @param applicationContext
	 * @throws BeansException
	 */
	public synchronized static void setApplicationContext(ApplicationContext arg0) {
		applicationContext = arg0;
	}

	/**
	 * @return applicationContext
	 */
	public static ApplicationContext getApplicationContext() {
		return applicationContext;
	}

	/**
	  * 获取对象
	  * @param name
	  * @return Object 一个以所给名字注册的bean的实例
	  * @throws BeansException
	  */
	public static Object getBean(String beanName) throws BeansException{
		return applicationContext.getBean(beanName);
	}
}
